/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define CMD_PWD_543_Pin GPIO_PIN_3
#define CMD_PWD_543_GPIO_Port GPIOE
#define CMD_RST_uP543_Pin GPIO_PIN_4
#define CMD_RST_uP543_GPIO_Port GPIOE
#define CMD_MST_RESET_BKF_Pin GPIO_PIN_5
#define CMD_MST_RESET_BKF_GPIO_Port GPIOE
#define CMD_EN_ALIM_BKF_Pin GPIO_PIN_6
#define CMD_EN_ALIM_BKF_GPIO_Port GPIOE
#define ETH_INT_Pin GPIO_PIN_13
#define ETH_INT_GPIO_Port GPIOC
#define ETH_INT_EXTI_IRQn EXTI15_10_IRQn
#define FREE_PF3_BKF_Pin GPIO_PIN_14
#define FREE_PF3_BKF_GPIO_Port GPIOC
#define CMD_SEL_MUX_UART_BKF_Pin GPIO_PIN_15
#define CMD_SEL_MUX_UART_BKF_GPIO_Port GPIOC
#define CMD_DSP_ON_Pin GPIO_PIN_0
#define CMD_DSP_ON_GPIO_Port GPIOC
#define RESET_LAN_Pin GPIO_PIN_4
#define RESET_LAN_GPIO_Port GPIOA
#define BF6_AD_Pin GPIO_PIN_5
#define BF6_AD_GPIO_Port GPIOA
#define MT_Pin GPIO_PIN_6
#define MT_GPIO_Port GPIOA
#define RELE_SCAMBIO_BF_VOIP_Pin GPIO_PIN_2
#define RELE_SCAMBIO_BF_VOIP_GPIO_Port GPIOB
#define RELE_SCAMBIO_N_R_Pin GPIO_PIN_7
#define RELE_SCAMBIO_N_R_GPIO_Port GPIOE
#define OUT_REAR_2_Pin GPIO_PIN_8
#define OUT_REAR_2_GPIO_Port GPIOE
#define OUT_REAR_1_Pin GPIO_PIN_11
#define OUT_REAR_1_GPIO_Port GPIOE
#define IN_REAR_2_Pin GPIO_PIN_12
#define IN_REAR_2_GPIO_Port GPIOE
#define IN_REAR_1_Pin GPIO_PIN_13
#define IN_REAR_1_GPIO_Port GPIOE
#define OUT_LED_PCS_Pin GPIO_PIN_14
#define OUT_LED_PCS_GPIO_Port GPIOE
#define USB_FS1_ID_Pin GPIO_PIN_11
#define USB_FS1_ID_GPIO_Port GPIOD
#define USB_FS1_VBUS_Pin GPIO_PIN_13
#define USB_FS1_VBUS_GPIO_Port GPIOD
#define USB_FS1_EN_Pin GPIO_PIN_14
#define USB_FS1_EN_GPIO_Port GPIOD
#define USB_FS1_FAULT_Pin GPIO_PIN_15
#define USB_FS1_FAULT_GPIO_Port GPIOD
#define OUT_FRONT_1_Pin GPIO_PIN_7
#define OUT_FRONT_1_GPIO_Port GPIOC
#define RELE_SCAMBIO_LAN_TS_Pin GPIO_PIN_15
#define RELE_SCAMBIO_LAN_TS_GPIO_Port GPIOA
#define SDIO_DETECT_Pin GPIO_PIN_0
#define SDIO_DETECT_GPIO_Port GPIOD
#define SDIO_CMD_Pin GPIO_PIN_1
#define SDIO_CMD_GPIO_Port GPIOD
#define RELE_SCAMBIO_LAN_VOIP_Pin GPIO_PIN_2
#define RELE_SCAMBIO_LAN_VOIP_GPIO_Port GPIOD
#define UART_5_IRQ_Pin GPIO_PIN_3
#define UART_5_IRQ_GPIO_Port GPIOD
#define UART_4_IRQ_Pin GPIO_PIN_4
#define UART_4_IRQ_GPIO_Port GPIOD
#define UART_3_IRQ_Pin GPIO_PIN_5
#define UART_3_IRQ_GPIO_Port GPIOD
#define UART_2_IRQ_Pin GPIO_PIN_6
#define UART_2_IRQ_GPIO_Port GPIOD
#define UART_1_IRQ_Pin GPIO_PIN_7
#define UART_1_IRQ_GPIO_Port GPIOD
#define CMD_UART_RST_Pin GPIO_PIN_6
#define CMD_UART_RST_GPIO_Port GPIOB
#define UART_1_CS_Pin GPIO_PIN_7
#define UART_1_CS_GPIO_Port GPIOB
#define UART_2_CS_Pin GPIO_PIN_8
#define UART_2_CS_GPIO_Port GPIOB
#define UART_4_CS_Pin GPIO_PIN_9
#define UART_4_CS_GPIO_Port GPIOB
#define UART_5_CS_Pin GPIO_PIN_0
#define UART_5_CS_GPIO_Port GPIOE
#define UART_3_CS_Pin GPIO_PIN_1
#define UART_3_CS_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
