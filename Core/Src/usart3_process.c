#include "usart3_process.h"

#include <string.h>

#include "./SYSTEM/usart/usart.h"
#include "usart2_process.h"

#define USART3_PROCESS_DMA_BUFFER_SIZE 256U
#define USART3_PROCESS_QUEUE_LENGTH     4U

/* DMA writes into this circular buffer; process only bytes not seen before. */
static uint8_t USART3_ProcessDmaBuffer[USART3_PROCESS_DMA_BUFFER_SIZE];
static uint16_t USART3_ProcessDmaPosition;
/* The frame buffer is published to the queue only after a complete frame. */
static USART3_ProcessFrame USART3_ProcessCurrentFrame;
static osMessageQueueId_t USART3_ProcessFrameQueue;
static uint8_t USART3_ProcessReceivingFrame;
static uint8_t USART3_ProcessFrameOverflow;
/* Kept between DMA callbacks so an escape sequence may span callbacks. */
static uint8_t USART3_ProcessEscapePending;
static uint8_t USART3_ProcessInitialized;

volatile HAL_StatusTypeDef USART3_ProcessStartResult = HAL_ERROR;
volatile uint32_t USART3_ProcessDroppedFrames;
volatile uint32_t USART3_ProcessOverflowFrames;
volatile uint32_t USART3_ProcessInvalidFrames;
volatile uint32_t USART3_ProcessUartErrors;

static void USART3_ProcessAppendByte(uint8_t byte)
{
  /* Discard the rest of an oversized frame, but count the overflow once. */
  if (USART3_ProcessCurrentFrame.length <
      USART3_PROCESS_MAX_FRAME_LENGTH)
  {
    USART3_ProcessCurrentFrame.data[USART3_ProcessCurrentFrame.length] = byte;
    USART3_ProcessCurrentFrame.length++;
  }
  else if (USART3_ProcessFrameOverflow == 0U)
  {
    USART3_ProcessFrameOverflow = 1U;
    USART3_ProcessOverflowFrames++;
  }
}

static void USART3_ProcessStartFrame(void)
{
  /* A delimiter also synchronizes the parser after noise or a bad frame. */
  USART3_ProcessReceivingFrame = 1U;
  USART3_ProcessFrameOverflow = 0U;
  USART3_ProcessEscapePending = 0U;
  USART3_ProcessCurrentFrame.length = 0U;
}

static void USART3_ProcessByte(uint8_t byte)
{
  if (USART3_ProcessEscapePending != 0U)
  {
    USART3_ProcessEscapePending = 0U;
    /* Decode the protocol's two permitted escape sequences. */
    if (byte == 0x5EU)
    {
      USART3_ProcessAppendByte(USART3_PROCESS_FRAME_START);
    }
    else if (byte == 0x5DU)
    {
      USART3_ProcessAppendByte(0x7DU);
    }
    else
    {
      /* Reject malformed escapes and wait for the next frame delimiter. */
      USART3_ProcessInvalidFrames++;
      USART3_ProcessReceivingFrame = 0U;
      USART3_ProcessFrameOverflow = 0U;
      USART3_ProcessCurrentFrame.length = 0U;
      if (byte == USART3_PROCESS_FRAME_START)
      {
        USART3_ProcessStartFrame();
      }
    }
    return;
  }

  if (USART3_ProcessReceivingFrame != 0U)
  {
    if (byte == 0x7DU)
    {
      USART3_ProcessEscapePending = 1U;
      return;
    }

    if ((byte == USART3_PROCESS_FRAME_START) ||
        (byte == USART3_PROCESS_FRAME_END))
    {
      /* 0x7E ends the current frame and starts synchronization for the next. */
      if ((USART3_ProcessFrameOverflow == 0U) &&
          (USART3_ProcessCurrentFrame.length > 0U))
      {
        if (osMessageQueuePut(USART3_ProcessFrameQueue,
                              &USART3_ProcessCurrentFrame, 0U, 0U) != osOK)
        {
          USART3_ProcessDroppedFrames++;
        }
      }

      USART3_ProcessStartFrame();
      return;
    }
  }
  else
  {
    if ((byte == USART3_PROCESS_FRAME_START) ||
        (byte == USART3_PROCESS_FRAME_END))
    {
      USART3_ProcessStartFrame();
    }
    return;
  }

  USART3_ProcessAppendByte(byte);
}

static void USART3_ProcessDmaData(uint16_t size)
{
  uint16_t position;

  if (size > USART3_PROCESS_DMA_BUFFER_SIZE)
  {
    USART3_ProcessUartErrors++;
    return;
  }

  /* Receive-to-idle reports the current DMA write offset; wrap at buffer end. */
  position = (size == USART3_PROCESS_DMA_BUFFER_SIZE) ? 0U : size;
  while (USART3_ProcessDmaPosition != position)
  {
    USART3_ProcessByte(
        USART3_ProcessDmaBuffer[USART3_ProcessDmaPosition]);
    USART3_ProcessDmaPosition++;
    if (USART3_ProcessDmaPosition == USART3_PROCESS_DMA_BUFFER_SIZE)
    {
      USART3_ProcessDmaPosition = 0U;
    }
  }
}

HAL_StatusTypeDef USART3_Process_Init(void)
{
  if (USART3_ProcessInitialized != 0U)
  {
    return HAL_OK;
  }

  USART3_ProcessStartResult = HAL_ERROR;
  /* Complete frames are copied to a queue so consumers run outside the ISR. */
  USART3_ProcessFrameQueue = osMessageQueueNew(
      USART3_PROCESS_QUEUE_LENGTH, sizeof(USART3_ProcessFrame), NULL);
  if (USART3_ProcessFrameQueue == NULL)
  {
    return USART3_ProcessStartResult;
  }

  memset(&USART3_ProcessCurrentFrame, 0,
         sizeof(USART3_ProcessCurrentFrame));
  USART3_ProcessDmaPosition = 0U;
  USART3_ProcessReceivingFrame = 0U;
  USART3_ProcessFrameOverflow = 0U;
  USART3_ProcessEscapePending = 0U;

  MX_USART3_UART_Init();
  /* Circular DMA continues receiving; IDLE/HT/TC events feed the parser. */
  USART3_ProcessStartResult =
      HAL_UARTEx_ReceiveToIdle_DMA(&huart3, USART3_ProcessDmaBuffer,
                                   USART3_PROCESS_DMA_BUFFER_SIZE);
  if (USART3_ProcessStartResult != HAL_OK)
  {
    (void)osMessageQueueDelete(USART3_ProcessFrameQueue);
    USART3_ProcessFrameQueue = NULL;
    return USART3_ProcessStartResult;
  }

  USART3_ProcessInitialized = 1U;
  return USART3_ProcessStartResult;
}

osStatus_t USART3_Process_GetFrame(USART3_ProcessFrame *frame,
                                   uint32_t timeout)
{
  if ((frame == NULL) || (USART3_ProcessFrameQueue == NULL))
  {
    return osErrorParameter;
  }

  /* Call from task context; timeout uses CMSIS-RTOS ticks/milliseconds. */
  return osMessageQueueGet(USART3_ProcessFrameQueue, frame, NULL, timeout);
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
  if ((huart != NULL) && (huart->Instance == USART3))
  {
    if (USART3_ProcessInitialized != 0U)
    {
      USART3_ProcessDmaData(size);
    }
  }
  else if ((huart != NULL) && (huart->Instance == USART2))
  {
    USART2_Process_HandleRxEvent(size);
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if ((huart != NULL) && (huart->Instance == USART2))
  {
    USART2_Process_HandleTxComplete();
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if ((huart != NULL) && (huart->Instance == USART3))
  {
    USART3_ProcessUartErrors++;
    if ((USART3_ProcessInitialized != 0U) &&
        (huart->RxState == HAL_UART_STATE_READY))
    {
      /* Drop any partial frame, then restart reception after a UART error. */
      USART3_ProcessDmaPosition = 0U;
      USART3_ProcessReceivingFrame = 0U;
      USART3_ProcessFrameOverflow = 0U;
      USART3_ProcessEscapePending = 0U;
      USART3_ProcessCurrentFrame.length = 0U;
      if (HAL_UARTEx_ReceiveToIdle_DMA(
              &huart3, USART3_ProcessDmaBuffer,
              USART3_PROCESS_DMA_BUFFER_SIZE) != HAL_OK)
      {
        USART3_ProcessUartErrors++;
      }
    }
  }
  else if ((huart != NULL) && (huart->Instance == USART2))
  {
    USART2_Process_HandleUartError();
  }
}
