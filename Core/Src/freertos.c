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
#include "queue.h"
#include "main.h"
#include "cmsis_os.h"
#include <stdio.h>
#include <string.h>

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs.h"
#include "tmc5130a.h"
#include "TMC_5130.h"
#include "ads1256.h"
//#include "debug_cli.h"
#include "system_runtime.h"
#include "usart2_process.h"
#include "usart3_process.h"
#include "./SYSTEM/usart/usart.h"
#include "TouchKey_8inch.h"
#include "ColorWm_8inch.h"
#include "MySaveLoadFile.h"
#include "ads1256.h"
#include "StepControl.h"
#include "wavelength_transfor.h"

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
QueueHandle_t g_ActionQueue = NULL;
QueueHandle_t g_WaveLengthStepperQueue = NULL;

volatile int32_t CurWaveLen;//当前所处的波长
volatile _SYS_ABS MySysAbs ;

volatile _MVStruct MVStruct ;
volatile _SetScaleStruct SetScale;



/* USER CODE END Variables */
/* Definitions for StartTask */
osThreadId_t StartTaskHandle;
// volatile TMC5130A_HomeResult TMC5130A_HomingResult =
//     TMC5130A_HOME_NOT_STARTED;
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
osThreadId_t ActionTaskHandle;
osThreadId_t WaveLengthStepperTaskHandle;
const osThreadAttr_t WaveLengthStepperTask_attributes = {
  .name = "WaveLengthStepper",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t ActionTask_attributes = {
  .name = "ActionTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
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
static void ActionTask(void *argument);
static void DispTask(void *argument);
static void USART3_ProcessTask(void *argument);
static void USART2_ProcessTask(void *argument);
void TaskWaveLengthStepper(void *p_arg);
//static void StartTask_CheckSysconfigFile(void);
//static void StartTask_ListRootFiles(void);

/* USER CODE END FunctionPrototypes */

void StartTaskFunc(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */






/**
 * @brief 创建并返回系统初始化事件组。
 * @param 无。
 * @retval 初始化事件组句柄；创建失败时返回 NULL。
 * @note 已创建时直接返回现有事件组句柄。
 */
EventGroupHandle_t MyInitFlag_Init(void)
{
	if (MyInitFlag == NULL)
	{
		MyInitFlag = xEventGroupCreate();
	}
	return MyInitFlag;
}

/**
 * @brief 创建并返回系统运行事件组。
 * @param 无。
 * @retval 系统运行事件组句柄；创建失败时返回 NULL。
 * @note 已创建时直接返回现有事件组句柄。
 */
EventGroupHandle_t MyOsFlag_Init(void)
{
	if (MyOsFlag == NULL)
	{
		MyOsFlag = xEventGroupCreate();
	}
	return MyOsFlag;
}




 /**
 * @brief 更新蜂鸣器的节拍计数并控制蜂鸣器输出引脚。
 * @param 无。
 * @retval 无。
 * @note 本函数由 FreeRTOS 每个系统节拍自动调用。
 */
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




/**
 * @brief 从 USART3 接收队列读取数据帧并以十六进制格式输出。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 数据帧接收错误时等待后继续处理。
 */
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

/**
 * @brief 处理 USART2 接收帧并将触摸协议数据转换为触摸事件。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 同时将收到的数据帧以十六进制格式输出。
 */
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
//      printf("USART2 frame queue error: %ld\r\n", (long)status);
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
 * @brief 初始化 FreeRTOS 队列、事件组及应用任务。
 * @param 无。
 * @retval 无。
 * @note 创建队列或关键任务失败时调用 Error_Handler。
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
  g_ActionQueue = xQueueCreate(1U, COLOR_WM_MESSAGE_SIZE);
  if (g_ActionQueue == NULL)
  {
    Error_Handler();
  }
  g_WaveLengthStepperQueue = xQueueCreate(
      1U, sizeof(StepperCommandMessage_t));
  if (g_WaveLengthStepperQueue == NULL)
  {
    Error_Handler();
  }
  TouchKeyEventQueueReady =
      (TouchKeyEventQueue_Init() == osOK) ? 1U : 0U;
  TouchKey_Init();
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of StartTask */
  StartTaskHandle = osThreadNew(StartTaskFunc, NULL, &StartTask_attributes);
  SysRunTaskHandle = osThreadNew(SysRunTask, NULL, &SysRunTask_attributes);
  ActionTaskHandle = osThreadNew(ActionTask, NULL, &ActionTask_attributes);
  if (ActionTaskHandle == NULL)
  {
    Error_Handler();
  }
  WaveLengthStepperTaskHandle = osThreadNew(
      TaskWaveLengthStepper, NULL, &WaveLengthStepperTask_attributes);
  if (WaveLengthStepperTaskHandle == NULL)
  {
    Error_Handler();
  }
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
 * @brief 执行系统启动初始化并启动应用运行任务。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；初始化后任务持续运行。
 * @note 初始化文件系统、传感器、串口接收和显示相关组件。
 */
/* USER CODE END Header_StartTaskFunc */
void StartTaskFunc(void *argument)
{
//  static uint8_t formatWork[_MAX_SS];
  ADS1256_Status ads1256InitResult;

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartTaskFunc */
  if (TouchKeyEventQueueReady == 0U)
  {
    printf("Failed to initialize touch event queue\r\n");
  }



  FileInit();
	wl_table_init();


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


  if(TMC_5130_Init())
  {
    printf("TMC5130初始化成功\r\n");
  }
  else
  {
    printf("TMC5130初始化失败\r\n");
  }

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

  DeviceInit();



  /* Infinite loop */
  for(;;)
  {
    osDelay(1000U);
  }
  /* USER CODE END StartTaskFunc */
}


/**
 * @brief 等待 ADS1256 数据就绪并读取采样值。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 读取失败时短暂延时后继续等待下一次采样。
 */
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

/**
 * @brief 周期性翻转系统运行指示灯。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 指示灯翻转间隔为 100 个系统节拍。
 */
void SysRunTask(void *argument)
{

  (void)argument;
  for (;;)
  {
    HAL_GPIO_TogglePin(SYS_LED1_GPIO_Port, SYS_LED1_Pin);
    osDelay(100U);
  }
}

/**
 * @brief 接收动作队列消息并执行对应的系统动作。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行，直至系统调度器停止。
 * @note 当前已实现 MSG_SCAN：设置波长扫描运行标志；未知消息类型会记录错误。
 */
static void ActionTask(void *argument)
{
  uint8_t actionMessage[COLOR_WM_MESSAGE_SIZE];
  BaseType_t queueStatus;

  (void)argument;

  for (;;)
  {
    queueStatus = xQueueReceive(g_ActionQueue, actionMessage, portMAX_DELAY);
    if (queueStatus != pdPASS)
    {
      printf("Failed to receive action queue message\r\n");
      continue;
    }

    switch (actionMessage[0])
    {
      case MSG_SCAN:
        taskENTER_CRITICAL();
        SysFlag.RunWaveScanFlag = 1U;
        taskEXIT_CRITICAL();
        printf("Scan action triggered\r\n");

        break;

      default:
        printf("Unsupported action message type: %u\r\n",
               (unsigned int)actionMessage[0]);
        break;
    }
  }
}
/**
 * @brief 初始化界面并持续读取触摸事件以切换显示窗口。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 触摸事件超时时刷新当前显示窗口。
 */
static void DispTask(void *argument)
{
  uint8_t event;
  osStatus_t status;

  (void)argument;

  ColorWindowsControl(WIN_NUM_WELCOME, 0U);
  DisplayWindow();

  osDelay(pdMS_TO_TICKS(1000)); 

  ColorWindowsControl(WIN_NUM_INIT, 0U);
  DisplayWindow();



  for (;;)
  {
    status = TouchKeyEventQueue_Get(&event, 1000U);
    if (status == osErrorTimeout)
    {
//      printf("Touch event queue receive error: %ld\r\n", (long)status);
      DisplayWindow();
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



/**
 * @brief 从波长步进电机命令队列接收命令并执行相应的电机动作。
 * @param p_arg 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 命令通过 FreeRTOS 队列按值传递；动作结束后设置波长任务完成事件位。
 */
void TaskWaveLengthStepper(void *p_arg)
{
	StepperCommandMessage_t message;
	BaseType_t queueStatus;
	int32_t currentPosition;
	int32_t targetPosition;
	int32_t steps;
	int32_t desWavelength;
	float correctionWavelength;

	(void)p_arg;

	for (;;)
	{
		queueStatus = xQueueReceive(g_WaveLengthStepperQueue, &message,
		                            portMAX_DELAY);
		if (queueStatus != pdPASS)
		{
			printf("Failed to receive wavelength stepper command\r\n");
			osDelay(1U);
			continue;
		}

#if defined(STEPPER_DEBUG) && (STEPPER_DEBUG > 0)
		if (MyOsFlag != NULL)
		{
			(void)xEventGroupSetBits(MyOsFlag, TASK_WAVE_LENGTH_FINISH_FLAG);
		}
		continue;
#endif

		switch (message.command)
		{
			case STEPPER_CMD_MOVE_TO_WAVELENGTH:
				TMC5130_SetMaxSpeed(TMC5130_NORMAL_MOVE_SPEED);
				currentPosition = WavelengthToPosition(CurWaveLen);
				targetPosition = WavelengthToPosition(
				    message.value.targetWavelength);
				steps = targetPosition - currentPosition;
				MoveNStep(WaveLenStepper, FORWARD, steps);
				while (TMC5130_GetMotionStatus() != STATUS_TARGET_REACHED)
				{
					osDelay(pdMS_TO_TICKS(5U));
				}
				CurWaveLen = message.value.targetWavelength;
				StepperControlStat[WaveLenStepper].Cnt = targetPosition;
				break;

			case STEPPER_CMD_INITIALIZE:
				break;

			case STEPPER_CMD_CORRECT_WAVELENGTH:
				desWavelength = CurWaveLen;
				correctionWavelength = message.value.correction;
				if (correctionWavelength > 0.0f)
				{
					steps = (int32_t)(correctionWavelength * 10.0f) + 200;
					MoveNStep(WaveLenStepper, FORWARD, steps);
					while (TMC5130_GetMotionStatus() != STATUS_TARGET_REACHED)
					{
						osDelay(pdMS_TO_TICKS(5U));
					}
						osDelay(pdMS_TO_TICKS(500U));
					MoveNStep(WaveLenStepper, BACKWARD, 200);
					while (TMC5130_GetMotionStatus() != STATUS_TARGET_REACHED)
					{
						osDelay(pdMS_TO_TICKS(5U));
					}
				}
				else if (correctionWavelength < 0.0f)
				{
					steps = (int32_t)(-correctionWavelength * 10.0f);
					MoveNStep(WaveLenStepper, BACKWARD, steps);
					while (TMC5130_GetMotionStatus() != STATUS_TARGET_REACHED)
					{
						osDelay(pdMS_TO_TICKS(5U));
					}
				}
				CurWaveLen = desWavelength;
				break;

			default:
				printf("Unsupported wavelength stepper command: %u\r\n",
				       (unsigned int)message.command);
				break;
		}

		if (MyOsFlag != NULL)
		{
			(void)xEventGroupSetBits(MyOsFlag, TASK_WAVE_LENGTH_FINISH_FLAG);
		}
	}
}

/**
 * @brief 设置蜂鸣器状态及需要保持开启的系统节拍数。
 * @param ticks 蜂鸣器开启的系统节拍数；为零时关闭蜂鸣器。
 * @retval 无。
 * @note 蜂鸣器引脚由系统节拍钩子根据该状态驱动。
 */
void SysRun_SetBeep(uint32_t ticks)
{
//  taskENTER_CRITICAL();
  Beep.cnt = ticks;
  Beep.En = (ticks > 0U) ? 1U : 0U;
//  taskEXIT_CRITICAL();
}

/**
 * @brief 获取蜂鸣器启用状态和剩余开启节拍数。
 * @param enabled 非 NULL 时用于返回蜂鸣器启用状态。
 * @param ticks 非 NULL 时用于返回剩余节拍数。
 * @retval 无。
 * @note 输出参数允许为 NULL，以忽略对应结果。
 */
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
