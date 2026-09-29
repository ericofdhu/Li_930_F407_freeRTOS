/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#define emit_led_ctrl1_Pin GPIO_PIN_0
#define emit_led_ctrl1_GPIO_Port GPIOF
#define emit_led_ctrl2_Pin GPIO_PIN_1
#define emit_led_ctrl2_GPIO_Port GPIOF
#define emit_led_ctrl3_Pin GPIO_PIN_2
#define emit_led_ctrl3_GPIO_Port GPIOF
#define emit_led_ctrl4_Pin GPIO_PIN_3
#define emit_led_ctrl4_GPIO_Port GPIOF
#define SYS_LED1_Pin GPIO_PIN_6
#define SYS_LED1_GPIO_Port GPIOF
#define SYS_LED2_Pin GPIO_PIN_7
#define SYS_LED2_GPIO_Port GPIOF
#define SPI1_CS_Pin GPIO_PIN_2
#define SPI1_CS_GPIO_Port GPIOC
#define LED_CUR_AD_IN_Pin GPIO_PIN_3
#define LED_CUR_AD_IN_GPIO_Port GPIOC
#define USART2_TX_Pin GPIO_PIN_2
#define USART2_TX_GPIO_Port GPIOA
#define USART2_RX_Pin GPIO_PIN_3
#define USART2_RX_GPIO_Port GPIOA
#define SPI1_SCK_Pin GPIO_PIN_5
#define SPI1_SCK_GPIO_Port GPIOA
#define SPI1_MISI_Pin GPIO_PIN_6
#define SPI1_MISI_GPIO_Port GPIOA
#define SPI1_MOSI_Pin GPIO_PIN_7
#define SPI1_MOSI_GPIO_Port GPIOA
#define Buzzer_Pin GPIO_PIN_12
#define Buzzer_GPIO_Port GPIOF
#define ADS_DRDY_Pin GPIO_PIN_12
#define ADS_DRDY_GPIO_Port GPIOE
#define ADS_RSR_Pin GPIO_PIN_13
#define ADS_RSR_GPIO_Port GPIOE
#define USART3_TX_Pin GPIO_PIN_10
#define USART3_TX_GPIO_Port GPIOB
#define USART3_RX_Pin GPIO_PIN_11
#define USART3_RX_GPIO_Port GPIOB
#define ADS_CS_Pin GPIO_PIN_12
#define ADS_CS_GPIO_Port GPIOB
#define ADS_SCLK_Pin GPIO_PIN_13
#define ADS_SCLK_GPIO_Port GPIOB
#define ADS_DOUT_Pin GPIO_PIN_14
#define ADS_DOUT_GPIO_Port GPIOB
#define ADS_DIN_Pin GPIO_PIN_15
#define ADS_DIN_GPIO_Port GPIOB
#define ESP32_RST_Pin GPIO_PIN_11
#define ESP32_RST_GPIO_Port GPIOD
#define IO_OUT_PRT_ONOFF_Pin GPIO_PIN_12
#define IO_OUT_PRT_ONOFF_GPIO_Port GPIOD
#define IO_IN_PRT_BUSY_Pin GPIO_PIN_13
#define IO_IN_PRT_BUSY_GPIO_Port GPIOD
#define USB_DISCONNECT_Pin GPIO_PIN_3
#define USB_DISCONNECT_GPIO_Port GPIOG
#define RELY_CONTROL_Pin GPIO_PIN_4
#define RELY_CONTROL_GPIO_Port GPIOG
#define Couple_Control_Pin GPIO_PIN_8
#define Couple_Control_GPIO_Port GPIOC
#define USART1_TX_Pin GPIO_PIN_9
#define USART1_TX_GPIO_Port GPIOA
#define USART1_RX_Pin GPIO_PIN_10
#define USART1_RX_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_10
#define UART_TX_GPIO_Port GPIOC
#define UART_RX_Pin GPIO_PIN_11
#define UART_RX_GPIO_Port GPIOC
#define TMC_DIAG0_Pin GPIO_PIN_11
#define TMC_DIAG0_GPIO_Port GPIOG
#define TMC_DIAG1_Pin GPIO_PIN_12
#define TMC_DIAG1_GPIO_Port GPIOG
#define TMC_POS_MAX_Pin GPIO_PIN_13
#define TMC_POS_MAX_GPIO_Port GPIOG
#define TMC_POS_ZERO_Pin GPIO_PIN_14
#define TMC_POS_ZERO_GPIO_Port GPIOG
#define TMC_SPI_SCK_Pin GPIO_PIN_3
#define TMC_SPI_SCK_GPIO_Port GPIOB
#define TMC_SPI_MISO_Pin GPIO_PIN_4
#define TMC_SPI_MISO_GPIO_Port GPIOB
#define TMC_SPI_MOSI_Pin GPIO_PIN_5
#define TMC_SPI_MOSI_GPIO_Port GPIOB
#define TMC_SPI_CS_Pin GPIO_PIN_6
#define TMC_SPI_CS_GPIO_Port GPIOB
#define TMC_EN_Pin GPIO_PIN_7
#define TMC_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
