/*
 * flash_rw.c
 * BLE version (compatible with BLE stack)
 *
 *  Created on: Oct 14, 2024
 *      Author: Massimiliano Cristarella
 */

#include "flash_rw.h"

#include "stm32_seq.h"
#include "stm32_timer.h"
#include "flash_manager.h"
#include "stm32wbaxx_hal_icache.h"

#include <stdio.h>
#include <string.h>

uint8_t wip = 0;
UTIL_TIMER_Object_t timeout;
uint8_t timeouts_n = 0;

static void timeout_erase_callback(void* arg)
{
  ++timeouts_n;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT);
}
static void timeout_write_callback(void* arg)
{
  ++timeouts_n;
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT);
}

byte memory[FLASH_RW_MEMORY_ARRAY_LENGTH];

/* Flag for write status  */
static FM_FlashOp_Status_t WriteStatus;
/* Flag for erase status  */
static FM_FlashOp_Status_t EraseStatus;

static void WriteCallback(FM_FlashOp_Status_t status)
{
  /* Update status */
  WriteStatus = status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt(1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT);
}
static void EraseCallback(FM_FlashOp_Status_t status)
{
  /* Update status */
  EraseStatus = status;

  /* Set event on Process request call */
  UTIL_SEQ_SetEvt (1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT);
}

static FM_CallbackNode_t WriteCallbackNode =
{
  /* Header for chained list */
  .NodeList =
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = WriteCallback
};
static FM_CallbackNode_t EraseCallbackNode =
{
  /* Header for chained list */
  .NodeList =
  {
    .next = NULL,
    .prev = NULL
  },
  /* Callback for request status */
  .Callback = EraseCallback
};

/**
  * @brief  Erase the FLASH page at the state start address
  * @param  state: Flash_RW_State*
  * @retval Flash_RW_Result: Result enum
  */
Flash_RW_Result Flash_RW_erase(Flash_RW_State* state)
{
  return Flash_RW_erase_pages(state, 1);
}

/**
  * @brief  Erase a number of FLASH page starting from the state start address
  * @param  state: Flash_RW_State*
  * @param  pages_n: Number of pages to erase
  * @retval Flash_RW_Result: Result enum
  */
Flash_RW_Result Flash_RW_erase_pages(Flash_RW_State* state, word pages_n)
{
  if (wip)
  {
    return FLASH_RW_ERROR_BUSY;
  }
  wip = 1;
  timeouts_n = 0;
  UTIL_TIMER_Create(&timeout, 1000, UTIL_TIMER_ONESHOT, timeout_erase_callback, NULL);
  FM_Cmd_Status_t status;
  UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT);
  do
  {
    status = FM_Erase(FLASH_RW_GET_PAGE_N(state->start_address), pages_n, &EraseCallbackNode);
    UTIL_TIMER_Start(&timeout);
    if (status == FM_ERROR)
    {
      printf("FLASH_RW: ERROR erasing FLASH!\n");
      printf("Erase command status = %d, operation status = %d\n\n", status, EraseStatus);
      return FLASH_RW_ERROR;
    }
    UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT);
    UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_FM_ERASE_CALLBACK_EVT);
    UTIL_TIMER_Stop(&timeout);
    if (timeouts_n > 5)
    {
      return FLASH_RW_ERROR_TIMEOUT;
    }
  }
  while (status == FM_BUSY || EraseStatus != FM_OPERATION_COMPLETE);
  wip = 0;
#if defined(ICACHE) && defined (HAL_ICACHE_MODULE_ENABLED)
  // FLASH data is cached inside ICACHE.
  // We need to invalidate it after erasing,
  // otherwise strange behaviors occur while reading
  // e.g. sometimes read data that has been erased/overwritten
  HAL_ICACHE_Invalidate();
#endif /* ICACHE && HAL_ICACHE_MODULE_ENABLED */
  return FLASH_RW_OK;
}

/**
  * @brief  Write a QWORD (16 bytes) inside FLASH
  * @param  state: Flash_RW_State*
  * @param  offset_qwords: Number of qwords from state->start_address where data writing begins
  * @param  data: Pointer to data to write
  * @param  size: Number of words to write
  * @param  purge: Set to 1 to erase FLASH content before writing, 0 to keep existing data and overwrite
  * @retval Flash_RW_Result: Result enum
  */
Flash_RW_Result Flash_RW_write_qword(Flash_RW_State* state, word offset_qwords, const word* data, word qwords_count)
{
  if (wip)
  {
    return FLASH_RW_ERROR_BUSY;
  }
  wip = 1;
  if ((word)data & MASK_WORD)
  {
    printf("FLASH_RW: ERROR! Data start address must be aligned to 32 bit (4 Byte)!\n");
    return FLASH_RW_ERROR_SRC_ADDR_NOT_ALIGNED;
  }
  word dest = state->start_address + BYTES_FROM_QWORDS(offset_qwords);
  timeouts_n = 0;
  UTIL_TIMER_Create(&timeout, 1000, UTIL_TIMER_ONESHOT, timeout_write_callback, NULL);
  FM_Cmd_Status_t status;
  UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT);
  do
  {
    status = FM_Write((word*)data, (word*)dest, WORDS_FROM_QWORDS(qwords_count), &WriteCallbackNode);
    UTIL_TIMER_Start(&timeout);
    if (status == FM_ERROR)
    {
      printf("FLASH_RW: ERROR writing FLASH!\n");
      printf("Write command status = %d, operation status = %d\n\n", status, WriteStatus);
      return FLASH_RW_ERROR;
    }
    UTIL_SEQ_WaitEvt(1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT);
    UTIL_SEQ_ClrEvt(1 << CFG_IDLEEVT_FM_WRITE_CALLBACK_EVT);
    UTIL_TIMER_Stop(&timeout);
    if (timeouts_n > 5)
    {
      return FLASH_RW_ERROR_TIMEOUT;
    }
  }
  while (status == FM_BUSY || WriteStatus != FM_OPERATION_COMPLETE);
  wip = 0;
  return FLASH_RW_OK;
}

/**
  * @brief  Write data inside FLASH
  * @param  state: Flash_RW_State*
  * @param  offset: Write data starting from an offset relative to state->start_address
  * @param  data: Pointer to data to write
  * @param  size: Size of data (in bytes) to write
  * @param  purge: Set to 1 to erase FLASH content before writing, 0 to keep existing data and overwrite
  * @retval Flash_RW_Result: Result enum
  */
Flash_RW_Result Flash_RW_write(Flash_RW_State* state, word offset, const void* data, word size, byte purge)
{
  if (state->start_address & MASK_QUADWORD)
  {
    printf("FLASH_RW: ERROR! Flash start address must be aligned to 128 bit (16 Byte)!\n");
    return FLASH_RW_ERROR_DST_ADDR_NOT_ALIGNED;
  }
  if (offset > FLASH_RW_MEMORY_ARRAY_LENGTH)
  {
    printf("FLASH_RW: ERROR! Specify an offset less than %lu!\n", FLASH_RW_MEMORY_ARRAY_LENGTH);
    return FLASH_RW_ERROR;
  }
  word end = offset + size; // pointer right after data array end
  if (end > FLASH_RW_MEMORY_ARRAY_LENGTH)
  {
    printf("FLASH_RW: ERROR! Source is too big! offset + size must be < %lu\n", FLASH_RW_MEMORY_ARRAY_LENGTH);
    return FLASH_RW_ERROR;
  }

  if(purge)
  {
    memset(memory, 0xFF, FLASH_RW_MEMORY_ARRAY_LENGTH);
  }
  else
  {
    memcpy(memory, (word*)state->start_address, FLASH_RW_MEMORY_ARRAY_LENGTH); // RAM <- FLASH
  }
  memcpy(&memory[offset], data, size); // RAM <- data

  Flash_RW_Result result;
  result = Flash_RW_erase(state);
  if (result != FLASH_RW_OK)
  {
    return result;
  }

  // put back in FLASH the updated ram content
  result = Flash_RW_write_qword(state, 0, (word*)memory, FLASH_RW_MEMORY_ARRAY_QUADWORDS);
  if (result != FLASH_RW_OK)
  {
    return result;
  }

#ifdef DEBUG
  // Verify data is written correctly in FLASH
  word diff = memcmp((void*)state->start_address, memory, FLASH_RW_MEMORY_ARRAY_LENGTH);
  if(diff)
  {
    printf("FLASH_RW: ERROR: data written does not match with RAM buffer!\n\n");
    return FLASH_RW_ERROR;
  }
#endif

  return FLASH_RW_OK;
}

/**
  * @brief  Read data from FLASH
  * @param  state: Flash_RW_State*
  * @param  offset: Read data starting from an offset relative to state->start_address
  * @param  dest: Pointer where data will be copied
  * @param  size: Size of data (in bytes) to read
  */
void Flash_RW_read(Flash_RW_State* state, word offset, void* dest, word size)
{
  memcpy(dest, (void*)(state->start_address + offset), size);
}

/**
  * @brief  Check if area inside FLASH is empty
  * @param  state: Flash_RW_State*
  * @param  offset: Write data starting from an offset relative to state->start_address
  * @param  size: Size of data (in bytes) to write
  * @retval byte: 1 is empty, 0 otherwise
  */
byte Flash_RW_is_empty(Flash_RW_State* state, word offset, word size)
{
  byte* ptr = (byte*)(state->start_address + offset);
  for(word i = 0; i < size; ++i)
  {
    if (ptr[i] != 0xFF)
    {
      return 0;
    }
  }
  return 1;
}


