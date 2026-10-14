//color_lcd
/****************************************Copyright (c)**************************************************
**                               �Ϻ����ܿ�ѧ�������޹�˾
**
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: color_lcd.c
**��   ��   ��: ʷ��
**����޸�����??: 2011��9��27��
**��        ��: ��COS-II�²���Ӧ�ó������ļ�
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: ʷ��
** ��  ��: v1.0
** �ա���: 2011��9��27��
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�------------------------------------------------------------------------------
** �޸���: 
** �ա���: 
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h>
#include "usart2_process.h"
#include <stdint.h>
#include "ColorLcd.h"
#include "stm32f4xx_hal.h"
#include "ColorWM_8inch.h"


static uint8_t ColorLcdTxFrame[USART2_PROCESS_MAX_FRAME_LENGTH + 5U];
static uint16_t ColorLcdTxFrameLength;
static uint8_t ColorLcdTxOverflow;
static StaticSemaphore_t ColorLcdTxMutexControl;
static const osMutexAttr_t ColorLcdTxMutexAttributes =
{
	"ColorLcdTx",
	osMutexPrioInherit,
	&ColorLcdTxMutexControl,
	sizeof(ColorLcdTxMutexControl)
};
static osMutexId_t ColorLcdTxMutex;
static osThreadId_t volatile ColorLcdTxOwner;
volatile HAL_StatusTypeDef ColorLcdDmaSendStatus = HAL_ERROR;


COLOR_LCD_CMD_STRUCT ColorLcdSendFrame; 



static uint8_t ColorLcdComPortIsOwner(void)
{
	return (ColorLcdTxOwner != NULL) &&
	       (ColorLcdTxOwner == osThreadGetId());
}

static void ColorLcdComPortBegin(void)
{
	osMutexId_t mutex;

	taskENTER_CRITICAL();
	if(ColorLcdTxMutex == NULL)
	{
		ColorLcdTxMutex = osMutexNew(&ColorLcdTxMutexAttributes);
	}
	mutex = ColorLcdTxMutex;
	taskEXIT_CRITICAL();

	if((mutex == NULL) ||
	   (osMutexAcquire(mutex, osWaitForever) != osOK))
	{
		ColorLcdDmaSendStatus = HAL_ERROR;
		return;
	}

	ColorLcdTxOwner = osThreadGetId();
	if(ColorLcdTxOwner == NULL)
	{
		(void)osMutexRelease(mutex);
		ColorLcdDmaSendStatus = HAL_ERROR;
		return;
	}
	ColorLcdTxFrameLength = 0U;
	ColorLcdTxOverflow = 0U;
}

static void ColorLcdComPortAppend(const uint8_t *data, uint16_t length)
{
	uint16_t index;

	if(ColorLcdComPortIsOwner() == 0U)
	{
		return;
	}

	if((data == NULL) && (length != 0U))
	{
		ColorLcdTxOverflow = 1U;
		return;
	}

	for(index = 0U; index < length; index++)
	{
		if(ColorLcdTxFrameLength >= (uint16_t)sizeof(ColorLcdTxFrame))
		{
			ColorLcdTxOverflow = 1U;
			return;
		}
		ColorLcdTxFrame[ColorLcdTxFrameLength++] = data[index];
	}
}

static void ColorLcdComPortSendByte(uint8_t byte)
{
	ColorLcdComPortAppend(&byte, 1U);
}

static void ColorLcdComPortSendWord(uint16_t word)
{
	uint8_t bytes[2];

	bytes[0] = (uint8_t)(word >> 8);
	bytes[1] = (uint8_t)word;
	ColorLcdComPortAppend(bytes, (uint16_t)sizeof(bytes));
}

static void ColorLcdComPortSendString(char *str)
{
	size_t length;

	if(str == NULL)
	{
		if(ColorLcdComPortIsOwner() != 0U)
		{
			ColorLcdTxOverflow = 1U;
		}
		return;
	}
	if(ColorLcdComPortIsOwner() == 0U)
	{
		return;
	}
	length = strlen(str);
	if(length > (sizeof(ColorLcdTxFrame) - ColorLcdTxFrameLength))
	{
		ColorLcdTxOverflow = 1U;
		return;
	}
	ColorLcdComPortAppend((const uint8_t *)str, (uint16_t)length);
}

static void ColorLcdComPortSendDat(uint8_t *dat, uint8_t len)
{
	ColorLcdComPortAppend(dat, len);
}

static void ColorLcdComPortEnd(void)
{
	uint16_t payloadLength;

	if(ColorLcdComPortIsOwner() == 0U)
	{
		ColorLcdDmaSendStatus = HAL_ERROR;
		return;
	}

	if((ColorLcdTxOverflow == 0U) &&
	   (ColorLcdTxFrameLength >= 5U) &&
	   (ColorLcdTxFrame[0] == USART2_PROCESS_FRAME_START) &&
	   (ColorLcdTxFrame[ColorLcdTxFrameLength - 4U] == USART2_PROCESS_FRAME_END_0) &&
	   (ColorLcdTxFrame[ColorLcdTxFrameLength - 3U] == USART2_PROCESS_FRAME_END_1) &&
	   (ColorLcdTxFrame[ColorLcdTxFrameLength - 2U] == USART2_PROCESS_FRAME_END_2) &&
	   (ColorLcdTxFrame[ColorLcdTxFrameLength - 1U] == USART2_PROCESS_FRAME_END_3))
	{
		payloadLength = ColorLcdTxFrameLength - 5U;
		do
		{
			ColorLcdDmaSendStatus = USART2_Process_SendFrame(
				&ColorLcdTxFrame[1], payloadLength);
			if(ColorLcdDmaSendStatus == HAL_BUSY)
			{
				osDelay(1U);
			}
		} while(ColorLcdDmaSendStatus == HAL_BUSY);
	}
	else
	{
		ColorLcdDmaSendStatus = HAL_ERROR;
	}

	ColorLcdTxOwner = NULL;
	if(osMutexRelease(ColorLcdTxMutex) != osOK)
	{
		ColorLcdDmaSendStatus = HAL_ERROR;
	}
}

//#if TOUCH_COLOR_LCD == SCREEN_7_INCHES || TOUCH_COLOR_LCD == SCREEN_5_INCHES





void ColorLcdSendCmd(uint8_t cmd,uint8_t* dat,uint8_t len)
{


	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(cmd);
	ColorLcdComPortSendDat(dat,len);
	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}

void ColorLcdModeConfig(uint8_t mode)
{
		
}


void ColorLcdColorConfig(uint16_t Fcolor,uint16_t Bcolor)
{

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x40);
	ColorLcdComPortSendWord(Fcolor);
	ColorLcdComPortSendWord(Bcolor);
	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();
}


void ColorLcdSetTextScale(uint8_t state,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye)
{

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x45);
	if(state == 1)
	{
		ColorLcdComPortSendWord(Xs);
		ColorLcdComPortSendWord(Ys);
		ColorLcdComPortSendWord(Xe);
		ColorLcdComPortSendWord(Ye);
	}
	else
	{
		ColorLcdComPortSendByte(0);
	}
	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();
}



void ColorLcdSendString(uint16_t x,uint16_t y,uint8_t lib,uint8_t c_mode,uint8_t c_dot,uint16_t fc,uint16_t bc,uint8_t* str)
{

	uint8_t len = 0;
	uint8_t dot = 0;
	len = strlen((char*)str);
	if(len == 0)
	{
		return;
	}

	if(lib == COLOR_LCD_LIB_24_24)
	{
		ColorLcdDispResetAt(x,y,x+len*12-1,y+24-1);
		dot = 2;
	}
	else if(lib == COLOR_LCD_LIB_32_32)
	{
		ColorLcdDispResetAt(x,y,x+len*16-1,y+31);
		dot = 3;
	}
	else if(lib == COLOR_LCD_LIB_16_16)
	{
		ColorLcdDispResetAt(x,y,x+len*8-1,y+16-1);
		dot = 1;
	}

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x98);
	ColorLcdComPortSendByte((uint8_t)(x>>8));	//x
	ColorLcdComPortSendByte((uint8_t)x);
	ColorLcdComPortSendByte((uint8_t)(y>>8));	//y
	ColorLcdComPortSendByte((uint8_t)y);
	ColorLcdComPortSendByte((uint8_t)lib);
	ColorLcdComPortSendByte((uint8_t)c_mode);
	ColorLcdComPortSendByte((uint8_t)dot);
	ColorLcdComPortSendByte((uint8_t)(uint8_t)(fc>>8));
	ColorLcdComPortSendByte((uint8_t)(uint8_t)fc);
	ColorLcdComPortSendByte((uint8_t)(uint8_t)(bc>>8));
	ColorLcdComPortSendByte((uint8_t)(uint8_t)bc);
	ColorLcdComPortSendString((char*)str);
	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();
		
}



void ColorLcdSendStringNotReset(uint16_t x,uint16_t y,uint8_t lib,uint8_t c_mode,uint8_t c_dot,uint16_t fc,uint16_t bc,uint8_t* str)
{

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x98);
	ColorLcdComPortSendByte((uint8_t)(x>>8));	//x
	ColorLcdComPortSendByte((uint8_t)x);
	ColorLcdComPortSendByte((uint8_t)(y>>8));	//y
	ColorLcdComPortSendByte((uint8_t)y);
	ColorLcdComPortSendByte((uint8_t)lib);
	ColorLcdComPortSendByte((uint8_t)c_mode);
	ColorLcdComPortSendByte((uint8_t)c_dot);
	ColorLcdComPortSendByte((uint8_t)(uint8_t)(fc>>8));
	ColorLcdComPortSendByte((uint8_t)(uint8_t)fc);
	ColorLcdComPortSendByte((uint8_t)(uint8_t)(bc>>8));
	ColorLcdComPortSendByte((uint8_t)(uint8_t)bc);
	ColorLcdComPortSendString((char*)str);
	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();
		
}






void ColorLcdChangeToWindows(uint8_t num)
{

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x70);

	ColorLcdComPortSendByte(num);	


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}



void ColorLcdDispIcon(uint8_t pic_num,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y)//͸��
{


	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x9c);
	

		ColorLcdComPortSendByte(pic_num);	


	ColorLcdComPortSendByte((uint8_t)(Xs>>8));
	ColorLcdComPortSendByte((uint8_t) Xs);
	ColorLcdComPortSendByte((uint8_t)(Ys>>8));
	ColorLcdComPortSendByte((uint8_t) Ys);
	
	ColorLcdComPortSendByte((uint8_t)(Xe>>8));
	ColorLcdComPortSendByte((uint8_t) Xe);
	ColorLcdComPortSendByte((uint8_t)(Ye>>8));
	ColorLcdComPortSendByte((uint8_t) Ye);
	
	ColorLcdComPortSendByte((uint8_t)(X>>8));
	ColorLcdComPortSendByte((uint8_t) X);
	ColorLcdComPortSendByte((uint8_t)(Y>>8));
	ColorLcdComPortSendByte((uint8_t) Y);


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}



void ColorLcdDispIcon1(uint8_t pic_num,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y)//��͸��
{


	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x71);
	

		ColorLcdComPortSendByte(pic_num);	


	ColorLcdComPortSendByte((uint8_t)(Xs>>8));
	ColorLcdComPortSendByte((uint8_t) Xs);
	ColorLcdComPortSendByte((uint8_t)(Ys>>8));
	ColorLcdComPortSendByte((uint8_t) Ys);
	
	ColorLcdComPortSendByte((uint8_t)(Xe>>8));
	ColorLcdComPortSendByte((uint8_t) Xe);
	ColorLcdComPortSendByte((uint8_t)(Ye>>8));
	ColorLcdComPortSendByte((uint8_t) Ye);
	
	ColorLcdComPortSendByte((uint8_t)(X>>8));
	ColorLcdComPortSendByte((uint8_t) X);
	ColorLcdComPortSendByte((uint8_t)(Y>>8));
	ColorLcdComPortSendByte((uint8_t) Y);


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}



void ColorLcdDispIcon_Ext(const SIMPLE_ICON* icon,uint16_t X,uint16_t Y)
{

	SIMPLE_ICON*	p;
	p = (SIMPLE_ICON*)icon;

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x9c);
	
		ColorLcdComPortSendByte(p->cut_num);	



	ColorLcdComPortSendByte((uint8_t)((p->left)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left));
	ColorLcdComPortSendByte((uint8_t)((p->top)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top));
	
	ColorLcdComPortSendByte((uint8_t)((p->left + p->width)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left + p->width));
	ColorLcdComPortSendByte((uint8_t)((p->top + p->height)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top + p->height));
	
	ColorLcdComPortSendByte((uint8_t)(X>>8));
	ColorLcdComPortSendByte((uint8_t) X);
	ColorLcdComPortSendByte((uint8_t)(Y>>8));
	ColorLcdComPortSendByte((uint8_t) Y);


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}

void ColorLcdDispIcon_Ext1(const SIMPLE_ICON* icon,uint16_t X,uint16_t Y)//��͸��
{

	SIMPLE_ICON*	p;
	p = (SIMPLE_ICON*)icon;

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x71);
	
		ColorLcdComPortSendByte(p->cut_num);	

	ColorLcdComPortSendByte((uint8_t)((p->left)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left));
	ColorLcdComPortSendByte((uint8_t)((p->top)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top));
	
	ColorLcdComPortSendByte((uint8_t)((p->left + p->width)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left + p->width));
	ColorLcdComPortSendByte((uint8_t)((p->top + p->height)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top + p->height));
	
	ColorLcdComPortSendByte((uint8_t)(X>>8));
	ColorLcdComPortSendByte((uint8_t) X);
	ColorLcdComPortSendByte((uint8_t)(Y>>8));
	ColorLcdComPortSendByte((uint8_t) Y);


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}


void ColorLcdDispIcon_Ext1_1(const SIMPLE_ICON* icon)//��͸��
{

	SIMPLE_ICON*	p;
	p = (SIMPLE_ICON*)icon;

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x71);
	
		ColorLcdComPortSendByte(p->cut_num);	

	ColorLcdComPortSendByte((uint8_t)((p->left)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left));
	ColorLcdComPortSendByte((uint8_t)((p->top)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top));
	
	ColorLcdComPortSendByte((uint8_t)((p->left + p->width)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left + p->width));
	ColorLcdComPortSendByte((uint8_t)((p->top + p->height)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top + p->height));
	
	ColorLcdComPortSendByte((uint8_t)((p->left)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left));
	ColorLcdComPortSendByte((uint8_t)((p->top)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top));


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}



void ColorLcdDispIcon_Ext2(const SIMPLE_ICON* icon,uint16_t X,uint16_t Y)//��͸�� �Ҳ������������л�
{

	SIMPLE_ICON*	p;
	p = (SIMPLE_ICON*)icon;

	ColorLcdComPortBegin();
	ColorLcdComPortSendByte(0xaa);
	ColorLcdComPortSendByte(0x71);
	
	
	ColorLcdComPortSendByte(p->cut_num);	
	
	
	
	
	ColorLcdComPortSendByte((uint8_t)((p->left)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left));
	ColorLcdComPortSendByte((uint8_t)((p->top)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top));
	
	ColorLcdComPortSendByte((uint8_t)((p->left + p->width)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->left + p->width));
	ColorLcdComPortSendByte((uint8_t)((p->top + p->height)>>8));
	ColorLcdComPortSendByte((uint8_t) (p->top + p->height));
	
	ColorLcdComPortSendByte((uint8_t)(X>>8));
	ColorLcdComPortSendByte((uint8_t) X);
	ColorLcdComPortSendByte((uint8_t)(Y>>8));
	ColorLcdComPortSendByte((uint8_t) Y);


	ColorLcdComPortSendByte(0xcc);
	ColorLcdComPortSendByte(0x33);
	ColorLcdComPortSendByte(0xc3);
	ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();

}

void ColorLcdDispMessagebox(uint16_t X,uint16_t Y,char* message,uint8_t IconType,uint8_t lib)
{
	uint8_t dot = 0;
	uint8_t codec = 0;
	uint16_t len = 0;
	int16_t tmp_y=0;
	
	len = len;
	tmp_y = tmp_y;
	if(lib == COLOR_LCD_LIB_32_32)
	{
		dot = 3;
		codec = GB2312;
		len = strlen(message)*16;
		tmp_y = -1;
	}
	else if(lib == COLOR_LCD_LIB_24_24)
	{
		dot = 2;
		codec = GB2312;
		len = strlen(message)*12;
		tmp_y = 3;
	}
	else if(lib == COLOR_LCD_LIB_16_16)
	{
		dot = 1;
		codec = GBK;
		len = strlen(message)*8;
		tmp_y = 7;
	}
	ColorLcdDispIcon_Ext1(ICON_MESSAGE_BOX2,X,Y);
	
	ColorLcdSetTextScale(1,		X+10,
								Y+50,
								X+ICON_MESSAGE_BOX2->width -10,
								Y+ICON_MESSAGE_BOX2->height -10);
	ColorLcdSendStringNotReset(X+10,Y+50,lib,FCOLOR_ON|codec,dot,COLOR_BLUE,COLOR_WHITE,(uint8_t*)message);
	ColorLcdSetTextScale(0,0,0,0,0);
	
}

void ColorLcdClrMessagebox(uint16_t X,uint16_t Y)
{
	ColorLcdDispResetAt(X,Y,
						X+ICON_MESSAGE_BOX2->width,
						Y+ICON_MESSAGE_BOX2->height);
}








void ColorLcdDispResetAt(uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye)
{

	ColorLcdDispIcon(ColorWm.FatherPicNumber,Xs,Ys,Xe,Ye,Xs,Ys);

}


void ColorLcdDispResetAtFrame(_FRAME* frame)
{
	ColorLcdDispIcon(ColorWm.FatherWindowNumber,frame->left,frame->top,frame->right	,frame->bottom,frame->left,frame->top);
}




void ColorLcdDispOKAt(uint16_t X,uint16_t Y)
{

	ColorLcdDispIcon(ICON_CUT_PIC_NUM_20,547,4,596,45,X,Y);

}


void ColorLcdDispKeyBoardAt(uint16_t X,uint16_t Y)
{
	KeyBoardPosition.x= X;
	KeyBoardPosition.y = Y;
	ColorLcdDispIcon(ICON_CUT_PIC_NUM_24,513,251,795,479,X,Y);
}





void ColorLcdClrKeyBoardAt(void)
{
	ColorLcdDispResetAt(KeyBoardPosition.x,KeyBoardPosition.y,KeyBoardPosition.x+795-513,KeyBoardPosition.y+479-251);
}

void ColorLcdDispMsgErr(uint16_t x, uint16_t y,char* msg)
{
	if(ColorWm.Timer)
	{
		ColorLcdDispMessagebox(x,y,msg,1,COLOR_LCD_FONT_16_16);
	}
	else
	{
		ColorWm.FatherMenuNumber = ColorWm.ErrRetMenuNumber;
	}
	ColorWm.Timer--;
}


void ColorLcdWriteOneLineToBuff(uint16_t addr,uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color)
{


		ColorLcdComPortBegin();
		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0xc0);
		ColorLcdComPortSendWord(addr);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x1);
		ColorLcdComPortSendWord(y1);
		ColorLcdComPortSendWord(color);
		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);
		ColorLcdComPortEnd();
}


void ColorLcdWriteMemory(uint16_t addr,uint16_t num)
{


		ColorLcdComPortBegin();
		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0xc0);
		ColorLcdComPortSendWord(addr);
		ColorLcdComPortSendWord(num);
		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);
		ColorLcdComPortEnd();
}





void ColorLcdDispBuffLines(uint16_t addr1,uint16_t line_number)
{


		ColorLcdComPortBegin();
		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0xc1);
		ColorLcdComPortSendByte(0x02);
		ColorLcdComPortSendWord(addr1);
		ColorLcdComPortSendWord(line_number);
		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);
		ColorLcdComPortEnd();
}



void ColorLcdDrawLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t width)
{


	

	ColorLcdComPortBegin();

		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0x56);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x1);
		ColorLcdComPortSendWord(y1);
		

		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);
	ColorLcdComPortEnd();
}


// void ColorLcdDrawLine_With_Color(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t width)
// {
// 

	

// 	ColorLcdComPortBegin();

// 		ColorLcdComPortSendByte(0xaa);
// 		ColorLcdComPortSendByte(0x56);
// 		ColorLcdComPortSendWord(x0);
// 		ColorLcdComPortSendWord(y0);
// 		ColorLcdComPortSendWord(x1);
// 		ColorLcdComPortSendWord(y1);
		

// 		ColorLcdComPortSendByte(0xcc);
// 		ColorLcdComPortSendByte(0x33);
// 		ColorLcdComPortSendByte(0xc3);
// 		ColorLcdComPortSendByte(0x3c);
// 	ColorLcdComPortEnd();
// }




void ColorLcdDrawOneLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1)
{



	ColorLcdComPortBegin();

		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0x56);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x1);
		ColorLcdComPortSendWord(y1);
		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);

	ColorLcdComPortEnd();

}

void ColorLcdDrawOnePoint(uint16_t x0,uint16_t y0,uint8_t width)
{



	ColorLcdComPortBegin();

		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0x51);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x0-1);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x0+1);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0-1);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0+1);

		ColorLcdComPortSendWord(x0-2);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x0+2);
		ColorLcdComPortSendWord(y0);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0-2);
		ColorLcdComPortSendWord(x0);
		ColorLcdComPortSendWord(y0+2);
		
		ColorLcdComPortSendWord(x0-1);
		ColorLcdComPortSendWord(y0-1);
		ColorLcdComPortSendWord(x0+1);
		ColorLcdComPortSendWord(y0+1);
		ColorLcdComPortSendWord(x0+1);
		ColorLcdComPortSendWord(y0-1);
		ColorLcdComPortSendWord(x0-1);
		ColorLcdComPortSendWord(y0+1);
		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);

	ColorLcdComPortEnd();

}


void ColorLcdDispCursor(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t state)
{



	ColorLcdComPortBegin();

		ColorLcdComPortSendByte(0xaa);
		ColorLcdComPortSendByte(0x44);
		ColorLcdComPortSendByte(state);
		ColorLcdComPortSendWord(x);
		ColorLcdComPortSendWord(y);
		ColorLcdComPortSendByte(width);
		ColorLcdComPortSendByte(height);
		ColorLcdComPortSendByte(0xcc);
		ColorLcdComPortSendByte(0x33);
		ColorLcdComPortSendByte(0xc3);
		ColorLcdComPortSendByte(0x3c);

	ColorLcdComPortEnd();

}











#if TOUCH_COLOR_LCD == SCREEN_8_INCHES

void ColorLcdJumpOutChildWindow(uint8_t ChildWinNum,uint16_t X,uint16_t Y)
{
//	WIN_KEY* c_wk;
	const CHILD_WINDOWS_CONTROL_TABLE* c_wk	;
	
	c_wk = ChildWinTbl;
	while(c_wk->win_num !=0xff)
	{
		if(c_wk->win_num == ChildWinNum)
		{
				ColorLcdDispIcon_Ext1(c_wk->ChildWinIcon,X,Y);
				break;
		}
		else
		{
			c_wk++;
		}
	}
}

void ColorLcdDispSmallNumberString(char* str, uint16_t xpos,uint16_t ypos)
{
#define SMALL_NUMBER_WIDTH		20
	uint8_t len = 0;
	uint8_t i=0;
	len = strlen(str);
	for(i=0;i<len;i++)
	{
		if(str[i]>='0' && str[i]<='9')
		{
			ColorLcdDispIcon_Ext1(ColorSmallNumberTable[str[i] -'0'].icon,xpos,ypos);		
			xpos += SMALL_NUMBER_WIDTH;
		}
		else if (str[i]=='%')
		{
			ColorLcdDispIcon_Ext1(ColorSmallNumberTable[11].icon,xpos,ypos);		
			xpos += 30;
			
		}
		else if (str[i]=='.')
		{
			ColorLcdDispIcon_Ext1(ColorSmallNumberTable[12].icon,xpos,ypos);	
			xpos += SMALL_NUMBER_WIDTH;
			
		}
		else if (str[i]=='-')
		{
			ColorLcdDispIcon_Ext1(ColorSmallNumberTable[13].icon,xpos,ypos);		
			xpos += SMALL_NUMBER_WIDTH;
			
		}
		else if (str[i]==' ')
		{
			ColorLcdDispIcon_Ext1(ColorSmallNumberTable[14].icon,xpos,ypos);		
			xpos += SMALL_NUMBER_WIDTH;
			
		}
		else if (str[i]=='n')
		{
			ColorLcdDispIcon_Ext1(ColorSmallNumberTable[15].icon,xpos,ypos);	
			xpos += 40;
			
		}
	}

}


void ColorLcdDispMiddleNumberString(char* str, uint16_t xpos,uint16_t ypos)
{
#define MIDDLE_NUMBER_WIDTH		30
	uint8_t len = 0;
	uint8_t i=0;
	len = strlen(str);
	for(i=0;i<len;i++)
	{
		if(str[i]>='0' && str[i]<='9')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[str[i] -'0'].icon,xpos,ypos);			
			xpos += MIDDLE_NUMBER_WIDTH;
		}
		else if (str[i]=='A')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[10].icon,xpos,ypos);			
			xpos += 45;
		}
		else if (str[i]=='%')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[11].icon,xpos,ypos);			
			xpos += 45;
		}
		else if (str[i]=='.')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[12].icon,xpos,ypos);			
			xpos += MIDDLE_NUMBER_WIDTH;
		}
		else if (str[i]=='-')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[13].icon,xpos,ypos);			
			xpos += MIDDLE_NUMBER_WIDTH;
		}
		else if (str[i]==' ')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[14].icon,xpos,ypos);			
			xpos += MIDDLE_NUMBER_WIDTH;
		}
		else if (str[i]=='n')
		{
			ColorLcdDispIcon_Ext1(ColorMiddleNumberTable[15].icon,xpos,ypos);			
			xpos += 60;
		}
	}
}

void ColorLcdDispDigitalNumberString(char* str, uint16_t xpos,uint16_t ypos)
{
#define DIGITAL_NUMBER_WIDTH		24
	uint8_t len = 0;
	uint8_t i=0;
	len = strlen(str);
	for(i=0;i<len;i++)
	{
		if(str[i]>='0' && str[i]<='9')
		{
			ColorLcdDispIcon_Ext1(ColorDigitalNumberTable[str[i] -'0'].icon,xpos+i*DIGITAL_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]=='.')
		{
			ColorLcdDispIcon_Ext1(ColorDigitalNumberTable[10].icon,xpos+i*DIGITAL_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]=='-')
		{
			ColorLcdDispIcon_Ext1(ColorDigitalNumberTable[11].icon,xpos+i*DIGITAL_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]==' ')
		{
			ColorLcdDispIcon_Ext1(ColorDigitalNumberTable[12].icon,xpos+i*DIGITAL_NUMBER_WIDTH,ypos);			
		}				
	}
}

void ColorLcdDispLargeNumberString(char* str, uint16_t xpos,uint16_t ypos)
{
#define LARGE_NUMBER_WIDTH	80
	uint8_t len = 0;
	uint8_t i=0;
	len = strlen(str);
	for(i=0;i<len;i++)
	{
		if(str[i]>='0' && str[i]<='9')
		{
			ColorLcdDispIcon_Ext1(ColorLargeNumberTable[str[i] -'0'].icon,xpos+i*LARGE_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]=='A')
		{
			ColorLcdDispIcon_Ext1(ColorLargeNumberTable[10].icon,xpos+i*LARGE_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]=='%')
		{
			ColorLcdDispIcon_Ext1(ColorLargeNumberTable[11].icon,xpos+i*LARGE_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]=='.')
		{
			ColorLcdDispIcon_Ext1(ColorLargeNumberTable[12].icon,xpos+i*LARGE_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]=='-')
		{
			ColorLcdDispIcon_Ext1(ColorLargeNumberTable[13].icon,xpos+i*LARGE_NUMBER_WIDTH,ypos);			
		}
		else if (str[i]==' ')
		{
			ColorLcdDispIcon_Ext1(ColorLargeNumberTable[14].icon,xpos+i*LARGE_NUMBER_WIDTH,ypos);			
		}
		else
		{
		}
				
	}
}
#endif
