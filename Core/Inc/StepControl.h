//STEP.H
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STEP_CONTROL_H
#define __STEP_CONTROL_H

#include "FreeRTOS.h"
#include "queue.h"
#include <stdint.h>



#define WaveLenStepper				0



#define FORWARD		0
#define BACKWARD 1

#define STEPPER_CMD_MOVE_TO_WAVELENGTH  1U
#define STEPPER_CMD_INITIALIZE          2U
#define STEPPER_CMD_CORRECT_WAVELENGTH  3U
#define TASK_WAVE_LENGTH_FINISH_FLAG    (1UL << 7U)

typedef struct
{
	uint8_t command;
	union
	{
		int32_t targetWavelength;
		float correction;
	} value;
} StepperCommandMessage_t;

extern QueueHandle_t g_WaveLengthStepperQueue;





extern volatile unsigned char  StepperIndex;

typedef struct {
	unsigned char err;//错误
	int32_t  Cnt;//当前相对于初始位置走的步数，为了在移动波长的时候，可以方便知道当前波长
}_STEPPER_CONTROL_STAT ;

extern volatile _STEPPER_CONTROL_STAT     StepperControlStat[4];



#define   OPEN_SLIT_IR()			//GPIO_SetBits(GPIOD, (0x01<<1) );GPIO_SetBits(GPIOD, (0x01<<0) )
#define   CLOSE_SLIT_IR()			//GPIO_ResetBits(GPIOD, (0x01<<1) );GPIO_ResetBits(GPIOD, (0x01<<0) )
#define   OPEN_WAVE_LEN_IR()		//GPIO_SetBits(GPIOE, (0x01<<1) )
#define   CLOSE_WAVE_LEN_IR()		//GPIO_ResetBits(GPIOE, (0x01<<1) )



void MoveNStep(uint8_t StepIndex, uint8_t dir, int32_t n);
void MoveToWaveLen(int32_t DesWaveLen);
uint8_t Find_Wavelength_init_pos(void);


void MoveToWL_With_Speed(int32_t DesWL,int32_t speed);
void MoveToWL_WithOut_Speed(int32_t DesWL);



















#endif



