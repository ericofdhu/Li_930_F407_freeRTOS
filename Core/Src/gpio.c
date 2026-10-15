/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.c
  * @brief   This file provides code for the configuration
  *          of all used GPIO pins.
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

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
#include "dac.h"
#include "main.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOF, emit_led_ctrl1_Pin|emit_led_ctrl2_Pin|emit_led_ctrl3_Pin|emit_led_ctrl4_Pin
                          |SYS_LED1_Pin|SYS_LED2_Pin|Buzzer_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI1_CS_GPIO_Port, SPI1_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ADS_RSR_GPIO_Port, ADS_RSR_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(ADS_CS_GPIO_Port, ADS_CS_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, ESP32_RST_Pin|IO_OUT_PRT_ONOFF_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOG, USB_DISCONNECT_Pin|RELY_CONTROL_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(Couple_Control_GPIO_Port, Couple_Control_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TMC_SPI_CS_Pin|TMC_EN_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : emit_led_ctrl1_Pin emit_led_ctrl2_Pin emit_led_ctrl3_Pin emit_led_ctrl4_Pin
                           SYS_LED1_Pin SYS_LED2_Pin Buzzer_Pin */
  GPIO_InitStruct.Pin = emit_led_ctrl1_Pin|emit_led_ctrl2_Pin|emit_led_ctrl3_Pin|emit_led_ctrl4_Pin
                          |SYS_LED1_Pin|SYS_LED2_Pin|Buzzer_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI1_CS_Pin */
  GPIO_InitStruct.Pin = SPI1_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(SPI1_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ADS_DRDY_Pin */
  GPIO_InitStruct.Pin = ADS_DRDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(ADS_DRDY_GPIO_Port, &GPIO_InitStruct);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /*Configure GPIO pin : ADS_RSR_Pin */
  GPIO_InitStruct.Pin = ADS_RSR_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ADS_RSR_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : ADS_CS_Pin */
  GPIO_InitStruct.Pin = ADS_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ADS_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : ESP32_RST_Pin IO_OUT_PRT_ONOFF_Pin */
  GPIO_InitStruct.Pin = ESP32_RST_Pin|IO_OUT_PRT_ONOFF_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /*Configure GPIO pin : IO_IN_PRT_BUSY_Pin */
  GPIO_InitStruct.Pin = IO_IN_PRT_BUSY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(IO_IN_PRT_BUSY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : USB_DISCONNECT_Pin RELY_CONTROL_Pin */
  GPIO_InitStruct.Pin = USB_DISCONNECT_Pin|RELY_CONTROL_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : Couple_Control_Pin */
  GPIO_InitStruct.Pin = Couple_Control_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(Couple_Control_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : TMC_DIAG0_Pin TMC_DIAG1_Pin TMC_POS_MAX_Pin TMC_POS_ZERO_Pin */
  GPIO_InitStruct.Pin = TMC_DIAG0_Pin|TMC_DIAG1_Pin|TMC_POS_MAX_Pin|TMC_POS_ZERO_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

  /*Configure GPIO pin : TMC_SPI_CS_Pin */
  GPIO_InitStruct.Pin = TMC_SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(TMC_SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : TMC_EN_Pin */
  GPIO_InitStruct.Pin = TMC_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
  HAL_GPIO_Init(TMC_EN_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 2 */
/**
 * @brief 打开指定的激发光源。
 * @param light 光源编号：0 表示同时打开全部四路，1 至 4 分别表示对应的单路。
 * @retval 无。
 */
void BSP_LIGHT_CONTROL_On(uint8_t light)
{
    switch (light) {
        case 0:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl1_Pin | emit_led_ctrl2_Pin | emit_led_ctrl3_Pin | emit_led_ctrl4_Pin, GPIO_PIN_SET);
             break;
        case 1:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl1_Pin, GPIO_PIN_SET);
             break;
        case 2:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl2_Pin, GPIO_PIN_SET);
             break;
        case 3:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl3_Pin, GPIO_PIN_SET);
             break;
        case 4:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl4_Pin, GPIO_PIN_SET);
             break;
        default:
             break;
    }
}

/**
 * @brief 关闭指定的激发光源。
 * @param light 光源编号：0 表示同时关闭全部四路，1 至 4 分别表示对应的单路。
 * @retval 无。
 */
void BSP_LIGHT_CONTROL_Off(uint8_t light)
{
    switch (light) {
        case 0:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl1_Pin | emit_led_ctrl2_Pin | emit_led_ctrl3_Pin | emit_led_ctrl4_Pin, GPIO_PIN_RESET);
             break;
        case 1:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl1_Pin, GPIO_PIN_RESET);
             break;
        case 2:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl2_Pin, GPIO_PIN_RESET);
             break;
        case 3:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl3_Pin, GPIO_PIN_RESET);
             break;
        case 4:
             HAL_GPIO_WritePin(GPIOF, emit_led_ctrl4_Pin, GPIO_PIN_RESET);
             break;
        default:
             break;
    }
}

/**
 * @brief 选择并打开指定的激发光源。
 * @param index 光源编号：1 至 4 表示对应的单路；其他值会先关闭全部光源且不打开任何一路。
 * @retval 无。
 */
void BSP_Light_Sel(uint8_t index)
{
    BSP_LIGHT_CONTROL_Off(0);
    BSP_LIGHT_CONTROL_On(index);
}



/**
 * @brief 初始化高压控制用的 DAC 通道。
 * @details 本函数依赖 MX_DAC_Init() 已完成 DAC 外设、通道 1 和 PA4 的 HAL 配置；本函数将输出值设为零并启动通道。
 * @retval 无；HAL 操作失败时调用 Error_Handler()。
 */
void BSP_DAC_HV_INIT(void)
{
  if (HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 0U) != HAL_OK)
  {
    Error_Handler();
  }

  if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
 * @brief 根据设定值更新高压控制 DAC 输出。
 * @details 按原有标定公式将输入值换算为 12 位 DAC 码值，并将结果限制在 0 至 4095；DAC 配置为无触发模式，写入数据后自动更新输出。
 * @param hv 高压设定值，换算公式为 ((hv + 25) * 275 * 4096) / 330 / 1250。
 * @retval 无；HAL 操作失败时调用 Error_Handler()。
 */
void BSP_Set_HV(uint16_t hv)
{
    uint32_t dacValue = (hv + 25U) * 275U * 4096U;

    dacValue /= 330U;
    dacValue /= 1250U;
    if (dacValue > 4095U)
    {
        dacValue = 4095U;
    }

    if (HAL_DAC_SetValue(&hdac, DAC_CHANNEL_1, DAC_ALIGN_12B_R, (uint32_t)dacValue) != HAL_OK)
    {
        Error_Handler();
    }

    if (HAL_DAC_Start(&hdac, DAC_CHANNEL_1) != HAL_OK)
    {
        Error_Handler();
    }
}


void BSP_IR_Emit_Control(uint8_t stat)
{
	if( stat == 0 )
	{
          HAL_GPIO_WritePin(Couple_Control_GPIO_Port, Couple_Control_Pin, GPIO_PIN_RESET);
	}
	else
	{
          HAL_GPIO_WritePin(Couple_Control_GPIO_Port, Couple_Control_Pin, GPIO_PIN_SET);
	}
}



/* USER CODE END 2 */
