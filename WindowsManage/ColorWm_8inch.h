//ColorWm_8inch.h
//icon_8inch.h
  //COLOR_WM.h
/****************************************Copyright (c)**************************************************
**                              彩色窗口管理模块
**
**
**--------------文件信息----------------------------------------------------------------------------
** 文件名：ColorWm_8inch.h
** 说明：彩色显示屏窗口管理模块头文件
** 创建日期：2011年9月27日
**--------------历史版本信息------------------------------------------------------------------------
** 版本：v1.0
** 日期：2011年9月27日
** 备注：初始版本
**
**--------------当前版本修订------------------------------------------------------------------------
** 修改者：
** 日期：
** 备注：
**
**------------------------------------------------------------------------------------------------------
********************************************************************************************************/
#ifndef __COLOR_WM_8INCH_H
#define __COLOR_WM_8INCH_H

#define COLOR_WM_MESSAGE_SIZE 32U

#include <stdint.h>
#include "FreeRTOS.h"
#include "event_groups.h"
#include "icon_8inch.h"
#include "TouchKey_8inch.h"
#include "ColorLcd.h"














//======================显示参数及绘图区坐标定义===============================================
//======================时间扫描绘图区=========================================================
#define   	TIME_DRAW_FRAME_TOP				(118)	
#define   	TIME_DRAW_FRAME_BOTTOM			(490)	
#define   	TIME_DRAW_FRAME_LEFT			(109)	
#define   	TIME_DRAW_FRAME_RIGHT			(709)

#define 	TIME_DRAW_FRAME_WIDTH			(TIME_DRAW_FRAME_RIGHT-TIME_DRAW_FRAME_LEFT)
#define 	TIME_DRAW_FRAME_HEIGHT			(TIME_DRAW_FRAME_BOTTOM-TIME_DRAW_FRAME_TOP)
//======================时间扫描绘图区=========================================================

//======================光谱扫描绘图区=========================================================
#define   	DRAW_FRAME_TOP					TIME_DRAW_FRAME_TOP
#define   	DRAW_FRAME_BOTTOM				TIME_DRAW_FRAME_BOTTOM	
#define   	DRAW_FRAME_LEFT					(TIME_DRAW_FRAME_LEFT)	
#define   	DRAW_FRAME_RIGHT				(TIME_DRAW_FRAME_RIGHT)

#define 	DRAW_FRAME_WIDTH				TIME_DRAW_FRAME_WIDTH
#define 	DRAW_FRAME_HEIGHT				TIME_DRAW_FRAME_HEIGHT

#define  	MAX_LINE_DISP_BUF				(2)
//======================光谱扫描绘图区=========================================================

//======================信噪比扫描绘图区=======================================================
#define SNR_DRAW_WL_TOP							101
#define SNR_DRAW_WL_BOTTOM						(SNR_DRAW_WL_TOP+185)
#define SNR_DRAW_WL_LEFT						87
#define SNR_DRAW_WL_RIGHT						(SNR_DRAW_WL_LEFT+600)

#define SNR_DRAW_TM_TOP							345
#define SNR_DRAW_TM_BOTTOM						(SNR_DRAW_TM_TOP+155)
#define SNR_DRAW_TM_LEFT						87
#define SNR_DRAW_TM_RIGHT						(SNR_DRAW_TM_LEFT+600)
//======================信噪比扫描绘图区=======================================================

//======================数据缓冲区定义 开始================================================================
#define  WAVE_SCAN_LINE_BUF_SIZE				(MAX_WAVELEN- MIN_WAVELEN)
#define  TIME_SCAN_LINE_BUF_SIZE				3600

#define  SCAN_LINE1_BUF_ADDR			0
#define  SCAN_LINE2_BUF_ADDR			((DRAW_FRAME_WIDTH+10)*5)
#define  SCAN_LINE3_BUF_ADDR			(SCAN_LINE2_BUF_ADDR+(DRAW_FRAME_WIDTH+10)*5)

#define  TIME_LINE1_BUF_ADDR			SCAN_LINE1_BUF_ADDR
//======================数据缓冲区定义 结束================================================================



//宏定义

//======================菜单编号================================================================
#define  MENU_NUMBER_ERR_MSG								0XA0
#define  MENU_NUMBER_WL_MOVE								0XA1
#define  MENU_NUMBER_ZEROING								0XA2
#define  MENU_NUMBER_BIAODING								0XA3
#define  MENU_NUMBER_DUOBOCHANGING					0XA4
#define  MENU_NUMBER_BLACK_CURRENT					0XA5
#define  MENU_NUMBER_PRINT									0XA6
#define  MENU_NUMBER_WAIT_SLIT_MOVE					0XA7
#define  MENU_NUMBER_WAIT_SLIT_CHOOSE				0XA8
#define  MENU_NUMBER_WAIT_KEY_BOARD					0XA9
#define  MENU_NUMBER_WAIT_MSG_BOX						0XAA
#define  MENU_NUMBER_WAIT_CHILD_WIN					0XAB
#define  MENU_NUMBER_WAIT_LANGUAGE_WIN			0XAC
#define  MENU_NUMBER_WAIT_MODE34_WIN				0XAD
#define  MENU_NUMBER_WAIT_SCALE_WIN					0XAE
#define  MENU_NUMBER_WAIT_CHANGE_LIGHT_WIN	0XAF
#define  MENU_NUMBER_WAIT_SAVE_NAME_WIN			0XB0
#define  MENU_NUMBER_WAIT_RENAME_WIN				0XB1
#define  MENU_NUMBER_WAIT_WL_CORRECT_WIN		0XB2
#define  MENU_NUMBER_WAIT_CHANGE_WL_WIN			0XB3
#define  MENU_NUMBER_WAIT_BACK_DOOR_WIN			0XB4
#define  MENU_NUMBER_UDISK									0XB5
#define  MENU_NUMBER_WAIT_SET_WAVELEN			0XB6
//======================菜单编号================================================================


//======================消息编号================================================================
#define MSG_AUTO_ZERO													1
#define MSG_MOVE														2
#define MSG_SCAN														3   //光谱扫描测试
#define MSG_SCAN_ZERO_LINE												4
#define MSG_FIND_MONTAIN_VALLY											5
#define MSG_CALC_CONC													6//计算浓度
#define MSG_1P_METHOD_GET_K												7//单点法获取校准系数K值
#define MSG_MULTI_POINT_METHOD_GET_BIAO_DING							8
#define MSG_TIME_SCAN													9   //动力学扫描测试
#define MSG_GET_BLACK													10
#define MSG_MULTI_WL_ZERO												11
#define MSG_MULTI_WL_TEST												12
#define MSG_DEVICE_INIT													13
#define MSG_PRINT														14
#define MSG_CELL														15
#define	MSG_CONFIRM														16
#define	MSG_CANCLE														17
#define	MSG_PAR_ERR														18
#define MSG_CHANGE_LIGHT												19
#define MSG_UDISK														20
#define MSG_SCAN_SNR													21 
//======================消息编号================================================================



#define    PORTUGAL_LANGUAGE							3
#define    SPANISH_LANGUAGE								2
#define    ENGLISH_LANGUAGE								1
#define    CHINESE_LANGUAGE								0

#define		CAPITAL_LETTER								0
#define		SMALL_LETTER								1

//======================窗口编号================================================================
#define    WIN_NUM_WELCOME									0
#define 	WIN_NUM_WARM_UP									1
#define    WIN_NUM_INIT										2
#define    WIN_NUM_MAIN_MENU								3
#define    WIN_NUM_GUDU_CESHI								5
#define    WIN_NUM_GUPU_CESHI								7
#define    WIN_NUM_GUPU_PAR			          				9
#define    WIN_NUM_SAVE_LOAD			          			10

#define    WIN_NUM_DING_LIANG_CESHI							20
#define    WIN_NUM_DING_LIANG_CESHI_BIAO_DING				22
#define    WIN_NUM_DING_LIANG_CURVE							23


#define    WIN_NUM_DING_LIANG_PAR							21

#define    WIN_NUM_DONG_LI_XUE_CESHI					24
#define    WIN_NUM_DONG_LI_XUE_PAR						25
#define    WIN_NUM_WAVE_CORRECT							26
#define    WIN_NUM_SNR_CESHI								27



#define    WIN_NUM_SYSTEM_CONFIG				        28
#define    WIN_NUM_TIME_CONFIG				        	29

#define    WIN_NUM_FAST_INIT				        	  41
#define    WIN_NUM_MONTAIN_VALLEY         	12

//======================子窗口编号==============================================================
#define    CHILD_WIN_NUM_34_MODE_SELECT								0xB0
#define    CHILD_WIN_NUM_SET_SLIT											0xB1
#define    CHILD_WIN_NUM_DIGITAL_KEYBOARD							0xB2
#define    CHILD_WIN_NUM_SET_SCALE										0xB3
#define    CHILD_WIN_NUM_SET_SCALE_DONG_LI_XUE				0xB4

#define    CHILD_WIN_NUM_ALPHABET_KEYBOARD						0xb5
#define    CHILD_WIN_NUM_MSG_BOX											0xb6
#define    CHILD_WIN_NUM_SET_LANGUAGE									0xB7




//======================窗口图片编号============================================================
#define    WIN_NUM_WELCOME_PIC												0
#define    WIN_NUM_INIT_PIC													0
#define    WIN_NUM_MAIN_MENU_PIC											2
#define    WIN_NUM_GUPU_CESHI_PIC											6
#define    WIN_NUM_GUPU_PAR_PIC			          							10
#define    WIN_NUM_SAVE_LOAD_PIC         			          				12

#define    WIN_NUM_DING_LIANG_CESHI_PIC										14
#define    WIN_NUM_DING_LIANG_CESHI_BIAO_DING_PIC							16
#define    WIN_NUM_DING_LIANG_CURVE_PIC										30


#define    WIN_NUM_DONG_LI_XUE_CESHI_PIC									22
#define    WIN_NUM_DONG_LI_XUE_PAR_PIC										24
#define    WIN_NUM_WAVE_CORRECT_PIC											26
#define    WIN_NUM_SNR_CESHI_PIC												27

#define    WIN_NUM_GUDU_CESHI_PIC										40


#define    WIN_NUM_WARM_UP_PIC											38
#define    WIN_NUM_MONTAIN_VALLEY_PIC         			12
#define    WIN_NUM_DING_LIANG_MP_BIAO_DING_PIC			22
#define    WIN_NUM_DING_LIANG_MP_CURVE_PIC					23
#define    WIN_NUM_SAVE_LOAD_DING_LIANG_K_PIC         		34
#define    WIN_NUM_SAVE_LOAD_DING_LIANG_1P_PIC         		34
#define    WIN_NUM_SAVE_LOAD_DING_LIANG_MP_PIC         		34
#define    WIN_NUM_SAVE_LOAD_DING_LIANG_BIAODING_MP_PIC   34
#define    WIN_NUM_SAVE_LOAD_DONG_LI_XUE_PIC         			35
#define    WIN_NUM_MULTI_WL_TEST_PIC         							27
//#define    WIN_NUM_SYSTEM_CONFIG_PIC				        			28
#define    WIN_NUM_TIME_CONFIG_PIC					        			40
#define    WIN_NUM_FAST_INIT_PIC					        			  0

#define    WIN_NUM_SYSTEM_CONFIG_PIC				        41


#define    KEY_BOARD_TYPE_NUMBER										0
#define    KEY_BOARD_TYPE_ALPHABAT									1
#define    KEY_BOARD_TYPE_YES_OR_NO									3//系统初始化确认
#define    KEY_BOARD_TYPE_YES_OR_NO_BUF_FULL				4//缓冲区已满确认
#define    KEY_BOARD_TYPE_LANGUAGE									5//语言选择
#define    KEY_BOARD_TYPE_CELL									6//样品池位置







//======================================数据结构定义================================================================
typedef struct {
	uint16_t Timer;  
	char  ErrMsg[40];	
	uint8_t ErrRetMenuNumber;
 	uint8_t  BackMenuNumber;
	
//================================================
	uint8_t IsChildFlag;
	uint8_t ChildToFatherBuff[4];

	uint8_t FatherWindowNumber;
	uint8_t FatherPicNumber;
	uint8_t FatherMenuNumber;
	uint8_t FatherPageNumber;
	void  (*FatherKFun) ();
	void  (*FatherWFun) ();
	void  (*FatherW_InitFun) ();
	uint16_t FatherWin_Xpos;
	uint16_t FatherWin_Ypos;
	
	uint8_t ChildWindowNumber;
	uint8_t ChildPicNumber;
	uint8_t ChildMenuNumber;
	uint8_t ChildPageNumber;
	void  (*ChildKFun) ();
	void  (*ChildWFun) ();
	void  (*ChildW_InitFun) ();
	uint16_t ChildWin_Xpos;
	uint16_t ChildWin_Ypos;
	SIMPLE_ICON* ChildWinIcon;	
	
	uint8_t SecondChildWindowNumber;
	uint8_t SecondChildPicNumber;
	uint8_t SecondChildMenuNumber;
	uint8_t SecondChildPageNumber;
	void  (*SecondChildKFun) ();
	void  (*SecondChildWFun) ();
	void  (*SecondChildW_InitFun) ();
	uint16_t SecondChildWin_Xpos;
	uint16_t SecondChildWin_Ypos;
	SIMPLE_ICON* SecondChildWinIcon;	
	
	
//================================================
}_COLOR_WM;
extern _COLOR_WM   ColorWm;


typedef struct {
  char msg[40];
	uint16_t	 SurviveTime;//消息框显示时长
	const SIMPLE_ICON*   MsgIcon;
	uint8_t Type;//类型：1，定时关闭；2，等待指定事件标志
	uint32_t WaitFlag;
}_MsgBox;

extern _MsgBox		MsgBox;






typedef struct {
	uint8_t  win_num;
	uint8_t  pic_num;
	const TOUCH_KEY_SQUARE* tks;
	void   (*KeystrokeFun)();
	void   (*WindowFun)();
	void   (*WindowInitFun)();
	
}FATHER_WINDOWS_CONTROL_TABLE;
extern const FATHER_WINDOWS_CONTROL_TABLE FatherWinTbl[];


typedef struct {
	uint8_t  win_num;
	const TOUCH_KEY_SQUARE* tks;
	void   (*KeystrokeFun)();
	void   (*WindowFun)();
	void   (*WindowInitFun)();
	const SIMPLE_ICON* ChildWinIcon;

}CHILD_WINDOWS_CONTROL_TABLE;

extern const CHILD_WINDOWS_CONTROL_TABLE ChildWinTbl[];

//绘图参数数据结构
typedef struct {

	uint16_t x;
	float y;

	uint32_t x_Width;
	uint32_t y_Height;

	uint32_t left;
	uint32_t right;
	float   top;
	float   bottom;

	// uint32 ref;

	uint32_t left_reserve;
	uint32_t right_reserve;
	float   top_reserve;
	float   bottom_reserve;
	
	uint16_t  Interval;


	uint32_t x_cur;
	int16_t y_cur;


	uint16_t	 CousorPos;

	uint8_t draw_mode; //0：光谱扫描，1：时间扫描

}_DrawStruct;
extern _DrawStruct gDraw ;
extern _DrawStruct gDraw_SNR_WL ;
extern _DrawStruct gDraw_SNR_TM ;


typedef struct {
	char*  msg;
	const SIMPLE_ICON*   MsgIcon;
}_DispMsgTable;

typedef struct _PIXEL_POINT
{
	uint16_t x;
	uint16_t y;
}PIXEL_POINT;

typedef struct {
	uint16_t y_val;
	uint32_t x_data;
	PIXEL_POINT pt;

}M1_M2;

typedef struct _DISP_BUF
{
	PIXEL_POINT  pixel[MAX_LINE_DISP_BUF];
	uint16_t num;
	uint16_t scan_sum;
	uint16_t donglixue_sum;	
}DISP_BUF, pDISP_BUF;
extern DISP_BUF  LineDispBuf; 


typedef struct GuPuCeShiStruct{
	uint32_t Speed;
	uint32_t Interval;

	uint32_t WaveLenStart; //光谱起始波长
	uint32_t WaveLenEnd; //光谱结束波长

	uint16_t data_E_Start;//数据起始
	uint16_t data_E_End;//数据结束


	uint8_t M1_M2_mode;
	M1_M2 M1;
	M1_M2 M2;

	uint8_t backup[2];	
	
}_GuPuCeShiStruct;
extern _GuPuCeShiStruct GuPuCeShiStruct ;







//=================================================================================================================












void ColorWindowsKeyM(uint8_t WindowNumber,uint8_t MenuNumber,void (*KF)(),void (*WF)());
void ColorWindowsKeyM_Ext(uint8_t WindowNumber,uint8_t MenuNumber,void (*KF)(),void (*WF)(),void (*w_init)());
void DisplayWindow(void);
void DisplayChildWindow(void);
void DisplaySecondChildWindow(void);
/* 从0至3号队列槽接收一条32字节的窗口结果消息。 */
osStatus_t ColorWM_WindowMessageReceive(uint8_t queueIndex,
                                        uint8_t *message,
                                        uint32_t timeout);
osStatus_t ColorWM_WindowMessageSend(uint8_t queueIndex,
                                     const uint8_t *message);

void ColorWindowsControl(uint8_t WindowNumber,uint8_t MenuNumber);


void WelcomeW_Init(void);
void WelcomeW(void);
void WelcomeK(void);

void WarmUpW_Init(void);
void WarmUpW(void);
void WarmUpK(void);


void FastInitW(void);

void InitW(void);
void InitK(void);

void MainMenuW(void);
void MainMenuK(void) ;


void GuPuCeShiW_Init(void);
void GuPuCeShiW(void);
void GuPuCeShiK(void);


// void GuPuParSetW_Init(void);
// void GuPuParSetW(void);
// void GuPuParSetK(void);


// void MontainValleyW_Init(void);
// void MontainValleyW(void);
// void MontainValleyK(void);


// void SaveLoadW_Init(void);
// void SaveLoadW(void);
// void SaveLoadK(void);


// void DingLiangCeShiW_Init(void);
// void DingLiangCeShiW(void);
// void DingLiangCeShiK(void);


// void DingLiangCeShiBiaoDingW_Init(void);
// void DingLiangCeShiBiaoDingW(void);
// void DingLiangCeShiBiaoDingK(void);




// void DingLiangMPParW_Init(void);
// void DingLiangMPParW(void);
// void DingLiangMPParK(void);


// void DingLiangMPCurveW_Init(void);
// void DingLiangMPCurveW(void);
// void DingLiangMPCurveK(void);




// void DongLiXueCeshiW_Init(void);
// void DongLiXueCeshiW(void);
// void DongLiXueCeshiK(void);

// void DongLiXueParW_Init(void);
// void DongLiXueParW(void);
// void DongLiXueParK(void);






// void SaveLoadDongLiXueW_Init(void);
// void SaveLoadDongLiXueW(void);
// void SaveLoadDongLiXueK(void);


// void SaveLoadDingLiangMpBiaoDingW_Init(void);
// void SaveLoadDingLiangMpBiaoDingW(void);
// void SaveLoadDingLiangMpBiaoDingK(void);


// void SaveLoadDingLiangMpTestW_Init(void);
// void SaveLoadDingLiangMpTestW(void);
// void SaveLoadDingLiangMpTestK(void);

// void SaveLoadDingLiang1pTestW_Init(void);
// void SaveLoadDingLiang1pTestW(void);
// void SaveLoadDingLiang1pTestK(void);

// void SaveLoadDingLiangKTestW_Init(void);
// void SaveLoadDingLiangKTestW(void);
// void SaveLoadDingLiangKTestK(void);


// //波长校正
// void WaveCorrectW_Init(void);
// void WaveCorrectW(void);
// void WaveCorrectK(void);

// void SystemConfigW_Init(void);
// void SystemConfigW(void);
// void SystemConfigK(void);

// void TimeConfigW_Init(void);
// void TimeConfigW(void);
// void TimeConfigK(void);



// void SNRCeShiW_Init(void);
// void SNRCeShiW(void);
// void SNRCeShiK(void);

//////////////////////////////////////////////////////////////////


void DataDisplayOnKeyBoard(char* str,uint8_t type);
void EnterKeyBoard(uint16_t x,uint16_t y,uint8_t type);
void ExitKeyBoard(uint8_t type);



void Child_34ModeSelectW(void);
void Child_34ModeSelectK(void);
void Child_34ModeSelectW_Init(void);


void Child_LanguageW(void);
void Child_LanguageK(void);
void Child_LanguageW_Init(void);



void Child_SetSlitW(void);
void Child_SetSlitK(void);
void Child_SetSlitW_Init(void);


void Child_SetCellW(void);
void Child_SetCellK(void);
void Child_SetCellW_Init(void);


void Child_DigitalKeyboardW_Init(void);
void Child_DigitalKeyboardW(void);
void Child_DigitalKeyboardK(void);


void Child_AlphabetKeyboardW_Init(void);
void Child_AlphabetKeyboardW(void);
void Child_AlphabetKeyboardK(void);


void Child_SetScaleW(void);
void Child_SetScaleK(void);
void Child_SetScaleW_Init(void);

void Child_SetScaleDongLiXueK(void);
void Child_SetScaleDongLiXueW(void);
void Child_SetScaleDongLiXueW_Init(void);




void Child_MsgBoxW_Init(void);
void Child_MsgBoxW(void);
void Child_MsgBoxK(void);


void EnterDigitalKeyBoard(uint16_t xpos,uint16_t ypos,uint8_t DataType,float max,float min);
void ExitDigitalKeyBoard(void);


void EnterAlphabetKeyBoard(uint16_t xpos,uint16_t ypos,uint8_t MaxLen);
void ExitAlphabetKeyBoard(void);


void ShowMsgBox(uint16_t xpos,uint16_t ypos,char* Msg,uint16_t SurviveTime,uint8_t Type,uint32_t WaitFlag);
void HideMsgBox(void);


void RedrawGuangPuLine(_DrawStruct tmpDraw);
void RedrawDongLiXueLine(_DrawStruct tmpDraw);

void CopyGuangPuDrawPara(void);
void CopyDongLiXueDrawPara(void);



PIXEL_POINT CalcCoordinate(uint32_t wave,_DrawStruct* ds,_FRAME* pFrame,int16_t sl);
PIXEL_POINT CalcXY(uint32_t wave,uint16_t data,_DrawStruct ds,_FRAME* pFrame);



#endif


