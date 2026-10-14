#ifndef USART3_PROCESS_H
#define USART3_PROCESS_H

#include <stdint.h>

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"

#define USART3_PROCESS_FRAME_START 0x7EU
#define USART3_PROCESS_FRAME_END   0x7EU
#define USART3_PROCESS_MAX_FRAME_LENGTH 256U

typedef struct
{
  /* Frame data excludes the 0x7E start and end delimiters. */
  uint16_t length;
  uint8_t data[USART3_PROCESS_MAX_FRAME_LENGTH];
} USART3_ProcessFrame;

extern volatile HAL_StatusTypeDef USART3_ProcessStartResult;
extern volatile uint32_t USART3_ProcessDroppedFrames;
extern volatile uint32_t USART3_ProcessOverflowFrames;
extern volatile uint32_t USART3_ProcessInvalidFrames;
extern volatile uint32_t USART3_ProcessUartErrors;

HAL_StatusTypeDef USART3_Process_Init(void);
osStatus_t USART3_Process_GetFrame(USART3_ProcessFrame *frame,
                                   uint32_t timeout);

#endif
