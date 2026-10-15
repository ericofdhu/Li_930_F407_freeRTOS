/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 主程序文件。
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
#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "dac.h"
#include "dma.h"
#include "fatfs.h"
#include "spi.h"
//#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "system_runtime.h"
#include "tmc_5130.h"
#include "StepControl.h"

#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./USMART/usmart.h"
#include "./MALLOC/malloc.h"



/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

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

/* USER CODE BEGIN PV */
int16_t GuangPuScanLine[WAVE_SCAN_LINE_BUF_SIZE+1]; 
int16_t DongLiXueScanLine[TIME_SCAN_LINE_BUF_SIZE+1]; 
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
/* USER CODE BEGIN PFP */
void Print_SystemVersion(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief 打印当前固件、硬件及通信协议版本信息。
  * @param 无。
  * @retval 无。
  */
void Print_SystemVersion(void)
{
  printf("\r\n========================================\r\n");
  printf(" Firmware Version : %s\r\n", FIRM_WARE_VERSION);
  printf(" Firmware Date    : %s\r\n", FIRM_WARE_DATE);
  printf(" Hardware/PCB Ver : %s\r\n", PCB_VERSION);
  printf(" Protocol Version : %u\r\n", (unsigned int)PROTOCAL_VERSION);
  printf(" Device Type      : %u\r\n", (unsigned int)DEVICE_TYPE);
  printf("========================================\r\n\r\n");
}
/* USER CODE END 0 */

/**
  * @brief 应用程序入口，完成 HAL、外设、内存和 FreeRTOS 内核的初始化并启动调度器。
  * @param 无。
  * @retval int 正常运行流程中调度器启动后不会返回；保留 int 返回类型以符合 C 程序入口约定。
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU 配置--------------------------------------------------------*/

  /* 复位外设，并初始化 Flash 接口及系统滴答定时器。 */
  HAL_Init();

  /* USER CODE BEGIN Init */
	sys_stm32_clock_init(336, 8, 2, 7); /* 配置 168 MHz 系统时钟。 */
	delay_init(168);                    /* 根据 168 MHz 主频初始化延时模块。 */
	usart_init(115200);                 /* 初始化波特率为 115200 的串口。 */
	usmart_dev.init(84);                /* 初始化 USMART 调试模块。 */

  /* USER CODE END Init */

  /* 配置系统时钟 */
//  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* 初始化已配置的外设 */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_SPI3_Init();
//  MX_USART1_UART_Init();
//  MX_USART2_UART_Init();
//  MX_USART3_UART_Init();
//  MX_UART4_Init();
  MX_ADC1_Init();
  MX_DAC_Init();
//  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */
	my_mem_init(SRAMIN);                /* 初始化内部 SRAM 内存池。 */
//	my_mem_init(SRAMEX);                /* 初始化外部 SRAM 内存池（当前未启用）。 */
	my_mem_init(SRAMCCM);               /* 初始化内部 CCM 内存池。 */

  /* USER CODE END 2 */

	Print_SystemVersion();

  /* 初始化 FreeRTOS 调度器及其对象 */
  osKernelInitialize();  /* 初始化 CMSIS-RTOS2 内核 */
  MX_FREERTOS_Init();

  /* 启动 FreeRTOS 调度器 */
  osKernelStart();

  /* 调度器启动后接管程序控制权，正常情况下不会执行到此处 */

  /* 无限循环；调度器启动后通常不会进入此处 */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief 使用外部高速时钟和 PLL 配置系统时钟及 AHB、APB 总线时钟。
  * @param 无。
  * @retval 无；时钟配置失败时调用 Error_Handler 进入错误处理。
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** 配置内部稳压器的输出电压
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** 按指定参数初始化 RCC 振荡器
  * 参数由 RCC_OscInitTypeDef 结构体提供。
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** 初始化 CPU、AHB 和 APB 总线时钟
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
 * @brief 执行设备启动初始化，包括清除旧状态、波长寻零、移动至 500 nm、完成提示及硬件收尾。
 * @param 无。
 * @retval 无。
 * @note 本函数必须运行于 FreeRTOS 任务上下文，并通过 MyInitFlag 事件组发布初始化结果。
 *       DeviceInitFlag 状态值为 0（未初始化或已中止）、1（初始化中）、2（初始化完成）。
 */
void DeviceInit(void)
{
	enum
	{
		DEVICE_INIT_NOT_STARTED = 0U,
		DEVICE_INIT_IN_PROGRESS = 1U,
		DEVICE_INIT_COMPLETE = 2U
	};
	const EventBits_t initResultBits = INIT_WL_GOOD_FLAG | INIT_WL_ERR_FLAG |
		INIT_WL_500_GOOD_FLAG | INIT_WL_500_ERR_FLAG | INIT_ALL_GOOD_FLAG;
	uint8_t homeResult;
	
	BSP_IR_Emit_Control(1);


	if (MyInitFlag == NULL)
	{
		SysFlag.DeviceInitFlag = DEVICE_INIT_NOT_STARTED;
		__log("设备初始化失败：初始化事件组未创建\r\n");
		return;
	}

	(void)xEventGroupClearBits(MyInitFlag, initResultBits);
	(void)osDelay(pdMS_TO_TICKS(4000U));

	SysFlag.DeviceInitFlag = DEVICE_INIT_IN_PROGRESS;
	__log("开始初始化，系统运行时间：%lu 秒\r\n",
		(unsigned long)(osKernelGetTickCount() / osKernelGetTickFreq()));

	(void)osDelay(pdMS_TO_TICKS(1000U));

	__log("开始查找波长初始位置\r\n");
	homeResult = Find_Wavelength_init_pos();
	__log("波长初始位置查找结束\r\n");
	if (homeResult != 0U)
	{
		(void)xEventGroupSetBits(MyInitFlag, INIT_WL_ERR_FLAG);
		SysFlag.DeviceInitFlag = DEVICE_INIT_NOT_STARTED;
		__log("设备初始化失败：未能找到波长初始位置\r\n");
		return;
	}

	(void)xEventGroupSetBits(MyInitFlag, INIT_WL_GOOD_FLAG);
	__log("波长初始位置初始化成功\r\n");

	if (SysFlag.StopInitFlag == 1U)
	{
		SysFlag.StopInitFlag = 0U;
		SysFlag.DeviceInitFlag = DEVICE_INIT_NOT_STARTED;
		return;
	}

	(void)osDelay(pdMS_TO_TICKS(1000U));
	MoveToWaveLen(5000);
	(void)xEventGroupSetBits(MyInitFlag, INIT_WL_500_GOOD_FLAG);
	__log("已移动至 500 nm\r\n");

	(void)osDelay(pdMS_TO_TICKS(1000U));
	BEEP_ONCE();
	(void)osDelay(pdMS_TO_TICKS(200U));
	BEEP_ONCE();
	(void)osDelay(pdMS_TO_TICKS(200U));
	BEEP_ONCE();
	(void)osDelay(pdMS_TO_TICKS(200U));

	TMC5130_ConfigReferenceSwitches(0U, 0U, 0U, 1U);
	(void)osDelay(pdMS_TO_TICKS(1000U));
	BSP_IR_Emit_Control(0U);

	(void)xEventGroupSetBits(MyInitFlag, INIT_ALL_GOOD_FLAG);
	SysFlag.DeviceInitFlag = DEVICE_INIT_COMPLETE;
	SysFlag.KeyDebugFlag = 0U;
	__log("设备初始化完成，系统运行时间：%lu 秒\r\n",
		(unsigned long)(osKernelGetTickCount() / osKernelGetTickFreq()));
}

/* USER CODE END 4 */

/**
  * @brief HAL 定时器周期回调；TIM6 更新中断发生时递增 HAL 系统时基。
  * @param htim 发生周期更新事件的 HAL 定时器句柄。
  * @retval 无。
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM6)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief HAL 错误处理函数；关闭全局中断并停留在死循环，避免错误状态下继续运行。
  * @param 无。
  * @retval 无。
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* 可在此处添加 HAL 错误状态的记录或上报处理。 */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief 完整断言失败回调，接收触发断言的源文件及代码行号。
  * @param file 发生断言失败的源文件路径字符串。
  * @param line 发生断言失败的源代码行号。
  * @retval 无。
  * @note 当前实现为空，可在此处添加断言错误的记录或报告逻辑。
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* 可在此处添加断言失败文件名和行号的记录或上报处理，
     ex: printf("参数错误：文件 %s，第 %d 行\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* 仅在启用完整断言功能时编译。 */
