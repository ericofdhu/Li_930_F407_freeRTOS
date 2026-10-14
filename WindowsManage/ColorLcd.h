//color_lcd
/****************************************Copyright (c)**************************************************
**                               上海精密科学仪器有限公司
**
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: color_lcd.h
**创   建   人: 史玮
**最后修改日期: 2011年9月27日
**描        述: μCOS-II下彩屏应用程序头文件
**--------------历史版本信息----------------------------------------------------------------------------
** 创建人: 史玮
** 版  本: v1.0
** 日　期: 2011年9月27日
** 描　述: 原始版本
**
**--------------当前版本修订------------------------------------------------------------------------------
** 修改人: 
** 日　期: 
** 描　述: 
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef __COLOR_LCD_H
#define __COLOR_LCD_H



#include <stdint.h>
#include "icon_8inch.h"


typedef struct {
		uint16_t left;
		uint16_t right;
		uint16_t   top;
		uint16_t   bottom;

}_FRAME;








#define LANGUAGE_PIC_INTERVAL_NUM		44

#define  COLOR_LCD_FRAME_TAIL  0x3cc333cc
#define  COLOR_LCD_FRAME_HEAD  0xAA

#define  COLOR_LCD_CMD_SHOW_PICTURE  		0X70
#define  COLOR_LCD_CMD_SHOW_CUT_PICTURE  	0X71



#define    COLOR_LCD_FONT_8_8			0X53
#define    COLOR_LCD_FONT_12_12			0X6E
#define    COLOR_LCD_FONT_16_16			0X54
#define    COLOR_LCD_FONT_24_24			0X6F
#define    COLOR_LCD_FONT_32_32			0X55


#define    COLOR_LCD_LIB_8_8				0X00
#define    COLOR_LCD_LIB_12_12			0X20
#define    COLOR_LCD_LIB_16_16			0X21
#define    COLOR_LCD_LIB_24_24			0X22
#define    COLOR_LCD_LIB_32_32			0X23


#define    FCOLOR_ON						0X80
#define    FCOLOR_OFF					0X00
#define    BCOLOR_ON						0X40
#define    BCOLOR_OFF					0X00

#define    GB2312							0X01
#define    GBK							0X02



//彩屏颜色定义
#define    COLOR_BLACK					0X0000
#define    COLOR_WHITE					0XFFFF
#define    COLOR_RED						0XF800
#define    COLOR_GREEN					0X07E0
#define    COLOR_BLUE						0X001F
#define    COLOR_PURPLE					0X801F
#define    COLOR_KEYBOARD_BACK	0X5347
#define    COLOR_GRAY						0X4a49
#define    COLOR_YELLOW					0XFFFC











#define    ICON_GUANGDU_T_ABS							0X0201
#define    ICON_GUANGDU_MOVE_CELL						0X0202
#define    ICON_GUANGDU_CELL_TO_R						0X0203
#define    ICON_GUANGDU_AUTO_ZERO						0X0204
#define    ICON_GUANGDU_TEST							0X0205
#define    ICON_GUANGDU_PRINT							0X0206






#define    ICON_GUANGPU_T								0X0301
#define    ICON_GUANGPU_ABS								0X0302
#define    ICON_GUANGPU_E								0X0303
#define    ICON_GUANGPU_FAST							0X0304
#define    ICON_GUANGPU_MIDDLE							0X0305
#define    ICON_GUANGPU_SLOW							0X0306
#define    ICON_GUANGPU_0_1NM							0X0307
#define    ICON_GUANGPU_0_5NM							0X0308
#define    ICON_GUANGPU_1_0NM							0X0309
#define    ICON_GUANGPU_2_0NM							0X030A
#define    ICON_GUANGPU_5_0NM							0X030B

#define    ICON_GUANGPU_CONTINUE						0X030C
#define    ICON_GUANGPU_ABOVE							0X030D
#define    ICON_GUANGPU_STOP_BASE_LINE				0X030E
#define    ICON_GUANGPU_STOP_AUTO_ZERO						0X030F
#define    ICON_GUANGPU_MOVE_CELL						0X0310

#define    ICON_GUANGPU_CELL_TO_R						0X0311
#define    ICON_GUANGPU_TEST							0X0312
#define    ICON_GUANGPU_LOAD							0X0313
#define    ICON_GUANGPU_1E								0X0314
#define    ICON_GUANGPU_2E								0X0315
#define    ICON_GUANGPU_3E								0X0316
#define    ICON_GUANGPU_4E								0X0317





#define    ICON_DINGLIANG_NONE								0X0701
#define    ICON_DINGLIANG_BAIFENHAO						0X0702
#define    ICON_DINGLIANG_PPB								0X0703
#define    ICON_DINGLIANG_ML								0X0704
#define    ICON_DINGLIANG_GL								0X0705
#define    ICON_DINGLIANG_MGML								0X0706
#define    ICON_DINGLIANG_UGML								0X0707
#define    ICON_DINGLIANG_NGML								0X0708
#define    ICON_DINGLIANG_K									0X0709
#define    ICON_DINGLIANG_ONE								0X070A
#define    ICON_DINGLIANG_MULTI								0X070B





#define    SCAN_LINE_ADDR										4000
#define    DONGLIXUE_LINE_ADDR									4100


typedef struct _COLOR_LCD_CMD_STRUCT
{
	uint8_t cmd;
	uint8_t dat[100];
}COLOR_LCD_CMD_STRUCT;
extern  COLOR_LCD_CMD_STRUCT ColorLcdSendFrame; 




typedef struct {
	uint16_t icon_name;//图标名字
	uint16_t cut_num;	//剪切的照片号
	uint16_t  left;     
	uint16_t   top;
	uint16_t   right;
	uint16_t   bottom;
}TOUCH_ICON;



void ColorLcdSendCmd(uint8_t cmd,uint8_t* dat,uint8_t len);
void ColorLcdColorConfig(uint16_t Fcolor,uint16_t Bcolor);
void ColorLcdSetTextScale(uint8_t state,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye);
void ColorLcdSendString(uint16_t x,uint16_t y,uint8_t lib,uint8_t c_mode,uint8_t c_dot,uint16_t fc,uint16_t bc,uint8_t* str);
void ColorLcdSendStringNotReset(uint16_t x,uint16_t y,uint8_t lib,uint8_t c_mode,uint8_t c_dot,uint16_t fc,uint16_t bc,uint8_t* str);
void ColorLcdChangeToWindows(uint8_t num);

void ColorLcdDispResetAt(uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye);
void ColorLcdDispResetAtFrame(_FRAME* frame);

void ColorLcdDispShengluehaoAt(uint16_t X,uint16_t Y);
void ColorLcdDispOKAt(uint16_t X,uint16_t Y);
void ColorLcdDispIcon(uint8_t pic_num,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y);
void ColorLcdDispIcon1(uint8_t pic_num,uint16_t Xs,uint16_t Ys,uint16_t Xe,uint16_t Ye,uint16_t X,uint16_t Y);//不透明
void ColorLcdDispKeyBoardAt(uint16_t X,uint16_t Y);
void ColorLcdClrKeyBoardAt(void);
void ColorLcdDispMsgErr(uint16_t x, uint16_t y,char* msg) ;
void ColorLcdDispIcon_Ext(const SIMPLE_ICON* icon,uint16_t X,uint16_t Y);
void ColorLcdDispIcon_Ext1(const SIMPLE_ICON* icon,uint16_t X,uint16_t Y);
void ColorLcdDispIcon_Ext2(const SIMPLE_ICON* icon,uint16_t X,uint16_t Y);//不透明 且不根据语言来切换

void ColorLcdWriteOneLineToBuff(uint16_t addr,uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color);
void ColorLcdWriteMemory(uint16_t addr,uint16_t num);
void ColorLcdDrawLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t width);
void ColorLcdDrawOneLine(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1);
void ColorLcdDispBuffLines(uint16_t addr1,uint16_t line_number);
void ColorLcdDispWaitAt(uint16_t X,uint16_t Y);
void ColorLcdClrWaitAt(uint16_t X,uint16_t Y);
void ColorLcdDispCursor(uint16_t x,uint16_t y,uint16_t width,uint16_t height,uint8_t state);
void ColorLcdDispLoadingAt(uint16_t X,uint16_t Y);
void ColorLcdClrLoadingAt(uint16_t X,uint16_t Y);
void ColorLcdDispStopAt(uint16_t X,uint16_t Y);
void ColorLcdClrStopAt(uint16_t X,uint16_t Y);
void ColorLcdDrawOnePoint(uint16_t x0,uint16_t y0,uint8_t width);
void ColorLcdDispMessagebox(uint16_t X,uint16_t Y,char* message,uint8_t IconType,uint8_t lib);
void ColorLcdClrMessagebox(uint16_t X,uint16_t Y);
//void ColorLcdDispRound(uint16_t x,uint16_t y);
//void ColorLcdDispBadFace(uint16_t x,uint16_t y);
void ColorLcdReadRTC(void);
void ColorLcdWriteRTC(void);
void ColorLcdSendPinYinReadHZ(char* Pinyin);

#if TOUCH_COLOR_LCD == SCREEN_8_INCHES

void ColorLcdJumpOutChildWindow(uint8_t ChildWinNum,uint16_t X,uint16_t Y);
void ColorLcdDispSmallNumberString(char* str, uint16_t xpos,uint16_t ypos);
void ColorLcdDispMiddleNumberString(char* str, uint16_t xpos,uint16_t ypos);
void ColorLcdDispDigitalNumberString(char* str, uint16_t xpos,uint16_t ypos);
void ColorLcdDispLargeNumberString(char* str, uint16_t xpos,uint16_t ypos);


void ColorLcdDispIcon_Ext1_1(const SIMPLE_ICON* icon);//不透明


#endif



#endif



