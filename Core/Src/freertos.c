/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs.h"
#include "tmc5130a.h"
#include "ads1256.h"
//#include "debug_cli.h"
#include "system_runtime.h"
#include "dwin.h"
#include "ui_manager.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
volatile BeepCtrl_t Beep = {0U, 0U};
//volatile DebugCLI_Status DebugCLI_StartResult = DEBUG_CLI_ERROR_RTOS;

/* USER CODE END Variables */
/* Definitions for StartTask */
osThreadId_t StartTaskHandle;
volatile TMC5130A_HomeResult TMC5130A_HomingResult =
    TMC5130A_HOME_NOT_STARTED;
const osThreadAttr_t StartTask_attributes = {
  .name = "StartTask",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t ADS1256TaskHandle;
const osThreadAttr_t ADS1256Task_attributes = {
  .name = "ADS1256Task",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
osThreadId_t SysRunTaskHandle;
const osThreadAttr_t SysRunTask_attributes = {
  .name = "SysRun",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void ADS1256_SamplingTask(void *argument);
void SysRunTask(void *argument);
//static void StartTask_CheckSysconfigFile(void);
//static void StartTask_ListRootFiles(void);

/* USER CODE END FunctionPrototypes */

void StartTaskFunc(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of StartTask */
  StartTaskHandle = osThreadNew(StartTaskFunc, NULL, &StartTask_attributes);
  SysRunTaskHandle = osThreadNew(SysRunTask, NULL, &SysRunTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartTaskFunc */
/**
  * @brief  Function implementing the StartTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTaskFunc */
void StartTaskFunc(void *argument)
{
  uint8_t tmc5130aInitResult;
  static uint8_t formatWork[_MAX_SS];
  ADS1256_Status ads1256InitResult;

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartTaskFunc */
  MX_FATFS_Init();
  if (retUSER == 0U)
  {
    USERFatFSResult = f_mount(&USERFatFS, USERPath, 1U);
    if (USERFatFSResult == FR_NO_FILESYSTEM)
    {
      USERFatFSResult = f_mkfs(USERPath, FM_ANY, 0U, formatWork,
                               sizeof(formatWork));
      if (USERFatFSResult == FR_OK)
      {
        USERFatFSResult = f_mount(&USERFatFS, USERPath, 1U);
      }
    }
  }
  else
  {
    USERFatFSResult = FR_NOT_READY;
  }

  tmc5130aInitResult = TMC5130A_Init();
  ads1256InitResult = ADS1256_Init();
  if (ads1256InitResult == ADS1256_OK)
  {
    ADS1256TaskHandle = osThreadNew(ADS1256_SamplingTask, NULL,
                                    &ADS1256Task_attributes);
    if (ADS1256TaskHandle == NULL)
    {
      ADS1256_LastStatus = ADS1256_ERROR_RTOS;
    }
  }

  if (tmc5130aInitResult != 0U)
  {
    TMC5130A_HomingResult = TMC5130A_FindHome();
  }
  else
  {
    TMC5130A_HomingResult = TMC5130A_HOME_INIT_ERROR;
  }

//  DebugCLI_StartResult = DebugCLI_Start();
//  if (DebugCLI_StartResult == DEBUG_CLI_OK)
//  {
//    if (USERFatFSResult == FR_OK)
//    {
//      DebugCLI_Write("Filesystem mounted\r\n");
//      StartTask_CheckSysconfigFile();
//    }
//    else
//    {
//      DebugCLI_Write("Filesystem initialization failed\r\n");
//    }
//  }
  DWIN_StartResult = DWIN_Init();
  UI_ManagerStartResult = UI_Manager_Init();

  /* Infinite loop */
  for(;;)
  {
    osDelay(1000U);
  }
  /* USER CODE END StartTaskFunc */
}

//static void StartTask_CheckSysconfigFile(void)
//{
//  static const char fileName[] = "Sysconfig.txt";
//  static const char initialContent[] = "this is a config file!";
//  char filePath[sizeof(USERPath) + sizeof(fileName)];
//  char readBuffer[64];
//  int pathLength;
//  UINT bytesRead;
//  UINT bytesWritten;
//  FRESULT result;
//  FRESULT closeResult;

//  pathLength = snprintf(filePath, sizeof(filePath), "%s%s",
//                       USERPath, fileName);
//  if ((pathLength < 0) || ((size_t)pathLength >= sizeof(filePath)))
//  {
//    DebugCLI_Write("Sysconfig.txt path is too long\r\n");
//    StartTask_ListRootFiles();
//    return;
//  }

//  result = f_open(&USERFile, filePath, FA_READ);
//  if ((result == FR_NO_FILE) || (result == FR_NO_PATH))
//  {
//    result = f_open(&USERFile, filePath, FA_CREATE_NEW | FA_WRITE);
//    if (result != FR_OK)
//    {
//      DebugCLI_Write("Cannot create Sysconfig.txt\r\n");
//      StartTask_ListRootFiles();
//      return;
//    }

//    result = f_write(&USERFile, initialContent,
//                     (UINT)(sizeof(initialContent) - 1U), &bytesWritten);
//    if ((result == FR_OK) &&
//        (bytesWritten == (UINT)(sizeof(initialContent) - 1U)))
//    {
//      DebugCLI_Write("Sysconfig.txt created\r\n");
//    }
//    else
//    {
//      DebugCLI_Write("Failed to write Sysconfig.txt\r\n");
//    }

//    closeResult = f_close(&USERFile);
//    if (closeResult != FR_OK)
//    {
//      DebugCLI_Write("Failed to close Sysconfig.txt\r\n");
//    }
//    StartTask_ListRootFiles();
//    return;
//  }
//  if (result != FR_OK)
//  {
//    DebugCLI_Write("Cannot open Sysconfig.txt\r\n");
//    StartTask_ListRootFiles();
//    return;
//  }

//  DebugCLI_Write("Sysconfig.txt contents:\r\n");
//  do
//  {
//    result = f_read(&USERFile, readBuffer,
//                    (UINT)(sizeof(readBuffer) - 1U), &bytesRead);
//    if (result != FR_OK)
//    {
//      DebugCLI_Write("\r\nFailed to read Sysconfig.txt\r\n");
//      break;
//    }
//    if (bytesRead == 0U)
//    {
//      break;
//    }

//    readBuffer[bytesRead] = '\0';
//    DebugCLI_Write(readBuffer);
//  } while (bytesRead == (UINT)(sizeof(readBuffer) - 1U));

//  closeResult = f_close(&USERFile);
//  if (closeResult != FR_OK)
//  {
//    DebugCLI_Write("\r\nFailed to close Sysconfig.txt\r\n");
//  }
//  else
//  {
//    DebugCLI_Write("\r\n");
//  }

//  StartTask_ListRootFiles();
//}

//static void StartTask_ListRootFiles(void)
//{
//  DIR directory;
//  FILINFO fileInfo;
//  char outputLine[sizeof(fileInfo.fname) + 3U];
//  int outputLength;
//  FRESULT result;
//  FRESULT closeResult;

//  DebugCLI_Write("Root directory files:\r\n");
//  result = f_opendir(&directory, USERPath);
//  if (result != FR_OK)
//  {
//    DebugCLI_Write("Cannot open root directory\r\n");
//    return;
//  }

//  for (;;)
//  {
//    result = f_readdir(&directory, &fileInfo);
//    if (result != FR_OK)
//    {
//      DebugCLI_Write("Failed to read root directory\r\n");
//      break;
//    }
//    if (fileInfo.fname[0] == '\0')
//    {
//      break;
//    }
//    if ((fileInfo.fattrib & AM_DIR) != 0U)
//    {
//      continue;
//    }

//    outputLength = snprintf(outputLine, sizeof(outputLine), "%s\r\n",
//                            fileInfo.fname);
//    if ((outputLength < 0) ||
//        ((size_t)outputLength >= sizeof(outputLine)))
//    {
//      DebugCLI_Write("Root filename is too long to display\r\n");
//      continue;
//    }
//    DebugCLI_Write(outputLine);
//  }

//  closeResult = f_closedir(&directory);
//  if (closeResult != FR_OK)
//  {
//    DebugCLI_Write("Failed to close root directory\r\n");
//  }
//}

void ADS1256_SamplingTask(void *argument)
{
  int32_t sample;

  (void)argument;
  for (;;)
  {
    if ((ADS1256_WaitDataReady(osWaitForever) == osOK) &&
        (ADS1256_ReadData(&sample) != ADS1256_OK))
    {
      osDelay(1U);
    }
  }
}

void SysRunTask(void *argument)
{
  uint8_t buzzerOn;

  (void)argument;
  for (;;)
  {
    HAL_GPIO_TogglePin(SYS_LED1_GPIO_Port, SYS_LED1_Pin);
    buzzerOn = 0U;
    taskENTER_CRITICAL();
    if (Beep.En == 1U)
    {
      if (Beep.cnt > 0U)
      {
        Beep.cnt--;
      }
      if (Beep.cnt == 0U)
      {
        Beep.En = 0U;
      }
      else
      {
        buzzerOn = 1U;
      }
    }
    taskEXIT_CRITICAL();
    HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin,
                      (buzzerOn != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    osDelay(100U);
  }
}

void SysRun_SetBeep(uint32_t ticks)
{
//  taskENTER_CRITICAL();
  Beep.cnt = ticks;
  Beep.En = (ticks > 0U) ? 1U : 0U;
//  taskEXIT_CRITICAL();
}

void SysRun_GetBeep(uint8_t *enabled, uint32_t *ticks)
{
//  taskENTER_CRITICAL();
  if (enabled != NULL)
  {
    *enabled = Beep.En;
  }
  if (ticks != NULL)
  {
    *ticks = Beep.cnt;
  }
//  taskEXIT_CRITICAL();
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
