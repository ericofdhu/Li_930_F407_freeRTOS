#include "dwin.h"

#include <string.h>

#include "cmsis_os2.h"
#include "main.h"
#include "./SYSTEM/usart/usart.h"

#define DWIN_RX_DMA_BUFFER_SIZE 256U
#define DWIN_RX_QUEUE_LENGTH   512U
#define DWIN_TX_TIMEOUT_MS    1000U
#define DWIN_FRAME_HEADER     0xAAU
#define DWIN_FRAME_END_0      0xCCU
#define DWIN_FRAME_END_1      0x33U
#define DWIN_FRAME_END_2      0xC3U
#define DWIN_FRAME_END_3      0x3CU
#define DWIN_FRAME_END_LENGTH 4U
#define DWIN_COMMAND_WRITE_VP 0x82U
#define DWIN_COMMAND_READ_VP  0x83U

typedef struct
{
  uint8_t state;
  uint16_t length;
  uint8_t data[255];
} DWIN_Parser;

static osMessageQueueId_t DWIN_RxQueue;
static osMutexId_t DWIN_TxMutex;
static osSemaphoreId_t DWIN_TxComplete;
static osThreadId_t DWIN_TaskHandle;
static const osThreadAttr_t DWIN_TaskAttributes = {
  .name = "DWIN",
  .stack_size = 1024U * 3U,
  .priority = (osPriority_t)osPriorityNormal
};
static uint8_t DWIN_RxDmaBuffer[DWIN_RX_DMA_BUFFER_SIZE];
static uint16_t DWIN_RxDmaPosition;
static volatile uint8_t DWIN_TxInProgress;
static volatile uint8_t DWIN_TxFailed;
static volatile uint8_t DWIN_Initialized;
static DWIN_EventCallback DWIN_Callback;

volatile DWIN_Status DWIN_StartResult = DWIN_ERROR_NOT_INITIALIZED;
volatile uint32_t DWIN_DroppedRxBytes;
volatile uint32_t DWIN_InvalidFrames;
volatile uint32_t DWIN_UartErrors;

static uint8_t DWIN_IsFrameEnd(const uint8_t *data, uint16_t length)
{
  return (uint8_t)((length >= DWIN_FRAME_END_LENGTH) &&
      (data[length - 4U] == DWIN_FRAME_END_0) &&
      (data[length - 3U] == DWIN_FRAME_END_1) &&
      (data[length - 2U] == DWIN_FRAME_END_2) &&
      (data[length - 1U] == DWIN_FRAME_END_3));
}

static void DWIN_ProcessFrame(const uint8_t *payload, uint8_t length)
{
  DWIN_Event event;
  uint16_t index;
  uint8_t count;

  if ((payload == NULL) || (length == 0U))
  {
    DWIN_InvalidFrames++;
    return;
  }

  memset(&event, 0, sizeof(event));
  event.command = payload[0];
  event.data_length = (uint8_t)(length - 1U);
  if (event.data_length > 0U)
  {
    memcpy(event.data, &payload[1], event.data_length);
  }

  if ((event.command == DWIN_COMMAND_READ_VP) && (length >= 4U))
  {
    count = payload[3];
    if (((uint16_t)count <= DWIN_MAX_VP_WORDS) &&
        (length == (uint8_t)(4U + (2U * count))))
    {
      event.has_vp_data = 1U;
      event.address = (uint16_t)(((uint16_t)payload[1] << 8) | payload[2]);
      event.word_count = count;
      for (index = 0U; index < count; index++)
      {
        event.words[index] =
            (uint16_t)(((uint16_t)payload[4U + (2U * index)] << 8) |
                       payload[5U + (2U * index)]);
      }
    }
    else
    {
      event.has_vp_data = 0U;
    }
  }

  if (DWIN_Callback != NULL)
  {
    DWIN_Callback(&event);
  }
}

static void DWIN_ParseByte(DWIN_Parser *parser, uint8_t byte)
{
  if (parser->state == 0U)
  {
    if (byte == DWIN_FRAME_HEADER)
    {
      parser->state = 1U;
      parser->length = 0U;
    }
  }
  else
  {
    if (parser->length >= sizeof(parser->data))
    {
      DWIN_InvalidFrames++;
      parser->state = (byte == DWIN_FRAME_HEADER) ? 1U : 0U;
      parser->length = 0U;
      return;
    }

    parser->data[parser->length++] = byte;
    if (DWIN_IsFrameEnd(parser->data, parser->length) != 0U)
    {
      uint16_t payload_length =
          (uint16_t)(parser->length - DWIN_FRAME_END_LENGTH);

      if (payload_length == 0U)
      {
        DWIN_InvalidFrames++;
      }
      else
      {
        DWIN_ProcessFrame(parser->data, (uint8_t)payload_length);
      }
      parser->state = 0U;
      parser->length = 0U;
    }
  }
}

static void DWIN_Task(void *argument)
{
  DWIN_Parser parser;
  uint8_t received_byte;

  (void)argument;
  memset(&parser, 0, sizeof(parser));
  for (;;)
  {
    if (osMessageQueueGet(DWIN_RxQueue, &received_byte, NULL,
                          osWaitForever) == osOK)
    {
      DWIN_ParseByte(&parser, received_byte);
    }
  }
}

DWIN_Status DWIN_SendCommand(uint8_t command, const uint8_t *data,
                             uint8_t data_length)
{
  uint8_t frame[262];
  uint16_t frame_length;
  uint16_t frame_data_length;

  if (DWIN_Initialized == 0U)
  {
    return DWIN_ERROR_NOT_INITIALIZED;
  }
  if ((data_length > 0U) && (data == NULL))
  {
    return DWIN_ERROR_ARGUMENT;
  }
  if (data_length > 254U)
  {
    return DWIN_ERROR_ARGUMENT;
  }
  if (osMutexAcquire(DWIN_TxMutex, osWaitForever) != osOK)
  {
    return DWIN_ERROR_RTOS;
  }

  frame[0] = DWIN_FRAME_HEADER;
  frame[1] = command;
  if (data_length > 0U)
  {
    memcpy(&frame[2], data, data_length);
  }
  frame_data_length = (uint16_t)(data_length + 2U);
  frame[frame_data_length++] = DWIN_FRAME_END_0;
  frame[frame_data_length++] = DWIN_FRAME_END_1;
  frame[frame_data_length++] = DWIN_FRAME_END_2;
  frame[frame_data_length++] = DWIN_FRAME_END_3;
  frame_length = frame_data_length;
  DWIN_TxFailed = 0U;
  DWIN_TxInProgress = 1U;
  if (HAL_UART_Transmit_DMA(&huart2, frame, frame_length) != HAL_OK)
  {
    DWIN_TxInProgress = 0U;
    DWIN_UartErrors++;
    (void)osMutexRelease(DWIN_TxMutex);
    return DWIN_ERROR_UART;
  }

  if (osSemaphoreAcquire(DWIN_TxComplete, DWIN_TX_TIMEOUT_MS) != osOK)
  {
    (void)HAL_UART_AbortTransmit(&huart2);
    DWIN_TxInProgress = 0U;
    (void)osSemaphoreAcquire(DWIN_TxComplete, 0U);
    DWIN_UartErrors++;
    (void)osMutexRelease(DWIN_TxMutex);
    return DWIN_ERROR_TIMEOUT;
  }

  if (DWIN_TxFailed != 0U)
  {
    (void)osMutexRelease(DWIN_TxMutex);
    return DWIN_ERROR_UART;
  }

  (void)osMutexRelease(DWIN_TxMutex);
  return DWIN_OK;
}

DWIN_Status DWIN_Init(void)
{
  if (DWIN_Initialized != 0U)
  {
    return DWIN_OK;
  }

  DWIN_StartResult = DWIN_ERROR_RTOS;
  DWIN_RxQueue = osMessageQueueNew(DWIN_RX_QUEUE_LENGTH,
                                   sizeof(uint8_t), NULL);
  DWIN_TxMutex = osMutexNew(NULL);
  DWIN_TxComplete = osSemaphoreNew(1U, 0U, NULL);
  if ((DWIN_RxQueue == NULL) || (DWIN_TxMutex == NULL) ||
      (DWIN_TxComplete == NULL))
  {
    if (DWIN_RxQueue != NULL)
    {
      (void)osMessageQueueDelete(DWIN_RxQueue);
      DWIN_RxQueue = NULL;
    }
    if (DWIN_TxMutex != NULL)
    {
      (void)osMutexDelete(DWIN_TxMutex);
      DWIN_TxMutex = NULL;
    }
    if (DWIN_TxComplete != NULL)
    {
      (void)osSemaphoreDelete(DWIN_TxComplete);
      DWIN_TxComplete = NULL;
    }
    return DWIN_StartResult;
  }

  DWIN_TaskHandle = osThreadNew(DWIN_Task, NULL, &DWIN_TaskAttributes);
  if (DWIN_TaskHandle == NULL)
  {
    (void)osMessageQueueDelete(DWIN_RxQueue);
    (void)osMutexDelete(DWIN_TxMutex);
    (void)osSemaphoreDelete(DWIN_TxComplete);
    DWIN_RxQueue = NULL;
    DWIN_TxMutex = NULL;
    DWIN_TxComplete = NULL;
    return DWIN_StartResult;
  }

  DWIN_RxDmaPosition = 0U;
  if (HAL_UARTEx_ReceiveToIdle_DMA(&huart2, DWIN_RxDmaBuffer,
                                  DWIN_RX_DMA_BUFFER_SIZE) != HAL_OK)
  {
    DWIN_StartResult = DWIN_ERROR_UART;
    (void)osThreadTerminate(DWIN_TaskHandle);
    (void)osMessageQueueDelete(DWIN_RxQueue);
    (void)osMutexDelete(DWIN_TxMutex);
    (void)osSemaphoreDelete(DWIN_TxComplete);
    DWIN_TaskHandle = NULL;
    DWIN_RxQueue = NULL;
    DWIN_TxMutex = NULL;
    DWIN_TxComplete = NULL;
    return DWIN_StartResult;
  }

  DWIN_Initialized = 1U;
  DWIN_StartResult = DWIN_OK;
  return DWIN_StartResult;
}


void DWIN_SetEventCallback(DWIN_EventCallback callback)
{
  DWIN_Callback = callback;
}



void DWIN_HandleRxEvent(uint16_t size)
{
  uint16_t position;

  if ((DWIN_Initialized == 0U) || (size > DWIN_RX_DMA_BUFFER_SIZE))
  {
    DWIN_UartErrors++;
    return;
  }

  position = (size == DWIN_RX_DMA_BUFFER_SIZE) ? 0U : size;
  while (DWIN_RxDmaPosition != position)
  {
    uint8_t byte = DWIN_RxDmaBuffer[DWIN_RxDmaPosition];
    if (osMessageQueuePut(DWIN_RxQueue, &byte, 0U, 0U) != osOK)
    {
      DWIN_DroppedRxBytes++;
    }
    DWIN_RxDmaPosition++;
    if (DWIN_RxDmaPosition == DWIN_RX_DMA_BUFFER_SIZE)
    {
      DWIN_RxDmaPosition = 0U;
    }
  }
}

void DWIN_HandleTxComplete(void)
{
  if (DWIN_TxInProgress != 0U)
  {
    DWIN_TxInProgress = 0U;
    if (osSemaphoreRelease(DWIN_TxComplete) != osOK)
    {
      DWIN_UartErrors++;
    }
  }
}

void DWIN_HandleUartError(void)
{
  DWIN_UartErrors++;
  if ((DWIN_TxInProgress != 0U) &&
      (huart2.gState == HAL_UART_STATE_READY))
  {
    DWIN_TxInProgress = 0U;
    DWIN_TxFailed = 1U;
    (void)osSemaphoreRelease(DWIN_TxComplete);
  }
  if ((DWIN_Initialized != 0U) &&
      (huart2.RxState == HAL_UART_STATE_READY) &&
      (HAL_UARTEx_ReceiveToIdle_DMA(&huart2, DWIN_RxDmaBuffer,
                                   DWIN_RX_DMA_BUFFER_SIZE) != HAL_OK))
  {
    DWIN_UartErrors++;
  }
}
