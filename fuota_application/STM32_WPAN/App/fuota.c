/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.c
  * @author  MCD Application Team
  * @brief   service1 definition.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "common_blesvc.h"
#include "fuota.h"

/* USER CODE BEGIN Includes */
#include "main.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/

/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef struct{
  uint16_t  FuotaSvcHdle;                  /**< Fuota Service Handle */
  uint16_t  ControlCharHdle;                  /**< CONTROL Characteristic Handle */
  uint16_t  ConfirmCharHdle;                  /**< CONFIRM Characteristic Handle */
  uint16_t  UpdataCharHdle;                  /**< UPDATA Characteristic Handle */
  uint16_t  AckCharHdle;                  /**< ACK Characteristic Handle */
  uint16_t  SlotsinfoCharHdle;                  /**< SLOTSINFO Characteristic Handle */
  uint16_t  ActslotsetCharHdle;                  /**< ACTSLOTSET Characteristic Handle */
/* USER CODE BEGIN Context */
  /* Place holder for Characteristic Descriptors Handle*/

/* USER CODE END Context */
}FUOTA_Context_t;

/* Private defines -----------------------------------------------------------*/
#define UUID_128_SUPPORTED  1

#if (UUID_128_SUPPORTED == 1)
#define BM_UUID_LENGTH  UUID_TYPE_128
#else
#define BM_UUID_LENGTH  UUID_TYPE_16
#endif

#define BM_REQ_CHAR_SIZE    (3)

/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
#define CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET         2
#define CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET              1
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static const uint16_t SizeControl = CFG_CONTROL_CHAR_MAX_LENGTH;
static const uint16_t SizeConfirm = 2;
static const uint16_t SizeUpdata = CFG_UPDATE_DATA_CHAR_MAX_LENGTH;
static const uint16_t SizeAck = 5;
static const uint16_t SizeSlotsinfo = CFG_SLOTS_INFO_CHAR_MAX_LENGTH;
static const uint16_t SizeActslotset = 1;

static FUOTA_Context_t FUOTA_Context;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static SVCCTL_EvtAckStatus_t FUOTA_EventHandler(void *p_pckt);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
/* USER CODE BEGIN PFD */

/* USER CODE END PFD */

/* Private functions ----------------------------------------------------------*/

#define COPY_UUID_128(uuid_struct, uuid_15, uuid_14, uuid_13, uuid_12, uuid_11, uuid_10, uuid_9, uuid_8, uuid_7, uuid_6, uuid_5, uuid_4, uuid_3, uuid_2, uuid_1, uuid_0) \
do {\
    uuid_struct[0] = uuid_0; uuid_struct[1] = uuid_1; uuid_struct[2] = uuid_2; uuid_struct[3] = uuid_3; \
    uuid_struct[4] = uuid_4; uuid_struct[5] = uuid_5; uuid_struct[6] = uuid_6; uuid_struct[7] = uuid_7; \
    uuid_struct[8] = uuid_8; uuid_struct[9] = uuid_9; uuid_struct[10] = uuid_10; uuid_struct[11] = uuid_11; \
    uuid_struct[12] = uuid_12; uuid_struct[13] = uuid_13; uuid_struct[14] = uuid_14; uuid_struct[15] = uuid_15; \
}while(0)

/*
 The following 128bits UUIDs have been generated from the random UUID
 generator:
 0000F100CC7A482A984A7F2ED5B3E58F: Service 128bits UUID
 0000F1108E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000F1208E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000F1308E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000F1408E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000F1908E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 0000F1918E2245419D4C21EDAE82ED19: Characteristic 128bits UUID
 */
#define COPY_FUOTA_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x00,0xcc,0x7a,0x48,0x2a,0x98,0x4a,0x7f,0x2e,0xd5,0xb3,0xe5,0x8f)
#define COPY_CONTROL_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x10,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_CONFIRM_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x20,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_UPDATA_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x30,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_ACK_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x40,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_SLOTSINFO_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x90,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)
#define COPY_ACTSLOTSET_UUID(uuid_struct)       COPY_UUID_128(uuid_struct,0x00,0x00,0xf1,0x91,0x8e,0x22,0x45,0x41,0x9d,0x4c,0x21,0xed,0xae,0x82,0xed,0x19)

/* USER CODE BEGIN PF */

/* USER CODE END PF */

/**
 * @brief  Event handler
 * @param  p_Event: Address of the buffer holding the p_Event
 * @retval Ack: Return whether the p_Event has been managed or not
 */
static SVCCTL_EvtAckStatus_t FUOTA_EventHandler(void *p_Event)
{
  SVCCTL_EvtAckStatus_t return_value;
  hci_event_pckt *p_event_pckt;
  evt_blecore_aci *p_blecore_evt;
  aci_gatt_attribute_modified_event_rp0 *p_attribute_modified;
  FUOTA_NotificationEvt_t                 notification;
  /* USER CODE BEGIN Service1_EventHandler_1 */

  /* USER CODE END Service1_EventHandler_1 */

  return_value = SVCCTL_EvtNotAck;
  p_event_pckt = (hci_event_pckt *)(((hci_uart_pckt*)p_Event)->data);

  switch(p_event_pckt->evt)
  {
    case HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE:
      p_blecore_evt = (evt_blecore_aci*)p_event_pckt->data;
      switch(p_blecore_evt->ecode)
      {
        case ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_BEGIN */
          p_attribute_modified = (aci_gatt_attribute_modified_event_rp0*)p_blecore_evt->data;
          notification.ConnectionHandle         = p_attribute_modified->Connection_Handle;
          notification.AttributeHandle          = p_attribute_modified->Attr_Handle;
          notification.DataTransfered.Length    = p_attribute_modified->Attr_Data_Length;
          notification.DataTransfered.p_Payload = p_attribute_modified->Attr_Data;
          if(p_attribute_modified->Attr_Handle == (FUOTA_Context.ConfirmCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_2 */

            /* USER CODE END Service1_Char_2 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_2_attribute_modified */

              /* USER CODE END Service1_Char_2_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service1_Char_2_Disabled_BEGIN */

                /* USER CODE END Service1_Char_2_Disabled_BEGIN */
                notification.EvtOpcode = FUOTA_CONFIRM_NOTIFY_DISABLED_EVT;
                FUOTA_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_Disabled_END */

                /* USER CODE END Service1_Char_2_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Notification_BEGIN */

                /* USER CODE END Service1_Char_2_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = FUOTA_CONFIRM_NOTIFY_ENABLED_EVT;
                FUOTA_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_2_COMSVC_Notification_END */

                /* USER CODE END Service1_Char_2_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_2_default */

                /* USER CODE END Service1_Char_2_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (FUOTA_Context.ConfirmCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (FUOTA_Context.AckCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;
            /* USER CODE BEGIN Service1_Char_4 */

            /* USER CODE END Service1_Char_4 */
            switch(p_attribute_modified->Attr_Data[0])
            {
              /* USER CODE BEGIN Service1_Char_4_attribute_modified */

              /* USER CODE END Service1_Char_4_attribute_modified */

              /* Disabled Notification management */
              case (!(COMSVC_Notification)):
                /* USER CODE BEGIN Service1_Char_4_Disabled_BEGIN */

                /* USER CODE END Service1_Char_4_Disabled_BEGIN */
                notification.EvtOpcode = FUOTA_ACK_NOTIFY_DISABLED_EVT;
                FUOTA_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_4_Disabled_END */

                /* USER CODE END Service1_Char_4_Disabled_END */
                break;

              /* Enabled Notification management */
              case COMSVC_Notification:
                /* USER CODE BEGIN Service1_Char_4_COMSVC_Notification_BEGIN */

                /* USER CODE END Service1_Char_4_COMSVC_Notification_BEGIN */
                notification.EvtOpcode = FUOTA_ACK_NOTIFY_ENABLED_EVT;
                FUOTA_Notification(&notification);
                /* USER CODE BEGIN Service1_Char_4_COMSVC_Notification_END */

                /* USER CODE END Service1_Char_4_COMSVC_Notification_END */
                break;

              default:
                /* USER CODE BEGIN Service1_Char_4_default */

                /* USER CODE END Service1_Char_4_default */
                break;
            }
          }  /* if(p_attribute_modified->Attr_Handle == (FUOTA_Context.AckCharHdle + CHARACTERISTIC_DESCRIPTOR_ATTRIBUTE_OFFSET))*/

          else if(p_attribute_modified->Attr_Handle == (FUOTA_Context.ControlCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = FUOTA_CONTROL_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_1_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            FUOTA_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (FUOTA_Context.ControlCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (FUOTA_Context.UpdataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = FUOTA_UPDATA_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service1_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_3_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            FUOTA_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (FUOTA_Context.UpdataCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/
          else if(p_attribute_modified->Attr_Handle == (FUOTA_Context.ActslotsetCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))
          {
            return_value = SVCCTL_EvtAckFlowEnable;

            notification.EvtOpcode = FUOTA_ACTSLOTSET_WRITE_NO_RESP_EVT;
            /* USER CODE BEGIN Service1_Char_6_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */

            /* USER CODE END Service1_Char_6_ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
            FUOTA_Notification(&notification);
          } /* if(p_attribute_modified->Attr_Handle == (FUOTA_Context.ActslotsetCharHdle + CHARACTERISTIC_VALUE_ATTRIBUTE_OFFSET))*/

          /* USER CODE BEGIN EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */

          /* USER CODE END EVT_BLUE_GATT_ATTRIBUTE_MODIFIED_END */
          break;/* ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE */
        }
        case ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE :
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_READ_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_READ_PERMIT_REQ_END */
          break;/* ACI_GATT_READ_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE:
        {
          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_BEGIN */

          /* USER CODE BEGIN EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */

          /* USER CODE END EVT_BLUE_GATT_WRITE_PERMIT_REQ_END */
          break;/* ACI_GATT_WRITE_PERMIT_REQ_VSEVT_CODE */
        }
        case ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE:
        {
          aci_gatt_tx_pool_available_event_rp0 *p_tx_pool_available_event;
          p_tx_pool_available_event = (aci_gatt_tx_pool_available_event_rp0 *) p_blecore_evt->data;
          UNUSED(p_tx_pool_available_event);

          /* USER CODE BEGIN ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */

          /* USER CODE END ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE */
          break;/* ACI_GATT_TX_POOL_AVAILABLE_VSEVT_CODE*/
        }
        case ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE:
        {
          aci_att_exchange_mtu_resp_event_rp0 *p_exchange_mtu;
          p_exchange_mtu = (aci_att_exchange_mtu_resp_event_rp0 *)  p_blecore_evt->data;
          UNUSED(p_exchange_mtu);

          /* USER CODE BEGIN ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */

          /* USER CODE END ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
          break;/* ACI_ATT_EXCHANGE_MTU_RESP_VSEVT_CODE */
        }
        /* USER CODE BEGIN BLECORE_EVT */
        /* Manage ACI_GATT_INDICATION_VSEVT_CODE occurring on Android 12 */
        case ACI_GATT_INDICATION_VSEVT_CODE:
        {
          tBleStatus status = BLE_STATUS_FAILED;
          aci_gatt_indication_event_rp0 *pr = (void*)p_blecore_evt->data;
          status = aci_gatt_confirm_indication(pr->Connection_Handle);
          if (status != BLE_STATUS_SUCCESS)
          {
            LOG_INFO_APP("  Fail   : aci_gatt_confirm_indication command, result: 0x%x \n", status);
          }
          else
          {
            LOG_INFO_APP("  Success: aci_gatt_confirm_indication command\n");
          }
        }
        break; /* end ACI_GATT_NOTIFICATION_VSEVT_CODE */

        /* USER CODE END BLECORE_EVT */
        default:
          /* USER CODE BEGIN EVT_DEFAULT */

          /* USER CODE END EVT_DEFAULT */
          break;
      }
      /* USER CODE BEGIN EVT_VENDOR*/

      /* USER CODE END EVT_VENDOR*/
      break; /* HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE */

      /* USER CODE BEGIN EVENT_PCKT_CASES*/

      /* USER CODE END EVENT_PCKT_CASES*/

    default:
      /* USER CODE BEGIN EVENT_PCKT*/

      /* USER CODE END EVENT_PCKT*/
      break;
  }

  /* USER CODE BEGIN Service1_EventHandler_2 */

  /* USER CODE END Service1_EventHandler_2 */

  return(return_value);
}/* end FUOTA_EventHandler */

/* Public functions ----------------------------------------------------------*/

/**
 * @brief  Service initialization
 * @param  None
 * @retval None
 */
void FUOTA_Init(void)
{
  Char_UUID_t  uuid;
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  uint8_t max_attr_record;

  /* USER CODE BEGIN SVCCTL_InitService1Svc_1 */

  /* USER CODE END SVCCTL_InitService1Svc_1 */

  /**
   *  Register the event handler to the BLE controller
   */
  SVCCTL_RegisterSvcHandler(FUOTA_EventHandler);

  /**
   * FUOTA
   *
   * Max_Attribute_Records = 1 + 2*6 + 1*no_of_char_with_notify_or_indicate_property + 1*no_of_char_with_broadcast_property
   * service_max_attribute_record = 1 for FUOTA +
   *                                2 for CONTROL +
   *                                2 for CONFIRM +
   *                                2 for UPDATA +
   *                                2 for ACK +
   *                                2 for SLOTSINFO +
   *                                2 for ACTSLOTSET +
   *                                1 for CONFIRM configuration descriptor +
   *                                1 for ACK configuration descriptor +
   *                              = 15
   * This value doesn't take into account number of descriptors manually added
   * In case of descriptors added, please update the max_attr_record value accordingly in the next SVCCTL_InitService User Section
   */
  max_attr_record = 15;

  /* USER CODE BEGIN SVCCTL_InitService */
  /* max_attr_record to be updated if descriptors have been added */

  /* USER CODE END SVCCTL_InitService */

  COPY_FUOTA_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_service(UUID_TYPE_128,
                             (Service_UUID_t *) &uuid,
                             PRIMARY_SERVICE,
                             max_attr_record,
                             &(FUOTA_Context.FuotaSvcHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_service command: FUOTA, error code: 0x%x \n\r", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_service command: FUOTA \n\r");
  }

  /**
   * CONTROL
   */
  COPY_CONTROL_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(FUOTA_Context.FuotaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeControl,
                          CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(FUOTA_Context.ControlCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CONTROL, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CONTROL\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char1 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char1 */

  /**
   * CONFIRM
   */
  COPY_CONFIRM_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(FUOTA_Context.FuotaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeConfirm,
                          CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(FUOTA_Context.ConfirmCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : CONFIRM, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : CONFIRM\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char2 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char2 */

  /**
   * UPDATA
   */
  COPY_UPDATA_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(FUOTA_Context.FuotaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeUpdata,
                          CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(FUOTA_Context.UpdataCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : UPDATA, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : UPDATA\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char3 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char3 */

  /**
   * ACK
   */
  COPY_ACK_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(FUOTA_Context.FuotaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeAck,
                          CHAR_PROP_READ | CHAR_PROP_NOTIFY,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_DONT_NOTIFY_EVENTS,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(FUOTA_Context.AckCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : ACK, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : ACK\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char4 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char4 */

  /**
   * SLOTSINFO
   */
  COPY_SLOTSINFO_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(FUOTA_Context.FuotaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeSlotsinfo,
                          CHAR_PROP_READ,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_VARIABLE,
                          &(FUOTA_Context.SlotsinfoCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : SLOTSINFO, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : SLOTSINFO\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char5 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char5 */

  /**
   * ACTSLOTSET
   */
  COPY_ACTSLOTSET_UUID(uuid.Char_UUID_128);
  ret = aci_gatt_add_char(FUOTA_Context.FuotaSvcHdle,
                          UUID_TYPE_128,
                          (Char_UUID_t *) &uuid,
                          SizeActslotset,
                          CHAR_PROP_READ | CHAR_PROP_WRITE_WITHOUT_RESP,
                          ATTR_PERMISSION_AUTHEN_READ | ATTR_PERMISSION_AUTHEN_WRITE,
                          GATT_NOTIFY_ATTRIBUTE_WRITE,
                          0x10,
                          CHAR_VALUE_LEN_CONSTANT,
                          &(FUOTA_Context.ActslotsetCharHdle));
  if (ret != BLE_STATUS_SUCCESS)
  {
    LOG_INFO_APP("  Fail   : aci_gatt_add_char command   : ACTSLOTSET, error code: 0x%2X\n", ret);
  }
  else
  {
    LOG_INFO_APP("  Success: aci_gatt_add_char command   : ACTSLOTSET\n");
  }

  /* USER CODE BEGIN SVCCTL_InitService1Char6 */
  /* Place holder for Characteristic Descriptors */

  /* USER CODE END SVCCTL_InitService1Char6 */

  /* USER CODE BEGIN SVCCTL_InitService1Svc_2 */

  /* USER CODE END SVCCTL_InitService1Svc_2 */

  return;
}

/**
 * @brief  Characteristic update
 * @param  CharOpcode: Characteristic identifier
 * @param  Service_Instance: Instance of the service to which the characteristic belongs
 *
 */
tBleStatus FUOTA_UpdateValue(FUOTA_CharOpcode_t CharOpcode, FUOTA_Data_t *pData)
{
  tBleStatus ret = BLE_STATUS_INVALID_PARAMS;
  /* USER CODE BEGIN Service1_App_Update_Char_1 */

  /* USER CODE END Service1_App_Update_Char_1 */

  switch(CharOpcode)
  {
    case FUOTA_CONTROL:
      ret = aci_gatt_update_char_value(FUOTA_Context.FuotaSvcHdle,
                                       FUOTA_Context.ControlCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CONTROL command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CONTROL command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_1*/

      /* USER CODE END Service1_Char_Value_1*/
      break;

    case FUOTA_CONFIRM:
      ret = aci_gatt_update_char_value(FUOTA_Context.FuotaSvcHdle,
                                       FUOTA_Context.ConfirmCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value CONFIRM command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value CONFIRM command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_2*/

      /* USER CODE END Service1_Char_Value_2*/
      break;

    case FUOTA_UPDATA:
      ret = aci_gatt_update_char_value(FUOTA_Context.FuotaSvcHdle,
                                       FUOTA_Context.UpdataCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value UPDATA command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value UPDATA command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_3*/

      /* USER CODE END Service1_Char_Value_3*/
      break;

    case FUOTA_ACK:
      ret = aci_gatt_update_char_value(FUOTA_Context.FuotaSvcHdle,
                                       FUOTA_Context.AckCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value ACK command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value ACK command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_4*/

      /* USER CODE END Service1_Char_Value_4*/
      break;

    case FUOTA_SLOTSINFO:
      ret = aci_gatt_update_char_value(FUOTA_Context.FuotaSvcHdle,
                                       FUOTA_Context.SlotsinfoCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value SLOTSINFO command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value SLOTSINFO command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_5*/

      /* USER CODE END Service1_Char_Value_5*/
      break;

    case FUOTA_ACTSLOTSET:
      ret = aci_gatt_update_char_value(FUOTA_Context.FuotaSvcHdle,
                                       FUOTA_Context.ActslotsetCharHdle,
                                       0, /* charValOffset */
                                       pData->Length, /* charValueLen */
                                       (uint8_t *)pData->p_Payload);
      if (ret != BLE_STATUS_SUCCESS)
      {
        LOG_INFO_APP("  Fail   : aci_gatt_update_char_value ACTSLOTSET command, error code: 0x%2X\n", ret);
      }
      else
      {
        LOG_INFO_APP("  Success: aci_gatt_update_char_value ACTSLOTSET command\n");
      }
      /* USER CODE BEGIN Service1_Char_Value_6*/

      /* USER CODE END Service1_Char_Value_6*/
      break;

    default:
      break;
  }

  /* USER CODE BEGIN Service1_App_Update_Char_2 */

  /* USER CODE END Service1_App_Update_Char_2 */

  return ret;
}
