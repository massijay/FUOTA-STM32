/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32wbaxx_hal.h"
#include "app_conf.h"
#include "app_entry.h"
#include "app_common.h"
#include "app_debug.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "addresses.h"
#include "slot_info.h"

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef __PACKED_STRUCT
{
  uint32_t version_code;
  char version_string[SLOTINFO_VERSION_STRING_LENGTH];
  uint8_t slot_n;
  char slot_id;
} FW_Metadata_t;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

extern const FW_Metadata_t FW_Metadata;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);
void MX_GPIO_Init(void);
void MX_GPDMA1_Init(void);
void MX_RAMCFG_Init(void);
void MX_RTC_Init(void);
void MX_USART1_UART_Init(void);
void MX_ADC4_Init(void);
void MX_CRC_Init(void);
void MX_RNG_Init(void);
void MX_ICACHE_Init(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CFG_SLOTS_INFO_CHAR_MAX_LENGTH 240
#define CFG_UPDATE_DATA_CHAR_MAX_LENGTH 247
#define CFG_CONTROL_CHAR_MAX_LENGTH 64
#define UPDATE_DATA_CHUNK_LENGTH 240
#define LD2_GREEN_Pin GPIO_PIN_11
#define LD2_GREEN_GPIO_Port GPIOB
#define LD3_RED_Pin GPIO_PIN_8
#define LD3_RED_GPIO_Port GPIOB
#define B3_Pin GPIO_PIN_7
#define B3_GPIO_Port GPIOB
#define B3_EXTI_IRQn EXTI7_IRQn
#define B2_Pin GPIO_PIN_6
#define B2_GPIO_Port GPIOB
#define B2_EXTI_IRQn EXTI6_IRQn
#define LD1_BLUE_Pin GPIO_PIN_4
#define LD1_BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

#define FW_VERSION_CODE 1
#define FW_VERSION_STRING "0.1.0"

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
