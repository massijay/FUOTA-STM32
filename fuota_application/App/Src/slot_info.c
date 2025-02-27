/*
 * slot_info.c
 *
 *  Created on: Oct 22, 2024
 *      Author: Massimiliano Cristarella
 */

#include "slot_info.h"

#include "flash_rw.h"
#include <string.h>

#define SLOTINFOS_LEN(size) (size / sizeof(SlotInfo))

extern uint8_t _slotinfo_start;
Flash_RW_State flash_state =
{
    .start_address = (uint32_t)&_slotinfo_start
};
struct
{
  uint32_t magic_number_idx;
  uint32_t slotInfosCount_idx;
  uint32_t activeSlotInfo_idx;
  uint32_t reserved_idx;
  uint32_t slotInfos_idx;
} saved_data =
{
    .magic_number_idx = 0,
    .slotInfosCount_idx = WORD_LENGTH,
    .activeSlotInfo_idx = 2 * WORD_LENGTH,
    .reserved_idx = 3 * WORD_LENGTH,
    .slotInfos_idx = 4 * WORD_LENGTH
};

static uint8_t is_magic_n_found(void)
{
  uint32_t magic_n;
  Flash_RW_read(&flash_state, saved_data.magic_number_idx, &magic_n, WORD_LENGTH);
  if(magic_n != GP_MEM_MAGIC_N)
  {
    return 0;
  }
  return 1;
}

static SlotInfo_Result write_magic_n(void)
{
  uint32_t magic_n = GP_MEM_MAGIC_N;
  Flash_RW_Result result = Flash_RW_write(&flash_state, saved_data.magic_number_idx, &magic_n, WORD_LENGTH, 1);
  if(result != FLASH_RW_OK)
  {
    return SLOTINFO_WRITE_ERROR;
  }
  return SLOTINFO_OK;
}

/**
 * @brief Initialize SlotInfo struct with data
 * @param slotInfo: Pointer to the SlotInfo to be initialized
 * @param id: Id of the SlotInfo, e.g. 'A'
 * @param version_code: Incremental integer used as an internal version number
 * @param version_string: String used as the version number shown to users. This should be set as <major>.<minor>.<revision>-<variant>, e.g. 1.2.14-C
 * @param is_valid: Set to 1 to mark the SlotInfo as valid, 0 otherwise
 */
void SlotInfo_init(SlotInfo* slotInfo, char id, uint32_t version_code, const char* version_string, uint8_t is_valid)
{
  memset(slotInfo, 0x00, sizeof(SlotInfo));
  slotInfo->id = id;
  slotInfo->is_valid = is_valid;
  slotInfo->version_code = version_code;
  strcpy(slotInfo->version_string, version_string);
}

/**
 * @brief Get the count of the written SlotInfos
 * @retval int: Count of SlotInfos, or negative int of SlotInfo_Result (e.g. -1 -> SLOTINFO_ERROR)
 */
int SlotInfo_get_count(void)
{
  uint32_t count;
  if(!is_magic_n_found())
  {
    return -SLOTINFO_MAGIC_N_NOT_FOUND;
  }
  if (Flash_RW_is_empty(&flash_state, saved_data.slotInfosCount_idx, WORD_LENGTH))
  {
    return -SLOTINFOS_COUNT_NOT_FOUND;
  }
  Flash_RW_read(&flash_state, saved_data.slotInfosCount_idx, &count, WORD_LENGTH);
  return (count & 0xFF);
}

/**
 * @brief Read SlotInfos from FLASH memory
 * @param slotInfos: Array where SlotInfos data will be copied to
 * @param size: Size of the array in bytes
 * @retval SlotInfo_Result
 */
SlotInfo_Result SlotInfos_read(SlotInfo* slotInfos, uint32_t size)
{
  int count = SlotInfo_get_count();
  if (count < 0)
  {
    return -count;
  }
  if(count != SLOTINFOS_LEN(size))
  {
    return SLOTINFOS_COUNT_MISMATCH;
  }
  if (Flash_RW_is_empty(&flash_state, saved_data.slotInfos_idx, size))
  {
    return SLOTINFOS_NOT_FOUND;
  }
  Flash_RW_read(&flash_state, saved_data.slotInfos_idx, slotInfos, size);
  return SLOTINFO_OK;
}

/**
 * @brief Write SlotInfos to FLASH memory
 * @param slotInfos: Array of SlotInfos to write
 * @param size: Size of the array in bytes
 * @param active_index: Set active index, or do not set if negative number is passed
 * @retval SlotInfo_Result
 */
SlotInfo_Result SlotInfos_write(SlotInfo* slotInfos, uint32_t size, int active_index)
{
  if(!is_magic_n_found() && write_magic_n())
  {
    return SLOTINFO_MAGIC_N_NOT_FOUND;
  }
  uint32_t count = (SLOTINFOS_LEN(size) & 0xFF);
  Flash_RW_Result result = Flash_RW_write(&flash_state, saved_data.slotInfosCount_idx, &count, WORD_LENGTH, 0);
  if (result != FLASH_RW_OK)
  {
    return SLOTINFO_WRITE_ERROR;
  }
  result = Flash_RW_write(&flash_state, saved_data.slotInfos_idx, slotInfos, size, 0);
  if (result != FLASH_RW_OK)
  {
    return SLOTINFO_WRITE_ERROR;
  }
  if (active_index >= 0)
  {
    return SlotInfo_set_active(active_index);
  }
  return SLOTINFO_OK;
}

/**
 * @brief Set SlotInfo to active using its index
 * @param slot_index: Index of the SlotInfo to set active (starting from 0), e.g. 1
 * @retval SlotInfo_Result
 */
SlotInfo_Result SlotInfo_set_active(uint8_t slot_index)
{
  int count = SlotInfo_get_count();
  if (count < 0)
  {
    return -count;
  }
  if (slot_index >= count)
  {
    return SLOTINFO_INDEX_OUT_OF_BOUND;
  }
  uint32_t index = slot_index & 0xFF;
  Flash_RW_Result result = Flash_RW_write(&flash_state, saved_data.activeSlotInfo_idx, &index, WORD_LENGTH, 0);
  if (result != FLASH_RW_OK)
  {
    return SLOTINFO_WRITE_ERROR;
  }
  return SLOTINFO_OK;
}

/**
 * @brief Get the index of the active SlotInfo
 * @retval int: Index of the active SlotInfo, or negative int of SlotInfo_Result (e.g. -1 -> SLOTINFO_ERROR)
 */
int SlotInfo_get_active(void)
{
  uint32_t active;
  if(!is_magic_n_found())
  {
    return -SLOTINFO_MAGIC_N_NOT_FOUND;
  }
  if (Flash_RW_is_empty(&flash_state, saved_data.activeSlotInfo_idx, WORD_LENGTH))
  {
    return -SLOTINFO_NONE_IS_ACTIVE;
  }
  Flash_RW_read(&flash_state, saved_data.activeSlotInfo_idx, &active, WORD_LENGTH);
  return (active & 0xFF);
}

/**
 * @brief Switch the active slot to the next one (or go back to the first if the active is the last one)
 * @retval SlotInfo_Result
 */
SlotInfo_Result SlotInfo_switch_active(void)
{
  int count = SlotInfo_get_count();
  if (count < 0)
  {
    return -count;
  }
  int active = SlotInfo_get_active();
  if (active < 0)
  {
    return -active;
  }
  active = (active + 1) % count;
  return SlotInfo_set_active((uint8_t)active);
}

/**
 * @brief Switch the current slot as active
 * @param addresses: Array of addresses (see SlotInfo_get_current_address_index)
 * @param count: Count of the addresses inside the array
 * @retval SlotInfo_Result
 */
SlotInfo_Result SlotInfo_set_current_active(uint32_t* addresses, uint8_t count)
{
  int curr_idx = SlotInfo_get_current_address_index(addresses, count);
  if (curr_idx < 0)
  {
    return -curr_idx;
  }
  int act_idx = SlotInfo_get_active();
  if (act_idx < 0)
  {
    return -act_idx;
  }
  if (curr_idx != act_idx)
  {
    SlotInfo_Result res = SlotInfo_set_active(curr_idx);
    return res;
  }
  return SLOTINFO_OK;
}

/**
 * @brief Get current application start address (VTOR) index, given an array with addresses
 * @param addresses: Array of addresses
 * @param count: Count of the addresses inside the array
 * @retval int: Index of the current app start address, or (negative) -SLOTINFO_UNRECOGNIZED_START_ADDRESS if it's not present in the addresses array
 */
int SlotInfo_get_current_address_index(uint32_t* addresses, uint8_t count)
{
  for (uint8_t i = 0; i < count; ++i)
  {
    if(SCB->VTOR == addresses[i])
    {
      return i;
    }
  }
  return -SLOTINFO_UNRECOGNIZED_START_ADDRESS;
}
