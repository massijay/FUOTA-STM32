/*
 * slot_info.h
 *
 *  Created on: Oct 22, 2024
 *      Author: Massimiliano Cristarella
 */

#ifndef INC_SLOT_INFO_H_
#define INC_SLOT_INFO_H_

#include <stdint.h>

#define GP_MEM_MAGIC_N 0x1B4D89DUL
#define SLOTINFO_VERSION_STRING_LENGTH 18

typedef enum
{
  SLOTINFO_OK = 0x00,
  SLOTINFO_ERROR,
  SLOTINFO_MAGIC_N_NOT_FOUND,
  SLOTINFOS_COUNT_NOT_FOUND,
  SLOTINFOS_COUNT_MISMATCH,
  SLOTINFOS_NOT_FOUND,
  SLOTINFO_WRITE_ERROR,
  SLOTINFO_NONE_IS_ACTIVE,
  SLOTINFO_INDEX_OUT_OF_BOUND,
  SLOTINFO_UNRECOGNIZED_START_ADDRESS
} SlotInfo_Result;

typedef struct
{
  uint32_t version_code;
  char version_string[SLOTINFO_VERSION_STRING_LENGTH];
  char id;
  uint8_t is_valid;
} SlotInfo;

void SlotInfo_init(SlotInfo* slotInfo, char id, uint32_t version_code, const char* version_string, uint8_t is_valid);
int SlotInfo_get_count(void);
SlotInfo_Result SlotInfos_read(SlotInfo* slotInfos, uint32_t size);
SlotInfo_Result SlotInfos_write(SlotInfo* slotInfos, uint32_t size, int active_index);
SlotInfo_Result SlotInfo_set_active(uint8_t slot_index);
int SlotInfo_get_active(void);
SlotInfo_Result SlotInfo_switch_active(void);
SlotInfo_Result SlotInfo_set_current_active(uint32_t* addresses, uint8_t count);
int SlotInfo_get_current_address_index(uint32_t* addresses, uint8_t count);

#endif /* INC_SLOT_INFO_H_ */
