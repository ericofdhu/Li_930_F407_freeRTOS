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
#include "CircularBuffer.h"
#include "rc_filter.h"

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
static osMutexId_t DisplayMutex;
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
/* Definitions for Task_Start */
osThreadId_t Task_Start_Handle;
// volatile TMC5130A_HomeResult TMC5130A_HomingResult =
//     TMC5130A_HOME_NOT_STARTED;
const osThreadAttr_t Task_Start_attributes = {
  .name = "Task_Start",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t Task_ADS1256_Handle;
const osThreadAttr_t Task_ADS1256_attributes = {
  .name = "Task_ADS1256",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityAboveNormal,
};
osThreadId_t Task_SysRun_Handle;
osThreadId_t Task_Action_Handle;
osThreadId_t Task_WaveLengthStepper_Handle;
const osThreadAttr_t Task_WaveLengthStepper_attributes = {
  .name = "Task_WaveLengthStepper",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t Task_Action_attributes = {
  .name = "Task_Action",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
const osThreadAttr_t Task_SysRun_attributes = {
  .name = "Task_SysRun",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
osThreadId_t Task_Disp_Handle;
const osThreadAttr_t Task_Disp_attributes = {
  .name = "Task_Disp",
  .stack_size = 1024 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t Task_ChildDisp_Handle;
const osThreadAttr_t Task_ChildDisp_attributes = {
  .name = "Task_ChildDisp",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t Task_USART3_Process_Handle;
const osThreadAttr_t Task_USART3_Process_attributes = {
  .name = "Task_USART3_Process",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
osThreadId_t Task_USART2_Process_Handle;
const osThreadAttr_t Task_USART2_Process_attributes = {
  .name = "Task_USART2_Process",
  .stack_size = 512 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void Task_ADS1256_Sampling(void *argument);
void Task_SysRun(void *argument);
static void Task_Action(void *argument);
static void Task_Disp(void *argument);
static void Task_ChildDisp(void *argument);
static void Task_USART3_Process(void *argument);
static void Task_USART2_Process(void *argument);
void Task_WaveLengthStepper(void *p_arg);
//static void Task_Start_CheckSysconfigFile(void);
//static void Task_Start_ListRootFiles(void);

/* USER CODE END FunctionPrototypes */

void Task_StartFunc(void *argument);

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
static void Task_USART3_Process(void *argument)
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
static void Task_USART2_Process(void *argument)
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
  /* creation of Task_Start */
  Task_Start_Handle = osThreadNew(Task_StartFunc, NULL, &Task_Start_attributes);
  Task_SysRun_Handle = osThreadNew(Task_SysRun, NULL, &Task_SysRun_attributes);
  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  (void)MyInitFlag_Init();
  (void)MyOsFlag_Init();
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_Task_StartFunc */
/**
 * @brief 完成应用启动阶段的外设和任务初始化。
 * @param argument 任务入口参数，本函数当前不使用。
 * @return 无。
 */
/* USER CODE END Header_Task_StartFunc */
void Task_StartFunc(void *argument)
{
//  static uint8_t formatWork[_MAX_SS];
  ADS1256_Status ads1256InitResult;

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN Task_StartFunc */
  if (TouchKeyEventQueueReady == 0U)
  {
    printf("Failed to initialize touch event queue\r\n");
  }



  FileInit();
	wl_table_init();


  ads1256InitResult = ADS1256_Init();
  Task_Action_Handle = osThreadNew(Task_Action, NULL, &Task_Action_attributes);
  if (Task_Action_Handle == NULL)
  {
    Error_Handler();
  }
  // if (ads1256InitResult == ADS1256_OK)
  // {
  //   Task_ADS1256_Handle = osThreadNew(Task_ADS1256_Sampling, NULL,
  //                                   &Task_ADS1256_attributes);
  //   if (Task_ADS1256_Handle == NULL)
  //   {
  //     ADS1256_LastStatus = ADS1256_ERROR_RTOS;
  //   }
  // }


  if(TMC_5130_Init())
  {
    printf("TMC5130初始化成功\r\n");
  }
  else
  {
    printf("TMC5130初始化失败\r\n");
  }


  Task_WaveLengthStepper_Handle = osThreadNew(
      Task_WaveLengthStepper, NULL, &Task_WaveLengthStepper_attributes);
  if (Task_WaveLengthStepper_Handle == NULL)
  {
    Error_Handler();
  }





  USART3_ProcessStartResult = USART3_Process_Init();
  if (USART3_ProcessStartResult == HAL_OK)
  {
    Task_USART3_Process_Handle = osThreadNew(
        Task_USART3_Process, NULL, &Task_USART3_Process_attributes);
    if (Task_USART3_Process_Handle == NULL)
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
    Task_USART2_Process_Handle = osThreadNew(
        Task_USART2_Process, NULL, &Task_USART2_Process_attributes);
    if (Task_USART2_Process_Handle == NULL)
    {
      printf("Failed to create USART2 process task\r\n");
    }

    if (TouchKeyEventQueueReady != 0U)
    {
      DisplayMutex = osMutexNew(NULL);
      if (DisplayMutex == NULL)
      {
        printf("Failed to create display mutex\r\n");
      }
      else
      {
        Task_Disp_Handle = osThreadNew(Task_Disp, NULL, &Task_Disp_attributes);
        if (Task_Disp_Handle == NULL)
        {
          printf("Failed to create parent display task\r\n");
        }

        Task_ChildDisp_Handle = osThreadNew(
            Task_ChildDisp, NULL, &Task_ChildDisp_attributes);
        if (Task_ChildDisp_Handle == NULL)
        {
          printf("Failed to create child display task\r\n");
        }
      }
    }
  }

  DeviceInit();



  /* Infinite loop */
  for(;;)
  {
    osDelay(1000U);
  }
  /* USER CODE END Task_StartFunc */
}


/**
 * @brief 等待 ADS1256 数据就绪并读取采样值。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 读取失败时短暂延时后继续等待下一次采样。
 */
void Task_ADS1256_Sampling(void *argument)
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
void Task_SysRun(void *argument)
{

  (void)argument;
  for (;;)
  {
    HAL_GPIO_TogglePin(SYS_LED1_GPIO_Port, SYS_LED1_Pin);
    osDelay(100U);
  }
}


int16_t get_normal_time_value(void)
{
//  int32_t sample = 0;
//  ADS1256_Status readStatus = ADS1256_ERROR_RTOS;

//  if (ADS1256_WaitDataReady(osWaitForever) == osOK)
//  {
//    readStatus = ADS1256_ReadData(&sample);
//  }

//  if (readStatus == ADS1256_OK)
//  {
//    CircularBuffer_Push(&cb_normal,
//                        ADS1256_ConvertToVoltage((uint16_t)((uint32_t)sample >> 8)));
//  }
//  else
//  {
//    printf("Failed to read ADS1256 sample\r\n");
//  }

//  int16_t tmp_in = CircularBuffer_GetAverage(&cb_normal);
//  int16_t tmp_voltage = rc_filter_process(&normal_filter, tmp_in);
//  return tmp_voltage;


	uint32_t   tmp_ad=0;
	
//	while(ADS1256_FLAG == 0);
	tmp_ad = AD_read_data();
//	ADS1256_FLAG = 0;
	CircularBuffer_Push(&cb_normal, ADS1256_ConvertToVoltage(tmp_ad));
	int16_t tmp_in  = CircularBuffer_GetAverage(&cb_normal);
	int16_t tmp_voltage = rc_filter_process(&normal_filter, tmp_in);

	return (int16_t)tmp_voltage;

}
int16_t get_normal_time_value_with_data(int16_t tmp_ad)
{

	CircularBuffer_Push(&cb_normal, ADS1256_ConvertToVoltage(tmp_ad));
	int16_t tmp_in  = CircularBuffer_GetAverage(&cb_normal);
	int16_t tmp_voltage = rc_filter_process(&normal_filter, tmp_in);

	return (int16_t)tmp_voltage;

}







/**
 * @brief 接收动作队列消息并执行对应的系统动作。
 * @param argument 任务参数，本任务不使用。
 * @retval 无；任务持续运行，直至系统调度器停止。
 * @note 当前已实现 MSG_SCAN：设置波长扫描运行标志；未知消息类型会记录错误。
 */
static void Task_Action(void *argument)
{
  uint8_t actionMessage[COLOR_WM_MESSAGE_SIZE];
  BaseType_t queueStatus;

  (void)argument;



	CircularBuffer_Init(&cb_normal,normal_Buffer,NORMAL_BUFF_LEN);
	rc_filter_init(&normal_filter, TIME_SCAN_CUTO_FREQ, TIME_SCAN_SAMPLE_FREQ);





  for (;;)
  {
    queueStatus = xQueueReceive(g_ActionQueue, actionMessage, pdMS_TO_TICKS(20U));
    if (queueStatus != pdPASS)
    {
      // printf("Failed to receive action queue message\r\n");
			MySysAbs.data_E = get_normal_time_value();
      printf("Normal time value: %d\r\n", MySysAbs.data_E);


    }
    else
    {

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
}
/**
 * @brief 初始化主窗口，并等待主窗口触摸事件后刷新主窗口显示。
 * @param argument FreeRTOS/CMSIS-RTOS 任务参数，本任务不使用。
 * @retval 无；任务持续运行以处理主窗口显示事件。
 * @note 主窗口及子窗口共用 LCD，绘制期间使用 DisplayMutex 防止并发访问。
 */
static void Task_Disp(void *argument)
{
  uint8_t event;
  osStatus_t status;

  (void)argument;

  status = osMutexAcquire(DisplayMutex, osWaitForever);
  if (status == osOK)
  {
    ColorWindowsControl(WIN_NUM_WELCOME, 0U);
    DisplayWindow();
    status = osMutexRelease(DisplayMutex);
    if (status != osOK)
    {
      printf("Failed to release display mutex after welcome screen: %ld\r\n",
             (long)status);
    }
  }
  else
  {
    printf("Failed to acquire display mutex for welcome screen: %ld\r\n",
           (long)status);
  }

  osDelay(pdMS_TO_TICKS(1000U));

  status = osMutexAcquire(DisplayMutex, osWaitForever);
  if (status == osOK)
  {
    ColorWindowsControl(WIN_NUM_INIT, 0U);
    DisplayWindow();
    status = osMutexRelease(DisplayMutex);
    if (status != osOK)
    {
      printf("Failed to release display mutex after init screen: %ld\r\n",
             (long)status);
    }
  }
  else
  {
    printf("Failed to acquire display mutex for init screen: %ld\r\n",
           (long)status);
  }

  for (;;)
  {
    status = TouchKeyEventQueue_Get(
        TOUCH_KEY_EVENT_FATHER, &event, pdMS_TO_TICKS(200U));
    if ((status != osOK) && (status != osErrorTimeout))
    {
      printf("Failed to receive parent window event: %ld\r\n", (long)status);
      osDelay(1U);
      continue;
    }

    status = osMutexAcquire(DisplayMutex, osWaitForever);
    if (status != osOK)
    {
      printf("Failed to acquire display mutex for parent window: %ld\r\n",
             (long)status);
      osDelay(1U);
      continue;
    }

    DisplayWindow();
    status = osMutexRelease(DisplayMutex);
    if (status != osOK)
    {
      printf("Failed to release display mutex for parent window: %ld\r\n",
             (long)status);
    }
  }
}

/**
 * @brief 等待子窗口触摸事件，并调用子窗口绘制回调刷新显示。
 * @param argument FreeRTOS/CMSIS-RTOS 任务参数，本任务不使用。
 * @retval 无；任务持续运行以处理子窗口显示事件。
 * @note 二级子窗口事件不会进入子窗口队列，因此本任务只绘制一级子窗口。
 */
static void Task_ChildDisp(void *argument)
{
  uint8_t event;
  osStatus_t status;

  (void)argument;

  for (;;)
  {
    status = TouchKeyEventQueue_Get(
        TOUCH_KEY_EVENT_CHILD, &event, pdMS_TO_TICKS(200U));
    if ((status != osOK) && (status != osErrorTimeout))
    {
      printf("Failed to receive child window event: %ld\r\n", (long)status);
      osDelay(1U);
      continue;
    }

    status = osMutexAcquire(DisplayMutex, osWaitForever);
    if (status != osOK)
    {
      printf("Failed to acquire display mutex for child window: %ld\r\n",
             (long)status);
      osDelay(1U);
      continue;
    }

    DisplayChildWindow();
    status = osMutexRelease(DisplayMutex);
    if (status != osOK)
    {
      printf("Failed to release display mutex for child window: %ld\r\n",
             (long)status);
    }
  }
}




/**
 * @brief 从波长步进电机命令队列接收命令并执行相应的电机动作。
 * @param p_arg 任务参数，本任务不使用。
 * @retval 无；任务持续运行。
 * @note 命令通过 FreeRTOS 队列按值传递；动作结束后设置波长任务完成事件位。
 */
void Task_WaveLengthStepper(void *p_arg)
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
