/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    service1.h
  * @author  MCD Application Team
  * @brief   Header for service1.c
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FUOTA_H
#define FUOTA_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported defines ----------------------------------------------------------*/
/* USER CODE BEGIN ED */

/* USER CODE END ED */

/* Exported types ------------------------------------------------------------*/
typedef enum
{
  FUOTA_CONTROL,
  FUOTA_CONFIRM,
  FUOTA_UPDATA,
  FUOTA_ACK,
  FUOTA_SLOTSINFO,
  FUOTA_ACTSLOTSET,
  /* USER CODE BEGIN Service1_CharOpcode_t */

  /* USER CODE END Service1_CharOpcode_t */
  FUOTA_CHAROPCODE_LAST
} FUOTA_CharOpcode_t;

typedef enum
{
  FUOTA_CONTROL_WRITE_NO_RESP_EVT,
  FUOTA_CONFIRM_NOTIFY_ENABLED_EVT,
  FUOTA_CONFIRM_NOTIFY_DISABLED_EVT,
  FUOTA_UPDATA_WRITE_NO_RESP_EVT,
  FUOTA_ACK_READ_EVT,
  FUOTA_ACK_NOTIFY_ENABLED_EVT,
  FUOTA_ACK_NOTIFY_DISABLED_EVT,
  FUOTA_SLOTSINFO_READ_EVT,
  FUOTA_ACTSLOTSET_READ_EVT,
  FUOTA_ACTSLOTSET_WRITE_NO_RESP_EVT,
  /* USER CODE BEGIN Service1_OpcodeEvt_t */

  /* USER CODE END Service1_OpcodeEvt_t */
  FUOTA_BOOT_REQUEST_EVT
} FUOTA_OpcodeEvt_t;

typedef struct
{
  uint8_t *p_Payload;
  uint8_t Length;

  /* USER CODE BEGIN Service1_Data_t */

  /* USER CODE END Service1_Data_t */
} FUOTA_Data_t;

typedef struct
{
  FUOTA_OpcodeEvt_t       EvtOpcode;
  FUOTA_Data_t             DataTransfered;
  uint16_t                ConnectionHandle;
  uint16_t                AttributeHandle;
  uint8_t                 ServiceInstance;
  /* USER CODE BEGIN Service1_NotificationEvt_t */

  /* USER CODE END Service1_NotificationEvt_t */
} FUOTA_NotificationEvt_t;

/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* External variables --------------------------------------------------------*/
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/* Exported macros -----------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void FUOTA_Init(void);
void FUOTA_Notification(FUOTA_NotificationEvt_t *p_Notification);
tBleStatus FUOTA_UpdateValue(FUOTA_CharOpcode_t CharOpcode, FUOTA_Data_t *pData);
/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

#ifdef __cplusplus
}
#endif

#endif /*FUOTA_H */
