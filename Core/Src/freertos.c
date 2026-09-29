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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "fatfs.h"
#include "tmc5130a.h"
#include "ads1256.h"

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
  ADS1256_Status ads1256InitResult;

  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartTaskFunc */
  MX_FATFS_Init();
  if (retUSER == 0U)
  {
    USERFatFSResult = f_mount(&USERFatFS, USERPath, 1U);
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

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
