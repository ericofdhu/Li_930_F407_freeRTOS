#ifndef USART2_PROCESS_H
#define USART2_PROCESS_H

#include <stdint.h>

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define USART2_PROCESS_FRAME_START       0xAAU
#define USART2_PROCESS_FRAME_END_0       0xCCU
#define USART2_PROCESS_FRAME_END_1       0x33U
#define USART2_PROCESS_FRAME_END_2       0xC3U
#define USART2_PROCESS_FRAME_END_3       0x3CU
#define USART2_PROCESS_MAX_FRAME_LENGTH  256U

typedef struct
{
  /* Payload only; start and end markers are not included. */
  uint16_t length;
  uint8_t data[USART2_PROCESS_MAX_FRAME_LENGTH];
} USART2_ProcessFrame;

extern volatile HAL_StatusTypeDef USART2_ProcessStartResult;
extern volatile uint32_t USART2_ProcessDroppedFrames;
extern volatile uint32_t USART2_ProcessOverflowFrames;
extern volatile uint32_t USART2_ProcessUartErrors;

HAL_StatusTypeDef USART2_Process_Init(void);
osStatus_t USART2_Process_GetFrame(USART2_ProcessFrame *frame,
                                   uint32_t timeout);
HAL_StatusTypeDef USART2_Process_SendFrame(const uint8_t *data,
                                           uint16_t length);

void USART2_Process_HandleRxEvent(uint16_t size);
void USART2_Process_HandleTxComplete(void);
void USART2_Process_HandleUartError(void);

#endif
