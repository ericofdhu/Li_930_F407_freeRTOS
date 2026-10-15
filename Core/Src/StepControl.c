

#include "math.h"
#include "TMC_5130.h"
#include "wavelength_transfor.h"
#include "StepControl.h"
#include "FreeRTOS.h"
#include "system_runtime.h"
#include "MyLib.h"
#include <stdio.h>

volatile _STEPPER_CONTROL_STAT     StepperControlStat[4];
volatile unsigned char  StepperIndex = 0;


#define MAX_ARRIVE_CNT			1000
uint8_t Find_Wavelength_init_pos(void)
{
	int32_t start_pos, cur_pos;
	uint8_t stat=0;
	
	TMC5130_SetMaxSpeed(TMC5130_HOMING_MOVE_SPEED); //SPI send: 0xA700030D40; // VMAX = 200 000

	start_pos = TMC5130_GetActualPosition();
	printf("起始位置: %d\r\n", start_pos);
	
	/* 正向运动 */
	printf("反向运动...找零位\r\n");
	TMC5130_MoveToPosition(start_pos - 256*800);
		
	while(1)
	{
		cur_pos = TMC5130_ReadRegister_stat(TMC5130_XACTUAL,&stat);
//		printf("pos= %10d stat= 0x%02x\r\n",cur_pos,stat);
		if(stat & 0x40)//到达0位
		{
				printf("到达0位置--0\r\n");
				TMC5130_SetMaxSpeed(150); //SPI send: 0xA700030D40; // VMAX = 200 000
				while(1)
				{
					TMC5130_MoveToPosition(cur_pos + 1);
					osDelay(pdMS_TO_TICKS(200));
					cur_pos = TMC5130_ReadRegister_stat(TMC5130_XACTUAL,&stat);
					printf("pos= %10d stat= 0x%02x\r\n",cur_pos,stat);
					if(stat & 0x40)
					{}
					else
					{
						break;
					}												
				}
				printf("到达0位置--1\r\n");
			
				TMC5130_Set_Zero_Pos();
				printf("寻找波长初始位置完成\r\n");
				CurWaveLen = 0;
				StepperControlStat[WaveLenStepper].Cnt = 0;
				return 0X00;
		}
		else if(stat & 0x20)//如果 position_reached，就说明波长电机有问题。因为肯定转不到1圈
		{
				TMC5130_Stop();
				printf("无法找到初始位置\r\n");
				return 0X01;
		}
		osDelay(pdMS_TO_TICKS(500));

	}		

}


/**
 * @brief 按指定步数控制指定步进电机移动。
 * @param StepIndex 步进电机索引。
 * @param dir 运动方向参数。
 * @param n 需要移动的步数，可为正数或负数。
 * @retval 无。
 * @note 当前实现只处理波长步进电机，并依据实际位置下发目标位置。
 */
void MoveNStep(uint8_t StepIndex, uint8_t dir, int32_t n)
{
	int32_t start_pos;
	int32_t stepDelta;
	int wl;

	stepDelta = ((dir == BACKWARD) && (n > 0)) ? -n : n;
	switch(StepIndex)
	{
		case WaveLenStepper:
			
			/* 获取当前位置 */
			start_pos = TMC5130_GetActualPosition();
			wl = PositionToWavelength(start_pos);
			printf("起始位置:pos=%5d,calc_wl=%5d\r\n",start_pos,wl);
			
			/* 正向运动 */
			TMC5130_MoveToPosition(start_pos + stepDelta);
			wl = PositionToWavelength(start_pos + stepDelta);
			printf("目标位置:pos=%5d,calc_wl=%5d\r\n",start_pos + stepDelta,wl);
		
			break;
		default:
			printf("other stepper\r\n");
		break;
	}

}							  




/**
 * @brief 通过波长电机命令队列提交目标波长并等待移动完成。
 * @param DesWaveLen 目标波长值，单位与波长换算函数一致。
 * @retval 无。
 * @note 队列或事件组未初始化、命令提交失败时记录错误并返回。
 */
void MoveToWaveLen(int32_t DesWaveLen)
{
	StepperCommandMessage_t message;
	EventBits_t eventBits;

	if ((g_WaveLengthStepperQueue == NULL) || (MyOsFlag == NULL))
	{
		printf("Wavelength stepper queue or event group is not ready\r\n");
		return;
	}

	message.command = STEPPER_CMD_MOVE_TO_WAVELENGTH;
	message.value.targetWavelength = DesWaveLen;
	(void)xEventGroupClearBits(MyOsFlag, TASK_WAVE_LENGTH_FINISH_FLAG);

	if (xQueueSend(g_WaveLengthStepperQueue, &message, portMAX_DELAY) != pdPASS)
	{
		printf("Failed to send wavelength stepper command\r\n");
		return;
	}

	eventBits = xEventGroupWaitBits(MyOsFlag,
	                                TASK_WAVE_LENGTH_FINISH_FLAG,
	                                pdTRUE,
	                                pdTRUE,
	                                portMAX_DELAY);
	if ((eventBits & TASK_WAVE_LENGTH_FINISH_FLAG) == 0U)
	{
		printf("Wavelength stepper completion was not signaled\r\n");
	}
}

void MoveToWL_With_Speed(int32_t DesWL,int32_t speed)
{
  int32_t tmpDesWavePos;


	TMC5130_SetMaxSpeed(speed); //SPI send: 0xA700030D40; // VMAX = 200 000

	tmpDesWavePos = WavelengthToPosition(DesWL);
	
	
	TMC5130_MoveToPosition(tmpDesWavePos);
	int32_t wl = PositionToWavelength(tmpDesWavePos);
//	printf("目标位置:pos=%5d,calc_wl=%5d\r\n",tmpDesWavePos,wl);

	while(TMC5130_GetMotionStatus() != STATUS_TARGET_REACHED)
	{
		_delay_us(100);
	}
	// OSSemPend(StepReachSem[WaveLenStepper],0,&err);
	CurWaveLen = DesWL;
	StepperControlStat[WaveLenStepper].Cnt = tmpDesWavePos;

}



void MoveToWL_WithOut_Speed(int32_t DesWL)
{
  int32_t tmpDesWavePos;

	tmpDesWavePos = WavelengthToPosition(DesWL);
	
	
	TMC5130_MoveToPosition(tmpDesWavePos);
	int32_t wl = PositionToWavelength(tmpDesWavePos);
//	printf("目标位置:pos=%5d,calc_wl=%5d\r\n",tmpDesWavePos,wl);
	
	while(1)
	{
		uint32_t actual_pos = TMC5130_ReadRegister(TMC5130_XACTUAL);
		if(actual_pos == tmpDesWavePos)
		{
			break;
		}
		else
		{
			_delay_us(10);
		}
	}

	CurWaveLen = DesWL;
	StepperControlStat[WaveLenStepper].Cnt = tmpDesWavePos;

}




















