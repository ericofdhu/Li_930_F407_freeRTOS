#include "debug_cli.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os2.h"
#include "main.h"
#include "usart.h"
#include "system_runtime.h"
#include "ads1256.h"
#include "tmc5130a.h"
#include "fatfs.h"
#include "dwin.h"
#include "ui_manager.h"

#define DEBUG_CLI_RX_QUEUE_LENGTH 128U
#define DEBUG_CLI_DMA_BUFFER_SIZE 128U
#define DEBUG_CLI_LINE_LENGTH      96U
#define DEBUG_CLI_TX_LENGTH       192U

struct _m_usmart_nametab
{
    void *func;
    const char *name;
};

static uint8_t DebugCLI_ParseTicks(const char *text, uint32_t *ticks);
static void DebugCLI_ShowStatus(void);
static void DebugCLI_CallHome(void);

static void DebugCLI_ExecStatus(const char *argument)
{
  if (argument != NULL)
  {
    DebugCLI_Write("\r\nERR: status takes no argument\r\n");
    return;
  }
  DebugCLI_ShowStatus();
}

static void DebugCLI_ExecBeep(const char *argument)
{
  uint32_t ticks;

  if ((argument == NULL) || (DebugCLI_ParseTicks(argument, &ticks) == 0U))
  {
    DebugCLI_Write("\r\nERR: expected beep ticks in range 0..60000\r\n");
    return;
  }
  SysRun_SetBeep(ticks);
  DebugCLI_Printf("\r\nBeep started for %lu ticks\r\n",
                  (unsigned long)ticks);
}

static void DebugCLI_ExecHome(const char *argument)
{
  if (argument != NULL)
  {
    DebugCLI_Write("\r\nERR: home takes no argument\r\n");
    return;
  }
  DebugCLI_CallHome();
}

static const struct _m_usmart_nametab usmart_nametab[] =
{
    { (void *)DebugCLI_ExecStatus, "status" },
    { (void *)DebugCLI_ExecBeep, "beep" },
    { (void *)DebugCLI_ExecHome, "home" },
};

static osMessageQueueId_t DebugCLI_RxQueue;
static osSemaphoreId_t DebugCLI_TxComplete;
static osMutexId_t DebugCLI_TxMutex;
static osThreadId_t DebugCLI_TaskHandle;
static const osThreadAttr_t DebugCLI_TaskAttributes = {
  .name = "DebugCLI",
  .stack_size = 1024U * 4U,
  .priority = (osPriority_t)osPriorityNormal
};
static uint8_t DebugCLI_DmaBuffer[DEBUG_CLI_DMA_BUFFER_SIZE];
static uint8_t DebugCLI_TxBuffer[DEBUG_CLI_TX_LENGTH];
static uint16_t DebugCLI_DmaPosition;
static volatile uint8_t DebugCLI_TxInProgress;

volatile uint32_t DebugCLI_DroppedRxBytes;
volatile uint32_t DebugCLI_UartErrors;

static void DebugCLI_Transmit(const uint8_t *data, uint16_t length)
{
  uint16_t chunkLength;

  if (osMutexAcquire(DebugCLI_TxMutex, osWaitForever) != osOK)
  {
    DebugCLI_UartErrors++;
    return;
  }

  while (length > 0U)
  {
    chunkLength = (length > DEBUG_CLI_TX_LENGTH) ?
                  DEBUG_CLI_TX_LENGTH : length;
    memcpy(DebugCLI_TxBuffer, data, chunkLength);

    DebugCLI_TxInProgress = 1U;
    if (HAL_UART_Transmit_DMA(&huart1, DebugCLI_TxBuffer,
                              chunkLength) != HAL_OK)
    {
      DebugCLI_TxInProgress = 0U;
      DebugCLI_UartErrors++;
      (void)osMutexRelease(DebugCLI_TxMutex);
      return;
    }

    if (osSemaphoreAcquire(DebugCLI_TxComplete, osWaitForever) != osOK)
    {
      DebugCLI_UartErrors++;
      (void)HAL_UART_AbortTransmit(&huart1);
      DebugCLI_TxInProgress = 0U;
      (void)osMutexRelease(DebugCLI_TxMutex);
      return;
    }

    data += chunkLength;
    length = (uint16_t)(length - chunkLength);
  }

  if (osMutexRelease(DebugCLI_TxMutex) != osOK)
  {
    DebugCLI_UartErrors++;
  }
}

void DebugCLI_Write(const char *text)
{
  DebugCLI_Transmit((const uint8_t *)text, (uint16_t)strlen(text));
}

void DebugCLI_Printf(const char *format, ...)
{
  char buffer[DEBUG_CLI_TX_LENGTH];
  va_list args;
  int length;

  va_start(args, format);
  length = vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if (length < 0)
  {
    DebugCLI_Write("\r\nERR: format\r\n");
    return;
  }
  if ((size_t)length >= sizeof(buffer))
  {
    length = (int)(sizeof(buffer) - 1U);
  }
  DebugCLI_Transmit((const uint8_t *)buffer, (uint16_t)length);
}

static void DebugCLI_ShowHelp(void)
{
  DebugCLI_Write(
      "\r\nCommands:\r\n"
      "  help                 Show this help\r\n"
      "  status               Show system status\r\n"
      "  beep <ticks>         Beep for ticks x 100 ms (0..60000)\r\n"
      "  create <filename>    Create an empty file in the USER volume\r\n"
      "  read <filename>      Display a file from the USER volume\r\n"
      "  write <filename> <text>  Create or overwrite a text file\r\n"
      "  call status          Call the whitelisted status function\r\n"
      "  call beep <ticks>    Call the whitelisted beep function\r\n"
      "  call home            Reinitialize TMC5130A and seek home\r\n"
      "  clear                Clear the terminal\r\n"
      "Only listed functions can be called.\r\n");
}

static char *DebugCLI_SkipSpaces(char *text)
{
  while ((*text == ' ') || (*text == '\t'))
  {
    text++;
  }
  return text;
}

static void DebugCLI_FileError(const char *operation, FRESULT result)
{
  DebugCLI_Printf("\r\nERR: %s failed (FatFs=%u)\r\n", operation,
                  (unsigned int)result);
}

static uint8_t DebugCLI_BuildFilePath(const char *fileName, char *filePath,
                                     size_t filePathSize)
{
  const char *character;
  int pathLength;

  if ((fileName == NULL) || (fileName[0] == '\0') ||
      (strcmp(fileName, ".") == 0) || (strcmp(fileName, "..") == 0) ||
      (filePath == NULL) || (filePathSize == 0U) ||
      (USERFatFSResult != FR_OK))
  {
    return 0U;
  }

  for (character = fileName; *character != '\0'; character++)
  {
    if ((*character == '/') || (*character == '\\') || (*character == ':'))
    {
      return 0U;
    }
  }

  pathLength = snprintf(filePath, filePathSize, "%s%s", USERPath, fileName);
  if ((pathLength < 0) || ((size_t)pathLength >= filePathSize))
  {
    return 0U;
  }

  return 1U;
}

static void DebugCLI_CreateFile(const char *fileName)
{
  char filePath[sizeof(USERPath) + DEBUG_CLI_LINE_LENGTH];
  FIL file;
  FRESULT result;
  FRESULT closeResult;

  if (DebugCLI_BuildFilePath(fileName, filePath, sizeof(filePath)) == 0U)
  {
    DebugCLI_Write("\r\nERR: invalid filename or filesystem is not mounted\r\n");
    return;
  }

  result = f_open(&file, filePath, FA_CREATE_NEW | FA_WRITE);
  if (result != FR_OK)
  {
    DebugCLI_FileError("create", result);
    return;
  }

  closeResult = f_close(&file);
  if (closeResult != FR_OK)
  {
    DebugCLI_FileError("close", closeResult);
    return;
  }

  DebugCLI_Printf("\r\nCreated %s\r\n", fileName);
}

static void DebugCLI_ReadFile(const char *fileName)
{
  char filePath[sizeof(USERPath) + DEBUG_CLI_LINE_LENGTH];
  uint8_t buffer[DEBUG_CLI_TX_LENGTH];
  FIL file;
  UINT bytesRead;
  FRESULT result;
  FRESULT closeResult;

  if (DebugCLI_BuildFilePath(fileName, filePath, sizeof(filePath)) == 0U)
  {
    DebugCLI_Write("\r\nERR: invalid filename or filesystem is not mounted\r\n");
    return;
  }

  result = f_open(&file, filePath, FA_READ);
  if (result != FR_OK)
  {
    DebugCLI_FileError("read/open", result);
    return;
  }

  DebugCLI_Printf("\r\n--- %s ---\r\n", fileName);
  do
  {
    result = f_read(&file, buffer, (UINT)sizeof(buffer), &bytesRead);
    if (result != FR_OK)
    {
      DebugCLI_FileError("read", result);
      break;
    }
    if (bytesRead > 0U)
    {
      DebugCLI_Transmit(buffer, (uint16_t)bytesRead);
    }
  } while ((result == FR_OK) && (bytesRead > 0U));

  closeResult = f_close(&file);
  if (closeResult != FR_OK)
  {
    DebugCLI_FileError("close", closeResult);
  }
  DebugCLI_Write("\r\n--------------------\r\n");
}

static void DebugCLI_WriteFile(const char *fileName, const char *content)
{
  char filePath[sizeof(USERPath) + DEBUG_CLI_LINE_LENGTH];
  FIL file;
  size_t contentLength;
  UINT bytesWritten;
  FRESULT result;
  FRESULT closeResult;

  if (DebugCLI_BuildFilePath(fileName, filePath, sizeof(filePath)) == 0U)
  {
    DebugCLI_Write("\r\nERR: invalid filename or filesystem is not mounted\r\n");
    return;
  }
  if ((content == NULL) || (content[0] == '\0'))
  {
    DebugCLI_Write("\r\nERR: write requires non-empty text\r\n");
    return;
  }

  contentLength = strlen(content);
  result = f_open(&file, filePath, FA_CREATE_ALWAYS | FA_WRITE);
  if (result != FR_OK)
  {
    DebugCLI_FileError("write/open", result);
    return;
  }

  result = f_write(&file, content, (UINT)contentLength, &bytesWritten);
  if ((result != FR_OK) || (bytesWritten != (UINT)contentLength))
  {
    DebugCLI_FileError("write", (result == FR_OK) ? FR_DISK_ERR : result);
  }
  else
  {
    DebugCLI_Printf("\r\nWrote %u bytes to %s\r\n",
                    (unsigned int)bytesWritten, fileName);
  }

  closeResult = f_close(&file);
  if (closeResult != FR_OK)
  {
    DebugCLI_FileError("close", closeResult);
  }
}

static void DebugCLI_ProcessFileCommand(const char *command,
                                        char *arguments)
{
  char *fileName;
  char *separator;
  char *content;

  arguments = DebugCLI_SkipSpaces(arguments);
  if (arguments[0] == '\0')
  {
    DebugCLI_Write("\r\nERR: filename is required\r\n");
    return;
  }

  fileName = arguments;
  separator = fileName;
  while ((*separator != '\0') &&
         (*separator != ' ') && (*separator != '\t'))
  {
    separator++;
  }

  if (*separator != '\0')
  {
    *separator++ = '\0';
  }
  content = DebugCLI_SkipSpaces(separator);

  if (strcmp(command, "create") == 0)
  {
    if (content[0] != '\0')
    {
      DebugCLI_Write("\r\nERR: usage: create <filename>\r\n");
      return;
    }
    DebugCLI_CreateFile(fileName);
  }
  else if (strcmp(command, "read") == 0)
  {
    if (content[0] != '\0')
    {
      DebugCLI_Write("\r\nERR: usage: read <filename>\r\n");
      return;
    }
    DebugCLI_ReadFile(fileName);
  }
  else if (strcmp(command, "write") == 0)
  {
    if (content[0] == '\0')
    {
      DebugCLI_Write("\r\nERR: usage: write <filename> <text>\r\n");
      return;
    }
    DebugCLI_WriteFile(fileName, content);
  }
}

static void DebugCLI_ShowStatus(void)
{
  uint8_t beepEnabled;
  uint32_t beepTicks;

  SysRun_GetBeep(&beepEnabled, &beepTicks);
  DebugCLI_Printf(
      "\r\nADS1256: status=%u raw=%ld\r\n"
      "TMC5130A homing=%u\r\n"
      "FatFs mount=%u\r\n"
      "DWIN: init=%u dropped_rx=%lu invalid_frames=%lu uart_errors=%lu\r\n"
      "UI: init=%u page=%u calibration=%u dropped_keys=%lu update_errors=%lu\r\n"
      "Beep: enabled=%u ticks=%lu\r\n"
      "USART1: dropped_rx=%lu errors=%lu\r\n",
      (unsigned int)ADS1256_LastStatus,
      (long)ADS1256_LastRawSample,
      (unsigned int)TMC5130A_HomingResult,
      (unsigned int)USERFatFSResult,
      (unsigned int)DWIN_StartResult,
      (unsigned long)DWIN_DroppedRxBytes,
      (unsigned long)DWIN_InvalidFrames,
      (unsigned long)DWIN_UartErrors,
      (unsigned int)UI_ManagerStartResult,
      (unsigned int)UI_CurrentPage,
      (unsigned int)UI_CalibrationResult,
      (unsigned long)UI_DroppedKeyEvents,
      (unsigned long)UI_UpdateErrors,
      (unsigned int)beepEnabled,
      (unsigned long)beepTicks,
      (unsigned long)DebugCLI_DroppedRxBytes,
      (unsigned long)DebugCLI_UartErrors);
}

static uint8_t DebugCLI_ParseTicks(const char *text, uint32_t *ticks)
{
  char *end;
  unsigned long value;

  if ((text == NULL) || (text[0] == '\0') || (ticks == NULL))
  {
    return 0U;
  }
  value = strtoul(text, &end, 10);
  if ((*end != '\0') || (end == text) || (value > 60000UL))
  {
    return 0U;
  }
  *ticks = (uint32_t)value;
  return 1U;
}

static void DebugCLI_CallHome(void)
{
  TMC5130A_HomeResult result;

  DebugCLI_Write("\r\nReinitializing TMC5130A and seeking home...\r\n");
  if (TMC5130A_Init() == 0U)
  {
    TMC5130A_HomingResult = TMC5130A_HOME_INIT_ERROR;
  }
  else
  {
    result = TMC5130A_FindHome();
    TMC5130A_HomingResult = result;
  }
  DebugCLI_Printf("Home result: %u\r\n", (unsigned int)TMC5130A_HomingResult);
}

static void DebugCLI_Call(const char *name, const char *argument)
{
  size_t index;
  void (*function)(const char *argument);

  if (name == NULL)
  {
    DebugCLI_Write("\r\nERR: call requires an allowed function name\r\n");
    return;
  }

  for (index = 0U; index < (sizeof(usmart_nametab) / sizeof(usmart_nametab[0])); index++)
  {
    if (strcmp(usmart_nametab[index].name, name) == 0)
    {
      function = (void (*)(const char *argument))usmart_nametab[index].func;
      if (function != NULL)
      {
        function(argument);
      }
      return;
    }
  }

  DebugCLI_Write("\r\nERR: function is not in the call whitelist; use help\r\n");
}

static void DebugCLI_ProcessLine(char *line)
{
  char *command;
  char *commandEnd;
  char *commandArguments;
  char *firstArgument;
  char *secondArgument;
  char *extraArgument;
  uint32_t ticks;

  command = DebugCLI_SkipSpaces(line);
  commandEnd = command;
  while ((*commandEnd != '\0') &&
         (*commandEnd != ' ') && (*commandEnd != '\t'))
  {
    commandEnd++;
  }

  if (*commandEnd != '\0')
  {
    char delimiter = *commandEnd;

    *commandEnd = '\0';
    if ((strcmp(command, "create") == 0) ||
        (strcmp(command, "read") == 0) ||
        (strcmp(command, "write") == 0))
    {
      commandArguments = commandEnd + 1;
      DebugCLI_ProcessFileCommand(command, commandArguments);
      return;
    }
    *commandEnd = delimiter;
  }
  else if ((strcmp(command, "create") == 0) ||
           (strcmp(command, "read") == 0) ||
           (strcmp(command, "write") == 0))
  {
    DebugCLI_ProcessFileCommand(command, commandEnd);
    return;
  }

  command = strtok(line, " \t");
  if (command == NULL)
  {
    return;
  }

  if (strcmp(command, "help") == 0)
  {
    if (strtok(NULL, " \t") != NULL)
    {
      DebugCLI_Write("\r\nERR: help takes no argument\r\n");
      return;
    }
    DebugCLI_ShowHelp();
  }
  else if (strcmp(command, "status") == 0)
  {
    if (strtok(NULL, " \t") != NULL)
    {
      DebugCLI_Write("\r\nERR: status takes no argument\r\n");
      return;
    }
    DebugCLI_ShowStatus();
  }
  else if (strcmp(command, "beep") == 0)
  {
    firstArgument = strtok(NULL, " \t");
    extraArgument = strtok(NULL, " \t");
    if ((extraArgument != NULL) ||
        (DebugCLI_ParseTicks(firstArgument, &ticks) == 0U))
    {
      DebugCLI_Write("\r\nERR: usage: beep <ticks>, range 0..60000\r\n");
      return;
    }
    SysRun_SetBeep(ticks);
    DebugCLI_Printf("\r\nBeep started for %lu ticks\r\n",
                    (unsigned long)ticks);
  }
  else if (strcmp(command, "call") == 0)
  {
    firstArgument = strtok(NULL, " \t");
    secondArgument = strtok(NULL, " \t");
    extraArgument = strtok(NULL, " \t");
    if (extraArgument != NULL)
    {
      DebugCLI_Write("\r\nERR: too many arguments\r\n");
      return;
    }
    DebugCLI_Call(firstArgument, secondArgument);
  }
  else if (strcmp(command, "clear") == 0)
  {
    if (strtok(NULL, " \t") != NULL)
    {
      DebugCLI_Write("\r\nERR: clear takes no argument\r\n");
      return;
    }
    DebugCLI_Write("\033[2J\033[H");
  }
  else
  {
    DebugCLI_Write("\r\nERR: unknown command; use help\r\n");
  }
}

static void DebugCLI_Task(void *argument)
{
  uint8_t receivedByte;
  uint16_t lineLength = 0U;
  uint8_t overflow = 0U;
  char line[DEBUG_CLI_LINE_LENGTH];

  (void)argument;
  DebugCLI_Write("\r\nLi_930 debug CLI ready. Type 'help'.\r\n> ");

  for (;;)
  {
    if (osMessageQueueGet(DebugCLI_RxQueue, &receivedByte, NULL,
                          osWaitForever) != osOK)
    {
      continue;
    }

    if ((receivedByte == '\r') || (receivedByte == '\n'))
    {
      if (receivedByte == '\n')
      {
        continue;
      }
      DebugCLI_Write("\r\n");
      if (overflow != 0U)
      {
        DebugCLI_Write("ERR: command line too long\r\n");
      }
      else
      {
        line[lineLength] = '\0';
        DebugCLI_ProcessLine(line);
      }
      lineLength = 0U;
      overflow = 0U;
      DebugCLI_Write("> ");
    }
    else if ((receivedByte == 0x08U) || (receivedByte == 0x7FU))
    {
      if ((lineLength > 0U) && (overflow == 0U))
      {
        lineLength--;
        DebugCLI_Write("\b \b");
      }
    }
    else if ((receivedByte >= 0x20U) && (receivedByte <= 0x7EU))
    {
      if ((lineLength < (DEBUG_CLI_LINE_LENGTH - 1U)) && (overflow == 0U))
      {
        line[lineLength++] = (char)receivedByte;
        DebugCLI_Transmit(&receivedByte, 1U);
      }
      else
      {
        overflow = 1U;
      }
    }
  }
}

DebugCLI_Status DebugCLI_Start(void)
{
  DebugCLI_TxMutex = osMutexNew(NULL);
  if (DebugCLI_TxMutex == NULL)
  {
    return DEBUG_CLI_ERROR_RTOS;
  }

  DebugCLI_TxComplete = osSemaphoreNew(1U, 0U, NULL);
  if (DebugCLI_TxComplete == NULL)
  {
    (void)osMutexDelete(DebugCLI_TxMutex);
    DebugCLI_TxMutex = NULL;
    return DEBUG_CLI_ERROR_RTOS;
  }

  DebugCLI_RxQueue = osMessageQueueNew(DEBUG_CLI_RX_QUEUE_LENGTH,
                                       sizeof(uint8_t), NULL);
  if (DebugCLI_RxQueue == NULL)
  {
    (void)osSemaphoreDelete(DebugCLI_TxComplete);
    DebugCLI_TxComplete = NULL;
    (void)osMutexDelete(DebugCLI_TxMutex);
    DebugCLI_TxMutex = NULL;
    return DEBUG_CLI_ERROR_RTOS;
  }

  DebugCLI_DmaPosition = 0U;
  if (HAL_UARTEx_ReceiveToIdle_DMA(&huart1, DebugCLI_DmaBuffer,
                                  DEBUG_CLI_DMA_BUFFER_SIZE) != HAL_OK)
  {
    (void)osMessageQueueDelete(DebugCLI_RxQueue);
    (void)osSemaphoreDelete(DebugCLI_TxComplete);
    DebugCLI_RxQueue = NULL;
    DebugCLI_TxComplete = NULL;
    (void)osMutexDelete(DebugCLI_TxMutex);
    DebugCLI_TxMutex = NULL;
    return DEBUG_CLI_ERROR_UART;
  }

  DebugCLI_TaskHandle = osThreadNew(DebugCLI_Task, NULL,
                                   &DebugCLI_TaskAttributes);
  if (DebugCLI_TaskHandle == NULL)
  {
    (void)HAL_UART_AbortReceive(&huart1);
    (void)osMessageQueueDelete(DebugCLI_RxQueue);
    (void)osSemaphoreDelete(DebugCLI_TxComplete);
    DebugCLI_RxQueue = NULL;
    DebugCLI_TxComplete = NULL;
    (void)osMutexDelete(DebugCLI_TxMutex);
    DebugCLI_TxMutex = NULL;
    return DEBUG_CLI_ERROR_RTOS;
  }

  return DEBUG_CLI_OK;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t size)
{
  if ((huart != NULL) && (huart->Instance == USART2))
  {
    DWIN_HandleRxEvent(size);
  }
  else if ((huart != NULL) && (huart->Instance == USART1))
  {
    uint16_t position;

    if (size > DEBUG_CLI_DMA_BUFFER_SIZE)
    {
      DebugCLI_UartErrors++;
      return;
    }

    position = (size == DEBUG_CLI_DMA_BUFFER_SIZE) ? 0U : size;
    while (DebugCLI_DmaPosition != position)
    {
      uint8_t receivedByte = DebugCLI_DmaBuffer[DebugCLI_DmaPosition];

      if (osMessageQueuePut(DebugCLI_RxQueue, &receivedByte, 0U, 0U) != osOK)
      {
        DebugCLI_DroppedRxBytes++;
      }
      DebugCLI_DmaPosition++;
      if (DebugCLI_DmaPosition == DEBUG_CLI_DMA_BUFFER_SIZE)
      {
        DebugCLI_DmaPosition = 0U;
      }
    }
  }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
  if ((huart != NULL) && (huart->Instance == USART2))
  {
    DWIN_HandleTxComplete();
  }
  else if ((huart != NULL) && (huart->Instance == USART1) &&
      (DebugCLI_TxInProgress != 0U))
  {
    DebugCLI_TxInProgress = 0U;
    if (osSemaphoreRelease(DebugCLI_TxComplete) != osOK)
    {
      DebugCLI_UartErrors++;
    }
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if ((huart != NULL) && (huart->Instance == USART2))
  {
    DWIN_HandleUartError();
  }
  else if ((huart != NULL) && (huart->Instance == USART1))
  {
    DebugCLI_UartErrors++;
    if ((DebugCLI_TxInProgress != 0U) &&
        (huart->gState == HAL_UART_STATE_READY))
    {
      DebugCLI_TxInProgress = 0U;
      (void)osSemaphoreRelease(DebugCLI_TxComplete);
    }
    if ((huart->RxState == HAL_UART_STATE_READY) &&
        (huart->hdmarx != NULL) &&
        (HAL_UARTEx_ReceiveToIdle_DMA(&huart1, DebugCLI_DmaBuffer,
                                    DEBUG_CLI_DMA_BUFFER_SIZE) != HAL_OK))
    {
      DebugCLI_UartErrors++;
    }
  }
}
