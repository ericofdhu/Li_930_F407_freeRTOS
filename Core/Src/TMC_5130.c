//TMC_5130
#include "main.h"
#include "spi.h"
#include "cmsis_os2.h"
#include "system_runtime.h"
#include "tmc_5130.h"
#include <stdio.h>


static osMutexId_t TMC5130_CS_Mutex;

void Delay(int32_t m)
{
	m = m * 5;
	while (m--);
}


unsigned char TMC_SPI_Byte_Write(unsigned char byte)
{
	uint8_t receivedByte = 0U;

	if (HAL_SPI_TransmitReceive(&hspi3, &byte, &receivedByte, 1U, 100U) != HAL_OK)
	{
		Error_Handler();
	}

	return receivedByte;
}

unsigned char TMC_SPI_Byte_Read(void)
{
  /* Return the byte read from the SPI bus */
  return TMC_SPI_Byte_Write(0x00);
}

void TMC5130_SetCS(uint8_t state)
{
	if (state != 0U)
	{
		HAL_GPIO_WritePin(TMC_SPI_CS_GPIO_Port, TMC_SPI_CS_Pin, GPIO_PIN_SET);
		if ((TMC5130_CS_Mutex == NULL) ||
		    (osMutexRelease(TMC5130_CS_Mutex) != osOK))
		{
			Error_Handler();
		}
	}
	else
	{
		if ((TMC5130_CS_Mutex == NULL) ||
		    (osMutexAcquire(TMC5130_CS_Mutex, osWaitForever) != osOK))
		{
			Error_Handler();
		}
		HAL_GPIO_WritePin(TMC_SPI_CS_GPIO_Port, TMC_SPI_CS_Pin, GPIO_PIN_RESET);
	}
}
void TMC5130_SetEN(uint8_t state)
{
    if(state)
        HAL_GPIO_WritePin(TMC_EN_GPIO_Port, TMC_EN_Pin, GPIO_PIN_SET);
    else
        HAL_GPIO_WritePin(TMC_EN_GPIO_Port, TMC_EN_Pin, GPIO_PIN_RESET);
}




uint8_t TMC5130_WriteRegister(uint8_t address, int32_t data)
{
    uint8_t buffer[5];
    
    buffer[0] = address | 0x80;
    buffer[1] = (data >> 24) & 0xFF;
    buffer[2] = (data >> 16) & 0xFF;
    buffer[3] = (data >> 8) & 0xFF;
    buffer[4] = data & 0xFF;
    
    TMC5130_SetCS(0);
    
    for(int i = 0; i < 5; i++)
    {
        buffer[i] = TMC_SPI_Byte_Write(buffer[i]);
    }
    
    TMC5130_SetCS(1);
    
    Delay(5);
		
	return buffer[0];
}
uint32_t TMC5130_ReadRegister(uint8_t address)
{
    uint8_t buffer[5];
    uint32_t result = 0;
    
//    buffer[0] = address | 0x80;
    buffer[0] = address & 0x7F;
    
    TMC5130_SetCS(0);
    Delay(10);
    
    for(int i = 0; i < 5; i++)
    {
        if(i == 0)
            buffer[i] = TMC_SPI_Byte_Write(buffer[i]);
        else
            buffer[i] = TMC_SPI_Byte_Write(0x00);
    }
		
    Delay(3);
    TMC5130_SetCS(1);
    Delay(3);
    TMC5130_SetCS(0);
		
    buffer[0] = address & 0x7F;
    for(int i = 0; i < 5; i++)
    {
        if(i == 0)
            buffer[i] = TMC_SPI_Byte_Write(buffer[i]);
        else
            buffer[i] = TMC_SPI_Byte_Write(0x00);
    }
		
    Delay(10);
    TMC5130_SetCS(1);
    
    result = ((uint32_t)buffer[1] << 24) | ((uint32_t)buffer[2] << 16) | 
             ((uint32_t)buffer[3] << 8) | buffer[4];
    
    return result;
}

uint32_t TMC5130_ReadRegister_stat(uint8_t address,uint8_t* stat)
{
    uint8_t buffer[5];
    uint32_t result = 0;
    
//    buffer[0] = address | 0x80;
    buffer[0] = address & 0x7F;
    
    TMC5130_SetCS(0);
    Delay(10);
    
    for(int i = 0; i < 5; i++)
    {
        if(i == 0)
            buffer[i] = TMC_SPI_Byte_Write(buffer[i]);
        else
            buffer[i] = TMC_SPI_Byte_Write(0x00);
    }
    
    Delay(3);
    TMC5130_SetCS(1);
    Delay(3);
    TMC5130_SetCS(0);
		
    buffer[0] = address & 0x7F;
    for(int i = 0; i < 5; i++)
    {
        if(i == 0)
            buffer[i] = TMC_SPI_Byte_Write(buffer[i]);
        else
            buffer[i] = TMC_SPI_Byte_Write(0x00);
    }
    Delay(10);
    TMC5130_SetCS(1);
    
    result = ((uint32_t)buffer[1] << 24) | ((uint32_t)buffer[2] << 16) | 
             ((uint32_t)buffer[3] << 8) | buffer[4];
		
		*stat = buffer[0];
    
    return result;
}

unsigned char TMC_5130_Read_Stat(void)
{
    uint8_t buffer[5];
    uint32_t result = 0;
    result = result;
    buffer[0] = TMC5130_GSTAT | 0x80;
    
    TMC5130_SetCS(0);
    Delay(10);
    
    for(int i = 0; i < 5; i++)
    {
        if(i == 0)
            buffer[i] = TMC_SPI_Byte_Write(buffer[i]);
        else
            buffer[i] = TMC_SPI_Byte_Write(0x00);
    }
    
    Delay(10);
    TMC5130_SetCS(1);
    
    result = ((uint32_t)buffer[1] << 24) | ((uint32_t)buffer[2] << 16) | 
             ((uint32_t)buffer[3] << 8) | buffer[4];
    
    return buffer[0];

}


unsigned char TMC_5130_Init(void)
{
    uint32_t status;
//		int32_t set_pos;
//		int32_t cur_pos;
    
    if (TMC5130_CS_Mutex == NULL)
    {
        TMC5130_CS_Mutex = osMutexNew(NULL);
        if (TMC5130_CS_Mutex == NULL)
        {
            Error_Handler();
        }
    }
	
    
//    TMC5130_SetCS(1);  
    TMC5130_SetEN(1);  
    
    Delay(100);
    
    TMC5130_SetEN(0);
    Delay(10);
	
//		TMC5130_SetCS(0);  

		status=status;
		status = TMC_5130_Read_Stat();

//    TMC5130_WriteRegister(TMC5130_GCONF, 0x00000000);
    status = TMC5130_ReadRegister(TMC5130_GCONF);
    TMC5130_WriteRegister(TMC5130_CHOPCONF, 0x000100C3);// CHOPCONF: TOFF=3, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
    status = TMC5130_ReadRegister(TMC5130_CHOPCONF);
    status = TMC5130_WriteRegister(TMC5130_IHOLD_IRUN, 0x00061F05);// IHOLD_IRUN: IHOLD=10, IRUN=31 (max. current), IHOLDDELAY=6
    status = TMC5130_WriteRegister(TMC5130_TPOWERDOWN, 0x0000000A);//TPOWERDOWN=10: Delay before power down in stand still
    status = TMC5130_WriteRegister(TMC5130_GCONF, 0x00000006); //internal _rsense = 1,EN_PWM_MODE=1 enables stealthChop (with default PWM_CONF)
		status = TMC5130_WriteRegister(TMC5130_TPWMTHRS, 0x000001F4);// TPWM_THRS=500 yields a switching velocity about 35000 = ca. 30RPM
    status = TMC5130_WriteRegister(TMC5130_PWMCONF, 0x000401C8);// PWM_CONF: AUTO=1, 2/1024 Fclk, Switch amplitude limit=200, Grad=1
    TMC5130_ConfigReferenceSwitches(1, 0, 0, 1);  
    
//    status = TMC5130_WriteRegister(TMC5130_VSTART, 20);     //SPI send: 0xAB0000000A; // VSTOP = 10 Stop velocity (Near to zero)
//    status = TMC5130_WriteRegister(TMC5130_A1, 15);    //SPI send: 0xA4000003E8; // A1 = 1 000 First acceleration
//    status = TMC5130_WriteRegister(TMC5130_V1, 20);   //SPI send: 0xA50000C350; // V1 = 50 000 Acceleration threshold velocity V1
//    status = TMC5130_WriteRegister(TMC5130_AMAX, 20);   //SPI send: 0xA6000001F4; // AMAX = 500 Acceleration above V1
//    status = TMC5130_WriteRegister(TMC5130_VMAX, 500); //SPI send: 0xA700030D40; // VMAX = 200 000
//    status = TMC5130_WriteRegister(TMC5130_DMAX, 20);    //SPI send: 0xA8000002BC; // DMAX = 700 Deceleration above V1
//    status = TMC5130_WriteRegister(TMC5130_D1, 15);     //SPI send: 0xAA00000578; // D1 = 1400 Deceleration below V1
//    status = TMC5130_WriteRegister(TMC5130_VSTOP, 10);     //SPI send: 0xAB0000000A; // VSTOP = 10 Stop velocity (Near to zero)
//    status = TMC5130_WriteRegister(TMC5130_RAMPMODE, 0);     //SPI send: 0xA000000000; // RAMPMODE = 0 (Target position move)



#define SPEED_MULTI			5
    status = TMC5130_WriteRegister(TMC5130_VSTART, 20*SPEED_MULTI);     //SPI send: 0xAB0000000A; // VSTOP = 10 Stop velocity (Near to zero)
    status = TMC5130_WriteRegister(TMC5130_A1, 15*SPEED_MULTI);    //SPI send: 0xA4000003E8; // A1 = 1 000 First acceleration
    status = TMC5130_WriteRegister(TMC5130_V1, 20*SPEED_MULTI);   //SPI send: 0xA50000C350; // V1 = 50 000 Acceleration threshold velocity V1
    status = TMC5130_WriteRegister(TMC5130_AMAX, 20*SPEED_MULTI);   //SPI send: 0xA6000001F4; // AMAX = 500 Acceleration above V1
    status = TMC5130_WriteRegister(TMC5130_VMAX, 500*SPEED_MULTI); //SPI send: 0xA700030D40; // VMAX = 200 000
    status = TMC5130_WriteRegister(TMC5130_DMAX, 20*SPEED_MULTI);    //SPI send: 0xA8000002BC; // DMAX = 700 Deceleration above V1
    status = TMC5130_WriteRegister(TMC5130_D1, 15*SPEED_MULTI);     //SPI send: 0xAA00000578; // D1 = 1400 Deceleration below V1
    status = TMC5130_WriteRegister(TMC5130_VSTOP, 10*SPEED_MULTI);     //SPI send: 0xAB0000000A; // VSTOP = 10 Stop velocity (Near to zero)
    status = TMC5130_WriteRegister(TMC5130_RAMPMODE, 0);     //SPI send: 0xA000000000; // RAMPMODE = 0 (Target position move)


//    status = TMC5130_ReadRegister(TMC5130_IOIN);
		


//		TMC5130_MoveToPosition(-51200);
		
    
    return 1;  
}




void TMC5130_Set_Scan_para(uint32_t speed)
{
		uint32_t speed_multi = 1;
    uint32_t status = 0;
		status=status;
	
		if(speed == SCAN_SPEED_FAST)
		{
			speed_multi = 20;
		}
		else if(speed == SCAN_SPEED_MIDDLE)
		{
			speed_multi = 10;
		}
		else if(speed == SCAN_SPEED_SLOW)
		{
			speed_multi = 5;
		}
		
    status = TMC5130_WriteRegister(TMC5130_VSTART, 20*speed_multi);     //SPI send: 0xAB0000000A; // VSTOP = 10 Stop velocity (Near to zero)
    status = TMC5130_WriteRegister(TMC5130_A1, 15*speed_multi);    //SPI send: 0xA4000003E8; // A1 = 1 000 First acceleration
    status = TMC5130_WriteRegister(TMC5130_V1, 20*speed_multi);   //SPI send: 0xA50000C350; // V1 = 50 000 Acceleration threshold velocity V1
    status = TMC5130_WriteRegister(TMC5130_AMAX, 20*speed_multi);   //SPI send: 0xA6000001F4; // AMAX = 500 Acceleration above V1
    status = TMC5130_WriteRegister(TMC5130_VMAX, 500*speed_multi); //SPI send: 0xA700030D40; // VMAX = 200 000
    status = TMC5130_WriteRegister(TMC5130_DMAX, 20*speed_multi);    //SPI send: 0xA8000002BC; // DMAX = 700 Deceleration above V1
    status = TMC5130_WriteRegister(TMC5130_D1, 15*speed_multi);     //SPI send: 0xAA00000578; // D1 = 1400 Deceleration below V1
    status = TMC5130_WriteRegister(TMC5130_VSTOP, 10*speed_multi);     //SPI send: 0xAB0000000A; // VSTOP = 10 Stop velocity (Near to zero)
    status = TMC5130_WriteRegister(TMC5130_RAMPMODE, 0);     //SPI send: 0xA000000000; // RAMPMODE = 0 (Target position move)
		
		
}




void TMC5130_MoveToPosition(int32_t target_position)
{
    
    TMC5130_WriteRegister(TMC5130_XTARGET, target_position);
    
    Delay(1);
}
void TMC5130_MoveToPositionWithSpeed(int32_t target_position, uint32_t speed)
{
    TMC5130_SetMaxSpeed(speed);
    
    TMC5130_MoveToPosition(target_position);
}
void TMC5130_SetRampMode(uint8_t mode)
{
    TMC5130_WriteRegister(TMC5130_RAMPMODE, mode);
}
void TMC5130_Stop(void)
{
    int32_t current_pos = TMC5130_GetActualPosition();
    current_pos = TMC5130_GetActualPosition();
    TMC5130_WriteRegister(TMC5130_XTARGET, current_pos);
}

void TMC5130_Set_Zero_Pos(void)
{
//    int32_t current_pos = TMC5130_GetActualPosition();
//    current_pos = TMC5130_GetActualPosition();
//    TMC5130_WriteRegister(TMC5130_XTARGET, current_pos);
		TMC5130_WriteRegister(TMC5130_XACTUAL, 0);
		TMC5130_WriteRegister(TMC5130_XTARGET, 0);
}


void TMC5130_Homing(int32_t vmax)
{
    TMC5130_WriteRegister(TMC5130_VMAX, vmax); //SPI send: 0xA700030D40; // VMAX = 200 000
    TMC5130_WriteRegister(TMC5130_RAMPMODE, 2);
	

}
void TMC5130_EmergencyStop(void)
{
    TMC5130_WriteRegister(TMC5130_VMAX, 0);
    
    Delay(10);
    
    TMC5130_WriteRegister(TMC5130_VMAX, 100000);
}
int32_t TMC5130_GetActualPosition(void)
{
    return (int32_t)TMC5130_ReadRegister(TMC5130_XACTUAL);
}
uint8_t TMC5130_GetMotionStatus(void)
{
    uint8_t stat = 0;
    uint32_t status = TMC5130_ReadRegister(TMC5130_RAMPMODE);
    uint32_t actual_pos = TMC5130_ReadRegister(TMC5130_XACTUAL);
    uint32_t target_pos = TMC5130_ReadRegister_stat(TMC5130_XTARGET, &stat);
    
    if(actual_pos == target_pos)
    {
//			printf("ACTUAL POS:%6d\r\n",actual_pos);
        return STATUS_TARGET_REACHED;
    }
		
		if(stat & (0x01<<7))//status_stop_right
		{
			printf("stop max position\r\n");
			return STATUS_REACH_MAX_POS;
		}
    
    switch(status & 0x03)
    {
        case 0: 
            return STATUS_ACCELERATING; 
        case 1: 
            return STATUS_CRUISING;
        case 2: 
            return STATUS_CRUISING;
        case 3: 
            return STATUS_IDLE;
        default:
            return STATUS_IDLE;
    }
}
void TMC5130_WaitForMotionComplete(void)
{
    uint32_t timeout = 0;
    
    while(TMC5130_GetMotionStatus() != STATUS_TARGET_REACHED)
    {
        Delay(1);
        timeout++;
        
        if(timeout > 5000) 
        {
            break;
        }
    }
}
void TMC5130_SetMaxSpeed(uint32_t max_speed)
{
    TMC5130_WriteRegister(TMC5130_VMAX, max_speed);
}
void TMC5130_SetAcceleration(uint32_t acceleration)
{
    TMC5130_WriteRegister(TMC5130_AMAX, acceleration);
    TMC5130_WriteRegister(TMC5130_A1, acceleration);
}
void TMC5130_SetDeceleration(uint32_t deceleration)
{
    TMC5130_WriteRegister(TMC5130_D1, deceleration);
}


void TMC5130_ConfigReferenceSwitches(uint8_t refL_enable, uint8_t refR_enable, uint8_t refL_polarity, uint8_t refR_polarity)
{
    uint32_t sw_mode_config = 0;
    
    if(refL_enable)
        sw_mode_config |= REFL_ENABLE;
    if(refL_polarity)
        sw_mode_config |= REFL_POL_INVERTED;
    
    if(refR_enable)
        sw_mode_config |= REFR_ENABLE;
    if(refR_polarity)
        sw_mode_config |= REFR_POL_INVERTED;
    
    TMC5130_WriteRegister(TMC5130_SW_MODE, sw_mode_config);
}
void TMC5130_EnableRefL(void)
{
    uint32_t current_config = TMC5130_ReadRegister(TMC5130_SW_MODE);
    current_config |= REFL_ENABLE;
    TMC5130_WriteRegister(TMC5130_SW_MODE, current_config);
}
void TMC5130_DisableRefL(void)
{
    uint32_t current_config = TMC5130_ReadRegister(TMC5130_SW_MODE);
    current_config &= ~REFL_ENABLE;
    TMC5130_WriteRegister(TMC5130_SW_MODE, current_config);
}
void TMC5130_EnableRefR(void)
{
    uint32_t current_config = TMC5130_ReadRegister(TMC5130_SW_MODE);
    current_config |= REFR_ENABLE;
    TMC5130_WriteRegister(TMC5130_SW_MODE, current_config);
}
void TMC5130_DisableRefR(void)
{
    uint32_t current_config = TMC5130_ReadRegister(TMC5130_SW_MODE);
    current_config &= ~REFR_ENABLE;
    TMC5130_WriteRegister(TMC5130_SW_MODE, current_config);
}
uint8_t TMC5130_GetRefLStatus(void)
{
    uint32_t io_in = TMC5130_ReadRegister(TMC5130_IOIN);
    return (io_in & (1UL << 5)) ? 1 : 0;
}
uint8_t TMC5130_GetRefRStatus(void)
{
    uint32_t io_in = TMC5130_ReadRegister(TMC5130_IOIN);
    return (io_in & (1UL << 4)) ? 1 : 0;
}
