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
#include "usart2_process.h"
#include "usart3_process.h"
#include "./SYSTEM/usart/usart.h"
#include "TouchKey_8inch.h"
#include "ColorWm_8inch.h"

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
volatile SYS_FLAG SysFlag = {0U};
static uint8_t TouchKeyEventQueueReady;
//volatile DebugCLI_Status DebugCLI_StartResult = DEBUG_CLI_ERROR_RTOS;
EventGroupHandle_t MyInitFlag = NULL;
EventGroupHandle_t MyOsFlag = NULL;

volatile int32_t CurWaveLen;//当前所处的波长
volatile _SYS_ABS MySysAbs ;

volatile _MVStruct MVStruct ;
volatile _GuPuCeShiStruct GuPuCeShiStruct ;



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
osThreadId_t DispTaskHandle;
const osThreadAttr_t DispTask_attributes = {
  .name = "DispTask",
  .stack_size = 2048 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t USART3_ProcessTaskHandle;
const osThreadAttr_t USART3_ProcessTask_attributes = {
  .name = "USART3_Process",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t USART2_ProcessTaskHandle;
const osThreadAttr_t USART2_ProcessTask_attributes = {
  .name = "USART2_Process",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void ADS1256_SamplingTask(void *argument);
void SysRunTask(void *argument);
static void DispTask(void *argument);
static void USART3_ProcessTask(void *argument);
static void USART2_ProcessTask(void *argument);
//static void StartTask_CheckSysconfigFile(void);
//static void StartTask_ListRootFiles(void);

/* USER CODE END FunctionPrototypes */

void StartTaskFunc(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */






EventGroupHandle_t MyInitFlag_Init(void)
{
	if (MyInitFlag == NULL)
	{
		MyInitFlag = xEventGroupCreate();
	}
	return MyInitFlag;
}

EventGroupHandle_t MyOsFlag_Init(void)
{
	if (MyOsFlag == NULL)
	{
		MyOsFlag = xEventGroupCreate();
	}
	return MyOsFlag;
}




 void vApplicationTickHook (void)
{
  uint8_t buzzerOn;
	buzzerOn = 0U;
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
	HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin,
										(buzzerOn != 0U) ? GPIO_PIN_SET : GPIO_PIN_RESET);

}




static void USART3_ProcessTask(void *argument)
{
  USART3_ProcessFrame frame;
  char output[USART3_PROCESS_MAX_FRAME_LENGTH * 3U + 48U];
  size_t outputLength;
  uint16_t index;
  int written;
  osStatus_t status;

  (void)argument;

  for (;;)
  {
    status = USART3_Process_GetFrame(&frame, osWaitForever);
    if (status != osOK)
    {
      printf("USART3 frame queue error: %ld\r\n", (long)status);
      osDelay(10U);
      continue;
    }

    written = snprintf(output, sizeof(output),
                       "USART3 RX (%u bytes):",
                       (unsigned int)frame.length);
    if ((written < 0) || ((size_t)written >= sizeof(output)))
    {
      printf("USART3 frame output format error\r\n");
      continue;
    }
    outputLength = (size_t)written;

    for (index = 0U; index < frame.length; index++)
    {
      written = snprintf(&output[outputLength],
                         sizeof(output) - outputLength,
                         " %02X", frame.data[index]);
      if ((written < 0) ||
          ((size_t)written >= (sizeof(output) - outputLength)))
      {
        break;
      }
      outputLength += (size_t)written;
    }

    if (index != frame.length)
    {
      printf("USART3 frame output buffer too small\r\n");
      continue;
    }

    output[outputLength++] = '\r';
    output[outputLength++] = '\n';
    output[outputLength] = '\0';
    printf("%s", output);
  }
}

static void USART2_ProcessTask(void *argument)
{
  USART2_ProcessFrame frame;
  uint8_t touchMessage[USART2_PROCESS_MAX_FRAME_LENGTH + 1U];
  char output[USART2_PROCESS_MAX_FRAME_LENGTH * 3U + 48U];
  size_t outputLength;
  uint16_t index;
  uint16_t requiredPayloadLength;
  int written;
  osStatus_t status;

  (void)argument;

  for (;;)
  {
    status = USART2_Process_GetFrame(&frame, osWaitForever);
    if (status != osOK)
    {
      printf("USART2 frame queue error: %ld\r\n", (long)status);
      osDelay(10U);
      continue;
    }

    if (frame.length > USART2_PROCESS_MAX_FRAME_LENGTH)
    {
      printf("USART2 frame length is invalid: %u\r\n",
             (unsigned int)frame.length);
      continue;
    }

    if (frame.length > 0U)
    {
      if (frame.data[0] == 0x73U)
      {
        requiredPayloadLength = 5U;
      }
      else if (frame.data[0] == 0x72U)
      {
        requiredPayloadLength = 1U;
      }
      else
      {
        requiredPayloadLength = 0U;
      }

      if (requiredPayloadLength > 0U)
      {
        if (frame.length < requiredPayloadLength)
        {
          printf("Incomplete USART2 touch frame: %u bytes\r\n",
                 (unsigned int)frame.length);
        }
        else
        {
          touchMessage[0] = USART2_PROCESS_FRAME_START;
          memcpy(&touchMessage[1], frame.data, frame.length);
          if (Get_TouchKeyAssignments(touchMessage) != 0U)
          {
            printf("Failed to queue USART2 touch event\r\n");
          }
        }
      }
    }

    written = snprintf(output, sizeof(output),
                       "USART2 RX (%u bytes):",
                       (unsigned int)frame.length);
    if ((written < 0) || ((size_t)written >= sizeof(output)))
    {
      printf("USART2 frame output format error\r\n");
      continue;
    }
    outputLength = (size_t)written;

    for (index = 0U; index < frame.length; index++)
    {
      written = snprintf(&output[outputLength],
                         sizeof(output) - outputLength,
                         " %02X", frame.data[index]);
      if ((written < 0) ||
          ((size_t)written >= (sizeof(output) - outputLength)))
      {
        break;
      }
      outputLength += (size_t)written;
    }

    if (index != frame.length)
    {
      printf("USART2 frame output buffer too small\r\n");
      continue;
    }

    output[outputLength++] = '\r';
    output[outputLength++] = '\n';
    output[outputLength] = '\0';
    printf("%s", output);
  }
}

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
  TouchKeyEventQueueReady =
      (TouchKeyEventQueue_Init() == osOK) ? 1U : 0U;
  TouchKey_Init();
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
  (void)MyInitFlag_Init();
  (void)MyOsFlag_Init();
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
  static uint8_t formatWork[_MAX_SS];
  ADS1256_Status ads1256InitResult;

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartTaskFunc */
  if (TouchKeyEventQueueReady == 0U)
  {
    printf("Failed to initialize touch event queue\r\n");
  }

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

//  tmc5130aInitResult = TMC5130A_Init();
//  if (tmc5130aInitResult != 0U)
//  {
//    TMC5130A_HomingResult = TMC5130A_FindHome();
//  }
//  else
//  {
//    TMC5130A_HomingResult = TMC5130A_HOME_INIT_ERROR;
//  }

//  DWIN_StartResult = DWIN_Init();
  // UI_ManagerStartResult = UI_Manager_Init();

  USART3_ProcessStartResult = USART3_Process_Init();
  if (USART3_ProcessStartResult == HAL_OK)
  {
    USART3_ProcessTaskHandle = osThreadNew(
        USART3_ProcessTask, NULL, &USART3_ProcessTask_attributes);
    if (USART3_ProcessTaskHandle == NULL)
    {
      printf("Failed to create USART3 process task\r\n");
    }
  }
  else
  {
    printf("Failed to initialize USART3 receiver: %d\r\n",
           (int)USART3_ProcessStartResult);
  }

  USART2_ProcessStartResult = USART2_Process_Init();
  if (USART2_ProcessStartResult != HAL_OK)
  {
    printf("Failed to initialize USART2 receiver: %d\r\n",
           (int)USART2_ProcessStartResult);
  }
  else
  {
    USART2_ProcessTaskHandle = osThreadNew(
        USART2_ProcessTask, NULL, &USART2_ProcessTask_attributes);
    if (USART2_ProcessTaskHandle == NULL)
    {
      printf("Failed to create USART2 process task\r\n");
    }

    if (TouchKeyEventQueueReady != 0U)
    {
      DispTaskHandle = osThreadNew(DispTask, NULL, &DispTask_attributes);
      if (DispTaskHandle == NULL)
      {
        printf("Failed to create display task\r\n");
      }
    }
  }

  /* Infinite loop */
  for(;;)
  {
    osDelay(1000U);
  }
  /* USER CODE END StartTaskFunc */
}


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

  (void)argument;
  for (;;)
  {
    HAL_GPIO_TogglePin(SYS_LED1_GPIO_Port, SYS_LED1_Pin);
    osDelay(100U);
  }
}

static void DispTask(void *argument)
{
  uint8_t event;
  osStatus_t status;

  (void)argument;

  ColorWindowsControl(WIN_NUM_WELCOME, 0U);
  DisplayWindow();

  osDelay(pdMS_TO_TICKS(1000)); 

  ColorWindowsControl(WIN_NUM_MAIN_MENU, 0U);
  DisplayWindow();



  for (;;)
  {
    status = TouchKeyEventQueue_Get(&event, 1000U);
    if (status == osErrorTimeout)
    {
      printf("Touch event queue receive error: %ld\r\n", (long)status);
      osDelay(1U);
      // continue;
    }
    else
    {

      switch (event)
      {
        case TOUCH_KEY_EVENT_FATHER:
          DisplayWindow();
          break;
        case TOUCH_KEY_EVENT_CHILD:
          DisplayChildWindow();
          break;
        case TOUCH_KEY_EVENT_SECOND_CHILD:
          DisplaySecondChildWindow();
          break;
        default:
          printf("Invalid touch event: %u\r\n", (unsigned int)event);
          break;
      }

    }

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
