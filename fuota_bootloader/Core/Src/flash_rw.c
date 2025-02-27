/*
 * flash_rw.c
 *
 *  Created on: Oct 14, 2024
 *      Author: Massimiliano Cristarella
 */

#include "flash_rw.h"

#include "stm32wbaxx_hal_icache.h"

#include <stdio.h>
#include <string.h>

static HAL_StatusTypeDef _erase_no_unlock(Flash_RW_State* state)
{
  FLASH_EraseInitTypeDef eraseInitStruct =
  {
      .TypeErase = FLASH_TYPEERASE_PAGES,
      .Page = FLASH_RW_GET_PAGE_N(state->start_address),
      .NbPages = 1
  };
  word pageError;
  while(HAL_FLASHEx_IsOperationSuspended())
  {
    printf("FLASH_RW: FLASH Operation pending, waiting...\n");
  }
  HAL_StatusTypeDef status = HAL_FLASHEx_Erase(&eraseInitStruct, &pageError);
#if defined(ICACHE) && defined (HAL_ICACHE_MODULE_ENABLED)
  // FLASH data is cached inside ICACHE.
  // We need to invalidate it after erasing,
  // otherwise strange behaviors occur while reading
  // e.g. sometimes read data that has been erased/overwritten
  HAL_ICACHE_Invalidate();
#endif /* ICACHE && HAL_ICACHE_MODULE_ENABLED */
  return status;
}

/**
  * @brief  Erase entire FLASH page
  * @param  state: Flash_RW_State
  * @retval Flash_RW_Result: Result enum
  */
Flash_RW_Result Flash_RW_erase(Flash_RW_State* state)
{
  HAL_FLASH_Unlock();
  if (_erase_no_unlock(state) != HAL_OK)
  {
    HAL_FLASH_Lock();
    printf("FLASH_RW: ERROR erasing FLASH!\n");
    return FLASH_RW_ERROR;
  }
  HAL_FLASH_Lock();
  return FLASH_RW_OK;
}

/**
  * @brief  Write data inside FLASH
  * @param  state: Flash_RW_State
  * @param  offset: Write data starting from an offset relative to state->start_address
  * @param  data: Address of data to write
  * @param  size: Size of data (in bytes) to write
  * @param  purge: Set to 1 to erase FLASH content before writing, 0 to keep existing data and overwrite
  * @retval Flash_RW_Result: Result enum
  */
Flash_RW_Result Flash_RW_write(Flash_RW_State* state, word offset, const void* data, word size, byte purge)
{
  if (state->start_address & MASK_QUADWORD)
  {
    printf("FLASH_RW: ERROR! Flash start address must be aligned to 128 bit (16 Byte)!\n");
    return FLASH_RW_ERROR;
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

  byte memory[FLASH_RW_MEMORY_ARRAY_LENGTH];

  HAL_FLASH_Unlock();

  if(purge)
  {
    memset(memory, 0xFF, FLASH_RW_MEMORY_ARRAY_LENGTH);
  }
  else
  {
    memcpy(memory, (word*)state->start_address, FLASH_RW_MEMORY_ARRAY_LENGTH); // RAM <- FLASH
  }
  memcpy(&memory[offset], data, size); // RAM <- data

  HAL_StatusTypeDef status;
  status = _erase_no_unlock(state);
  if (status != HAL_OK)
  {
    HAL_FLASH_Lock();
    printf("FLASH_RW: ERROR erasing FLASH!\n");
    return FLASH_RW_ERROR;
  }

  // put back in FLASH the updated ram content
  for(word i = 0; i < FLASH_RW_MEMORY_ARRAY_QUADWORDS; ++i)
  {
    status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, state->start_address + i * QUADWORD_LENGTH, (word)&memory[i * QUADWORD_LENGTH]);
    if(status != HAL_OK)
    {
      HAL_FLASH_Lock();
      printf("FLASH_RW: ERROR writing FLASH!\n");
      return FLASH_RW_ERROR;
    }
  }

  HAL_FLASH_Lock();

  // Verify data is written correctly in FLASH
  word diff = memcmp((void*)state->start_address, memory, FLASH_RW_MEMORY_ARRAY_LENGTH);
  if(diff)
  {
    printf("FLASH_RW: ERROR: data written does not match with RAM buffer!\n\n");
    return FLASH_RW_ERROR;
  }

  return FLASH_RW_OK;
}

/**
  * @brief  Read data from FLASH
  * @param  state: Flash_RW_State
  * @param  offset: Read data starting from an offset relative to state->start_address
  * @param  dest: Address where data will be copied
  * @param  size: Size of data (in bytes) to read
  */
void Flash_RW_read(Flash_RW_State* state, word offset, void* dest, word size)
{
  memcpy(dest, (void*)(state->start_address + offset), size);
}

/**
  * @brief  Check if area inside FLASH is empty
  * @param  state: Flash_RW_State
  * @param  offset: Write data starting from an offset relative to state->start_address
  * @param  size: Size of data (in bytes) to write
  * @retval byte: 1 is empty, 0 otherwise
  */
byte Flash_RW_is_empty(Flash_RW_State* state, word offset, word size)
{
  byte* ptr = (byte*)(state->start_address + offset);
  for(uint32_t i = 0; i < size; ++i)
  {
    if (ptr[i] != 0xFF)
    {
      return 0;
    }
  }
  return 1;
}


