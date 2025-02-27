/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1_app.c
  * @author  MCD Application Team
  * @brief   service1_app application definition.
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
#include "main.h"
#include "app_common.h"
#include "app_ble.h"
#include "ll_sys_if.h"
#include "dbg_trace.h"
#include "ble.h"
#include "fuota_app.h"
#include "fuota.h"
#include "stm32_seq.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

typedef enum
{
  Confirm_NOTIFICATION_OFF,
  Confirm_NOTIFICATION_ON,
  Ack_NOTIFICATION_OFF,
  Ack_NOTIFICATION_ON,
  /* USER CODE BEGIN Service1_APP_SendInformation_t */

  /* USER CODE END Service1_APP_SendInformation_t */
  FUOTA_APP_SENDINFORMATION_LAST
} FUOTA_APP_SendInformation_t;

typedef struct
{
  FUOTA_APP_SendInformation_t     Confirm_Notification_Status;
  FUOTA_APP_SendInformation_t     Ack_Notification_Status;
  /* USER CODE BEGIN Service1_APP_Context_t */

  /* USER CODE END Service1_APP_Context_t */
  uint16_t              ConnectionHandle;
} FUOTA_APP_Context_t;

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Private macros ------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
static FUOTA_APP_Context_t FUOTA_APP_Context;

uint8_t a_FUOTA_UpdateCharData[247];

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static void FUOTA_Confirm_SendNotification(void);
static void FUOTA_Ack_SendNotification(void);

/* USER CODE BEGIN PFP */
void FuotaManager_Init(void);
void FuotaManager_HandleControlWrite(FUOTA_Data_t* received);
void FuotaManager_HandleUpDataWrite(FUOTA_Data_t* received);
void FuotaManager_HandleActSlotWrite(FUOTA_Data_t* received);
void FuotaManager_UpdateConfirmNotificationData(FUOTA_Data_t* notification_data);
void FuotaManager_UpdateAckNotificationData(FUOTA_Data_t* notification_data);

/* USER CODE END PFP */

/* Functions Definition ------------------------------------------------------*/
void FUOTA_Notification(FUOTA_NotificationEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_Notification_1 */

  /* USER CODE END Service1_Notification_1 */
  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_Notification_Service1_EvtOpcode */

    /* USER CODE END Service1_Notification_Service1_EvtOpcode */

    case FUOTA_CONTROL_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char1_WRITE_NO_RESP_EVT */
      FuotaManager_HandleControlWrite(&p_Notification->DataTransfered);

      /* USER CODE END Service1Char1_WRITE_NO_RESP_EVT */
      break;

    case FUOTA_CONFIRM_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_ENABLED_EVT */
      FUOTA_APP_Context.Confirm_Notification_Status = Confirm_NOTIFICATION_ON;
      /* USER CODE END Service1Char2_NOTIFY_ENABLED_EVT */
      break;

    case FUOTA_CONFIRM_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char2_NOTIFY_DISABLED_EVT */
      FUOTA_APP_Context.Confirm_Notification_Status = Confirm_NOTIFICATION_OFF;
      /* USER CODE END Service1Char2_NOTIFY_DISABLED_EVT */
      break;

    case FUOTA_UPDATA_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char3_WRITE_NO_RESP_EVT */
      FuotaManager_HandleUpDataWrite(&p_Notification->DataTransfered);

      /* USER CODE END Service1Char3_WRITE_NO_RESP_EVT */
      break;

    case FUOTA_ACK_READ_EVT:
      /* USER CODE BEGIN Service1Char4_READ_EVT */

      /* USER CODE END Service1Char4_READ_EVT */
      break;

    case FUOTA_ACK_NOTIFY_ENABLED_EVT:
      /* USER CODE BEGIN Service1Char4_NOTIFY_ENABLED_EVT */
      FUOTA_APP_Context.Ack_Notification_Status = Ack_NOTIFICATION_ON;
      /* USER CODE END Service1Char4_NOTIFY_ENABLED_EVT */
      break;

    case FUOTA_ACK_NOTIFY_DISABLED_EVT:
      /* USER CODE BEGIN Service1Char4_NOTIFY_DISABLED_EVT */
      FUOTA_APP_Context.Ack_Notification_Status = Ack_NOTIFICATION_OFF;
      /* USER CODE END Service1Char4_NOTIFY_DISABLED_EVT */
      break;

    case FUOTA_SLOTSINFO_READ_EVT:
      /* USER CODE BEGIN Service1Char5_READ_EVT */

      /* USER CODE END Service1Char5_READ_EVT */
      break;

    case FUOTA_ACTSLOTSET_READ_EVT:
      /* USER CODE BEGIN Service1Char6_READ_EVT */

      /* USER CODE END Service1Char6_READ_EVT */
      break;

    case FUOTA_ACTSLOTSET_WRITE_NO_RESP_EVT:
      /* USER CODE BEGIN Service1Char6_WRITE_NO_RESP_EVT */
      FuotaManager_HandleActSlotWrite(&p_Notification->DataTransfered);

      /* USER CODE END Service1Char6_WRITE_NO_RESP_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_Notification_default */

      /* USER CODE END Service1_Notification_default */
      break;
  }
  /* USER CODE BEGIN Service1_Notification_2 */

  /* USER CODE END Service1_Notification_2 */
  return;
}

void FUOTA_APP_EvtRx(FUOTA_APP_ConnHandleNotEvt_t *p_Notification)
{
  /* USER CODE BEGIN Service1_APP_EvtRx_1 */

  /* USER CODE END Service1_APP_EvtRx_1 */

  switch(p_Notification->EvtOpcode)
  {
    /* USER CODE BEGIN Service1_APP_EvtRx_Service1_EvtOpcode */

    /* USER CODE END Service1_APP_EvtRx_Service1_EvtOpcode */
    case FUOTA_CONN_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_CONN_HANDLE_EVT */

      /* USER CODE END Service1_APP_CONN_HANDLE_EVT */
      break;

    case FUOTA_DISCON_HANDLE_EVT :
      /* USER CODE BEGIN Service1_APP_DISCON_HANDLE_EVT */

      /* USER CODE END Service1_APP_DISCON_HANDLE_EVT */
      break;

    default:
      /* USER CODE BEGIN Service1_APP_EvtRx_default */

      /* USER CODE END Service1_APP_EvtRx_default */
      break;
  }

  /* USER CODE BEGIN Service1_APP_EvtRx_2 */

  /* USER CODE END Service1_APP_EvtRx_2 */

  return;
}

void FUOTA_APP_Init(void)
{
  UNUSED(FUOTA_APP_Context);
  FUOTA_Init();

  /* USER CODE BEGIN Service1_APP_Init */
  FuotaManager_Init();

  UTIL_SEQ_RegTask(1U << TASK_CONFIRM_NOTIF, UTIL_SEQ_RFU, FUOTA_Confirm_SendNotification);
  UTIL_SEQ_RegTask(1U << TASK_ACK_NOTIF, UTIL_SEQ_RFU, FUOTA_Ack_SendNotification);

  /* USER CODE END Service1_APP_Init */
  return;
}

/* USER CODE BEGIN FD */

/* USER CODE END FD */

/*************************************************************
 *
 * LOCAL FUNCTIONS
 *
 *************************************************************/
__USED void FUOTA_Confirm_SendNotification(void) /* Property Notification */
{
  FUOTA_APP_SendInformation_t notification_on_off = Confirm_NOTIFICATION_OFF;
  FUOTA_Data_t fuota_notification_data;

  fuota_notification_data.p_Payload = (uint8_t*)a_FUOTA_UpdateCharData;
  fuota_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char2_NS_1*/
  FuotaManager_UpdateConfirmNotificationData(&fuota_notification_data);
  notification_on_off = FUOTA_APP_Context.Confirm_Notification_Status;

  /* USER CODE END Service1Char2_NS_1*/

  if (notification_on_off != Confirm_NOTIFICATION_OFF)
  {
    FUOTA_UpdateValue(FUOTA_CONFIRM, &fuota_notification_data);
  }

  /* USER CODE BEGIN Service1Char2_NS_Last*/

  /* USER CODE END Service1Char2_NS_Last*/

  return;
}

__USED void FUOTA_Ack_SendNotification(void) /* Property Notification */
{
  FUOTA_APP_SendInformation_t notification_on_off = Ack_NOTIFICATION_OFF;
  FUOTA_Data_t fuota_notification_data;

  fuota_notification_data.p_Payload = (uint8_t*)a_FUOTA_UpdateCharData;
  fuota_notification_data.Length = 0;

  /* USER CODE BEGIN Service1Char4_NS_1*/
  FuotaManager_UpdateAckNotificationData(&fuota_notification_data);
  notification_on_off = FUOTA_APP_Context.Ack_Notification_Status;

  /* USER CODE END Service1Char4_NS_1*/

  if (notification_on_off != Ack_NOTIFICATION_OFF)
  {
    FUOTA_UpdateValue(FUOTA_ACK, &fuota_notification_data);
  }

  /* USER CODE BEGIN Service1Char4_NS_Last*/

  /* USER CODE END Service1Char4_NS_Last*/

  return;
}

/* USER CODE BEGIN FD_LOCAL_FUNCTIONS*/

/**
  * @brief  Initialize FUOTA service logic
  */
__WEAK void FuotaManager_Init(void)
{
}

/**
  * @brief  Handler function called when a write without response occurs on Control characteristic
  * @param  received: Pointer to FUOTA_Data_t struct containing the payload and its length
  */
__WEAK void FuotaManager_HandleControlWrite(FUOTA_Data_t* received)
{
}

/**
  * @brief  Handler function called when a write without response occurs on UpdateData characteristic
  * @param  received: Pointer to FUOTA_Data_t struct containing the payload and its length
  */
__WEAK void FuotaManager_HandleUpDataWrite(FUOTA_Data_t* received)
{
}

/**
  * @brief  Handler function called when a write without response occurs on ActiveSlotSet characteristic
  * @param  received: Pointer to FUOTA_Data_t struct containing the payload and its length
  */
__WEAK void FuotaManager_HandleActSlotWrite(FUOTA_Data_t* received)
{
}

/**
  * @brief  Function called before sending a Confirm characteristic notification
  * @param  notification_data: Pointer to FUOTA_Data_t struct to fill with data to be sent
  */
__WEAK void FuotaManager_UpdateConfirmNotificationData(FUOTA_Data_t* notification_data)
{
}

/**
  * @brief  Function called before sending a Ack characteristic notification
  * @param  notification_data: Pointer to FUOTA_Data_t struct to fill with data to be sent
  */
__WEAK void FuotaManager_UpdateAckNotificationData(FUOTA_Data_t* notification_data)
{
}

/* USER CODE END FD_LOCAL_FUNCTIONS*/
