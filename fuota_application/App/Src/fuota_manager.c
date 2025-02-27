/*
 * fuota_manager.c
 *
 *  Created on: Dec 2, 2024
 *      Author: Massimiliano Cristarella
 */
#include <stdint.h>
#include <string.h>
#include "ble.h"
#include "fuota.h"
#include "fuota_state_machine.h"
#include "slot_info.h"
#include "addresses.h"
#include "flash_rw.h"
#include "main.h"
#include "stm32_timer.h"
#include "stm32_seq.h"
#include "app_ble.h"
#include "simple_crc32.h"
#include "log_module.h"


typedef enum
{
  FUOTA_CONTROL_REQUEST = 0x00,
  FUOTA_CONTROL_UPLOAD_FINISH = 0x01,
  FUOTA_CONTROL_UPLOAD_CANCEL = 0x02
} FuotaControlChar_Action;

typedef enum
{
  FUOTA_ACK_INFO = 0x00,
  FUOTA_ACK_RETRANSMIT = 0x01,
  FUOTA_ACK_BUSY = 0x02
} FuotaAck_Status;

typedef enum
{
  FUOTA_CONFIRM_OK = 0x00,
  FUOTA_CONFIRM_ERROR = 0x01
} FuotaConfirmChar_Result;

typedef struct
{
  FuotaState Fuota_State;
  uint8_t NewSlotIndex;
  SlotInfo NewSlotInfo;
  uint32_t NextUpdateChunk;
  FuotaConfirmChar_Result ConfirmResult;
  FuotaAck_Status AckStatus;
} FuotaManager_State;

typedef __PACKED_STRUCT
{
  uint8_t count;
  uint8_t active;
} Fuota_SlotsInfo_Payload_Header;

typedef __PACKED_STRUCT
{
  uint32_t version_code;
  char version_string[SLOTINFO_VERSION_STRING_LENGTH];
  char id;
  uint8_t is_valid;
} Fuota_SlotsInfo_Payload_Data;

typedef __PACKED_STRUCT
{
  uint8_t result;
  uint8_t slot_n;

} Fuota_Confirm_Payload_Data;

typedef __PACKED_STRUCT
{
  uint8_t ack_status;
  uint32_t next_chunk;
} Fuota_Ack_Payload_Data;

const uint32_t SlowAdvInterval = CONN_INT_MS(1000);
const uint32_t FastAdvInterval = CONN_INT_MS(11.25);

FuotaManager_State fuota_manager_state;
SlotInfo slots[APP_SLOTS_N];
uint32_t boot_addresses[APP_SLOTS_N] = {APP_SLOT0_ADDRESS, APP_SLOT1_ADDRESS};
Flash_RW_State update_slot;
uint8_t update_buffer[UPDATE_DATA_CHUNK_LENGTH];
UTIL_TIMER_Object_t reset_timer;

static void update_slotinfos_char(void);
static void slotInfo_to_Payload_Data(SlotInfo* slotInfo, Fuota_SlotsInfo_Payload_Data* data);
static void slotInfo_from_Payload_Data(SlotInfo* slotInfo, Fuota_SlotsInfo_Payload_Data* data);
static void system_reset_callback(void* arg);
static void handle_control_request(FUOTA_Data_t* received);
static void handle_control_uploadFinish(FUOTA_Data_t* received);
static void handle_control_uploadCancel(FUOTA_Data_t* received);

void FuotaManager_Init(void)
{
  fuota_manager_state.Fuota_State = STATE_IDLE;
  update_slotinfos_char();
}

void FuotaManager_HandleControlWrite(FUOTA_Data_t* received)
{
  FuotaControlChar_Action action = (FuotaControlChar_Action)received->p_Payload[0];
  LOG_DEBUG_APP("Control: 0x%02X%02X\n", received->p_Payload[0], received->p_Payload[1]);
  if (action == FUOTA_CONTROL_REQUEST)
  {
    Log_Module_Set_Verbose_Level(LOG_VERBOSE_WARNING);
  }
  else
  {
    Log_Module_Set_Verbose_Level(APPLI_CONFIG_LOG_LEVEL);
  }

  switch (action)
  {
    case FUOTA_CONTROL_REQUEST:
      handle_control_request(received);
      break;
    case FUOTA_CONTROL_UPLOAD_FINISH:
      handle_control_uploadFinish(received);
      break;
    case FUOTA_CONTROL_UPLOAD_CANCEL:
      handle_control_uploadCancel(received);
      break;
    default:
      break;
  }
}

void FuotaManager_HandleUpDataWrite(FUOTA_Data_t* received)
{
  if (TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_DATA_DOWNLOAD))
  {
    // Check if we received the expected chunk
    // The first 4 Bytes of te payload contain the Chunk number
    // NOTE: Attention to endianess! STM32 memory is little-endian (LSB first)
    // We send the chunk number in little-endian format from the companion application
    //  so no processing is needed
    uint32_t chunk_number = *(uint32_t*)(received->p_Payload);
    //LOG_WARNING_APP("\nchunk number 0x%lX\npayload length %d\n", chunk_number, p_Notification->DataTransfered.Length);

    if (fuota_manager_state.AckStatus == FUOTA_ACK_BUSY)
    {
      // Busy writing previous data, notify the client!
      UTIL_SEQ_SetTask(1U << TASK_ACK_NOTIF, CFG_SEQ_PRIO_0);
    }
    else if (chunk_number != fuota_manager_state.NextUpdateChunk)
    {
      // Unexpected chunk, notify the client!
      fuota_manager_state.AckStatus = FUOTA_ACK_RETRANSMIT;
      UTIL_SEQ_SetTask(1U << TASK_ACK_NOTIF, CFG_SEQ_PRIO_0);
    }
    else
    {
      fuota_manager_state.AckStatus = FUOTA_ACK_BUSY;
      if (chunk_number == 0)
      {
        update_slot.start_address = boot_addresses[fuota_manager_state.NewSlotIndex];

        SlotInfo_set_current_active(boot_addresses, APP_SLOTS_N);
        SlotInfos_read(slots, sizeof(slots));
        slots[fuota_manager_state.NewSlotIndex].is_valid = 0;
        if (SlotInfos_write(slots, sizeof(slots), -1) != SLOTINFO_OK)
        {
          LOG_ERROR_APP("\nERROR setting Slot %d as invalid\n", fuota_manager_state.NewSlotIndex);
          return;
        }
        update_slotinfos_char();
        if (Flash_RW_erase_pages(&update_slot, APP_SLOT_PAGES) != FLASH_RW_OK)
        {
          LOG_ERROR_APP("\nERROR erasing Slot %d in FLASH\n", fuota_manager_state.NewSlotIndex);
          return;
        }
      }

      Flash_RW_Result result = FLASH_RW_OK;

      // Copy update data in an array with a word-aligned (4 byte) address
      memcpy(update_buffer, &received->p_Payload[sizeof(chunk_number)], UPDATE_DATA_CHUNK_LENGTH);
      result = Flash_RW_write_qword(&update_slot, QWORDS(chunk_number), (uint32_t*)update_buffer, QWORDS(UPDATE_DATA_CHUNK_LENGTH));
      if (result == FLASH_RW_OK)
      {
        fuota_manager_state.NextUpdateChunk += UPDATE_DATA_CHUNK_LENGTH;
        fuota_manager_state.AckStatus = FUOTA_ACK_INFO;
        UTIL_SEQ_SetTask(1U << TASK_ACK_NOTIF, CFG_SEQ_PRIO_0);
      }
      else
      {
        LOG_ERROR_APP("\nERROR %d writing firmware in FLASH at address 0x%08X !\n", result, boot_addresses[fuota_manager_state.NewSlotIndex]);
        LOG_ERROR_APP("Update failed\n");
        fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_ERROR;
        UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
        TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_UPD_FAILED);
      }
    }
  }
  else
  {
    // TODO Centralized errors
    fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_ERROR;
    UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
    LOG_ERROR_APP("\nERROR: Update data not allowed\n");
    LOG_ERROR_APP("ERROR: Fuota State %d does not allow transition %d\n", fuota_manager_state.Fuota_State, TRANSITION_DATA_DOWNLOAD);
  }
}

void FuotaManager_HandleActSlotWrite(FUOTA_Data_t* received)
{
  uint8_t slot_idx = *received->p_Payload;
  SlotInfo_Result result = SlotInfo_set_active(slot_idx);
  update_slotinfos_char();
  if (result == SLOTINFO_OK)
  {
    LOG_INFO_APP("\nSlot %u is now active!\n", slot_idx);
  }
  else if (result == SLOTINFO_INDEX_OUT_OF_BOUND)
  {
    LOG_ERROR_APP("\nERROR: Received SlotInfo index %u does not exist!\n", slot_idx);
    return;
  }
  else if (result == SLOTINFO_WRITE_ERROR)
  {
    LOG_ERROR_APP("\nERROR: Impossible to write FLASH memory!\n");
    return;
  }
  else
  {
    LOG_ERROR_APP("\nERROR code 0x%2X occurred when setting active slot!\n", result);
    return;
  }
  LOG_INFO_APP("\n\nSlot %u set to active!\nRebooting...\n\n", slot_idx);
  UTIL_TIMER_Create(&reset_timer, 1000, UTIL_TIMER_ONESHOT, system_reset_callback, NULL);
  UTIL_TIMER_Start(&reset_timer);
}

void FuotaManager_UpdateConfirmNotificationData(FUOTA_Data_t* notification_data)
{
  // Value length is set to 2 byte (inside .ioc)
  Fuota_Confirm_Payload_Data* data = (Fuota_Confirm_Payload_Data*)notification_data->p_Payload;
  data->result = (uint8_t)fuota_manager_state.ConfirmResult;
  data->slot_n = fuota_manager_state.NewSlotIndex;
  notification_data->Length = sizeof(*data);
}

void FuotaManager_UpdateAckNotificationData(FUOTA_Data_t* notification_data)
{
  // Send chunk number keeping the little-endian format
  Fuota_Ack_Payload_Data* data = (Fuota_Ack_Payload_Data*)notification_data->p_Payload;
  data->ack_status = (uint8_t)fuota_manager_state.AckStatus;
  data->next_chunk = fuota_manager_state.NextUpdateChunk;
  notification_data->Length = sizeof(*data);
}

static void handle_control_request(FUOTA_Data_t* received)
{
  if (TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_UPD_REQUEST))
  {
    APP_BLE_UpdateConnectionInterval(FastAdvInterval, FastAdvInterval);
    int index = SlotInfo_get_current_address_index(boot_addresses, APP_SLOTS_N);
    if (index >= 0)
    {
      Fuota_SlotsInfo_Payload_Data data;
      memcpy(&data, &received->p_Payload[1], sizeof(data));
      slotInfo_from_Payload_Data(&fuota_manager_state.NewSlotInfo, &data);
      fuota_manager_state.NewSlotInfo.is_valid = 0;

      uint8_t new_idx = (index + 1) % APP_SLOTS_N;
      fuota_manager_state.NewSlotIndex = new_idx;
      fuota_manager_state.NextUpdateChunk = 0;
      fuota_manager_state.AckStatus = FUOTA_ACK_INFO;
      fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_OK;
      UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
    }
    else
    {
      fuota_manager_state.NewSlotIndex = 0xFF;
      fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_ERROR;
      UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
      LOG_ERROR_APP("\nERROR: boot address index not found error %d\n", index);
      TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_UPD_CANCEL);
    }
  }
  else
  {
    // TODO Centralized errors
    fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_ERROR;
    UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
    LOG_ERROR_APP("\nERROR: Update request not allowed\n");
    LOG_ERROR_APP("ERROR: Fuota State %d does not allow transition %d\n", fuota_manager_state.Fuota_State, TRANSITION_UPD_REQUEST);
  }
}

static void handle_control_uploadFinish(FUOTA_Data_t* received)
{
  if (TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_DOWNLOAD_FINISH))
  {
    uint32_t received_crc = *(uint32_t*)(&received->p_Payload[1]);
    uint32_t crc32 = SimpleCrc32((uint8_t*)update_slot.start_address, fuota_manager_state.NextUpdateChunk);
    if (received_crc == crc32)
    {
      SlotInfos_read(slots, sizeof(slots));
      fuota_manager_state.NewSlotInfo.is_valid = 1;
      memcpy(&slots[fuota_manager_state.NewSlotIndex], &fuota_manager_state.NewSlotInfo, sizeof(SlotInfo));
      if (SlotInfos_write(slots, sizeof(slots), fuota_manager_state.NewSlotIndex) == SLOTINFO_OK)
      {
        update_slotinfos_char();
        TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_UPD_SUCCESS);
        fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_OK;
        UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
        LOG_WARNING_APP("\n\nUpdate complete!\nRebooting...\n\n");
        UTIL_TIMER_Create(&reset_timer, 1000, UTIL_TIMER_ONESHOT, system_reset_callback, NULL);
        UTIL_TIMER_Start(&reset_timer);
      }
      else
      {
        LOG_ERROR_APP("\nERROR writing SlotInfo at address 0x%08X !\n", boot_addresses[fuota_manager_state.NewSlotIndex]);
      }
    }
    else
    {
      TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_UPD_FAILED);
      LOG_ERROR_APP("\nERROR Computed CRC32 not matching with received! Update failed!\n");
      LOG_ERROR_APP("Received Crc32 = 0x%08X, Computed = 0x%08X\n", received_crc, crc32);
      fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_ERROR;
      UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
    }
  }
  else
  {
    // TODO Centralized errors
    fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_ERROR;
    UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
    LOG_ERROR_APP("\nERROR: Upload finish not allowed\n");
    LOG_ERROR_APP("ERROR: Fuota State %d does not allow transition %d\n", fuota_manager_state.Fuota_State, TRANSITION_DOWNLOAD_FINISH);
  }
}

static void handle_control_uploadCancel(FUOTA_Data_t* received)
{
  TryTransition(&fuota_manager_state.Fuota_State, TRANSITION_UPD_CANCEL);
  fuota_manager_state.ConfirmResult = FUOTA_CONFIRM_OK;
  UTIL_SEQ_SetTask(1U << TASK_CONFIRM_NOTIF, CFG_SEQ_PRIO_0);
}

static void slotInfo_to_Payload_Data(SlotInfo* slotInfo, Fuota_SlotsInfo_Payload_Data* data)
{
  data->version_code = slotInfo->version_code;
  memcpy(data->version_string, slotInfo->version_string, sizeof(data->version_string));
  data->id = slotInfo->id;
  data->is_valid = slotInfo->is_valid;
}

static void slotInfo_from_Payload_Data(SlotInfo* slotInfo, Fuota_SlotsInfo_Payload_Data* data)
{
  SlotInfo_init(slotInfo, data->id, data->version_code, data->version_string, data->is_valid);
}

static void update_slotinfos_char(void)
{
  SlotInfo_Result res = SlotInfos_read(slots, sizeof(slots));
  if(res != SLOTINFO_OK)
  {
    LOG_ERROR_APP("\nError reading SlotInfos, cannot update characteristic! code 0x%02X\n", res);
    return;
  }
  uint8_t count = (uint8_t)SlotInfo_get_count();
  int active = SlotInfo_get_active();
  if (active < 0)
  {
    LOG_ERROR_APP("\nError reading active SlotInfo index, cannot update characteristic! code 0x%02X\n", -active);
    return;
  }

  FUOTA_Data_t data;
  uint8_t payload[CFG_SLOTS_INFO_CHAR_MAX_LENGTH];
  data.p_Payload = payload;
  data.Length = 0;

  //TODO add current slot to header?
  Fuota_SlotsInfo_Payload_Header header;
  header.count = count;
  header.active = (uint8_t)active;
  memcpy(&data.p_Payload[data.Length], &header, sizeof(header));
  data.Length += sizeof(header);

  for (uint8_t i = 0; i < count; ++i)
  {
    Fuota_SlotsInfo_Payload_Data info;
    slotInfo_to_Payload_Data(&slots[i], &info);
    memcpy(&data.p_Payload[data.Length], &info, sizeof(info));
    data.Length += sizeof(info);
  }

  FUOTA_UpdateValue(FUOTA_SLOTSINFO, &data);
}

static void system_reset_callback(void* arg)
{
  HAL_NVIC_SystemReset();
}

