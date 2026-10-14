#include "usart2_process.h"
#include <stdint.h>
#include <string.h>

#include "./SYSTEM/usart/usart.h"

#define USART2_PROCESS_DMA_BUFFER_SIZE 256U
#define USART2_PROCESS_QUEUE_LENGTH     4U
#define USART2_PROCESS_FRAME_OVERHEAD   5U
#define USART2_PROCESS_END_LENGTH       4U

static const uint8_t USART2_ProcessEndMarker[USART2_PROCESS_END_LENGTH] =
{
  USART2_PROCESS_FRAME_END_0,
  USART2_PROCESS_FRAME_END_1,
  USART2_PROCESS_FRAME_END_2,
  USART2_PROCESS_FRAME_END_3
};

static uint8_t USART2_ProcessDmaBuffer[USART2_PROCESS_DMA_BUFFER_SIZE];
static uint16_t USART2_ProcessDmaPosition;
static USART2_ProcessFrame USART2_ProcessCurrentFrame;
static osMessageQueueId_t USART2_ProcessFrameQueue;
static osMutexId_t USART2_ProcessTxMutex;
static uint8_t USART2_ProcessEndCandidate[USART2_PROCESS_END_LENGTH];
static uint8_t USART2_ProcessEndCandidateLength;
static uint8_t USART2_ProcessReceivingFrame;
static uint8_t USART2_ProcessFrameOverflow;
static uint8_t USART2_ProcessInitialized;
static volatile uint8_t USART2_ProcessTxInProgress;
static uint8_t USART2_ProcessTxBuffer[
    USART2_PROCESS_MAX_FRAME_LENGTH + USART2_PROCESS_FRAME_OVERHEAD];

volatile HAL_StatusTypeDef USART2_ProcessStartResult = HAL_ERROR;
volatile uint32_t USART2_ProcessDroppedFrames;
volatile uint32_t USART2_ProcessOverflowFrames;
volatile uint32_t USART2_ProcessUartErrors;

static void USART2_ProcessAppendByte(uint8_t byte)
{
  if (USART2_ProcessCurrentFrame.length <
      USART2_PROCESS_MAX_FRAME_LENGTH)
  {
    USART2_ProcessCurrentFrame.data[USART2_ProcessCurrentFrame.length] = byte;
    USART2_ProcessCurrentFrame.length++;
  }
  else if (USART2_ProcessFrameOverflow == 0U)
  {
    USART2_ProcessFrameOverflow = 1U;
    USART2_ProcessOverflowFrames++;
  }
}

static uint8_t USART2_ProcessCandidateIsEndPrefix(void)
{
  uint8_t index;

  for (index = 0U; index < USART2_ProcessEndCandidateLength; index++)
  {
    if (USART2_ProcessEndCandidate[index] != USART2_ProcessEndMarker[index])
    {
      return 0U;
    }
  }

  return 1U;
}

static void USART2_ProcessResetFrame(void)
{
  USART2_ProcessReceivingFrame = 0U;
  USART2_ProcessFrameOverflow = 0U;
  USART2_ProcessEndCandidateLength = 0U;
  USART2_ProcessCurrentFrame.length = 0U;
}

static void USART2_ProcessFinishFrame(void)
{
  if (USART2_ProcessFrameOverflow == 0U)
  {
    if (osMessageQueuePut(USART2_ProcessFrameQueue,
                          &USART2_ProcessCurrentFrame, 0U, 0U) != osOK)
    {
      USART2_ProcessDroppedFrames++;
    }
  }

  USART2_ProcessResetFrame();
}

static void USART2_ProcessFlushCandidateByte(void)
{
  uint8_t index;

  USART2_ProcessAppendByte(USART2_ProcessEndCandidate[0]);
  for (index = 1U; index < USART2_ProcessEndCandidateLength; index++)
  {
    USART2_ProcessEndCandidate[index - 1U] =
        USART2_ProcessEndCandidate[index];
  }
  USART2_ProcessEndCandidateLength--;
}

static void USART2_ProcessByte(uint8_t byte)
{
  if (USART2_ProcessReceivingFrame == 0U)
  {
    if (byte == USART2_PROCESS_FRAME_START)
    {
      USART2_ProcessReceivingFrame = 1U;
      USART2_ProcessFrameOverflow = 0U;
      USART2_ProcessEndCandidateLength = 0U;
      USART2_ProcessCurrentFrame.length = 0U;
    }
    return;
  }

  USART2_ProcessEndCandidate[USART2_ProcessEndCandidateLength] = byte;
  USART2_ProcessEndCandidateLength++;

  while ((USART2_ProcessEndCandidateLength > 0U) &&
         (USART2_ProcessCandidateIsEndPrefix() == 0U))
  {
    USART2_ProcessFlushCandidateByte();
  }

  if (USART2_ProcessEndCandidateLength == USART2_PROCESS_END_LENGTH)
  {
    USART2_ProcessFinishFrame();
  }
}

static void USART2_ProcessDmaData(uint16_t size)
{
  uint16_t position;

  if (size > USART2_PROCESS_DMA_BUFFER_SIZE)
  {
    USART2_ProcessUartErrors++;
    return;
  }

  position = (size == USART2_PROCESS_DMA_BUFFER_SIZE) ? 0U : size;
  while (USART2_ProcessDmaPosition != position)
  {
    USART2_ProcessByte(USART2_ProcessDmaBuffer[USART2_ProcessDmaPosition]);
    USART2_ProcessDmaPosition++;
    if (USART2_ProcessDmaPosition == USART2_PROCESS_DMA_BUFFER_SIZE)
    {
      USART2_ProcessDmaPosition = 0U;
    }
  }
}

HAL_StatusTypeDef USART2_Process_Init(void)
{
  if (USART2_ProcessInitialized != 0U)
  {
    return HAL_OK;
  }

  USART2_ProcessStartResult = HAL_ERROR;
  USART2_ProcessFrameQueue = osMessageQueueNew(
      USART2_PROCESS_QUEUE_LENGTH, sizeof(USART2_ProcessFrame), NULL);
  USART2_ProcessTxMutex = osMutexNew(NULL);
  if ((USART2_ProcessFrameQueue == NULL) ||
      (USART2_ProcessTxMutex == NULL))
  {
    if (USART2_ProcessFrameQueue != NULL)
    {
      (void)osMessageQueueDelete(USART2_ProcessFrameQueue);
      USART2_ProcessFrameQueue = NULL;
    }
    if (USART2_ProcessTxMutex != NULL)
    {
      (void)osMutexDelete(USART2_ProcessTxMutex);
      USART2_ProcessTxMutex = NULL;
    }
    return USART2_ProcessStartResult;
  }

  USART2_ProcessDmaPosition = 0U;
  USART2_ProcessTxInProgress = 0U;
  USART2_ProcessResetFrame();

  MX_USART2_UART_Init();
  USART2_ProcessStartResult =
      HAL_UARTEx_ReceiveToIdle_DMA(&huart2, USART2_ProcessDmaBuffer,
                                   USART2_PROCESS_DMA_BUFFER_SIZE);
  if (USART2_ProcessStartResult != HAL_OK)
  {
    (void)osMessageQueueDelete(USART2_ProcessFrameQueue);
    (void)osMutexDelete(USART2_ProcessTxMutex);
    USART2_ProcessFrameQueue = NULL;
    USART2_ProcessTxMutex = NULL;
    return USART2_ProcessStartResult;
  }

  USART2_ProcessInitialized = 1U;
  return USART2_ProcessStartResult;
}

osStatus_t USART2_Process_GetFrame(USART2_ProcessFrame *frame,
                                   uint32_t timeout)
{
  if ((frame == NULL) || (USART2_ProcessFrameQueue == NULL))
  {
    return osErrorParameter;
  }

  return osMessageQueueGet(USART2_ProcessFrameQueue, frame, NULL, timeout);
}

HAL_StatusTypeDef USART2_Process_SendFrame(const uint8_t *data,
                                           uint16_t length)
{
  HAL_StatusTypeDef result;
  uint16_t index;

  if ((USART2_ProcessInitialized == 0U) ||
      (USART2_ProcessTxMutex == NULL) ||
      ((data == NULL) && (length != 0U)) ||
      (length > USART2_PROCESS_MAX_FRAME_LENGTH))
  {
    return HAL_ERROR;
  }

  if (osMutexAcquire(USART2_ProcessTxMutex, osWaitForever) != osOK)
  {
    return HAL_ERROR;
  }

  if (USART2_ProcessTxInProgress != 0U)
  {
    (void)osMutexRelease(USART2_ProcessTxMutex);
    return HAL_BUSY;
  }

  USART2_ProcessTxBuffer[0] = USART2_PROCESS_FRAME_START;
  if (length > 0U)
  {
    memcpy(&USART2_ProcessTxBuffer[1], data, length);
  }
  for (index = 0U; index < USART2_PROCESS_END_LENGTH; index++)
  {
    USART2_ProcessTxBuffer[1U + length + index] =
        USART2_ProcessEndMarker[index];
  }

  USART2_ProcessTxInProgress = 1U;
  result = HAL_UART_Transmit_DMA(&huart2, USART2_ProcessTxBuffer,
                                (uint16_t)(length +
                                           USART2_PROCESS_FRAME_OVERHEAD));
  if (result != HAL_OK)
  {
    USART2_ProcessTxInProgress = 0U;
  }

  (void)osMutexRelease(USART2_ProcessTxMutex);
  return result;
}

void USART2_Process_HandleRxEvent(uint16_t size)
{
  if (USART2_ProcessInitialized != 0U)
  {
    USART2_ProcessDmaData(size);
  }
}

void USART2_Process_HandleTxComplete(void)
{
  USART2_ProcessTxInProgress = 0U;
}

void USART2_Process_HandleUartError(void)
{
  USART2_ProcessUartErrors++;
  if (huart2.gState == HAL_UART_STATE_READY)
  {
    USART2_ProcessTxInProgress = 0U;
  }

  if ((USART2_ProcessInitialized != 0U) &&
      (huart2.RxState == HAL_UART_STATE_READY))
  {
    USART2_ProcessDmaPosition = 0U;
    USART2_ProcessResetFrame();
    if (HAL_UARTEx_ReceiveToIdle_DMA(
            &huart2, USART2_ProcessDmaBuffer,
            USART2_PROCESS_DMA_BUFFER_SIZE) != HAL_OK)
    {
      USART2_ProcessUartErrors++;
    }
  }
}
