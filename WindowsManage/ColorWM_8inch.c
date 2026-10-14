/****************************************Copyright (c)**************************************************
**                               上海精密科学仪器有限公司
**
**
**--------------文件信息--------------------------------------------------------------------------------
**文   件   名: color_ColorWm.c
**创   建   人: 史玮
**最后修改日期: 2011年9月27日
**描        述: μCOS-II下彩屏显示界面管理应用程序c文件
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
//#define SYS_DATA_STRUCT_GLOBAL

#include "TouchKey_8inch.h"
#include "ColorLcd.h"
#include "ColorWM_8inch.h"
#include "icon_8inch.h"
#include "fatfs.h"
#include "wavelength_transfor.h"
#include "spectral_peak_detector.h"
#include "FreeRTOS.h"
#include "event_groups.h"
#include "task.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "system_runtime.h"
#include "MyLib.h"
#include "MyPinYin.h"
#include "MySaveLoadFile.h"



uint8_t g_save_load_type = 0;
_DrawStruct gDraw ;
_DrawStruct gDraw_SNR_WL ;
_DrawStruct gDraw_SNR_TM ;

_COLOR_WM   ColorWm;
_MsgBox		MsgBox;

DISP_BUF  LineDispBuf; 
_GuPuCeShiStruct GuPuCeShiStruct ;


#define COLOR_WM_MESSAGE_QUEUE_COUNT 4U
#define COLOR_WM_MESSAGE_QUEUE_LENGTH 1U

static osMessageQueueId_t ColorWM_MessageQueues[COLOR_WM_MESSAGE_QUEUE_COUNT];

/**
 * @brief 校验队列索引，并在队列尚未创建时创建窗口消息队列。
 * @param queueIndex - 从 0 开始的队列索引，有效范围为 0 至 COLOR_WM_MESSAGE_QUEUE_COUNT - 1。
 * @return osStatus_t：成功返回 osOK；索引无效返回 osErrorParameter；创建失败返回 osErrorNoMemory。
 * @note 首次调用时创建并保存 CMSIS-RTOS 队列句柄。
 */
static osStatus_t ColorWM_EnsureMessageQueue(uint8_t queueIndex)
{
	if (queueIndex >= COLOR_WM_MESSAGE_QUEUE_COUNT)
	{
		return osErrorParameter;
	}

	if (ColorWM_MessageQueues[queueIndex] == NULL)
	{
		ColorWM_MessageQueues[queueIndex] = osMessageQueueNew(
			COLOR_WM_MESSAGE_QUEUE_LENGTH, COLOR_WM_MESSAGE_SIZE, NULL);
		if (ColorWM_MessageQueues[queueIndex] == NULL)
		{
			return osErrorNoMemory;
		}
	}

	return osOK;
}



/**
 * @brief 从窗口消息队列接收一条消息。
 * @param queueIndex - 从 0 开始的队列索引。
 * @param message - 接收 COLOR_WM_MESSAGE_SIZE 字节消息的缓冲区，不得为 NULL。
 * @param timeout - CMSIS-RTOS 等待超时，单位为系统节拍。
 * @return osStatus_t：返回队列接收状态，可能包括参数错误、分配错误、超时或资源错误。
 * @note 可能按指定超时时间阻塞；队列尚未创建时会先创建队列。
 */
osStatus_t ColorWM_WindowMessageReceive(uint8_t queueIndex,
                                        uint8_t *message,
                                        uint32_t timeout)
{
	osStatus_t status;

	if (message == NULL)
	{
		return osErrorParameter;
	}

	status = ColorWM_EnsureMessageQueue(queueIndex);
	if (status != osOK)
	{
		return status;
	}

	return osMessageQueueGet(ColorWM_MessageQueues[queueIndex], message,
	                         NULL, timeout);
}

/**
 * @brief 清除窗口消息队列中的待处理消息；队列为空时不报错。
 * @return osStatus_t：消息清除成功或队列原本为空时返回 osOK，否则返回队列操作错误。
 * @note 以非阻塞方式接收消息，并将队列为空视为成功。
 */
static osStatus_t ColorWM_ClearWindowMessage(uint8_t queueIndex)
{
	osStatus_t status;
	uint8_t staleMessage[COLOR_WM_MESSAGE_SIZE];

	status = ColorWM_EnsureMessageQueue(queueIndex);
	if (status != osOK)
	{
		return status;
	}

	status = osMessageQueueGet(ColorWM_MessageQueues[queueIndex],
	                           staleMessage, NULL, 0U);
	return ((status == osOK) || (status == osErrorResource)) ? osOK : status;
}

/**
 * @brief 以非阻塞方式向窗口消息队列发送一条定长消息。
 * @param queueIndex - 从 0 开始的队列索引。
 * @param message - 包含 COLOR_WM_MESSAGE_SIZE 字节数据的输入缓冲区，不得为 NULL。
 * @return osStatus_t：返回队列发送状态，可能包括参数错误、分配错误或队列已满。
 * @note 队列尚未创建时会先创建队列；发送等待时间为零。
 */
static osStatus_t ColorWM_PostWindowMessage(uint8_t queueIndex,
                                            const uint8_t *message)
{
	osStatus_t status;

	if (message == NULL)
	{
		return osErrorParameter;
	}

	status = ColorWM_EnsureMessageQueue(queueIndex);
	if (status != osOK)
	{
		return status;
	}

	return osMessageQueuePut(ColorWM_MessageQueues[queueIndex], message,
	                         0U, 0U);
}

/**
 * @brief 对外提供向窗口消息队列发送消息的接口。
 * @param queueIndex - 从 0 开始的队列索引。
 * @param message - 包含 COLOR_WM_MESSAGE_SIZE 字节数据的输入缓冲区，不得为 NULL。
 * @return osStatus_t：返回内部非阻塞队列发送操作的状态。
 * @note 将消息转交给 ColorWM_PostWindowMessage 处理。
 */
osStatus_t ColorWM_WindowMessageSend(uint8_t queueIndex,
                                     const uint8_t *message)
{
	return ColorWM_PostWindowMessage(queueIndex, message);
}





const FATHER_WINDOWS_CONTROL_TABLE FatherWinTbl[] = {

	{WIN_NUM_WELCOME,	
		WIN_NUM_WELCOME_PIC,								
		WindowWelcom_key_table,			
		WelcomeK,					
		WelcomeW,				
		WelcomeW_Init},
		
		

	{WIN_NUM_MAIN_MENU,		
		WIN_NUM_MAIN_MENU_PIC,								
		WindowMainMenu_key_table,		
		MainMenuK,				
		MainMenuW,			
		(void*)0},

	//光谱测试														
	{WIN_NUM_GUPU_CESHI,	
		WIN_NUM_GUPU_CESHI_PIC,							
		WindowGuangPu_key_table,		
		GuPuCeShiK,				
		GuPuCeShiW,		
		GuPuCeShiW_Init},
	

		
	{0xff,0,0,0,0}
};



const CHILD_WINDOWS_CONTROL_TABLE ChildWinTbl[] = {

	{CHILD_WIN_NUM_MSG_BOX,	Child_MsgBox_key_table,			Child_MsgBoxW,				
		Child_MsgBoxK,	Child_MsgBoxW_Init,		CWindow_MsgBox},


	// {CHILD_WIN_NUM_SET_LANGUAGE,					Child_W_Language_key_table,							Child_LanguageW,				
	// 	Child_LanguageK,					Child_LanguageW_Init,						CWindow_Language},

		
	{CHILD_WIN_NUM_DIGITAL_KEYBOARD,	Child_W_DigitalKeyboard_key_table,			Child_DigitalKeyboardW,				
		Child_DigitalKeyboardK,	Child_DigitalKeyboardW_Init,		CWindow_DIGITAL_KEYBOARD},

	{CHILD_WIN_NUM_ALPHABET_KEYBOARD,	Child_W_AlphabetKeyboard_key_table,			Child_AlphabetKeyboardW,				
		Child_AlphabetKeyboardK,	Child_AlphabetKeyboardW_Init,		CWindow_ALPHABET_KEYBOARD},
		
{0xff,0,0,0,0,0}
	
};

const _DispMsgTable DispMsgTable[] = {

{"",DISP_MSG1}
};





const _FRAME SNR_WL_FRAME = {SNR_DRAW_WL_LEFT,SNR_DRAW_WL_RIGHT,SNR_DRAW_WL_TOP,SNR_DRAW_WL_BOTTOM}; 
const _FRAME SNR_TM_FRAME = {SNR_DRAW_TM_LEFT,SNR_DRAW_TM_RIGHT,SNR_DRAW_TM_TOP,SNR_DRAW_TM_BOTTOM}; 
const _FRAME WL_FRAME = {DRAW_FRAME_LEFT,DRAW_FRAME_RIGHT,DRAW_FRAME_TOP,DRAW_FRAME_BOTTOM}; 
const _FRAME TM_FRAME = {DRAW_FRAME_LEFT,DRAW_FRAME_RIGHT,DRAW_FRAME_TOP,DRAW_FRAME_BOTTOM}; 





/**
 * @brief 返回当前活动窗口层级对应的消息队列编号。
 * @return uint8_t：父窗口返回 0，第一层子窗口返回 1，第二层子窗口返回 2。
 * @note 读取 ColorWm.IsChildFlag；该值表示窗口层级，并非 RTOS 任务优先级。
 */
uint8_t GetCurrentTaskPrio(void)
{
	/* The FreeRTOS display runs in one task; use the active window layer. */
	if (ColorWm.IsChildFlag == 0U)
	{
		return 0U;
	}
	if (ColorWm.IsChildFlag == 1U)
	{
		return 1U;
	}
	return 2U;
}



// uint8_t FindSelectFileIndex(void)
// {
// 	uint8_t i=0;

// 	{
// 		if(FileNameList[i].select == 1)
// 		{
// 				return i;
// 		}
// 	}
// 	return 0xff;
// }



/**
 * @brief 在回调函数已设置时，依次调用父窗口按键处理和界面绘制函数。
 * @return 无返回值。
 * @note 先调用父窗口按键回调，再调用父窗口绘制回调。
 */
void DisplayWindow(void)
{
	
		if(ColorWm.FatherKFun != (void*)0)
		(*ColorWm.FatherKFun)();  
		if(ColorWm.FatherWFun != (void*)0)
		(*ColorWm.FatherWFun)();

}



/**
 * @brief 在回调函数已设置时，依次调用第一层子窗口按键处理和界面绘制函数。
 * @return 无返回值。
 * @note 先调用子窗口按键回调，再调用子窗口绘制回调。
 */
void DisplayChildWindow(void)
{
	if(ColorWm.ChildKFun != (void*)0)
		(*ColorWm.ChildKFun)();  
	if(ColorWm.ChildWFun != (void*)0)
		(*ColorWm.ChildWFun)();
// 	if(ColorWm.ChildWFun == (void*)0  && ColorWm.ChildKFun == (void*)0)
// 		OSTaskSuspend(APP_TASK_CHILD_DISP_PRIO);

}


/**
 * @brief 在回调函数已设置时，依次调用第二层子窗口按键处理和界面绘制函数。
 * @return 无返回值。
 * @note 先调用第二层子窗口按键回调，再调用其绘制回调。
 */
void DisplaySecondChildWindow(void)
{
	if(ColorWm.SecondChildKFun != (void*)0)
		(*ColorWm.SecondChildKFun)();  
	if(ColorWm.SecondChildWFun != (void*)0)
		(*ColorWm.SecondChildWFun)();

}






/**
 * @brief 从控制表中选择父窗口并切换到对应界面状态。
 * @param WindowNumber - 要切换到的父窗口编号。
 * @param MenuNumber - 写入所选窗口的初始菜单或页面编号。
 * @return 无返回值。
 * @note 更新父窗口状态、切换 LCD 窗口，并在存在初始化回调时调用该回调。未找到窗口编号时不改变当前状态。
 */
void ColorWindowsControl(uint8_t WindowNumber,uint8_t MenuNumber)
{
	uint8_t i=0;
	uint8_t table_size = 0;
	
	table_size = sizeof(FatherWinTbl)/sizeof(FATHER_WINDOWS_CONTROL_TABLE);
	
	for(i=0;i<table_size;i++)
	{
		if(FatherWinTbl[i].win_num == WindowNumber)
		{
			ColorWm.FatherWindowNumber  =  FatherWinTbl[i].win_num;
			ColorWm.FatherPicNumber    = 	FatherWinTbl[i].pic_num;
			ColorWm.FatherMenuNumber    =  MenuNumber;
			ColorWm.FatherPageNumber = 0;
			ColorWm.FatherKFun  =  FatherWinTbl[i].KeystrokeFun;
			ColorWm.FatherWFun     =  FatherWinTbl[i].WindowFun;
			ColorWm.FatherW_InitFun     =  FatherWinTbl[i].WindowInitFun;
			ColorLcdChangeToWindows(ColorWm.FatherPicNumber );
			if(FatherWinTbl[i].WindowInitFun != 0)
			{
				(*FatherWinTbl[i].WindowInitFun)();
			}
			break;
		}
	}
}

/**
 * @brief 从子窗口控制表中打开指定子窗口，并设置其屏幕位置。
 * @param ChildWinNum - 要打开的子窗口编号。
 * @param xpos - 子窗口的横坐标。
 * @param ypos - 子窗口的纵坐标。
 * @return 无返回值。
 * @note 根据当前层级设置第一层或第二层子窗口的回调、图标和位置，切换 LCD 界面并调用初始化回调。嵌套层级已满或找不到对应窗口/图标时不执行操作。
 */
void ColorWindowsJumpOutChild(uint8_t ChildWinNum,uint16_t xpos,uint16_t ypos)//
{
	uint8_t i;
	uint8_t table_size =
		(uint8_t)(sizeof(ChildWinTbl) / sizeof(ChildWinTbl[0]));
	const CHILD_WINDOWS_CONTROL_TABLE *child = NULL;

	if (ColorWm.IsChildFlag >= 2U)
	{
		return;
	}

	for (i = 0U; i < table_size; i++)
	{
		if (ChildWinTbl[i].win_num == 0xffU)
		{
			break;
		}
		if (ChildWinTbl[i].win_num == ChildWinNum)
		{
			child = &ChildWinTbl[i];
			break;
		}
	}

	if ((child == NULL) || (child->ChildWinIcon == NULL))
	{
		return;
	}

	if (ColorWm.IsChildFlag == 0U)
	{
		ColorWm.ChildWindowNumber = child->win_num;
		ColorWm.ChildPageNumber = 0U;
		ColorWm.ChildKFun = child->KeystrokeFun;
		ColorWm.ChildWFun = child->WindowFun;
		ColorWm.ChildW_InitFun = child->WindowInitFun;
		ColorWm.ChildWinIcon = (SIMPLE_ICON *)child->ChildWinIcon;
		ColorWm.ChildWin_Xpos = xpos;
		ColorWm.ChildWin_Ypos = ypos;
	}
	else
	{
		ColorWm.SecondChildWindowNumber = child->win_num;
		ColorWm.SecondChildPageNumber = 0U;
		ColorWm.SecondChildKFun = child->KeystrokeFun;
		ColorWm.SecondChildWFun = child->WindowFun;
		ColorWm.SecondChildW_InitFun = child->WindowInitFun;
		ColorWm.SecondChildWinIcon = (SIMPLE_ICON *)child->ChildWinIcon;
		ColorWm.SecondChildWin_Xpos = xpos;
		ColorWm.SecondChildWin_Ypos = ypos;
	}

	ColorWm.IsChildFlag++;
	if (ColorWm.IsChildFlag == 1U)
	{
		osDelay(200U);
	}
	ColorLcdJumpOutChildWindow(child->win_num, xpos, ypos);
	if (child->WindowInitFun != NULL)
	{
		(*child->WindowInitFun)();
	}
}
	
/**
 * @brief 当指定编号与当前活动子窗口一致时关闭该子窗口。
 * @return 无返回值。
 * @note 恢复被覆盖的 LCD 区域，清除当前层级的回调和图标，并减少嵌套层级。当前没有子窗口或编号不匹配、窗口不存在时不执行操作。
 */
void ColorWindowsJumpInChild(uint8_t ChildWinNum)
{
	uint8_t i;
	uint8_t table_size =
		(uint8_t)(sizeof(ChildWinTbl) / sizeof(ChildWinTbl[0]));
	const CHILD_WINDOWS_CONTROL_TABLE *child = NULL;
	uint8_t activeChildWindow;
	uint16_t xpos;
	uint16_t ypos;

	if (ColorWm.IsChildFlag == 0U)
	{
		return;
	}

	activeChildWindow = (ColorWm.IsChildFlag == 1U) ?
		ColorWm.ChildWindowNumber : ColorWm.SecondChildWindowNumber;
	if (ChildWinNum != activeChildWindow)
	{
		return;
	}

	for (i = 0U; i < table_size; i++)
	{
		if (ChildWinTbl[i].win_num == 0xffU)
		{
			break;
		}
		if (ChildWinTbl[i].win_num == ChildWinNum)
		{
			child = &ChildWinTbl[i];
			break;
		}
	}

	if ((child == NULL) || (child->ChildWinIcon == NULL))
	{
		return;
	}

	if (ColorWm.IsChildFlag == 1U)
	{
		xpos = ColorWm.ChildWin_Xpos;
		ypos = ColorWm.ChildWin_Ypos;
	}
	else
	{
		xpos = ColorWm.SecondChildWin_Xpos;
		ypos = ColorWm.SecondChildWin_Ypos;
	}

	ColorLcdDispResetAt(xpos, ypos,
		(uint16_t)(xpos + child->ChildWinIcon->width),
		(uint16_t)(ypos + child->ChildWinIcon->height));

	if (ColorWm.IsChildFlag == 1U)
	{
		ColorWm.ChildKFun = NULL;
		ColorWm.ChildWFun = NULL;
		ColorWm.ChildW_InitFun = NULL;
		ColorWm.ChildWinIcon = NULL;
	}
	else
	{
		ColorWm.SecondChildKFun = NULL;
		ColorWm.SecondChildWFun = NULL;
		ColorWm.SecondChildW_InitFun = NULL;
		ColorWm.SecondChildWinIcon = NULL;
	}

	ColorWm.IsChildFlag--;
}


/**
 * @brief 清除调用方的结果队列后，准备并打开消息框子窗口。
 * @param xpos - 消息框的横坐标。
 * @param ypos - 消息框的纵坐标。
 * @param Msg - 以空字符结尾的消息文本，不得为 NULL。
 * @param SurviveTime - 定时消息框的初始显示时长。
 * @param Type - 消息框处理函数使用的行为类型。
 * @param WaitFlag - 事件等待型消息框使用的事件组位掩码。
 * @return 无返回值。
 * @note 复制消息文本，并在配置了匹配项时选择对应图标；随后清除调用方层级的结果队列并打开子窗口。参数无效、嵌套层级已满或队列操作失败时打印错误且不打开窗口。
 */
void ShowMsgBox(uint16_t xpos,uint16_t ypos,char* Msg,uint16_t SurviveTime,uint8_t Type,uint32_t WaitFlag)
{
	uint8_t i;
	uint8_t msg_id = GetCurrentTaskPrio();
	uint8_t queue_id = (uint8_t)(msg_id + 1U);
	uint8_t num = (uint8_t)(sizeof(DispMsgTable) / sizeof(DispMsgTable[0]));
	osStatus_t status;

	if (Msg == NULL)
	{
		printf("Cannot show message box with a null message\r\n");
		return;
	}
	if (ColorWm.IsChildFlag >= 2U)
	{
		printf("Cannot show message box: child window depth is full\r\n");
		return;
	}

	MsgBox.MsgIcon = NULL;
	(void)snprintf(MsgBox.msg, sizeof(MsgBox.msg), "%s", Msg);
	for (i = 0U; i < num; i++)
	{
		if (strcmp(DispMsgTable[i].msg, Msg) == 0)
		{
			(void)snprintf(MsgBox.msg, sizeof(MsgBox.msg), "%s",
			               DispMsgTable[i].msg);
			MsgBox.MsgIcon = DispMsgTable[i].MsgIcon;
			break;
		}
	}

	MsgBox.SurviveTime = SurviveTime;
	MsgBox.Type = Type;
	MsgBox.WaitFlag = WaitFlag;

	status = ColorWM_ClearWindowMessage(queue_id);
	if (status != osOK)
	{
		printf("Failed to clear message box result queue: %ld\r\n",
		       (long)status);
		return;
	}

	ColorWindowsJumpOutChild(CHILD_WIN_NUM_MSG_BOX,xpos,ypos);
}

/**
 * @brief 请求关闭当前活动的消息框子窗口。
 * @return 无返回值。
 * @note 调用 ColorWindowsJumpInChild；只有消息框为当前活动子窗口时才会关闭。
 */
void HideMsgBox(void)
{


	ColorWindowsJumpInChild(CHILD_WIN_NUM_MSG_BOX);
}





/**
 * @brief 初始化数字键盘状态并打开数字键盘子窗口。
 * @param xpos - 键盘窗口的横坐标。
 * @param ypos - 键盘窗口的纵坐标。
 * @param DataType - 调用方指定的数字结果类型。
 * @param max - 允许输入的上限，包含该值。
 * @param min - 允许输入的下限，包含该值。
 * @return 无返回值。
 * @note 清空输入和结果状态，保存类型及范围，清除调用方层级的结果队列并打开子窗口。队列操作失败时打印错误且不打开窗口。
 */
void EnterDigitalKeyBoard(uint16_t xpos,uint16_t ypos,uint8_t DataType,
                          float max,float min)
{
	uint8_t msg_id = GetCurrentTaskPrio();
	osStatus_t status;

	DigitalKeyboard.len = 0U;
	DigitalKeyboard.Str[0] = '\0';
	DigitalKeyboard.KeyboardMax = max;
	DigitalKeyboard.KeyboardMin = min;
	DigitalKeyboard.RetDataType = DataType;
	DigitalKeyboard.RetStatus = 0U;

	status = ColorWM_ClearWindowMessage((uint8_t)(msg_id + 1U));
	if (status != osOK)
	{
		printf("Failed to clear keyboard result queue: %ld\r\n",
		       (long)status);
		return;
	}

	ColorWindowsJumpOutChild(CHILD_WIN_NUM_DIGITAL_KEYBOARD,xpos,ypos);
}







/**
 * @brief 校验并转换数字键盘输入，发送结果后关闭键盘。
 * @return 无返回值。
 * @note 发送取消状态、带转换数值的确认状态或参数错误状态。发送成功后更新 RetStatus、清空输入并关闭子窗口；发送失败时打印错误并保持键盘打开。
 */
void ExitDigitalKeyBoard(void)
{
	uint8_t msg_id = GetCurrentTaskPrio();
	uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
	osStatus_t status;
	char *parseEnd;
	double parsedValue;
	int32_t integerValue;
	uint32_t unsignedValue;
	float floatValue;
	uint8_t isInRange = 0U;

	if (DigitalKeyboard.len == 0U)
	{
		message[0] = MSG_CANCLE;
	}
	else
	{
		if (DigitalKeyboard.len >= sizeof(DigitalKeyboard.Str))
		{
			message[0] = MSG_PAR_ERR;
		}
		else
		{
			DigitalKeyboard.Str[DigitalKeyboard.len] = '\0';
			switch (DigitalKeyboard.RetDataType)
			{
				case DATA_TYPE_WAVELEN:
				case DATA_TYPE_NORMAL:
					integerValue = EnterData_ASCIItoHEX(
						DigitalKeyboard.Str, DigitalKeyboard.RetDataType);
					memcpy(&message[1], &integerValue, sizeof(integerValue));
					isInRange = ((float)integerValue >=
					             DigitalKeyboard.KeyboardMin) &&
					            ((float)integerValue <=
					             DigitalKeyboard.KeyboardMax);
					break;

				case DATA_TYPE_TIME:
					unsignedValue = (uint32_t)EnterData_ASCIItoHEX(
						DigitalKeyboard.Str, DATA_TYPE_TIME);
					memcpy(&message[1], &unsignedValue,
					       sizeof(unsignedValue));
					isInRange = ((float)unsignedValue >=
					             DigitalKeyboard.KeyboardMin) &&
					            ((float)unsignedValue <=
					             DigitalKeyboard.KeyboardMax);
					break;

				case DATA_TYPE_A:
				case DATA_TYPE_E:
					parsedValue = strtod((const char *)DigitalKeyboard.Str,
					                     &parseEnd);
					floatValue = (float)parsedValue;
					memcpy(&message[1], &floatValue, sizeof(floatValue));
					isInRange = (parseEnd !=
					             (char *)DigitalKeyboard.Str) &&
					            (*parseEnd == '\0') &&
					            (floatValue >=
					             DigitalKeyboard.KeyboardMin) &&
					            (floatValue <=
					             DigitalKeyboard.KeyboardMax);
					break;

				default:
					message[0] = MSG_PAR_ERR;
					break;
			}
		}

		if (message[0] != MSG_PAR_ERR)
		{
			message[0] = (isInRange != 0U) ? MSG_CONFIRM : MSG_PAR_ERR;
		}
	}

	status = ColorWM_PostWindowMessage(msg_id, message);
	if (status != osOK)
	{
		printf("Failed to post keyboard result: %ld\r\n", (long)status);
		return;
	}

	DigitalKeyboard.RetStatus = (message[0] == MSG_CONFIRM) ? 1U : 0U;
	DigitalKeyboard.len = 0U;
	DigitalKeyboard.Str[0] = '\0';
	ColorWindowsJumpInChild(CHILD_WIN_NUM_DIGITAL_KEYBOARD);
}

/**
 * @brief 重置文本输入状态并打开字母键盘子窗口。
 * @param xpos - 键盘窗口的横坐标。
 * @param ypos - 键盘窗口的纵坐标。
 * @param MaxLen - 允许输入的最大文本长度，会限制在消息和存储缓冲区容量以内。
 * @return 无返回值。
 * @note 清空英文、拼音、候选字及显示状态，清除调用方层级的结果队列后打开子窗口。队列操作失败时打印错误且不打开窗口。
 */
void EnterAlphabetKeyBoard(uint16_t xpos,uint16_t ypos,uint8_t MaxLen)
{
	uint8_t msg_id = GetCurrentTaskPrio();
	uint8_t maxLen = MaxLen;
	osStatus_t status;

	if (maxLen == 0U)
	{
		maxLen = 1U;
	}
	if (maxLen > (COLOR_WM_MESSAGE_SIZE - 2U))
	{
		maxLen = (uint8_t)(COLOR_WM_MESSAGE_SIZE - 2U);
	}
	if (maxLen >= sizeof(AlphabetKeyboard.Str))
	{
		maxLen = (uint8_t)(sizeof(AlphabetKeyboard.Str) - 1U);
	}

	AlphabetKeyboard.MaxLen = maxLen;
	AlphabetKeyboard.len = 0U;
	AlphabetKeyboard.Str[0] = '\0';
	AlphabetKeyboard.Enlen = 0U;
	AlphabetKeyboard.EnStr[0] = '\0';
	AlphabetKeyboard.PinYinlen = 0U;
	AlphabetKeyboard.PinYinStr[0] = '\0';
	AlphabetKeyboard.HZ_num = 0U;
	AlphabetKeyboard.DispPage = 0U;
	AlphabetKeyboard.DispBuff[0] = '\0';

	status = ColorWM_ClearWindowMessage((uint8_t)(msg_id + 1U));
	if (status != osOK)
	{
		printf("Failed to clear alphabet keyboard result queue: %ld\r\n",
		       (long)status);
		return;
	}

	ColorWindowsJumpOutChild(CHILD_WIN_NUM_ALPHABET_KEYBOARD,xpos,ypos);
}


/**
 * @brief 清除字母键盘输入及候选状态，然后关闭子窗口。
 * @return 无返回值。
 * @note 重置英文、拼音、汉字候选和显示缓冲区后请求关闭子窗口。
 */
void ExitAlphabetKeyBoard(void)
{
	AlphabetKeyboard.len = 0U;
	AlphabetKeyboard.Str[0] = '\0';
	AlphabetKeyboard.Enlen = 0U;
	AlphabetKeyboard.EnStr[0] = '\0';
	AlphabetKeyboard.PinYinlen = 0U;
	AlphabetKeyboard.PinYinStr[0] = '\0';
	AlphabetKeyboard.HZ_num = 0U;
	AlphabetKeyboard.DispPage = 0U;
	AlphabetKeyboard.DispBuff[0] = '\0';
	ColorWindowsJumpInChild(CHILD_WIN_NUM_ALPHABET_KEYBOARD);
}





/**
 * @brief 初始化欢迎界面，并显示启动状态或 PC 连接状态。
 * @return 无返回值。
 * @note 等待一秒；PC 未连接时显示启动提示，已连接时显示 PC 连接图标。
 */
void WelcomeW_Init(void)
{


	osDelay(pdMS_TO_TICKS(1000));

	
	if(SysFlag.PcConnectFlag == FALSE)
	{
		ColorLcdSendString(400,450,COLOR_LCD_LIB_32_32,FCOLOR_ON|GB2312,0X03,COLOR_BLUE,COLOR_WHITE,(uint8_t*)"System Booting...");

	}
	else
	{
		ColorLcdDispIcon_Ext1(ICON_CONNECT_PC,ICON_CONNECT_PC->left,ICON_CONNECT_PC->top);
	}

}

/**
 * @brief 更新欢迎界面的绘制内容。
 * @return 无返回值。
 * @note 当前函数体为空，保留为欢迎窗口注册使用的绘制回调。
 */
void WelcomeW(void)
{
}

/**
 * @brief 处理欢迎界面的按键输入，并切换到主菜单。
 * @return 无返回值。
 * @note 按下断开连接键时清除 PC 连接标志；若正在扫描则请求紧急停止，然后切换到主菜单。
 */
void WelcomeK(void)
{
	if(TouchKey.Trg == 1)
	{
		if (TouchKey.current_keyassignments == KEY_WELCOME_DIS_CONNECT)
		{
			SysFlag.PcConnectFlag = FALSE;

				//ColorWindowsKeyM_Ext(WIN_NUM_GUDUSHEZHI,0,DispGuDuCeShiK,DispGuDuCeShiW,DispGuDuCeShiW_Init);
			if (SysFlag.RunWaveScanFlag == 1 || SysFlag.RunBaseScanFlag == 1)
			{
				SysFlag.EmergercyStopFlag = 1;
			}
			ColorWindowsControl(WIN_NUM_MAIN_MENU,0);
		} 


	}
}


	



/**
 * @brief 推进并显示仪器初始化流程。
 * @return 无返回值。
 * @note 读取初始化事件标志并更新进度；波长或 500 点校准失败时显示错误消息；所有初始化标志完成后进入主菜单。静态进度和状态变量会在多次调用之间保留。
 */
void InitW(void)
{
	static uint16_t pos = 0;
	static uint8_t stat = 3;
	static uint16_t percent = 0;
	EventBits_t tFlag = 0;
//	uint8_t	 i = 0;
	char str[20];

	if(SysFlag.KeyDebugFlag == 1)
	{
		sprintf(str,"PCB ver:%s",PCB_VERSION);
		ColorLcdSendString(140,100,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8_t*)str);


	}



	ColorLcdDispIcon1(ICON_CUT_PIC_NUM_1,245,328,245+percent*33/10,328+20,245,328);
	percent++;
	
	if (MyInitFlag == NULL)
	{
		MyInitFlag_Init();
	}

	switch(stat)
	{
			
		case 3:
			tFlag = xEventGroupGetBits(MyInitFlag);
			tFlag = tFlag & (INIT_WL_GOOD_FLAG | INIT_WL_ERR_FLAG  );
			if(tFlag == INIT_WL_GOOD_FLAG)
			{
				stat = 4;
//				cnt=0;
			}
			else if (tFlag == INIT_WL_ERR_FLAG)
			{
				stat = 5;
				ShowMsgBox(200,160,"锟斤拷锟斤拷锟斤拷始锟斤拷锟斤拷锟斤拷",300,2,SYS_MOVE_WL_FLAG);
			}
			else
			{
				if(pos < 15)
				{
				}
			}
			break;
		case 4:
			tFlag = xEventGroupGetBits(MyInitFlag);
			tFlag = tFlag & (INIT_WL_500_GOOD_FLAG | INIT_WL_500_ERR_FLAG);
			if(tFlag == INIT_WL_500_GOOD_FLAG)
			{
				stat = 6;
//				cnt=0;
			}
			else if (tFlag == INIT_WL_500_ERR_FLAG)
			{
				stat = 5;
				ShowMsgBox(200,160,"锟斤拷锟斤拷500校准锟斤拷锟斤拷",300,2,SYS_MOVE_WL_FLAG);
			}
			else
			{
				if(pos < 17)
				{
				}
			}
			break;
			
			
		case 5:
			osDelay(pdMS_TO_TICKS(1000));
			break;

		case 6:
			tFlag = xEventGroupGetBits(MyInitFlag);
			tFlag = tFlag & (INIT_ALL_GOOD_FLAG);
			if(tFlag == INIT_ALL_GOOD_FLAG)
			{
				pos = 0;
				stat = 0;
//				cnt=0;
				ColorWindowsControl(WIN_NUM_MAIN_MENU,0);
			}
			break;

	}


}
/**
 * @brief 处理初始化界面的触摸状态及长按调试模式切换。
 * @return 无返回值。
 * @note 触摸状态变化时重置长按计数；在根菜单长按后门按键达到设定时长时切换 SysFlag.KeyDebugFlag 并鸣响提示。
 */
void InitK(void)
{
	static uint16_t count = 0;
	uint32_t tmp_key=0;
	if(TouchKey.Trg != 0)
	{
//		if(InitWin.Step == 5)
////				ColorWindowsKeyM_Ext(WIN_NUM_GUDUSHEZHI,0,DispGuDuCeShiK,DispGuDuCeShiW,DispGuDuCeShiW_Init);
//					ColorWindowsControl(WIN_NUM_GUDU_CESHI,0);

		count = 0;
	}
	else if (TouchKey.Release == 1)
	{
		TouchKey.Release = 0;
		count = 0;
	}
	else if (TouchKey.Press == 1)
	{
		tmp_key = TouchKey.current_keyassignments;
		if (tmp_key == KEY_BACK_DOOR)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{
				if(SysFlag.KeyDebugFlag == 0)
				{
					if(count++ > 50)
					{
						SysFlag.KeyDebugFlag = 1;
						BEEP_ONCE();
						count = 0;
					}
				}
				else
				{
					if(count++ > 50)
					{
						SysFlag.KeyDebugFlag = 0;
						BEEP_ONCE();
						count = 0;
					}
				}
			}
			else
			{
				count = 0;
			}
		}
		else
		{
			count = 0;
		} 
	}
}


/**
 * @brief 绘制主菜单上的临时错误消息并处理其显示超时。
 * @return 无返回值。
 * @note 递减 ColorWm.Timer；计时未结束时绘制消息，计时结束后清除消息并恢复保存的菜单编号。
 */
void MainMenuW(void)
{ 
	if (ColorWm.FatherMenuNumber == MENU_NUMBER_ERR_MSG)
	{
		if(ColorWm.Timer--)
		{
			ColorLcdDispMessagebox(190,181,ColorWm.ErrMsg,2,COLOR_LCD_LIB_16_16);
		}
		else
		{
			ColorLcdClrMessagebox(190,181);
			ColorWm.FatherMenuNumber = ColorWm.ErrRetMenuNumber;
		}
	}
  
}


/**
 * @brief 处理主菜单按键释放事件，并切换到所选测量界面。
 * @return 无返回值。
 * @note 仅在根菜单响应光谱、定量、动力学或信噪比菜单项，处理后清除按键释放标志。
 */
void MainMenuK(void)
{ 

	uint32_t tmp_key = 0;

	tmp_key = TouchKey.back_keyassignments;
	
	if(TouchKey.Release== 1)
	{
		if (tmp_key == KEY_MAIN_MENU_GUANG_PU)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	//ColorWindowsKeyM_Ext(WIN_NUM_GUPUSHEZHI,0,DispGuPuCeShiK,DispGuPuCeShiW,DispGuPuCeShiW_Init);
				ColorWindowsControl(WIN_NUM_GUPU_CESHI,0);
			}
		} 

		else if (tmp_key == KEY_MAIN_MENU_DING_LIANG)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	
				ColorWindowsControl(WIN_NUM_DING_LIANG_CESHI,2);
			}
		} 
		else if (tmp_key == KEY_MAIN_MENU_DONG_LI_XUE)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	
				ColorWindowsControl(WIN_NUM_DONG_LI_XUE_CESHI,0);
			}
		} 
 		else if (tmp_key == KEY_MAIN_MENU_SYSTEM_CONFIG)
 		{
				if(ColorWm.FatherMenuNumber == 0)
				{
					ColorWindowsControl(WIN_NUM_SNR_CESHI,0);
				}
 		} 
		TouchKey.Release=0;
	}	
}






#define MSB_BOX_POS_X   300	

void GuPuCeShiW_Init(void)
{
// 	CPU_SR        cpu_sr;
// 	uint16 tmp;
	char str[30];
	char str1[30];
	
//	sprintf(str,"%s",SlitTable[1+SlitPosition.CurPos]);


	sprintf(str,"%4.0f",gDraw.top);
	ColorLcdSendString(50,DRAW_FRAME_TOP,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8_t*)str);
	sprintf(str1,"%4.0f",gDraw.bottom);
	ColorLcdSendString(50,DRAW_FRAME_BOTTOM-24,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8_t*)str1);


	sprintf(str,"%d.%dnm",	gDraw.left/Unit_1nm,
							gDraw.left%Unit_1nm
						);
	ColorLcdSendString(DRAW_FRAME_LEFT-strlen(str)*6,DRAW_FRAME_BOTTOM,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8_t*)str);


	sprintf(str,"%4d.%dnm",	gDraw.right/Unit_1nm,
						gDraw.right%Unit_1nm
						);
	ColorLcdSendString(DRAW_FRAME_RIGHT - strlen(str)*6,DRAW_FRAME_BOTTOM,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8_t*)str);



	sprintf(str,"%4d.%d",CurWaveLen/Unit_1nm,CurWaveLen%Unit_1nm);

	
	sprintf(str,"%4d",MySysAbs.data_E);
	ColorLcdSendString(300,25,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8_t*)str);


	if(MVStruct.sensitive_level == 0)
	{
		MVStruct.sensitive_level = 1;
	}

	sprintf(str,"%d",MVStruct.sensitive_level);
	ColorLcdSendString(315,550,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8_t*)str);



	ColorLcdColorConfig(COLOR_RED,COLOR_WHITE);
	ColorLcdDrawOneLine(38,168,80,168);

	osDelay(pdMS_TO_TICKS(500));
	ColorLcdColorConfig(COLOR_BLUE,COLOR_WHITE);


	GuPuCeShiStruct.M1_M2_mode = 0;
	GuPuCeShiStruct.M1.x_data = gDraw.left;
	GuPuCeShiStruct.M2.x_data = gDraw.left;

	
	
}

extern PeakInfo gPeak[30];

void GuPuCeShiW(void)
{
	char str[40];
	uint16_t tmp;
	
	if(ColorWm.FatherMenuNumber==0)
	{
			tmp = LineDispBuf.scan_sum;
			if(tmp>0)
			{
				ColorLcdDispBuffLines(SCAN_LINE1_BUF_ADDR,tmp);
			}
		
			sprintf(str,"%4d",MySysAbs.data_E);
			ColorLcdSendString(300,25,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8_t*)str);

	}
	else if (ColorWm.FatherMenuNumber == 2)//扫描的时候
	{
		EventBits_t eventBits = 0U;

		if(MyOsFlag != NULL)
		{
			eventBits = xEventGroupWaitBits(MyOsFlag,
			                                (EventBits_t)SYS_SCAN_WL_FINISH_FLAG,
			                                pdTRUE,
			                                pdTRUE,
			                                0U);
		}

		if((eventBits & (EventBits_t)SYS_SCAN_WL_FINISH_FLAG) == (EventBits_t)SYS_SCAN_WL_FINISH_FLAG)
		{
			ColorWm.FatherMenuNumber = 0;
			ColorLcdDispIcon_Ext1_1(ICON_TEST_GUANGPU);
			
			for(int i=0;i<30;i++)
			{
				gPeak[i].is_peak = 0;
				gPeak[i].wavelength = 0;
				gPeak[i].intensity = 0;
			}
			char str[20];

			detect_peaks_and_valleys(gDraw.left,gDraw.right,gDraw.Interval,0.08,100/gDraw.Interval,gPeak,30);
			for(int i=0;i<30;i++)
			{
				if(gPeak[i].is_peak && gPeak[i].wavelength >0)
				{
					uint16_t tmp_data = gPeak[i].intensity;
					uint16_t tmp_wave = gPeak[i].wavelength;
					PIXEL_POINT pt = CalcXY(tmp_wave, tmp_data,gDraw,(_FRAME*)&WL_FRAME);
					sprintf(str,"%3d.%d:%d" ,tmp_wave/10,tmp_wave%10,tmp_data);
					ColorLcdSendString(pt.x,pt.y,COLOR_LCD_LIB_12_12/*COLOR_LCD_LIB_24_24*/,FCOLOR_ON|GB2312,0X03,0,COLOR_WHITE,(uint8_t*)str);
				}
			}
		}

	}
	else if(ColorWm.FatherMenuNumber == 3)//重新画曲线的时候
	{
		RedrawGuangPuLine(gDraw);
		ColorWm.FatherMenuNumber = 0;
	}
	else if (ColorWm.FatherMenuNumber == 4)//扫描准备中
	{
		uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
		uint8_t msg_id = GetCurrentTaskPrio();
		osStatus_t status = ColorWM_WindowMessageReceive(
			msg_id, message, pdMS_TO_TICKS(40U));

		if(status == osOK)
		{
			if(message[0] == MSG_CONFIRM)
			{
				ColorWm.FatherMenuNumber = 2;
			}
		}
		else if(status != osErrorTimeout)
		{
			printf("Failed to receive scan confirmation: %ld\r\n",
			       (long)status);
		}
	}
	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_PRINT)
	{
		uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
		uint8_t msg_id = GetCurrentTaskPrio();
		osStatus_t status = ColorWM_WindowMessageReceive(
			msg_id, message, pdMS_TO_TICKS(40U));

		if(status == osOK)
		{
			if(message[0] == MSG_CONFIRM)
			{
				ColorWm.FatherMenuNumber = 0;
			}
			else if(message[0] == MSG_CANCLE)
			{
				ColorWm.FatherMenuNumber = 0;
			}
		}
		else if(status != osErrorTimeout)
		{
			printf("Failed to receive print confirmation: %ld\r\n",
			       (long)status);
		}
	}
	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_KEY_BOARD)
	{
		uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
		char str[20];
		uint8_t msg_id = GetCurrentTaskPrio();
		osStatus_t status = ColorWM_WindowMessageReceive(
			msg_id, message, pdMS_TO_TICKS(40U));

		if(status == osOK)
		{
			if(message[0] == MSG_CONFIRM)
			{
				
				int32_t tmp=0;	
				memcpy((uint8_t*)&tmp,(uint8_t*)&message[1],4);

				if(ColorWm.FatherPageNumber == 1)
				{
					MVStruct.sensitive_level = tmp;
				}							

				ColorWm.FatherMenuNumber = 0;
			}
			else if(message[0] == MSG_PAR_ERR)
			{

				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;


			}
			else if(message[0] == MSG_CANCLE)
			{
				ColorWm.FatherMenuNumber = 0;
			}
			GuPuCeShiW_Init();
		}
		else
		{
			if(status != osErrorTimeout)
			{
				printf("Failed to receive keyboard result: %ld\r\n",
				       (long)status);
			}
			if(ColorWm.FatherPageNumber == 1)
			{
				sprintf(str,"%d",MVStruct.sensitive_level);
				ColorLcdSendString(315,550,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8_t*)str);
			}
		}
	}
	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_CHILD_WIN)//等待标尺设置返回
	{
		uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
		uint8_t msg_id = GetCurrentTaskPrio();
		osStatus_t status = ColorWM_WindowMessageReceive(
			msg_id, message, pdMS_TO_TICKS(40U));

		if(status == osOK)
		{
			if(message[0] == MSG_CONFIRM)
			{
				GuPuCeShiW_Init();
				ColorWm.FatherMenuNumber = 0;
			}
			else if(message[0] == MSG_CANCLE)
			{
				ColorWm.FatherMenuNumber = 0;
			}
		}
		else if(status != osErrorTimeout)
		{
			printf("Failed to receive child window result: %ld\r\n",
			       (long)status);
		}
	}
}


void GuPuCeShiK(void)
{
	uint32_t tmp_key = 0;
	
	if(TouchKey.Release== 1)
	{
		tmp_key = TouchKey.back_keyassignments;

		if (tmp_key == KEY_GUANGPU_SET_SCALE)
		{
			if (ColorWm.FatherMenuNumber == 0)//设置标尺
			{
				SetScale.left =gDraw.left;
				SetScale.right = gDraw.right;
				SetScale.top = gDraw.top;
				SetScale.bottom = gDraw.bottom;
				
				
				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;
				ColorWindowsJumpOutChild(CHILD_WIN_NUM_SET_SCALE, 100,60);
			}
		}
		else if(tmp_key == KEY_GUANGPU_SENS_SENSITIVE_LEVEL)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	

				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
				EnterDigitalKeyBoard(500,155,DATA_TYPE_NORMAL,3,1);
			}
		}

		else if(tmp_key == KEY_GUANGPU_TO_CORRECT_WAVE)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	

				detect_peaks_and_valleys(gDraw.left,gDraw.right,gDraw.Interval,0.08,100/gDraw.Interval,gPeak,30);
				for(int i=0;i<10;i++)
				{
					gNew_wl_table[i].wl = 0;
					gNew_wl_table[i].pos = 0;

					if(gPeak[i].is_peak && gPeak[i].wavelength >0)
					{
						uint16_t tmp_data = gPeak[i].intensity;
						uint16_t tmp_wave = gPeak[i].wavelength;
						gNew_wl_table[i].wl = tmp_wave;
						gNew_wl_table[i].pos = WavelengthToPosition(tmp_wave);
					}
				}
				ColorWindowsControl(WIN_NUM_WAVE_CORRECT,0);



			}
		}
		else if(tmp_key == KEY_GUANGPU_CLEAR)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	
				MVStruct.M_sum = 0;
				MVStruct.V_sum = 0;	
				LineDispBuf.scan_sum = 0;
				ColorLcdDispResetAt(DRAW_FRAME_LEFT,DRAW_FRAME_TOP,DRAW_FRAME_RIGHT,DRAW_FRAME_BOTTOM);
			}
		}
		else if(tmp_key == KEY_GUANGPU_M1_M2)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	
				if(GuPuCeShiStruct.M1_M2_mode != 0)
				{
					GuPuCeShiStruct.M1_M2_mode = 0;
					ColorLcdDispIcon_Ext1_1(ICON_M1);
				}
				else
				{
					GuPuCeShiStruct.M1_M2_mode = 1;
					ColorLcdDispIcon_Ext1_1(ICON_M2);
				}
			}
		}
		else if(tmp_key == KEY_GUANGPU_SET_PARA)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{	
				ColorWindowsControl(WIN_NUM_GUPU_PAR,0);
			}
		}
		else if (tmp_key == KEY_GUANGPU_SAVE_LOAD)
		{						
			if(ColorWm.FatherMenuNumber == 0)
			{	
				g_save_load_type = SAVE_LOAD_TYPE_GUANG_PU;
				ColorWindowsControl(WIN_NUM_SAVE_LOAD,0);
			}
		}
		else if(tmp_key == KEY_GUANGPU_FIND_PEAK)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{

				for(int i=0;i<30;i++)
				{
					gPeak[i].is_peak = 0;
					gPeak[i].wavelength = 0;
					gPeak[i].intensity = 0;
				}
				char str[20];

				detect_peaks_and_valleys(gDraw.left,gDraw.right,gDraw.Interval,0.08,100/gDraw.Interval,gPeak,30);
				for(int i=0;i<30;i++)
				{
					if(gPeak[i].is_peak && gPeak[i].wavelength >0)
					{
						uint16_t tmp_data = gPeak[i].intensity;
						uint16_t tmp_wave = gPeak[i].wavelength;
						PIXEL_POINT pt = CalcXY(tmp_wave, tmp_data,gDraw,(_FRAME*)&WL_FRAME);
						sprintf(str,"%3d.%d:%d" ,tmp_wave/10,tmp_wave%10,tmp_data);
						ColorLcdSendString(pt.x,pt.y,COLOR_LCD_LIB_12_12/*COLOR_LCD_LIB_24_24*/,FCOLOR_ON|GB2312,0X03,0,COLOR_WHITE,(uint8_t*)str);
					}
				}

			}
		}
		else if(tmp_key == KEY_GUANGPU_TEST)
		{
			if(ColorWm.FatherMenuNumber == 0)
			{
				if(GuPuCeShiStruct.WaveLenEnd <GuPuCeShiStruct.WaveLenStart)
				{

				}
				else if((GuPuCeShiStruct.data_E_End<GuPuCeShiStruct.data_E_Start) )
				{

				}
				else
				{
					gDraw.x_Width= GuPuCeShiStruct.WaveLenEnd - GuPuCeShiStruct.WaveLenStart;
					gDraw.y_Height = GuPuCeShiStruct.data_E_End - GuPuCeShiStruct.data_E_Start;
					gDraw.left = GuPuCeShiStruct.WaveLenStart;
					gDraw.right = GuPuCeShiStruct.WaveLenEnd;
					gDraw.top = GuPuCeShiStruct.data_E_End;
					gDraw.bottom = GuPuCeShiStruct.data_E_Start;
					gDraw.Interval = GuPuCeShiStruct.Interval;
					gDraw.x_cur = GuPuCeShiStruct.WaveLenStart;


					gDraw.left_reserve = gDraw.left;
					gDraw.right_reserve= gDraw.right;
					gDraw.top_reserve= gDraw.top;
					gDraw.bottom_reserve= gDraw.bottom;

					gDraw.y_cur = 0.0;	

					memcpy((uint8_t*)&SysPara.GuPuPar,(uint8_t*)&GuPuCeShiStruct,sizeof(GuPuCeShiStruct));
					UpdateSysParameter();
					LineDispBuf.num = LineDispBuf.scan_sum = 0;
					GuPuCeShiW_Init();
					
					// ColorLcdDispIcon_Ext1(ICON_STOP_GUANGPU,718,	256);
					ColorLcdDispResetAt(DRAW_FRAME_LEFT,DRAW_FRAME_TOP,DRAW_FRAME_RIGHT,DRAW_FRAME_BOTTOM);
					osDelay(500);
					ColorLcdDispIcon_Ext1_1(ICON_STOP_GUANGPU);

					
					ColorWm.FatherMenuNumber = 4;
					
					uint8_t actionMessage[COLOR_WM_MESSAGE_SIZE] = {0U};
					actionMessage[0] = MSG_SCAN;
					if ((g_ActionQueue == NULL) ||
					    (xQueueSend(g_ActionQueue, actionMessage, 0U) != pdPASS))
					{
						printf("Failed to queue scan action\r\n");
					}

					
				}
			}
			else if(ColorWm.FatherMenuNumber == 2 )
			{
				SysFlag.EmergercyStopFlag = 1;
			}
		}
		TouchKey.Release = 0;
	}
	else if (TouchKey.Trg == 1)
	{
		tmp_key = TouchKey.current_keyassignments;
		if (tmp_key == KEY_GUANGPU_MAIN_MENU)
		{
			if (ColorWm.FatherMenuNumber == 0)//
			{
				ColorWindowsControl(WIN_NUM_MAIN_MENU,0);
			}
		}
		else if ((tmp_key == KEY_GUANGPU_CUR_CURSOR) || (tmp_key == KEY_GUANGPU_CURSOR_LEFT) || (tmp_key == KEY_GUANGPU_CURSOR_RIGHT))
		{
			if (ColorWm.FatherMenuNumber == 0)//
			{
				// uint32 wave = 0;
				char str[20];

				M1_M2* p = NULL;
				if(GuPuCeShiStruct.M1_M2_mode == 0)
				{
					p = &GuPuCeShiStruct.M1;
				}
				else
				{
					p = &GuPuCeShiStruct.M2;
				}
				if(tmp_key == KEY_GUANGPU_CUR_CURSOR)
				{
					p->x_data = (TouchKey.x - DRAW_FRAME_LEFT)*(gDraw.right - gDraw.left)/DRAW_FRAME_WIDTH + gDraw.left;
					p->x_data = p->x_data -(p->x_data% gDraw.Interval);
				}
				else if (tmp_key == KEY_GUANGPU_CURSOR_LEFT)
				{
					if(p->x_data>gDraw.left)
					{
						p->x_data-=gDraw.Interval;
					}
				}
				else if (tmp_key == KEY_GUANGPU_CURSOR_RIGHT)
				{
					if(p->x_data<gDraw.right)
					{
						p->x_data+=gDraw.Interval;
					}
				}

				p->y_val = GuangPuScanLine[p->x_data-MIN_WAVELEN];

//==================================================================================================================
				sprintf(str,"%6d",p->y_val);
//==================================================================================================================
				

				p->pt = CalcXY(p->x_data, p->y_val,gDraw,(_FRAME*)&WL_FRAME);
				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);
				for(int i=0;i<MVStruct.M_sum;i++)
				{
					uint16_t index = MVStruct.Montain[i];
					uint16_t tmp_data = GuangPuScanLine[gDraw.left-MIN_WAVELEN+index];
					uint16_t tmp_wave = index+gDraw.left;
					PIXEL_POINT pt = CalcXY(tmp_wave, tmp_data,gDraw,(_FRAME*)&WL_FRAME);

					sprintf(str,"%3d.%d:%d" ,tmp_wave/10,tmp_wave%10,tmp_data);
					ColorLcdSendString(pt.x,pt.y,COLOR_LCD_LIB_12_12/*COLOR_LCD_LIB_24_24*/,FCOLOR_ON|GB2312,0X03,0,COLOR_WHITE,(uint8_t*)str);
				}	

			}
		}

		
	}
	else if(TouchKey.Press == 1)
	{
		tmp_key = TouchKey.current_keyassignments;
		if ((tmp_key == KEY_GUANGPU_CUR_CURSOR) || (tmp_key == KEY_GUANGPU_CURSOR_LEFT) || (tmp_key == KEY_GUANGPU_CURSOR_RIGHT))
		{
			if (ColorWm.FatherMenuNumber == 0)//
			{
//				uint32 wave = 0;
				char str[20];
				M1_M2* p = NULL;
				if(GuPuCeShiStruct.M1_M2_mode == 0)
				{
					p = &GuPuCeShiStruct.M1;
				}
				else
				{
					p = &GuPuCeShiStruct.M2;
				}

				if(tmp_key == KEY_GUANGPU_CUR_CURSOR)
				{
					p->x_data = (TouchKey.x - DRAW_FRAME_LEFT)*(gDraw.right - gDraw.left)/DRAW_FRAME_WIDTH + gDraw.left;
					p->x_data = p->x_data -(p->x_data% gDraw.Interval);
				}
				else if (tmp_key == KEY_GUANGPU_CURSOR_LEFT)
				{
					if(p->x_data>gDraw.left)
					{
						p->x_data-=gDraw.Interval;
					}
				}
				else if (tmp_key == KEY_GUANGPU_CURSOR_RIGHT)
				{
					if(p->x_data<gDraw.right)
					{
						p->x_data+=gDraw.Interval;
					}
				}
				p->y_val = GuangPuScanLine[p->x_data-MIN_WAVELEN];

//==================================================================================================================
				sprintf(str,"%6d",p->y_val);
//==================================================================================================================
				

				p->pt = CalcXY(p->x_data, p->y_val,gDraw,(_FRAME*)&WL_FRAME);
				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);

				for(int i=0;i<MVStruct.M_sum;i++)
				{
					uint16_t index = MVStruct.Montain[i];
					uint16_t tmp_data = GuangPuScanLine[gDraw.left-MIN_WAVELEN+index];
					uint16_t tmp_wave = index+gDraw.left;
					PIXEL_POINT pt = CalcXY(tmp_wave, tmp_data,gDraw,(_FRAME*)&WL_FRAME);

					sprintf(str,"%3d.%d:%d" ,tmp_wave/10,tmp_wave%10,tmp_data);
					ColorLcdSendString(pt.x,pt.y,COLOR_LCD_LIB_12_12/*COLOR_LCD_LIB_24_24*/,FCOLOR_ON|GB2312,0X03,0,COLOR_WHITE,(uint8_t*)str);
				}	
			}
		}

	}

	
}


// void GuPuParSetW_Init(void)
// {
// 	char str[30];
	
// 	if (GuPuCeShiStruct.Speed== SCAN_SPEED_SLOW)	
// 	{
// 		ColorLcdDispIcon_Ext1(ICON_SPEED_SLOW_SELECT,ICON_SPEED_SLOW_SELECT->left,ICON_SPEED_SLOW_SELECT->top);
// 	}
// 	else if (GuPuCeShiStruct.Speed== SCAN_SPEED_MIDDLE)	
// 	{
// 		ColorLcdDispIcon_Ext1(ICON_SPEED_MIDDLE_SELECT,ICON_SPEED_MIDDLE_SELECT->left,ICON_SPEED_MIDDLE_SELECT->top);
// 	}
// 	else if (GuPuCeShiStruct.Speed== SCAN_SPEED_FAST)	
// 	{
// 		ColorLcdDispIcon_Ext1(ICON_SPEED_FAST_SELECT,ICON_SPEED_FAST_SELECT->left,ICON_SPEED_FAST_SELECT->top);
// 	}
	
// 	if (GuPuCeShiStruct.Interval== Unit_0_1nm)	
// 	{
// 		ColorLcdDispIcon_Ext1(ICON_INTERVAL_01_SELECT,ICON_INTERVAL_01_SELECT->left,ICON_INTERVAL_01_SELECT->top);
// 	}
// 	else if (GuPuCeShiStruct.Interval== Unit_1nm)	
// 	{
// 		ColorLcdDispIcon_Ext1(ICON_INTERVAL_10_SELECT,ICON_INTERVAL_10_SELECT->left,ICON_INTERVAL_10_SELECT->top);
// 	}
	

// 	sprintf(str,"%4d.%d",	GuPuCeShiStruct.WaveLenStart/Unit_1nm,
// 							GuPuCeShiStruct.WaveLenStart%Unit_1nm
// 						);
// 		ColorLcdSendString(222,170,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);


// 	sprintf(str,"%4d.%d",	GuPuCeShiStruct.WaveLenEnd/Unit_1nm,
// 						GuPuCeShiStruct.WaveLenEnd%Unit_1nm
// 						);
// 	ColorLcdSendString(407,170,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
	
// 	sprintf(str,"%4d",GuPuCeShiStruct.data_E_Start);
// 	ColorLcdSendString(222,212,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 	sprintf(str,"%4d",GuPuCeShiStruct.data_E_End);
// 	ColorLcdSendString(407,212,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);



// 	if(SysPara.LightPowerS.emit_light == EMIT_LIGHT_1)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_EMIT_LIGHT1_SELECT);
// 	}
// 	else if(SysPara.LightPowerS.emit_light == EMIT_LIGHT_2)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_EMIT_LIGHT2_SELECT);
// 	}
// 	else if(SysPara.LightPowerS.emit_light == EMIT_LIGHT_3)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_EMIT_LIGHT3_SELECT);
// 	}
// 	else if(SysPara.LightPowerS.emit_light == EMIT_LIGHT_4)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_EMIT_LIGHT4_SELECT);
// 	}
	
// 	if(SysPara.LightPowerS.pmt_on_off == PMT_ON)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_PMT_ON_SELECT);
// 	}
// 	else
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_PMT_OFF_SELECT);
// 	}
	
// 	sprintf(str,"%4d",SysPara.LightPowerS.pmt_voltage);
// 	ColorLcdSendString(222,426,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);

// 	if(SysPara.LightPowerS.gain == PMT_GAIN_1)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_GAIN1_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.gain == PMT_GAIN_2)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_GAIN2_SELECT);	
// 	}
// 	else if (SysPara.LightPowerS.gain == PMT_GAIN_4)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_GAIN4_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.gain == PMT_GAIN_8)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_GAIN8_SELECT);
// 	}



// }

// void GuPuParSetW(void)
// {
	
// 	if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_KEY_BOARD)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		char str[20];
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
				
// 				int32 tmp=0;	
// 				memcpy((uint8*)&tmp,(uint8*)&msg[1],4);

// 				if(ColorWm.FatherPageNumber == 1)
// 				{
// 					// memcpy((uint8*)&GuPuCeShiStruct.WaveLenStart,(uint8*)&msg[1],4);
// 					GuPuCeShiStruct.WaveLenStart = tmp;
// 				}
// 				else if(ColorWm.FatherPageNumber == 2)
// 				{
// 					// memcpy((uint8*)&GuPuCeShiStruct.WaveLenEnd,(uint8*)&msg[1],4);
// 					GuPuCeShiStruct.WaveLenEnd = tmp;
// 				}
// 				else if(ColorWm.FatherPageNumber == 3)
// 				{
// 					// memcpy((uint8*)&GuPuCeShiStruct.data_E_Start,(uint8*)&msg[1],4);
// 					GuPuCeShiStruct.data_E_Start = tmp;
// 				}
// 				else if(ColorWm.FatherPageNumber == 4)
// 				{
// 					// memcpy((uint8*)&GuPuCeShiStruct.data_E_End,(uint8*)&msg[1],4);
// 					GuPuCeShiStruct.data_E_End = tmp;
// 				}
// 				else if(ColorWm.FatherPageNumber == 5)
// 				{
// 					// memcpy((uint8*)&SysPara.LightPowerS.pmt_voltage,(uint8*)&msg[1],4);
// 					SysPara.LightPowerS.pmt_voltage = tmp;
// 					if(	SysPara.LightPowerS.pmt_on_off == PMT_ON)
// 					{
// 						BSP_Set_HV(SysPara.LightPowerS.pmt_voltage);
// 					}
// 				}
// 				ColorWm.FatherMenuNumber = 0;
// 				GuPuParSetW_Init();
// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 				GuPuParSetW_Init();
// 			}
// 		}
// 		else
// 		{
// 				if(ColorWm.FatherPageNumber == 1)
// 				{

// 					sprintf(str,"%4d.%d",	GuPuCeShiStruct.WaveLenStart/Unit_1nm,
// 											GuPuCeShiStruct.WaveLenStart%Unit_1nm
// 										);
// 					ColorLcdSendString(222,170,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 				else if(ColorWm.FatherPageNumber == 2)
// 				{

// 					sprintf(str,"%4d.%d",	GuPuCeShiStruct.WaveLenEnd/Unit_1nm,
// 										GuPuCeShiStruct.WaveLenEnd%Unit_1nm
// 										);
// 					ColorLcdSendString(407,170,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 				else if(ColorWm.FatherPageNumber == 3)
// 				{
// 						sprintf(str,"%4d",GuPuCeShiStruct.data_E_Start);
// 						ColorLcdSendString(222,212,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 				else if(ColorWm.FatherPageNumber == 4)
// 				{
// 						sprintf(str,"%4d",GuPuCeShiStruct.data_E_End);
// 						ColorLcdSendString(407,212,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 				else if(ColorWm.FatherPageNumber == 5)
// 				{
// 						sprintf(str,"%4d",SysPara.LightPowerS.pmt_voltage);
// 						ColorLcdSendString(222,426,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}

// 		}
// 	}
	
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_CHILD_WIN)
// 	{
		
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			GuPuParSetW_Init();
// 		}
		
// 	}


	

// }

// void GuPuParSetK(void)
// {
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_SET_GUPU_CONFIRM)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				UpdateSysParameter();
// 				ColorWindowsControl(WIN_NUM_GUPU_CESHI,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_CANCLE)
// 		{
			
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				ColorWindowsControl(WIN_NUM_GUPU_CESHI,0);
// 			}
// 		}

// 		else if (tmp_key == KEY_SET_GUPU_SPEED_SLOW)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_SPEED_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_SPEED_SLOW_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
				
// //				ColorLcdDispIcon_Ext1(ICON_SPEED_ALL_NOT_SELECT,175,294);
// //				ColorLcdDispIcon_Ext1(ICON_SPEED_SLOW_SELECT,175,294);
// 				GuPuCeShiStruct.Speed = SCAN_SPEED_SLOW;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_SPEED_MIDDLE)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_SPEED_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_SPEED_MIDDLE_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// //				ColorLcdDispIcon_Ext1(ICON_SPEED_ALL_NOT_SELECT,175,294);
// //				ColorLcdDispIcon_Ext1(ICON_SPEED_MIDDLE_SELECT,314,294);
// 				GuPuCeShiStruct.Speed = SCAN_SPEED_MIDDLE;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_SPEED_FAST)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_SPEED_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_SPEED_FAST_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// //				ColorLcdDispIcon_Ext1(ICON_SPEED_ALL_NOT_SELECT,175,294);
// //				ColorLcdDispIcon_Ext1(ICON_SPEED_FAST_SELECT,464,294);
// 				GuPuCeShiStruct.Speed = SCAN_SPEED_FAST;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_INTERVAL_01)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_INTERVAL_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_INTERVAL_01_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// //				ColorLcdDispIcon_Ext1(ICON_INTERVAL_ALL_NOT_SELECT,174,355);
// //				ColorLcdDispIcon_Ext1(ICON_INTERVAL_01_SELECT,174,355);
// 				GuPuCeShiStruct.Interval = Unit_0_1nm;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_INTERVAL_10)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_INTERVAL_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_INTERVAL_10_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// //				ColorLcdDispIcon_Ext1(ICON_INTERVAL_ALL_NOT_SELECT,174,355);
// //				ColorLcdDispIcon_Ext1(ICON_INTERVAL_10_SELECT,309,355);
// 				GuPuCeShiStruct.Interval = Unit_1nm;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_EMIT_LIGHT_1)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_EMIT_LIGHT_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_EMIT_LIGHT1_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_1;
// 				BSP_Light_Sel(1);
				
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_EMIT_LIGHT_2)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_EMIT_LIGHT_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_EMIT_LIGHT2_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_2;
// 				BSP_Light_Sel(2);
				
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_EMIT_LIGHT_3)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_EMIT_LIGHT_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_EMIT_LIGHT3_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_3;
// 				BSP_Light_Sel(3);
				
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_EMIT_LIGHT_4)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_EMIT_LIGHT_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_EMIT_LIGHT4_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_4;
// 				BSP_Light_Sel(4);
// 			}
// 		}
		
// 		else if (tmp_key == KEY_SET_GUPU_PMT_ON)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_PMT_NONE_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				ColorLcdDispIcon_Ext1_1(ICON_PMT_ON_SELECT);
// 				SysPara.LightPowerS.pmt_on_off = PMT_ON;
				
// 				BSP_Set_HV(SysPara.LightPowerS.pmt_voltage);
				
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_PMT_OFF)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_PMT_NONE_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_PMT_OFF_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.pmt_on_off = PMT_OFF;
// 				BSP_Set_HV(0);
// 			}
// 		}
		
// 		else if (tmp_key == KEY_SET_GUPU_GAIN_1)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_GAIN_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_GAIN1_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.gain = PMT_GAIN_1;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_GAIN_2)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_GAIN_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_GAIN2_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.gain = PMT_GAIN_2;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_GAIN_4)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_GAIN_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_GAIN4_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.gain = PMT_GAIN_4;
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_GAIN_8)	
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_GAIN_ALL_NOT_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				p = (SIMPLE_ICON*)ICON_GAIN8_SELECT;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 				SysPara.LightPowerS.gain = PMT_GAIN_8;
// 			}
// 		}
		
// 		else if (tmp_key == KEY_SET_GUPU_WL_START)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(500,155,DATA_TYPE_WAVELEN,MAX_WAVELEN,MIN_WAVELEN);
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_WL_END)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(500,155,DATA_TYPE_WAVELEN,MAX_WAVELEN,MIN_WAVELEN);
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_DATA_START)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(500,155,DATA_TYPE_NORMAL,5000,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_DATA_END)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
				
// 				EnterDigitalKeyBoard(500,155,DATA_TYPE_NORMAL,5000,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_SET_GUPU_PMT_VOLTAGE)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
				
// 				EnterDigitalKeyBoard(500,155,DATA_TYPE_NORMAL,1000,0);
// 			}
// 		}
		
		
		
// 		TouchKey.Release = 0;
// 	}

// }


// void SaveLoadW_Init(void)
// {

// 	uint8 i = 0;
// 	char str[25];
// 	SysLoadAllFileNames(g_save_load_type);
// 	for(i=0;i<MAX_FILE_SAVE_NUM;i++)
// 	{
// 		if(FileNameList[i].name[0]!='\0')
// 		{	
// 			sprintf(str,"%-20s",FileNameList[i].name);
// 			ColorLcdSendString(  	WindowSaveLoad_key_table[i].left,
// 														WindowSaveLoad_key_table[i].top,
// 														COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,
// 														(uint8*)str);
// 		}
// 		OSTimeDly(3);
// 	}
// }


// void SaveLoadW(void)
// {
// //	uint8 tIndex=0;
// //	char str[15];
	

// 	{

// 	}

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 tIndex=0xff;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{

// 			{

// 				FileNameList[tIndex].select = 0;
// 				if(tIndex!=0xff)
// 				{
// 					// SysSaveScanLine((char*)&msg[1],tIndex);
// 					SysSaveFiles(g_save_load_type, tIndex, (char*)&msg[1], "12345",5);
// 				}
// 				ColorWm.FatherMenuNumber = 0;
// 				SaveLoadW_Init();

// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 				SaveLoadW_Init();
// 			}
// 		}

// 		{

// 			if(tIndex != 0xff)
// 			{
// 				char str[25];

// 				ColorLcdSendString(  	WindowSaveLoad_key_table[tIndex].left,
// 															WindowSaveLoad_key_table[tIndex].top,
// 															COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON| GB2312,0X02,COLOR_BLUE,COLOR_WHITE,
// 															(uint8*)str);
// 			}
// 		}
// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_RENAME_WIN)
// 	{
		
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 tIndex=0xff;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{

// 			{

// 				FileNameList[tIndex].select = 0;
// 				if(tIndex!=0xff)
// 				{
// 					// SysRenameScanLine((char*)&msg[1],tIndex);
// 					SysRenameFileByTypeAndName(g_save_load_type, FileNameList[tIndex].name, (char*)&msg[1]);
// 				}
// 				ColorWm.FatherMenuNumber = 0;
// 				SaveLoadW_Init();

// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 				SaveLoadW_Init();
// 			}
// 		}

// 		{

// 			if(tIndex != 0xff)
// 			{
// 				char str[25];

// 				ColorLcdSendString(  	WindowSaveLoad_key_table[tIndex].left,
// 															WindowSaveLoad_key_table[tIndex].top,
// 															COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON| GB2312,
// 															0X02,
// 															COLOR_BLUE,
// 															COLOR_WHITE,
// 															(uint8*)str);
// 			}
// 		}

// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_MSG_BOX)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 				SaveLoadW_Init();
// 			}
			
// 		}
// 	}

// }

// void SaveLoadK(void)
// {
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;

// 	if(TouchKey.Trg == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if(tmp_key == KEY_SAVE_LOAD_CLOSE)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{
// 				if(g_save_load_type == SAVE_LOAD_TYPE_GUANG_PU)
// 				{
// 					ColorWindowsControl(WIN_NUM_GUPU_CESHI,3);
// 				}	
// 				else if(g_save_load_type == SAVE_LOAD_TYPE_DING_LIANG)
// 				{
// 					ColorWindowsControl(WIN_NUM_DING_LIANG_CESHI,0);
// 				}
// 				else if(g_save_load_type == SAVE_LOAD_TYPE_DONG_LI_XUE)
// 				{
// 					ColorWindowsControl(WIN_NUM_DONG_LI_XUE_CESHI,3);
// 				}
// 			}
// 		}

// 		{
// 			if(ColorWm.FatherMenuNumber == 0 || ColorWm.FatherMenuNumber == 1)
// 			{
// 				uint16 tIndex = 0xff;
// 				char str[15];



// 				{
// 					FileNameList[tIndex].select = 0;
// 					sprintf(str,"%-20s",FileNameList[tIndex].name);
// 					ColorLcdSendString(  	
// 										WindowSaveLoad_key_table[tIndex].left,
// 										WindowSaveLoad_key_table[tIndex].top,
// 										COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,
// 										(uint8*)str);
// 				}

				

// 				FileNameList[tIndex].select = 1;
// 				sprintf(str,"%-20s",FileNameList[tIndex].name);
// 				ColorLcdSendString(  	WindowSaveLoad_key_table[tIndex].left,
// 															WindowSaveLoad_key_table[tIndex].top,
// 															COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,
// 															(uint8*)str);
// 				ColorWm.FatherMenuNumber = 1;
// 			}
// 		}
// 	}
// 	else if (TouchKey.Release == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_SAVE_LOAD_SAVE)
// 		{

// 			{

// //				{
// 					uint8 tIndex = 0xff;
// 					uint16 xpos=0;
// 					uint16 ypos=0;

// 					if(tIndex != 0xff)
// 					{
// 						xpos = 80;
// 						if((tIndex%15)<7)
// 						{	
// 							ypos = WindowSaveLoad_key_table[tIndex].top + 30;
// 						}
// 						else
// 						{	
// 							ypos = WindowSaveLoad_key_table[tIndex].top -285;
// 						}

// 						ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_SAVE_NAME_WIN;

// 					}
// //				}
// 			}
// 		}

// 		{
// 			if(ColorWm.FatherMenuNumber == 1)
// 			{
// 				uint8 i = 0;
// 				for(i=0;i<MAX_FILE_SAVE_NUM;i++)
// 				{
// 					if(FileNameList[i].select == 1)
// 					{
// 						if(strlen(FileNameList[i].name) != 0)
// 						{
// 							// SysLoadScanLineRedrawPar(i);
// 							uint32 ret_len = 0;

// 							ret_len = SysLoadFileByTypeAndName(g_save_load_type, i,(char*)FileNameList[i].name, &ret_len);
// 							if(g_save_load_type == SAVE_LOAD_TYPE_GUANG_PU)
// 							{
// 								ColorWindowsControl(WIN_NUM_GUPU_CESHI,3);
// 							}	
// 							else if(g_save_load_type == SAVE_LOAD_TYPE_DING_LIANG)
// 							{
// 								ColorWindowsControl(WIN_NUM_DING_LIANG_CESHI,0);
// 							}
// 							else if(g_save_load_type == SAVE_LOAD_TYPE_DONG_LI_XUE)
// 							{
// 								ColorWindowsControl(WIN_NUM_DONG_LI_XUE_CESHI,3);
// 							}

// 						}
// 						break;
// 					}
// 				}
// 			}
// 		}

// 		{
// 			if(ColorWm.FatherMenuNumber == 1)
// 			{
// 					uint8 tIndex = 0xff;

// //					memset(FileNameList[tIndex].name,0x00,10);
// 					// SysDeleteScanLine(tIndex);
// 					SysDeleteFileByTypeAndName(g_save_load_type,tIndex,FileNameList[tIndex].name);


// 					{
// 						char str[21];
// 						FileNameList[tIndex].select = 0;
// 						memset(str,' ',20);
// 						str[20] = '\0';
// 						ColorLcdSendString(  	
// 											WindowSaveLoad_key_table[tIndex].left,
// 											WindowSaveLoad_key_table[tIndex].top,
// 											COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,
// 											(uint8*)str);
// 					}


// 					SaveLoadW_Init();
// 			}
// 			ColorWm.FatherMenuNumber = 0;
// 		}

// 		{
// 			if(ColorWm.FatherMenuNumber == 1)
// 			{
// 					uint8 tIndex = 0xff;
// 					uint16 xpos=0;
// 					uint16 ypos=0;

// 					if(tIndex != 0xff && strlen(FileNameList[tIndex].name) != 0)
// 					{
// 						xpos = 80;
// 						if((tIndex%15)<7)
// 						{	
// 							ypos = WindowSaveLoad_key_table[tIndex].top + 30;
// 						}
// 						else
// 						{	
// 							ypos = WindowSaveLoad_key_table[tIndex].top -285;
// 						}
// 						ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_RENAME_WIN;

// 					}
// 			}
// 		}
// 		TouchKey.Release = 0;
// 	}
// }




// //====================

// //====================
// #define DING_LIANG_CESHI_NUM_PER_PAGE 15

// void DingLiangCeShiW_Init(void)
// {
// 	char str[50];



// 	sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);



// 	sprintf(str,"%4d" , MySysAbs.data_E);


// 	if(DingLiangCeShiStruct.MultiPointMethod_b >= 0.0)
// 	{
// 		sprintf(str,"Conc = %.4f * En + %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 	}
// 	else
// 	{
// 		sprintf(str,"Conc = %.4f * En %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 	}
// 	ColorLcdSendString(142,546,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
	
// 	sprintf(str,"r = %.4f",DingLiangCeShiStruct.MultiPointMethod_r);
// 	ColorLcdSendString(142,571,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// 	sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.SampleBaseValue);
// 	ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
	
// 	if(DingLiangCeShiStruct.SampleEnBase == 1)
// 	{
// 			SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_EN_BASE;
// 			ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 	}
// 	else
// 	{
// 			SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_DIS_BASE;
// 			ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 	}

// }

// void DingLiangCeShiW(void)
// {   
// 	uint16 i =0;
// 	char str[50];
// 	const uint16 y_pos_table[15] = {113,138,163,188,213,238,263,288,313,338,363,388,413,438,463};

// 	if(ColorWm.FatherMenuNumber == 0)
// 	{

// 		sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);



// 		sprintf(str,"%4d" , MySysAbs.data_E);


// 		if(DingLiangCeShiStruct.MultiPointMethod_b >= 0.0)
// 		{
// 			sprintf(str,"Conc = %.4f * En + %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 		}
// 		else
// 		{
// 			sprintf(str,"Conc = %.4f * En %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 		}
// 		ColorLcdSendString(142,546,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
		
// 		OSTimeDly(OS_TICKS_PER_SEC/50);
// 		sprintf(str,"r = %.4f",DingLiangCeShiStruct.MultiPointMethod_r);
// 		ColorLcdSendString(142,571,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	}

// 	{
// 		uint8 err = 0;
// 		OSFlagPend(	MyOsFlag,
// 					SYS_CAL_CONC_FLAG,
// 					OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,
// 					0,
// 					&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(DingLiangCeShiStruct.MultiPointMethod_num == 0)
// 			{
// 				ColorLcdDispResetAt(40,135,650,528);
// 				ColorWm.FatherPageNumber = 0;
// 			}
// 			else if((DingLiangCeShiStruct.MultiPointMethod_num-1)/DING_LIANG_CESHI_NUM_PER_PAGE != ColorWm.FatherPageNumber)
// 			{
// 				ColorLcdDispResetAt(40,135,650,528);
// 				ColorWm.FatherPageNumber = (DingLiangCeShiStruct.MultiPointMethod_num-1)/DING_LIANG_CESHI_NUM_PER_PAGE;
// 				ColorWm.FatherMenuNumber = 2;
// 			}
// 			else
// 			{
// 				i = DingLiangCeShiStruct.MultiPointMethod_num-1;


// 				sprintf(str,"%03d       %4d             %7.4f",
// 							i+1,
// //							SlitTable[1+DingLiangCeShiStruct.MultiPointMethod_cell[i]],
// 							DingLiangCeShiStruct.MultiPointMethod_data[i],
// 							DingLiangCeShiStruct.MultiPointMethod_conc[i]);

// 				ColorWm.FatherMenuNumber = 0;
// 			}

		
// 		}
		
// 	}

// 	{
// 		for(i=ColorWm.FatherPageNumber*DING_LIANG_CESHI_NUM_PER_PAGE;i<(ColorWm.FatherPageNumber+1)*DING_LIANG_CESHI_NUM_PER_PAGE;i++)
// 		{
			
// 			if(i == DingLiangCeShiStruct.MultiPointMethod_num)
// 			{
// 				break;
// 			}

// 			sprintf(str,"%03d       %4d             %7.4f",
// 						i+1,
// //						SlitTable[1+DingLiangCeShiStruct.MultiPointMethod_cell[i]],
// 						DingLiangCeShiStruct.MultiPointMethod_data[i],
// 						DingLiangCeShiStruct.MultiPointMethod_conc[i]);

// 			OSTimeDly(OS_TICKS_PER_SEC/40);
// 		}
// 		ColorWm.FatherMenuNumber = 0;
// 	}

// 	{
// 		uint8 err=0;
// 		OSFlagPend(MyOsFlag,SYS_CAL_CONC_FLAG,OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,OS_TICKS_PER_SEC/100,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 				sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.SampleBaseValue);
// 				ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				ColorWm.FatherMenuNumber = 0;
// 		}
// 		else
// 		{
// 				sprintf(str,"    ");
// 				ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 		}
// 	}	

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();


// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				memcpy((uint8*)&DingLiangCeShiStruct.TestWaveLen,(uint8*)&msg[1],4);
// 				ActionMsg[0] = MSG_MOVE;//

// 				memcpy((uint8*)&ActionMsg[1],(uint8*)&DingLiangCeShiStruct.TestWaveLen,4);
// 				OSMboxPost(ActionMbox,ActionMsg);
				
// 				DingLiangCeShiW_Init();
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WL_MOVE;


// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{
// 				DingLiangCeShiW_Init();

// 				ColorWm.FatherMenuNumber = 0;
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_MSG_BOX;

// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				DingLiangCeShiW_Init();
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}

// 	}



// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 2;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 2;
// 			}
// 			sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		}
// 		else
// 		{
// 			sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		}
// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_CHILD_WIN)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 2;
// 			}
// 		}
// 	}
	
// }

// void DingLiangCeShiK(void)
// {     

// //	int32 tmp=0;	
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
	
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DING_LIANG_CESHI_SET_PAR)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				// ColorWindowsControl(WIN_NUM_DING_LIANG_MP_PAR,0);
// 			}
// 		} 
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_LAST_PAGE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				if(ColorWm.FatherPageNumber > 0)
// 				{
// 					ColorLcdDispResetAt(24,111,24+473,111+378);
// 					ColorWm.FatherPageNumber--;
// 					ColorWm.FatherMenuNumber = 2;
// 				}
// 			}
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_NEXT_PAGE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				if((ColorWm.FatherPageNumber+1)*DING_LIANG_CESHI_NUM_PER_PAGE<DingLiangCeShiStruct.MultiPointMethod_num)
// 				{
// 					ColorLcdDispResetAt(24,111,24+473,111+378);
// 					ColorWm.FatherPageNumber++;
// 					ColorWm.FatherMenuNumber = 2;
// 				}
// 			}
			
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_CLEAR)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				uint8 i=0;
// 				for(i=0;i<MAX_MULTI_POINT_METHOD_POINT;i++)
// 				{
// 					DingLiangCeShiStruct.MultiPointMethod_data[i] = 0;
// 					DingLiangCeShiStruct.MultiPointMethod_conc[i] = 0;
// 				}
// 				DingLiangCeShiStruct.MultiPointMethod_num = 0;
// 				ColorWm.FatherPageNumber = 0;
				
// 				ColorLcdDispResetAt(40,135,630,530);
				
// 			}
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_SAVELOAD)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				g_save_load_type = 1;
// 				ColorWindowsControl(WIN_NUM_SAVE_LOAD,0);
// 			}
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_SET_WAVE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(220,190,DATA_TYPE_WAVELEN,MAX_WAVELEN,MIN_WAVELEN);
// 			}
// 		} 	
		
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_TEST)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				if(DingLiangCeShiStruct.MultiPointMethod_num < MAX_K_METHOD_POINT)
// 				{
// 					ActionMsg[0] = MSG_CALC_CONC;

// 					OSMboxPost(ActionMbox,ActionMsg);
// 					ColorWm.FatherMenuNumber = 1;
// 				}
// 				else
// 				{
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}
// 			}
// 		}

// 		{
// 					ActionMsg[0] = MSG_CALC_CONC;


// 					OSMboxPost(ActionMbox,ActionMsg);
// 					ColorWm.FatherMenuNumber = 4;
// 		}
// 		else if(tmp_key == KEY_DING_LIANG_CESHI_EN_BASE)
// 		{
// 			// Handle the enable base action here
// 			if(DingLiangCeShiStruct.SampleEnBase == 1)
// 			{
// 				DingLiangCeShiStruct.SampleEnBase = 0;
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_DIS_BASE;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 			}
// 			else
// 			{
// 				DingLiangCeShiStruct.SampleEnBase = 1;
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_EN_BASE;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 			}
// 		}
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DING_LIANG_CESHI_MAIN_MENU)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWindowsControl(WIN_NUM_MAIN_MENU,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_CALIB)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWindowsControl(WIN_NUM_DING_LIANG_CESHI_BIAO_DING,0);
// 			}
// 		}
// 	}

	
// }










// void DingLiangCeShiBiaoDingW_Init(void)
// {
// 	char str[30];
// 	uint8 i=0;
// 	uint16 ypos = 0;
// 	uint16 xpos = 0;
// 	TOUCH_KEY_SQUARE* p;
	
// 		for(i=0;i<MAX_MULTI_POINT_BIAO_DING_NUM;i++)
// 		{

// 			{
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[i];
// 				xpos = p->left + 32;
// 				ypos = p->top + 1;
// 				sprintf(str,"%6.3f",DingLiangCeShiStruct.MP_Biaoding.Conc[i]);
// 				ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 			}

// 			{
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[i+MAX_MULTI_POINT_BIAO_DING_NUM];
// 				xpos = p->left + 36;
// 				ypos = p->top + 1;
// 				sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[i]);
// 				ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 			}
// 			OSTimeDly(OS_TICKS_PER_SEC/40);
// 		}

// 		if(DingLiangCeShiStruct.MultiPointMethod_b >= 0.0)
// 		{
// 			sprintf(str,"Conc = %.4f * En + %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 		}
// 		else
// 		{
// 			sprintf(str,"Conc = %.4f * En %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 		}
// 		ColorLcdSendString(142,546,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 		sprintf(str,"r = %.4f",DingLiangCeShiStruct.MultiPointMethod_r);
// 		ColorLcdSendString(142,568,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
		
		
// 		sprintf(str,"%4d" , MySysAbs.data_E);

// 		sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.BaseValue);
// 		ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
		
// 		if(DingLiangCeShiStruct.MP_Biaoding.EnBase == 1)
// 		{
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_EN_BASE;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 		}
// 		else
// 		{
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_DIS_BASE;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 		}
// }



// void DingLiangCeShiBiaoDingW(void)
// {   
// 	char str[30];
// 	uint8 i=0;
// 	uint16 ypos = 0;
// 	uint16 xpos = 0;
// 	TOUCH_KEY_SQUARE* p;

// 	{

// 		sprintf(str,"%4d" , MySysAbs.data_E);

// 		sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.BaseValue);
// 		ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// 		sprintf(str,"r = %.4f",DingLiangCeShiStruct.MultiPointMethod_r);
// 		ColorLcdSendString(142,568,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 		OSTimeDly(OS_TICKS_PER_SEC/50);
		
// 		if(DingLiangCeShiStruct.MultiPointMethod_b >= 0.0)
// 		{
// 			sprintf(str,"Conc = %.4f * En + %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 		}
// 		else
// 		{
// 			sprintf(str,"Conc = %.4f * En %.4f",DingLiangCeShiStruct.MultiPointMethod_k,DingLiangCeShiStruct.MultiPointMethod_b);
// 		}
// 		ColorLcdSendString(142,546,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 	}

// 	{
// 	}

// 	{
// 		uint8 err=0;
// 		OSFlagPend(MyOsFlag,SYS_GET_BIAO_DING_MP_FLAG,OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,OS_TICKS_PER_SEC/100,&err);
// 		if(err == OS_NO_ERR)
// 		{

// 			{
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[DingLiangCeShiStruct.MP_Biaoding.Select+MAX_MULTI_POINT_BIAO_DING_NUM];
// 				xpos = p->left + 36;
// 				ypos = p->top + 1;

// 				{
// 					sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 					ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				}
// 				else
// 				{
// 					sprintf(str,"        ");
// 					ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				}
// 			}
// 			ColorWm.FatherMenuNumber = 0;
// 		}
// 		else
// 		{

// 			{
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[DingLiangCeShiStruct.MP_Biaoding.Select+MAX_MULTI_POINT_BIAO_DING_NUM];
// 				xpos = p->left + 36;
// 				ypos = p->top + 1;

// 				{
// 					sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 					ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				}
// 				else
// 				{
// 					sprintf(str,"        ");
// 					ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				}
// 			}
// 		}
// 	}

// 	{
// 		uint8 err=0;
// 		OSFlagPend(MyOsFlag,SYS_GET_BIAO_DING_MP_FLAG,OS_FLAG_WAIT_SET_ALL|OS_FLAG_CONSUME,OS_TICKS_PER_SEC/100,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 				sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.BaseValue);
// 				ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				ColorWm.FatherMenuNumber = 0;
// 		}
// 		else
// 		{
// 				sprintf(str,"    ");
// 				ColorLcdSendString(658,237,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 		}
// 	}	
	

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();


// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{

// 			{
// 				memcpy((uint8*)&DingLiangCeShiStruct.MP_Biaoding.Conc[DingLiangCeShiStruct.MP_Biaoding.Select],(uint8*)&msg[1],4);
// 				DingLiangCeShiStruct.MP_Biaoding.IsConc |= (0x01<<DingLiangCeShiStruct.MP_Biaoding.Select) ;

// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[DingLiangCeShiStruct.MP_Biaoding.Select];
// 				xpos = p->left + 32;
// 				ypos = p->top + 1;
// 				sprintf(str,"%6.3f",DingLiangCeShiStruct.MP_Biaoding.Conc[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 				ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 				DingLiangCeShiStruct.MP_Biaoding.Select = 0;
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_MSG_BOX;
// 				DingLiangCeShiStruct.MP_Biaoding.Select = 0;
// 				ShowMsgBox(200,265,"ERROR:0<c<1000",20,0,0);
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[DingLiangCeShiStruct.MP_Biaoding.Select];
// 				xpos = p->left + 32;
// 				ypos = p->top + 1;
// 				if(DingLiangCeShiStruct.MP_Biaoding.IsConc &(0x01<<DingLiangCeShiStruct.MP_Biaoding.Select))
// 				{
// 					sprintf(str,"%6.3f",DingLiangCeShiStruct.MP_Biaoding.Conc[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 				}
// 				else
// 				{
// 					sprintf(str,"      ");
// 				}
// 				ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				DingLiangCeShiStruct.MP_Biaoding.Select = 0;
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			for(i=3;i<15;i++)
// 			{

// 				{
// 					p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[i+MAX_MULTI_POINT_BIAO_DING_NUM];
// 					xpos = p->left + 36;
// 					ypos = p->top + 1;
// 					sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[i]);
// 					ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 				}
// 			}
// 		}
// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_MSG_BOX)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				for(i=3;i<5;i++)
// 				{

// 					{
// 						p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[i];
// 						xpos = p->left + 32;
// 						ypos = p->top + 1;
// 						sprintf(str,"%6.3f",DingLiangCeShiStruct.MP_Biaoding.Conc[i]);
// 						ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 					}

// 					{
// 						p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[i+MAX_MULTI_POINT_BIAO_DING_NUM];
// 						xpos = p->left + 36;
// 						ypos = p->top + 1;
// 						sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[i]);
// 						ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 					}
// 				}
// 				ColorWm.FatherMenuNumber = 0;
// 			}
			
// 		}
// 	}

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();


// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				memcpy((uint8*)&DingLiangCeShiStruct.TestWaveLen,(uint8*)&msg[1],4);
// 				ActionMsg[0] = MSG_MOVE;//

// 				memcpy((uint8*)&ActionMsg[1],(uint8*)&DingLiangCeShiStruct.TestWaveLen,4);
// 				OSMboxPost(ActionMbox,ActionMsg);
				
// 				DingLiangCeShiBiaoDingW_Init();
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WL_MOVE;


// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{
// 				DingLiangCeShiW_Init();

// 				ColorWm.FatherMenuNumber = 0;
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_MSG_BOX;

// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				DingLiangCeShiW_Init();
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}

// 	}

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		}
// 		else
// 		{
// 			sprintf(str,"%3d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		}
// 	}
// }

// void DingLiangCeShiBiaoDingK(void)
// {     

// //	int32 tmp=0;	
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
// 	uint16 ypos=0;
// 	uint16 xpos=0;
// 	TOUCH_KEY_SQUARE* p;
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key >= KEY_DING_LIANG_CESHI_BIAODING_SEL_1 && tmp_key <= KEY_DING_LIANG_CESHI_BIAODING_SEL_10)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				char str[10];
// 				DingLiangCeShiStruct.MP_Biaoding.Select = tmp_key - KEY_DING_LIANG_CESHI_BIAODING_SEL_1;
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[DingLiangCeShiStruct.MP_Biaoding.Select];
// 				xpos = p->left + 32;
// 				ypos = p->top + 1;

// 				{
// 					sprintf(str,"%6.3f", DingLiangCeShiStruct.MP_Biaoding.Conc[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 				}

// 				{
// 					sprintf(str,"      ");
// 				}
// 				ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
				
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(400,265,DATA_TYPE_A,10000,0);
// 			}
// 		}
// 		else if (tmp_key >= KEY_DING_LIANG_CESHI_BIAODING_DATA_1 && tmp_key <= KEY_DING_LIANG_CESHI_BIAODING_DATA_10)
// 		{

// 			{
// 				char str[10];
// 				DingLiangCeShiStruct.MP_Biaoding.Select = tmp_key - KEY_DING_LIANG_CESHI_BIAODING_DATA_1;
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[MAX_MULTI_POINT_BIAO_DING_NUM+DingLiangCeShiStruct.MP_Biaoding.Select];
// 				xpos = p->left + 36;
// 				ypos = p->top + 1;

// 				{
// 					if(DingLiangCeShiStruct.MP_Biaoding.IsBiaoDing & (0x01<<DingLiangCeShiStruct.MP_Biaoding.Select))
// 					{
// 						sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 					}

// 					{
// 						sprintf(str,"    ");
// 					}
// 					ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// 				}
// 			}

// 			{
// 				char str[10];
// 				p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[MAX_MULTI_POINT_BIAO_DING_NUM+DingLiangCeShiStruct.MP_Biaoding.Select];
// 				xpos = p->left + 36;
// 				ypos = p->top + 1;


// 				{
// 					if(DingLiangCeShiStruct.MP_Biaoding.IsBiaoDing & (0x01<<DingLiangCeShiStruct.MP_Biaoding.Select))
// 					{
// 						sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 					}

// 					{
// 						sprintf(str,"    ");
// 					}

// 				}

// 				if(DingLiangCeShiStruct.MP_Biaoding.Select != tmp_key - KEY_DING_LIANG_CESHI_BIAODING_DATA_1)
// 				{	

// 					DingLiangCeShiStruct.MP_Biaoding.Select = tmp_key - KEY_DING_LIANG_CESHI_BIAODING_DATA_1;
// 					p = (TOUCH_KEY_SQUARE*)&WindowDingLiangCeShiBiaoDing_key_table[MAX_MULTI_POINT_BIAO_DING_NUM+DingLiangCeShiStruct.MP_Biaoding.Select];
// 					xpos = p->left + 36;
// 					ypos = p->top + 1;

// 					{
// 						if(DingLiangCeShiStruct.MP_Biaoding.IsBiaoDing & (0x01<<DingLiangCeShiStruct.MP_Biaoding.Select))
// 						{
// 							sprintf(str,"%4d", (uint16)DingLiangCeShiStruct.MP_Biaoding.Data[DingLiangCeShiStruct.MP_Biaoding.Select]);
// 						}

// 						{
// 							sprintf(str,"    ");
// 						}
// 						ColorLcdSendString(xpos,ypos,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// 					}
// 				}
// 				else
// 				{

// 				}

// 			}
// 		}
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_BIAODING_TEST)
// 		{

// 			{
// 				if(DingLiangCeShiStruct.MP_Biaoding.IsConc & (0x01<<DingLiangCeShiStruct.MP_Biaoding.Select))	
// 				{

// 					ActionMsg[0] = MSG_MULTI_POINT_METHOD_GET_BIAO_DING;


// 					OSMboxPost(ActionMbox,ActionMsg);
// 					ColorWm.FatherMenuNumber = 3;
// 				}
// 			}

// 			{
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_MSG_BOX;

			
// 			}
// 		}

// 		{
// 					ActionMsg[0] = MSG_MULTI_POINT_METHOD_GET_BIAO_DING;


// 					OSMboxPost(ActionMbox,ActionMsg);
// 					ColorWm.FatherMenuNumber = 4;
// 		}
// 		else if(tmp_key == KEY_DING_LIANG_CESHI_BIAODING_EN_BASE)
// 		{
// 			// Handle the enable base action here
// 			if(DingLiangCeShiStruct.MP_Biaoding.EnBase == 1)
// 			{
// 				DingLiangCeShiStruct.MP_Biaoding.EnBase = 0;
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_DIS_BASE;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 			}
// 			else
// 			{
// 				DingLiangCeShiStruct.MP_Biaoding.EnBase = 1;
// 				SIMPLE_ICON* p = (SIMPLE_ICON*)ICON_BIAO_DING_EN_BASE;
// 				ColorLcdDispIcon_Ext1(p,p->left,p->top);
// 			}
// 		}
// 		else if(tmp_key == KEY_DING_LIANG_CESHI_BIAODING_SET_WAVE)
// 		{

// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_SET_WAVELEN;
// 				EnterDigitalKeyBoard(220,190,DATA_TYPE_WAVELEN,MAX_WAVELEN,MIN_WAVELEN);
// 			}
// 		}




// 		else if (tmp_key == KEY_DING_LIANG_CESHI_BIAODING_CREATE_CURVE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0 || ColorWm.FatherMenuNumber == 1)//
// 			{
// 				int16 baseVal = 0;
// 				if(DingLiangCeShiStruct.MP_Biaoding.EnBase)
// 				{
// 					baseVal = DingLiangCeShiStruct.MP_Biaoding.BaseValue;
// 				}
// 				if(MinTwoMulti(DingLiangCeShiStruct.MP_Biaoding,
// 												&DingLiangCeShiStruct.MultiPointMethod_k,
// 												&DingLiangCeShiStruct.MultiPointMethod_b,
// 												&DingLiangCeShiStruct.MultiPointMethod_r,baseVal) == 0xff)
// 				{
// 					ColorWm.FatherPageNumber = 2;
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_MSG_BOX;

// 				}
// 				else
// 				{
// 					ColorWm.FatherMenuNumber = 0;
// 				}
// 			}
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_BIAODING_DISP_CURVE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0 || ColorWm.FatherMenuNumber == 1)//
// 			{

// 				int16 baseVal = 0;
// 				if(DingLiangCeShiStruct.MP_Biaoding.EnBase)
// 				{
// 					baseVal = DingLiangCeShiStruct.MP_Biaoding.BaseValue;
// 				}
// 				if(MinTwoMulti(DingLiangCeShiStruct.MP_Biaoding,
// 												&DingLiangCeShiStruct.MultiPointMethod_k,
// 												&DingLiangCeShiStruct.MultiPointMethod_b,
// 												&DingLiangCeShiStruct.MultiPointMethod_r,baseVal) == 0xff)
// 				{
// 					ColorWm.FatherPageNumber = 2;
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_MSG_BOX;

// 				}
// 				else
// 				{
// 					ColorWindowsControl(WIN_NUM_DING_LIANG_CURVE,0);
// 				}
// 			}
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_BIAODING_BACK)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0 || ColorWm.FatherMenuNumber == 1)//
// 			{
// 					// DingLiangCeShiStruct.MultiPointMethod_k = DingLiangCeShiStruct.MultiPointMethod_k_backup;
// 					// DingLiangCeShiStruct.MultiPointMethod_b = DingLiangCeShiStruct.MultiPointMethod_b_backup;
// 					// DingLiangCeShiStruct.MultiPointMethod_r = DingLiangCeShiStruct.MultiPointMethod_r_backup;
// 					ColorWindowsControl(WIN_NUM_DING_LIANG_CESHI,0);
// 			}
// 		} 	
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_BIAODING_CLR_DATA)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				uint8 i=0;
// 				for(i=0;i<MAX_MULTI_POINT_BIAO_DING_NUM;i++)
// 				{
// //					DingLiangCeShiStruct.MP_Biaoding.Slit[i] = SlitPosition.CurPos;
// 					DingLiangCeShiStruct.MP_Biaoding.Conc[i] = 0;
// 					DingLiangCeShiStruct.MP_Biaoding.Data[i] = 0;
// 				}
				
// 				DingLiangCeShiStruct.MP_Biaoding.IsConc = 0;
// 				DingLiangCeShiStruct.MP_Biaoding.IsBiaoDing = 0;
// 				DingLiangCeShiStruct.MP_Biaoding.Select = 0;
				
// 				ColorLcdDispResetAt(24,111,24+473,111+378);
				
// 			}
// 		} 
// 		else if (tmp_key == KEY_DING_LIANG_CESHI_BIAODING_SAVE_LOAD)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
//  				g_save_load_type = SAVE_LOAD_TYPE_CALIBRATION;
// 				ColorWindowsControl(WIN_NUM_SAVE_LOAD,0);
// 			}
// 		} 
// 		else
// 		{
// 			if (ColorWm.FatherMenuNumber == 1)//
// 			{
// 				DingLiangCeShiStruct.MP_Biaoding.Select = 0;
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}

		
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 	}

	
// }




// void DingLiangMPCurveW_Init(void)
// {
	
// 	char str[50];
// 	uint8 i = 0,j=0;
// 	float nd_max = 0;
// 	float nd_min = 0;
// 	float fdata[10][2];
// 	float k_tmp=0.0,b_tmp=0.0,r_tmp = 0.0;
// 	float ftmp[2];
// 	float tmp = 0,tmp1 = 0;
// 	uint8 count = 0;
// 	int16 x0=0,y0=0,x1=0,y1=0;


	
	
// 	k_tmp = DingLiangCeShiStruct.MultiPointMethod_k;
// 	b_tmp = DingLiangCeShiStruct.MultiPointMethod_b;
// 	r_tmp = DingLiangCeShiStruct.MultiPointMethod_r;

// 	for(i=0;i<MAX_MULTI_POINT_BIAO_DING_NUM;i++)
// 	{
// 		if(DingLiangCeShiStruct.MP_Biaoding.IsBiaoDing & (0x01<<i))
// 		{
// 			fdata[count][1] = DingLiangCeShiStruct.MP_Biaoding.Conc[i];
// 			if(DingLiangCeShiStruct.MP_Biaoding.EnBase)
// 			{
// 				fdata[count][0] = DingLiangCeShiStruct.MP_Biaoding.Data[i] - DingLiangCeShiStruct.MP_Biaoding.BaseValue;
// 			}
// 			else
// 			{
// 				fdata[count][0] = DingLiangCeShiStruct.MP_Biaoding.Data[i];
// 			}	
// 			//fdata[count][0] = DingLiangCeShiStruct.MP_Biaoding.Data[i];
// 			count++;
// 		}
// 	}

// 	if(b_tmp >= 0.0)
// 	{
// 		sprintf(str,"Conc = %.4f * En + %.4f   r=%.4f",k_tmp,b_tmp,r_tmp);
// 	}
// 	else
// 	{
// 		sprintf(str,"Conc = %.4f * En %.4f   r=%.4f",k_tmp,b_tmp,r_tmp);
// 	}
// 	ColorLcdSendString(110,80,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);



// 	{
// 		for(i=0;i<count-1-j;i++)
// 		{
// 			if(fdata[i][0]> fdata[i+1][0])
// 			{

// 				ftmp[1] = fdata[i][1];

// 				fdata[i][0] = fdata[i+1][0];
// 				fdata[i][1] = fdata[i+1][1];

// 				fdata[i+1][0] = ftmp[0];
// 				fdata[i+1][1] = ftmp[1];
// 			}
// 		}
// 	}

// 	nd_max = fdata[0][1];
// 	nd_min = fdata[0][1];
// 	for(i=0;i<count;i++)
// 	{
// 		if(nd_max < fdata[i][1])
// 		{
// 			nd_max = fdata[i][1];
// 		}
// 		if(nd_min > fdata[i][1])
// 		{
// 			nd_min = fdata[i][1];
// 		}
// 	}


// 	ColorLcdSendString(DRAW_FRAME_RIGHT- strlen(str)*6,DRAW_FRAME_BOTTOM+5,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
	

// 	ColorLcdSendString(DRAW_FRAME_LEFT-strlen(str)*6,DRAW_FRAME_BOTTOM+5,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	ColorLcdSendString(DRAW_FRAME_LEFT-strlen(str)*12,DRAW_FRAME_TOP,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	ColorLcdSendString(DRAW_FRAME_LEFT-strlen(str)*12,DRAW_FRAME_BOTTOM-24,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);






	

// 	OSTimeDly(3);
// 	ColorLcdColorConfig(COLOR_RED,COLOR_WHITE);
// 	OSTimeDly(3);
// 	for(i=0;i<count;i++)
// 	{
// 		x0 = (fdata[i][0]-fdata[0][0])*(DRAW_FRAME_RIGHT-DRAW_FRAME_LEFT)/(fdata[count-1][0] - fdata[0][0]) + DRAW_FRAME_LEFT;
// 		y0 =DRAW_FRAME_BOTTOM -  (fdata[i][1] - nd_min)*(DRAW_FRAME_BOTTOM-DRAW_FRAME_TOP)/(nd_max - nd_min);
// 	  ColorLcdDrawOnePoint(x0,y0,0);
		

// 		if(x0+3+strlen(str)*8 > DRAW_FRAME_RIGHT)
// 		{
// 			ColorLcdSendString(x0-3 - strlen(str)*8,y0-8,COLOR_LCD_LIB_16_16,FCOLOR_ON|GB2312,0X01,COLOR_RED,COLOR_WHITE,(uint8*)str);
// 		}
// 		else
// 		{
// 			ColorLcdSendString(x0+3,y0-8,COLOR_LCD_LIB_16_16,FCOLOR_ON|GB2312,0X01,COLOR_RED,COLOR_WHITE,(uint8*)str);
// 		}
// 		OSTimeDly(2);
// 	}

// 	tmp=k_tmp*fdata[0][0]+b_tmp;   
// 	x0 = 0;
// 	y0 =(tmp- nd_min)*(DRAW_FRAME_HEIGHT)/(nd_max - nd_min);

	

// 	tmp1=k_tmp*fdata[count-1][0]+b_tmp;   
// 	x1 = DRAW_FRAME_WIDTH;
// 	y1 =(tmp1- nd_min)*(DRAW_FRAME_HEIGHT)/(nd_max - nd_min);

// 	k_tmp = 1.0*(y1-y0)/(x1-x0);
// 	b_tmp = 1.0*y0;

// 	if(k_tmp > 0)
// 	{
// 		if(y0>=0)
// 		{
// 			x0 = x0;
// 			y0 = y0;
// 		}
// 		else
// 		{
// 			x0 = -y0/k_tmp;
// 			y0 = 0;
// 		}

// 		if(y1<=DRAW_FRAME_HEIGHT)
// 		{
// 			x1 = x1;
// 			y1 = y1;
// 		}
// 		else
// 		{
// 			x1 = (DRAW_FRAME_HEIGHT-y0)/k_tmp;
// 			y1 = DRAW_FRAME_HEIGHT;
// 		}
// 	}
// 	else if (k_tmp <=0)
// 	{
// 		if(y0<=DRAW_FRAME_HEIGHT)
// 		{
// 			x0 = x0;
// 			y0 = y0;
// 		}
// 		else
// 		{
// 			x0 = (DRAW_FRAME_HEIGHT-y0)/k_tmp;
// 			y0 = DRAW_FRAME_HEIGHT;
// 		}

// 		if(y1>=0)
// 		{
// 			x1 = x1;
// 			y1 = y1;
// 		}
// 		else
// 		{
// 			x1 = (-y0)/k_tmp;
// 			y1 = 0;
// 		}
// 	}


// 	ColorLcdColorConfig(COLOR_BLUE,COLOR_WHITE);
// 	OSTimeDly(5);
// 	ColorLcdDrawOneLine(x0+DRAW_FRAME_LEFT,DRAW_FRAME_BOTTOM-y0,x1+DRAW_FRAME_LEFT,DRAW_FRAME_BOTTOM-y1);




	
	
// }



// void DingLiangMPCurveW(void)
// {
// 	if (ColorWm.FatherMenuNumber == MENU_NUMBER_PRINT)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			OSTimeDly(OS_TICKS_PER_SEC/6);
// 			DingLiangMPCurveW_Init();
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}
// 	}
// }



// void DingLiangMPCurveK(void)
// {
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
	
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DING_LIANG_MP_CURVE_BACK)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWindowsControl(WIN_NUM_DING_LIANG_CESHI_BIAO_DING,0);
// 			}
// 		}
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 	}
	
	
// }








// void DongLiXueCeshiW_Init(void)
// {

// 	char str[30];
// 	char str1[30];
	



// 	sprintf(str,"%4.0f",gDraw.top);
// 	ColorLcdSendString(50,DRAW_FRAME_TOP,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 	sprintf(str1,"%4.0f",gDraw.bottom);
// 	ColorLcdSendString(50,DRAW_FRAME_BOTTOM-24,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str1);


// 	sprintf(str,"%4ds",	gDraw.left);
// 	ColorLcdSendString(	DRAW_FRAME_LEFT-strlen(str)*6,
// 											DRAW_FRAME_BOTTOM,
// 											COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
	

// 	sprintf(str,"%4ds",	gDraw.right);
// 	ColorLcdSendString(	DRAW_FRAME_RIGHT - strlen(str)*6,
// 											DRAW_FRAME_BOTTOM,
// 											COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	ColorLcdColorConfig(COLOR_BLUE,COLOR_WHITE);


// }
	
	
// void DongLiXueCeshiW(void)
// {
// 	char str[40];
// 	CPU_SR        cpu_sr;
// 	uint16 tmp = 0;
// 	uint8 hour;
// 	uint8 minute;
// 	uint8 second;
// 	uint8 err=0;

// 	if (ColorWm.FatherMenuNumber == 0)//
// 	{
// 		sprintf(str,"%4d.%d",CurWaveLen/Unit_1nm,CurWaveLen%Unit_1nm);

		
// 		sprintf(str,"%4d",MySysAbs.data_E);
// 		ColorLcdSendString(300,25,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);

// 		OS_ENTER_CRITICAL();
// 		tmp = LineDispBuf.num;
// 		OS_EXIT_CRITICAL();
// 		if(tmp>0)
// 		{
// 			ColorLcdDispBuffLines(TIME_LINE1_BUF_ADDR,tmp);
// 		}
// 	}

// 	{
	
// 		sprintf(str,"%4d",gDraw.y_cur);
// 		ColorLcdSendString(300,25,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);
// 		// sprintf(str,"%4d.%d",CurWaveLen/Unit_1nm,CurWaveLen%Unit_1nm);


		
// 		uint32 CurTime = gDraw.x_cur*gDraw.Interval;
// 		hour = CurTime/3600;
// 		minute = (CurTime%3600)/60;
// 		second = (CurTime%60);
// 		sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 		ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 		if(OSFlagAccept(MyOsFlag,SYS_TIME_SCAN_FLAG,OS_FLAG_CONSUME|OS_FLAG_WAIT_SET_ALL,&err) == SYS_TIME_SCAN_FLAG)
// 		{
			
			
// 			sprintf(str,"S/N:%4d",gSN);
// 			ColorLcdSendString(470,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);

// 			ColorWm.FatherMenuNumber = 0;
// 			ColorLcdDispIcon_Ext1_1(ICON_TEST_GUANGPU);
// 		}
	
// 	}

// 	{
// 		RedrawDongLiXueLine(gDraw);
// 		ColorWm.FatherMenuNumber = 0;
// 	}

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				sprintf(str,"%4d.%d",CurWaveLen/Unit_1nm,CurWaveLen%Unit_1nm);

// 				ColorWm.FatherMenuNumber = 1;
// 			}
// 		}
// 	}

// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_CHILD_WIN)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// //				if(TimeLineDraw.left != TimeLineDraw.left_reserve || TimeLineDraw.right != TimeLineDraw.right_reserve || 
// //					 TimeLineDraw.top != TimeLineDraw.top_reserve || TimeLineDraw.bottom != TimeLineDraw.bottom_reserve)
// //				{
// //					TimeLineDraw.left = TimeLineDraw.left_reserve;
// //					TimeLineDraw.right = TimeLineDraw.right_reserve;
// //					TimeLineDraw.top = TimeLineDraw.top_reserve;
// //					TimeLineDraw.bottom = TimeLineDraw.bottom_reserve;
// //					OSTimeDly(OS_TICKS_PER_SEC/5);
// //					DongLiXueCeshiW_Init();
// //					OSTimeDly(OS_TICKS_PER_SEC/4);
// //					RedrawDongLiXueLine(TimeLineDraw);
// //				}
// //				else
// //				{
// //					OSTimeDly(OS_TICKS_PER_SEC/6);
// //					DongLiXueCeshiW_Init();
// //				}
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}
// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_SCALE_WIN)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// //				if(TimeLineDraw.left != SetScale.left || TimeLineDraw.right != SetScale.right || 
// //					 TimeLineDraw.top != SetScale.top || TimeLineDraw.bottom != SetScale.bottom)
// //				{
// //					TimeLineDraw.left = SetScale.left;
// //					TimeLineDraw.right = SetScale.right;
// //					TimeLineDraw.top = SetScale.top;
// //					TimeLineDraw.bottom = SetScale.bottom;
// //					OSTimeDly(OS_TICKS_PER_SEC/5);
// //					DongLiXueCeshiW_Init();
// //					OSTimeDly(OS_TICKS_PER_SEC/4);
// //					RedrawDongLiXueLine(TimeLineDraw);
// //				}
// //				else
// //				{
// //					OSTimeDly(OS_TICKS_PER_SEC/6);
// //					DongLiXueCeshiW_Init();
// //				}
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}
// 	}
// }
	
	
// void DongLiXueCeshiK(void)
// {
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
// 	char str[40];
// 	uint8 hour;
// 	uint8 minute;
// 	uint8 second;

// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DONG_LI_XUE_CESHI_SET_PARA)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWindowsControl(WIN_NUM_DONG_LI_XUE_PAR,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_SAVE_LOAD)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				g_save_load_type  = SAVE_LOAD_TYPE_DONG_LI_XUE;
// 				ColorWindowsControl(WIN_NUM_SAVE_LOAD,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_CLEAR)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				LineDispBuf.num = 0;
// 				ColorLcdDispResetAt(DRAW_FRAME_LEFT,DRAW_FRAME_TOP,DRAW_FRAME_RIGHT,DRAW_FRAME_BOTTOM);
// 			}
// 		}

// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_TEST)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{
// 				if((DongLiXueCeShi.data_End<DongLiXueCeShi.data_Start))
// 				{

// 				}
// 				else
// 				{
// 					// TimeLineDraw.TimeUnit = DongLiXueCeShi.TimeUnit;
// 					gDraw.Interval= DongLiXueCeShi.Interval;
// 					gDraw.x_Width= DongLiXueCeShi.SampleTime*DongLiXueCeShi.TimeUnit;
// 					if(gDraw.x_Width % gDraw.Interval)
// 					{
// 						gDraw.x_Width = gDraw.x_Width + gDraw.Interval - gDraw.x_Width%gDraw.Interval;
// 					}
// 					gDraw.left = 0;
// 					gDraw.right = gDraw.x_Width;
// 					gDraw.left_reserve = 0;
// 					gDraw.right_reserve= gDraw.right;

// 					gDraw.y_Height = 		DongLiXueCeShi.data_End - DongLiXueCeShi.data_Start;
// 					gDraw.top = 			DongLiXueCeShi.data_End;
// 					gDraw.bottom = 			DongLiXueCeShi.data_Start;
// 					gDraw.top_reserve= 		gDraw.top;
// 					gDraw.bottom_reserve= 	gDraw.bottom;


// 					memcpy((uint8*)&SysPara.DongLiXuePar,(uint8*)&DongLiXueCeShi,sizeof(DongLiXueCeShi));
// 					UpdateSysParameter();
// 					ColorLcdDispIcon_Ext1_1(ICON_STOP_GUANGPU);
// 					ColorLcdSendString(470,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)"S/N:    ");

// 					DongLiXueCeshiW_Init();
// 					ColorLcdDispResetAt(TIME_DRAW_FRAME_LEFT,TIME_DRAW_FRAME_TOP,TIME_DRAW_FRAME_RIGHT,TIME_DRAW_FRAME_BOTTOM);
// 					OS_ENTER_CRITICAL();
// 					LineDispBuf.num = 0;
// 					OS_EXIT_CRITICAL();

// 					ColorWm.FatherMenuNumber = 4;


// 					ActionMsg[0] = MSG_TIME_SCAN;//
// 					OSMboxPost(ActionMbox,ActionMsg);
						
// 				}
// 			}
// 			else if(ColorWm.FatherMenuNumber == 1)
// 			{
// 					SysFlag.EmergercyStopFlag = 1;
// 			}
// 		}
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DONG_LI_XUE_CESHI_MAIN_MENU)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWindowsControl(WIN_NUM_MAIN_MENU,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_CUR_CURSOR)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				M1_M2* p = &DongLiXueCeShi.M1;



// 				uint32 tmp = (TouchKey.x - DRAW_FRAME_LEFT)*(gDraw.right - gDraw.left)/DRAW_FRAME_WIDTH + gDraw.left;
// 				p->x_data = (tmp / gDraw.Interval);

// //				memcpy((uint8*)&p->y_val,(uint8*)&ScanLine[p->x_data],sizeof(_SCAN_LINE));
// 				p->y_val = DongLiXueScanLine[p->x_data];
// //==================================================================================================================
// 				uint32 cur_time = p->x_data*gDraw.Interval;
// 				hour = cur_time/3600;
// 				minute = (cur_time%3600)/60;
// 				second = (cur_time%60);
// 				sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 				ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 				sprintf(str,"%6d",p->y_val);

// //==================================================================================================================
				

// 				p->pt = CalcXY(p->x_data*gDraw.Interval, p->y_val,gDraw,(_FRAME*)&TM_FRAME);
// 				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);
// 			}
			
				





// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_CURSOR_LEFT)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				char str[20];

// 				M1_M2* p = &DongLiXueCeShi.M1;



// 				if(p->x_data>gDraw.left)
// 				{
// 					p->x_data-=gDraw.Interval;
// 				}
// //				memcpy((uint8*)&p->y_val,(uint8*)&ScanLine[p->x_data],sizeof(_SCAN_LINE));
// 				p->y_val = DongLiXueScanLine[p->x_data];

// //==================================================================================================================
// 				uint32 cur_time = p->x_data*gDraw.Interval;
// 				hour = cur_time/3600;
// 				minute = (cur_time%3600)/60;
// 				second = (cur_time%60);
// 				sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 				ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 				sprintf(str,"%6d",p->y_val);

// //==================================================================================================================
				

// 				p->pt = CalcXY(p->x_data*gDraw.Interval, p->y_val,gDraw,(_FRAME*)&TM_FRAME);
// 				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);


// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_CURSOR_RIGHT)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				char str[20];

// 				M1_M2* p = &DongLiXueCeShi.M1;



// 				if(p->x_data<gDraw.right)
// 				{
// 					p->x_data+=gDraw.Interval;
// 				}
// //				memcpy((uint8*)&p->y_val,(uint8*)&ScanLine[p->x_data],sizeof(_SCAN_LINE));
// 				p->y_val = DongLiXueScanLine[p->x_data];

// //==================================================================================================================
// 				uint32 cur_time = p->x_data*gDraw.Interval;
// 				hour = cur_time/3600;
// 				minute = (cur_time%3600)/60;
// 				second = (cur_time%60);
// 				sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 				ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 				sprintf(str,"%6d",p->y_val);

// //==================================================================================================================
				

// 				p->pt = CalcXY(p->x_data*gDraw.Interval, p->y_val,gDraw,(_FRAME*)&TM_FRAME);
// 				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);
// 			}
// 		}
// 	}
// 	else if(TouchKey.Press == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DONG_LI_XUE_CESHI_CUR_CURSOR)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				M1_M2* p = &DongLiXueCeShi.M1;



// 				uint32 tmp = (TouchKey.x - DRAW_FRAME_LEFT)*(gDraw.right - gDraw.left)/DRAW_FRAME_WIDTH + gDraw.left;
// 				p->x_data = (tmp / gDraw.Interval);

// //				memcpy((uint8*)&p->y_val,(uint8*)&ScanLine[p->x_data],sizeof(_SCAN_LINE));
// 				p->y_val = DongLiXueScanLine[p->x_data];
// //==================================================================================================================
// 				uint32 cur_time = p->x_data*gDraw.Interval;
// 				hour = cur_time/3600;
// 				minute = (cur_time%3600)/60;
// 				second = (cur_time%60);
// 				sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 				ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 				sprintf(str,"%6d",p->y_val);

// //==================================================================================================================
				

// 				p->pt = CalcXY(p->x_data*gDraw.Interval, p->y_val,gDraw,(_FRAME*)&TM_FRAME);
// 				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_CURSOR_LEFT)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				char str[20];

// 				M1_M2* p = &DongLiXueCeShi.M1;



// 				if(p->x_data>gDraw.left)
// 				{
// 					p->x_data-=gDraw.Interval;
// 				}
// //				memcpy((uint8*)&p->y_val,(uint8*)&ScanLine[p->x_data],sizeof(_SCAN_LINE));
// 				p->y_val = DongLiXueScanLine[p->x_data];

// //==================================================================================================================
// 				uint32 cur_time = p->x_data*gDraw.Interval;
// 				hour = cur_time/3600;
// 				minute = (cur_time%3600)/60;
// 				second = (cur_time%60);
// 				sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 				ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 				sprintf(str,"%6d",p->y_val);

// //==================================================================================================================
				

// 				p->pt = CalcXY(p->x_data*gDraw.Interval, p->y_val,gDraw,(_FRAME*)&TM_FRAME);
// 				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_CESHI_CURSOR_RIGHT)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				char str[20];

// 				M1_M2* p = &DongLiXueCeShi.M1;



// 				if(p->x_data<gDraw.right)
// 				{
// 					p->x_data+=gDraw.Interval;
// 				}
// //				memcpy((uint8*)&p->y_val,(uint8*)&ScanLine[p->x_data],sizeof(_SCAN_LINE));
// 				p->y_val = DongLiXueScanLine[p->x_data];

// //==================================================================================================================
// 				uint32 cur_time = p->x_data*gDraw.Interval;
// 				hour = cur_time/3600;
// 				minute = (cur_time%3600)/60;
// 				second = (cur_time%60);
// 				sprintf(str,"%02d:%02d:%02d",hour,minute,second);
// 				ColorLcdSendString(300,495,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);


// 				sprintf(str,"%6d",p->y_val);

// //==================================================================================================================
				

// 				p->pt = CalcXY(p->x_data*gDraw.Interval, p->y_val,gDraw,(_FRAME*)&TM_FRAME);
// 				ColorLcdDrawLine(p->pt.x,DRAW_FRAME_TOP,p->pt.x,DRAW_FRAME_BOTTOM,0);
// 			}
// 		}
// 	}

// }









// const PIXEL_POINT dlx_par_wl = {313,76};
// const PIXEL_POINT dlx_par_sample_time = {313,117};
// const PIXEL_POINT dlx_par_data_start = {313,203};
// const PIXEL_POINT dlx_par_data_end = {476,203};
// const PIXEL_POINT dlx_par_interval = {313,247};
// const PIXEL_POINT dlx_par_pmt_voltage = {244,428};



// void DongLiXueParW_Init(void)
// {
// 	char str[30];


// 	sprintf(str,"%4d.%d",DongLiXueCeShi.TestWaveLen/Unit_1nm,DongLiXueCeShi.TestWaveLen%Unit_1nm);
// 	ColorLcdSendString(dlx_par_wl.x,dlx_par_wl.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_PURPLE,COLOR_GREEN,(uint8*)str);

// 	ColorLcdSendString(dlx_par_sample_time.x,dlx_par_sample_time.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_PURPLE,COLOR_GREEN,(uint8*)str);


// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_SECOND);
// 	}

// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_MINUTE);
// 	}


// 	ColorLcdSendString(dlx_par_data_start.x,dlx_par_data_start.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_PURPLE,COLOR_GREEN,(uint8*)str);

// 	ColorLcdSendString(dlx_par_data_end.x,dlx_par_data_end.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_PURPLE,COLOR_GREEN,(uint8*)str);


// 	ColorLcdSendString(dlx_par_interval.x,dlx_par_interval.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_PURPLE,COLOR_GREEN,(uint8*)str);


// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_MINUTE);
// 	}

// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_SECOND);
// 	}


// 	if(SysPara.LightPowerS.gain == PMT_GAIN_1)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN1_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.gain == PMT_GAIN_2)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN2_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.gain == PMT_GAIN_4)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN4_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.gain == PMT_GAIN_8)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN8_SELECT);
// 	}


// 	if(SysPara.LightPowerS.emit_light == EMIT_LIGHT_1)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT1_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.emit_light == EMIT_LIGHT_2)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT2_SELECT);
// 	}	
// 	else if (SysPara.LightPowerS.emit_light == EMIT_LIGHT_3)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT3_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.emit_light == EMIT_LIGHT_4)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT4_SELECT);
// 	}

// 	if(SysPara.LightPowerS.pmt_on_off == PMT_ON)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_PMT_ON_SELECT);
// 	}
// 	else if (SysPara.LightPowerS.pmt_on_off == PMT_OFF)
// 	{
// 		ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_PMT_OFF_SELECT);
// 	}
	

// 	ColorLcdSendString(dlx_par_pmt_voltage.x,dlx_par_pmt_voltage.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_PURPLE,COLOR_GREEN,(uint8*)str);


// }

// void DongLiXueParW(void)
// {

// 	char str[20];
// 	if(ColorWm.FatherMenuNumber == 0)//
// 	{


// 	}

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			sprintf(str,"%4d.%d" ,CurWaveLen/10,CurWaveLen%10);


// 			DongLiXueParW_Init();
// 		}
// 		else
// 		{
// 			sprintf(str,"%4d.%d" ,CurWaveLen/10,CurWaveLen%10);

// 		}
// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_KEY_BOARD)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				int32 tmp = 0;
// 				memcpy(&tmp,(uint8*)&msg[1],4);

// 				{
// 					DongLiXueCeShi.TestWaveLen = tmp;
// 					ColorWm.FatherMenuNumber = 0;
// 					ActionMsg[0] = MSG_MOVE;//

// 					memcpy((uint8*)&ActionMsg[1],(uint8*)&DongLiXueCeShi.TestWaveLen,4);
// 					OSMboxPost(ActionMbox,ActionMsg);
				
// 					DongLiXueParW_Init();
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WL_MOVE;




// 				}

// 				{
// 					// memcpy((uint8*)&DongLiXueCeShi.SampleTime,(uint8*)&msg[1],2);
// 					DongLiXueCeShi.SampleTime = tmp;
// 					DongLiXueParW_Init();
// 					ColorWm.FatherMenuNumber = 0;
// 				}

// 				{
// 					// memcpy((uint8*)&DongLiXueCeShi.data_Start,(uint8*)&msg[1],4);
// 					DongLiXueCeShi.data_Start = tmp;
// 					DongLiXueParW_Init();
// 					ColorWm.FatherMenuNumber = 0;
// 				}

// 				{
// 					// memcpy((uint8*)&DongLiXueCeShi.data_End,(uint8*)&msg[1],4);
// 					DongLiXueCeShi.data_End = tmp;
// 					DongLiXueParW_Init();
// 					ColorWm.FatherMenuNumber = 0;
// 				}

// 				{
// 					// memcpy((uint8*)&DongLiXueCeShi.Interval,(uint8*)&msg[1],2);
// 					DongLiXueCeShi.Interval = tmp;
// 					DongLiXueParW_Init();
// 					ColorWm.FatherMenuNumber = 0;
// 				}

// 				{
// 					// memcpy((uint8*)&SysPara.LightPowerS.pmt_voltage,(uint8*)&msg[1],2);
// 					SysPara.LightPowerS.pmt_voltage = tmp;	
// 					if(SysPara.LightPowerS.pmt_on_off == PMT_ON)
// 					{
// 						BSP_Set_HV(SysPara.LightPowerS.pmt_voltage);
// 					}
// 					else
// 					{
// 						BSP_Set_HV(0);
// 					}
// 					DongLiXueParW_Init();
// 					ColorWm.FatherMenuNumber = 0;
// 				}
// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{

// 				{
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}

// 				{
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}

// 				{
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}

// 				{
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}

// 				{
// 						ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}

// 				{
// 					ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 				}
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 				DongLiXueParW_Init();
// 			}
// 		}
// 		else
// 		{


// 				{
// 					sprintf(str,"%4d.%d",	DongLiXueCeShi.TestWaveLen/Unit_1nm,
// 											DongLiXueCeShi.TestWaveLen%Unit_1nm
// 										);
// 					ColorLcdSendString(dlx_par_wl.x,dlx_par_wl.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}

// 				{
// 					sprintf(str,"%6d",DongLiXueCeShi.SampleTime);
// 					ColorLcdSendString(dlx_par_sample_time.x,dlx_par_sample_time.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 				else if(ColorWm.FatherPageNumber == 3)
// 				{
// 					sprintf(str,"%4d",DongLiXueCeShi.data_Start);
// 					ColorLcdSendString(dlx_par_data_start.x,dlx_par_data_start.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 				else if(ColorWm.FatherPageNumber == 4)
// 				{
// 					sprintf(str,"%4d",DongLiXueCeShi.data_End);
// 					ColorLcdSendString(dlx_par_data_end.x,dlx_par_data_end.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}

// 				{
// 					sprintf(str,"%4d",DongLiXueCeShi.Interval); 
// 					ColorLcdSendString(dlx_par_interval.x,dlx_par_interval.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}

// 				{
// 					sprintf(str,"%4d",SysPara.LightPowerS.pmt_voltage); 
// 					ColorLcdSendString(dlx_par_pmt_voltage.x,dlx_par_pmt_voltage.y,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GRAY,(uint8*)str);
// 				}
// 		}
// 	}
// }

// void DongLiXueParK(void)
// {

// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
	
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_DONG_LI_XUE_PAR_UNIT_SECOND)
// 		{

// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_NONE);
// 				DongLiXueCeShi.TimeUnit = TIME_UNIT_SECOND;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_SECOND);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_UNIT_MINUTE)
// 		{

// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_NONE);
// 				DongLiXueCeShi.TimeUnit = TIME_UNIT_MINUTE;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_UNIT_MINUTE);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_SET_WAVE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(390,165,DATA_TYPE_WAVELEN,MAX_WAVELEN,MIN_WAVELEN);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_SET_TIME)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(320,250,DATA_TYPE_TIME,7200,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_DATA_START)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;

// 				EnterDigitalKeyBoard(320,320,DATA_TYPE_NORMAL,2000,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_DATA_END)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(320,320,DATA_TYPE_NORMAL,2000,0);
// 			}
// 		}

// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWm.FatherPageNumber = 5;//
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(320,320,DATA_TYPE_TIME,60,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_PMT_VOLTAGE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(320,320,DATA_TYPE_NORMAL,1000,0);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_1)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_1;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT1_SELECT);
// 				BSP_Light_Sel(1);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_2)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_2;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT2_SELECT);
// 				BSP_Light_Sel(2);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_3)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_3;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT3_SELECT);
// 				BSP_Light_Sel(3);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_4)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.emit_light = EMIT_LIGHT_4;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_EMIT_LIGHT4_SELECT);
// 				BSP_Light_Sel(4);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_GAIN_1)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.gain = PMT_GAIN_1;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN1_SELECT);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_GAIN_2)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.gain = PMT_GAIN_2;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN2_SELECT);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_GAIN_4)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.gain = PMT_GAIN_4;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN4_SELECT);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_GAIN_8)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN_ALL_NOT_SELECT);
// 				SysPara.LightPowerS.gain = PMT_GAIN_8;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_GAIN8_SELECT);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_PMT_OFF)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_PMT_NONE_SELECT);
// 				SysPara.LightPowerS.pmt_on_off = PMT_OFF;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_PMT_OFF_SELECT);
// 				BSP_Set_HV(0);
// 			}
// 		}
// 		else if(tmp_key == KEY_DONG_LI_XUE_PAR_PMT_ON)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_PMT_NONE_SELECT);
// 				SysPara.LightPowerS.pmt_on_off = PMT_ON;
// 				ColorLcdDispIcon_Ext1_1(ICON_DONG_LI_XUE_PMT_ON_SELECT);
// 				BSP_Set_HV(SysPara.LightPowerS.pmt_voltage);
// 			}
// 		}



// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_CONFIRM)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
				
// 				memcpy(&SysPara.DongLiXuePar,&DongLiXueCeShi,sizeof(_DongLiXueCeShiStruct));
// 				UpdateSysParameter();
// 				if(DongLiXueCeShi.TestWaveLen == CurWaveLen)
// 				{
// 					ColorWindowsControl(WIN_NUM_DONG_LI_XUE_CESHI,0);
// 				}
// 				else
// 				{
// 					ColorWindowsControl(WIN_NUM_DONG_LI_XUE_CESHI,3);
// 				}
// 			}
// 		}
// 		else if (tmp_key == KEY_DONG_LI_XUE_PAR_CANCLE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				UpdateSysParameter();
// 				ColorWindowsControl(WIN_NUM_DONG_LI_XUE_CESHI,0);
// 			}
// 		}

		
		
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 	}
	


// }




// const int wl_y_pos[10] = {114,152,190,228,266,304,342,380,418,456};

// void WaveCorrectW_Init(void)
// {
	
// 	char str[10];

// 	for(int i=0;i<10;i++)
// 	{
// 		sprintf(str,"%7d",gNew_wl_table[i].pos);
// 		ColorLcdSendString(140,wl_y_pos[i]+10,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// 		sprintf(str,"%3d.%d",gNew_wl_table[i].wl/10,gNew_wl_table[i].wl%10);
// 		ColorLcdSendString(340,wl_y_pos[i]+10,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// 		OSTimeDly(20);
		
// 	}	

// }

// void WaveCorrectW(void)
// {
// 	if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_KEY_BOARD)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		char str[20];
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				int32 tmp=0;	
// 				memcpy((uint8*)&tmp,(uint8*)&msg[1],4);

// 				if(ColorWm.FatherPageNumber >= 1 && ColorWm.FatherPageNumber <= 10)
// 				{
// 					gNew_wl_table[ColorWm.FatherPageNumber-1].wl = tmp;
// 				}
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{
// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;

// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			WaveCorrectW_Init();
// 		}
// 		else
// 		{
// 			if(ColorWm.FatherPageNumber >= 1 && ColorWm.FatherPageNumber <= 10)
// 			{

// 				sprintf(str,"%3d.%d",gNew_wl_table[ColorWm.FatherPageNumber-1].wl/10,gNew_wl_table[ColorWm.FatherPageNumber-1].wl%10);
// 				ColorLcdSendString(340,wl_y_pos[ColorWm.FatherPageNumber-1]+10,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 			}

// 		}
// 	}

	
// }

// void WaveCorrectK(void)
// {

// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
	
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key >= KEY_WAVE_CORRECT_REAL_WAVE_1 && tmp_key <= KEY_WAVE_CORRECT_REAL_WAVE_10)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_KEY_BOARD;
// 				EnterDigitalKeyBoard(500,155,DATA_TYPE_WAVELEN,MAX_WAVELEN,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_WAVE_CORRECT_SAVE)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				USERFatFSResult = wl_table_save(gNew_wl_table);
// 			}
// 		}
// 		else if (tmp_key == KEY_WAVE_CORRECT_LOAD_DEFAULT)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				wl_table_load(gNew_wl_table);
// 				WaveCorrectW_Init();
// 			}
// 		}
// 		else if (tmp_key == KEY_WAVE_CORRECT_CONFIRM)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				ColorWindowsControl(WIN_NUM_GUPU_CESHI,0);
// 			}
// 		}

		
		
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 	}
	
// }












// void SNRCeShiW_Init(void)
// {
// // 	CPU_SR        cpu_sr;
// // 	uint16 tmp;
// 	char str[30];
// //	char str1[30];
	
// 	sprintf(str,"%4d.%d",CurWaveLen/Unit_1nm,CurWaveLen%Unit_1nm);

	
// 	sprintf(str,"%4d",MySysAbs.data_E);
// 	ColorLcdSendString(300,25,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);



// 	sprintf(str,"%4.0f",gDraw_SNR_WL.top);
// 	ColorLcdSendString(SNR_WL_FRAME.left-strlen(str)*12-12,SNR_WL_FRAME.top,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 	sprintf(str,"%4.0f",gDraw_SNR_WL.bottom);
// 	ColorLcdSendString(SNR_WL_FRAME.left-strlen(str)*12-12,SNR_WL_FRAME.bottom-24,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	sprintf(str,"%d.%dnm",	gDraw_SNR_WL.left/Unit_1nm,
// 							gDraw_SNR_WL.left%Unit_1nm
// 						);
// 	ColorLcdSendString(SNR_WL_FRAME.left-strlen(str)*6,SNR_WL_FRAME.bottom,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
	

// 	sprintf(str,"%4d.%dnm",	gDraw_SNR_WL.right/Unit_1nm,
// 						gDraw_SNR_WL.right%Unit_1nm
// 						);
// 	ColorLcdSendString(SNR_WL_FRAME.right - strlen(str)*6,SNR_WL_FRAME.bottom,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);




// 	sprintf(str,"%4.0f",gDraw_SNR_TM.top);
// 	ColorLcdSendString(SNR_TM_FRAME.left-strlen(str)*12-12,SNR_TM_FRAME.top,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 	sprintf(str,"%4.0f",gDraw_SNR_TM.bottom);	
// 	ColorLcdSendString(SNR_TM_FRAME.left-strlen(str)*12-12,SNR_TM_FRAME.bottom-24,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	sprintf(str,"%d",	gDraw_SNR_TM.left
// 						);
// 	ColorLcdSendString(SNR_TM_FRAME.left-strlen(str)*6,SNR_TM_FRAME.bottom,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);


// 	sprintf(str,"%4d",	gDraw_SNR_TM.right
// 						);
// 	ColorLcdSendString(SNR_TM_FRAME.right - strlen(str)*6,SNR_TM_FRAME.bottom,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);

// }


// void SNRCeShiW(void)
// {
// 	char str[40];
// 	uint16 tmp;
// 	CPU_SR        cpu_sr;
// 	uint8 err=0;
	
// 	if(ColorWm.FatherMenuNumber==0)
// 	{
// 			OS_ENTER_CRITICAL();
// 			tmp = LineDispBuf.scan_sum;
// 			OS_EXIT_CRITICAL();
// 			if(tmp>0)
// 			{
// 				ColorLcdDispBuffLines(SCAN_LINE1_BUF_ADDR,tmp);
// 			}
		
// 			sprintf(str,"%4d",MySysAbs.data_E);
// 			ColorLcdSendString(300,25,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);

// 	}

// 	{
		
// 		if(OSFlagAccept(MyOsFlag,SYS_SCAN_WL_FINISH_FLAG,OS_FLAG_CONSUME|OS_FLAG_WAIT_SET_ALL,&err) == SYS_SCAN_WL_FINISH_FLAG)
// 		{
// 		}

// 		if(OSFlagAccept(MyOsFlag,SYS_MONTAIN_VALLY_FLAG,OS_FLAG_CONSUME|OS_FLAG_WAIT_SET_ALL,&err) == SYS_MONTAIN_VALLY_FLAG)
// 		{
// 			sprintf(str,"%4.0f",gDraw_SNR_TM.top);
// 			ColorLcdSendString(SNR_TM_FRAME.left-strlen(str)*12-12,SNR_TM_FRAME.top,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 			sprintf(str,"%4.0f",gDraw_SNR_TM.bottom);	
// 			ColorLcdSendString(SNR_TM_FRAME.left-strlen(str)*12-12,SNR_TM_FRAME.bottom-24,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_WHITE,(uint8*)str);
// 			sprintf(str,"%4d.%d",CurWaveLen/Unit_1nm,CurWaveLen%Unit_1nm);

// 		}

// 		if(OSFlagAccept(MyOsFlag,SYS_TIME_SCAN_FLAG,OS_FLAG_CONSUME|OS_FLAG_WAIT_SET_ALL,&err) == SYS_TIME_SCAN_FLAG)
// 		{
// 			sprintf(str,"S/N:%4d",gSN);
// 			ColorLcdSendString(90,550,COLOR_LCD_LIB_24_24,FCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);

			
// 			ColorWm.FatherMenuNumber = 0;
// 			ColorLcdDispIcon_Ext1_1(ICON_TEST_GUANGPU);
// 		}

// 	}

// 	{
// 		RedrawGuangPuLine(gDraw);
// //		gDraw.Redraw = 0;
// 		ColorWm.FatherMenuNumber = 0;
// 	}

// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 2;
// 			}
// 		}
// 	}
// 	else if (ColorWm.FatherMenuNumber == MENU_NUMBER_PRINT)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 		}
// 	}
// 	if (ColorWm.FatherMenuNumber == MENU_NUMBER_WAIT_KEY_BOARD)
// 	{
// 		uint8* msg;
// 		uint8 err=0;
// 		char str[20];
// 		uint8 msg_id=0;
// 		msg_id = GetCurrentTaskPrio();
// 		msg =(uint8*)OSMboxPend(WindowMbox[msg_id+1],OS_TICKS_PER_SEC/25,&err);
// 		if(err == OS_NO_ERR)
// 		{
// 			if(msg[0] == MSG_CONFIRM)
// 			{
				
// 				int32 tmp=0;	
// 				memcpy((uint8*)&tmp,(uint8*)&msg[1],4);

// 				if(ColorWm.FatherPageNumber == 1)
// 				{
// 					// memcpy((uint8*)&GuPuCeShiStruct.WaveLenStart,(uint8*)&msg[1],4);
// 					MVStruct.sensitive_level = tmp;
// 				}							

// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			else if(msg[0] == MSG_PAR_ERR)
// 			{

// 				ColorWm.FatherMenuNumber = MENU_NUMBER_WAIT_CHILD_WIN;


// 			}
// 			else if(msg[0] == MSG_CANCLE)
// 			{
// 				ColorWm.FatherMenuNumber = 0;
// 			}
// 			GuPuCeShiW_Init();
// 		}
// 		else
// 		{
// 			if(ColorWm.FatherPageNumber == 1)
// 			{
// 				sprintf(str,"%d",MVStruct.sensitive_level);
// 				ColorLcdSendString(315,550,COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,COLOR_GREEN,(uint8*)str);
// 			}
// 		}
// 	}
// }


// void SNRCeShiK(void)
// {
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
	
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();

// 		if(tmp_key == KEY_SNR_CESHI_CLEAR)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				MVStruct.M_sum = 0;
// 				MVStruct.V_sum = 0;	
// 				LineDispBuf.scan_sum = 0;
// 				ColorLcdDispResetAtFrame((_FRAME*)&SNR_WL_FRAME);
// 			}
// 		}
// 		else if(tmp_key == KEY_SNR_CESHI_SET_PARA)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				ColorWindowsControl(WIN_NUM_GUPU_PAR,0);
// 			}
// 		}
// 		else if (tmp_key == KEY_SNR_CESHI_SAVE_LOAD)
// 		{						
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{	
// 				g_save_load_type = SAVE_LOAD_TYPE_GUANG_PU;
// 				ColorWindowsControl(WIN_NUM_SAVE_LOAD,0);
// 			}
// 		}
// 		else if(tmp_key == KEY_SNR_CESHI_TEST)
// 		{
// 			if(ColorWm.FatherMenuNumber == 0)
// 			{
// 				if(SnrCeShiStruct.WaveLenEnd <SnrCeShiStruct.WaveLenStart)
// 				{

// 				}
// 				else if((SnrCeShiStruct.WL_E_End<SnrCeShiStruct.WL_E_Start) )
// 				{

// 				}
// 				else
// 				{
// 					gDraw_SNR_WL.x_Width= SnrCeShiStruct.WaveLenEnd - SnrCeShiStruct.WaveLenStart;
// 					gDraw_SNR_WL.y_Height = SnrCeShiStruct.WL_E_End - SnrCeShiStruct.WL_E_Start;
// 					gDraw_SNR_WL.left = SnrCeShiStruct.WaveLenStart;
// 					gDraw_SNR_WL.right = SnrCeShiStruct.WaveLenEnd;
// 					gDraw_SNR_WL.top = SnrCeShiStruct.WL_E_End;
// 					gDraw_SNR_WL.bottom = SnrCeShiStruct.WL_E_Start;
// 					gDraw_SNR_WL.Interval = SnrCeShiStruct.WL_Interval;
// 					gDraw_SNR_WL.x_cur = SnrCeShiStruct.WaveLenStart;
// 					gDraw_SNR_WL.y_cur = 0.0;



// 					gDraw_SNR_TM.x_Width= SnrCeShiStruct.SampleTime;
// 					gDraw_SNR_TM.y_Height = SnrCeShiStruct.TM_E_End - SnrCeShiStruct.TM_E_Start;
// 					gDraw_SNR_TM.left = 0;
// 					gDraw_SNR_TM.right = SnrCeShiStruct.SampleTime;
// 					gDraw_SNR_TM.top = SnrCeShiStruct.TM_E_End;
// 					gDraw_SNR_TM.bottom = SnrCeShiStruct.TM_E_Start;
// 					gDraw_SNR_TM.Interval = SnrCeShiStruct.TM_Interval;
// 					gDraw_SNR_TM.x_cur = 0;
// 					gDraw_SNR_TM.y_cur = 0.0;



// 					// memcpy((uint8*)&SysPara.GuPuPar,(uint8*)&GuPuCeShiStruct,sizeof(GuPuCeShiStruct));
// 					// UpdateSysParameter();
// 					LineDispBuf.num = LineDispBuf.scan_sum = 0;
// 					SNRCeShiW_Init();
					
// 					// ColorLcdDispIcon_Ext1(ICON_STOP_GUANGPU,718,	256);
// 					ColorLcdDispResetAtFrame((_FRAME*)&SNR_WL_FRAME);
// 					ColorLcdDispResetAtFrame((_FRAME*)&SNR_TM_FRAME);
// 					OSTimeDly(OS_TICKS_PER_SEC/5);
					
// 					ColorWm.FatherMenuNumber = 4;

					
// 					ActionMsg[0] = MSG_SCAN_SNR;//
// //					SysFlag.ScanFlag = 0;
// 					OSMboxPost(ActionMbox,ActionMsg);
					
// 				}
// 			}
// 			else if(ColorWm.FatherMenuNumber == 2 )
// 			{
// 					SysFlag.EmergercyStopFlag = 1;
// 			}
// 		}
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_SNR_CESHI_MAIN_MENU)
// 		{
// 			if (ColorWm.FatherMenuNumber == 0)//
// 			{
// 				ColorWindowsControl(WIN_NUM_MAIN_MENU,0);
// 			}
// 		}
		
// 	}
// 	else if(TouchKey.Press == 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.current_keyassignments;
// 		OS_EXIT_CRITICAL();
// 	}

	
// }




















// void Child_LanguageW_Init(void)
// {

// 		{
// 			ColorLcdDispIcon_Ext2(ICON_CHINESE_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}
// 		else if (SysPara.language == ENGLISH_LANGUAGE)
// 		{
// 			ColorLcdDispIcon_Ext2(ICON_ENGLISH_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}

// 		{
// 			ColorLcdDispIcon_Ext2(ICON_SPAIN_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}

// 		{
// 			ColorLcdDispIcon_Ext2(ICON_PORTUGAL_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}


// }

// void Child_LanguageW(void)
// {
// }

// void Child_LanguageK(void)
// {
// 	CPU_SR        cpu_sr;
// 	uint32 tmp_key = 0;
// 	if(TouchKey.Release== 1)
// 	{
// 		OS_ENTER_CRITICAL();
// 		tmp_key = TouchKey.back_keyassignments;
// 		OS_EXIT_CRITICAL();
// 		if (tmp_key == KEY_LANGUAGE_CLOSE)
// 		{
// 			if (ColorWm.ChildMenuNumber == 0)//
// 			{
// 				uint8 msg_id=0;
// 				msg_id = GetCurrentTaskPrio();

// 				ColorWindowsJumpInChild(CHILD_WIN_NUM_SET_LANGUAGE);

// 				WindowMsg[msg_id][0] = MSG_CONFIRM;
// 				OSMboxPost(WindowMbox[msg_id],WindowMsg[msg_id]);

				
// 			}
// 		}
// 		else if (tmp_key == KEY_LANGUAGE_ENGLISH)
// 		{
// 			SysPara.language = ENGLISH_LANGUAGE;
// 			UpdateSysParameter();
// 			ColorLcdDispIcon_Ext2(ICON_ENGLISH_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}
// 		else if (tmp_key == KEY_LANGUAGE_CHINESE)
// 		{
// 			SysPara.language = CHINESE_LANGUAGE;
// 			UpdateSysParameter();
// 			ColorLcdDispIcon_Ext2(ICON_CHINESE_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}
// 		else if (tmp_key == KEY_LANGUAGE_SPAIN)
// 		{
// 			SysPara.language = SPANISH_LANGUAGE;
// 			UpdateSysParameter();
// 			ColorLcdDispIcon_Ext2(ICON_SPAIN_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}
// 		else if (tmp_key == KEY_LANGUAGE_PORTUGAL)
// 		{
// 			SysPara.language = PORTUGAL_LANGUAGE;
// 			UpdateSysParameter();
// 			ColorLcdDispIcon_Ext2(ICON_PORTUGAL_SELECT,
// 									ColorWm.ChildWin_Xpos + Child_W_Language_key_table[0].left,
// 									ColorWm.ChildWin_Ypos + Child_W_Language_key_table[0].top);
// 		}
// 		TouchKey.Release = 0;
// 	}
// 	else if (TouchKey.Trg == 1)
// 	{
// 	}
// }








/**
 * @brief 初始化数字键盘显示缓冲区。
 * @return 无返回值。
 * @note 清空数字输入字符串及其长度。
 */
void Child_DigitalKeyboardW_Init(void)
{
	DigitalKeyboard.Str[0] = 0x00;
	DigitalKeyboard.len = 0;
}

/**
 * @brief 在当前子窗口层级的位置绘制数字键盘输入内容。
 * @return 无返回值。
 * @note 格式化 DigitalKeyboard.Str，并根据当前窗口层级选择第一层或第二层子窗口坐标。
 */
void Child_DigitalKeyboardW(void)
{
//	uint8_t tLen = 0;
	char tStr[15];
	uint16_t xpos = 0;
	uint16_t ypos = 0;
	uint8_t msg_id=0;
	msg_id = GetCurrentTaskPrio();
	if(msg_id == 1)
	{
		xpos = ColorWm.ChildWin_Xpos+12;
		ypos = ColorWm.ChildWin_Ypos+14;
	}
	else if (msg_id == 2)
	{
		xpos = ColorWm.SecondChildWin_Xpos+12;
		ypos = ColorWm.SecondChildWin_Ypos+14;
	}

	sprintf(tStr,"%8s",(char*)DigitalKeyboard.Str);
	ColorLcdDispDigitalNumberString(tStr,xpos,ypos);
}

/**
 * @brief 处理数字键盘按键事件，并更新或提交输入值。
 * @return 无返回值。
 * @note 处理数字、小数点、退格、正负号切换和确认键；确认键调用 ExitDigitalKeyBoard 完成校验及结果发送。
 */
void Child_DigitalKeyboardK(void)
{
	
	uint32_t tmp_key = 0;


	if(TouchKey.Release== 1)
	{
		tmp_key = TouchKey.back_keyassignments;
		TouchKey.Release = 0;
	}
	else if (TouchKey.Trg == 1)
	{
 		tmp_key = TouchKey.current_keyassignments;
		if (tmp_key >= KEY_DIGITAL_0 && tmp_key <= KEY_DIGITAL_9)
		{
				if(DigitalKeyboard.len < MAX_ENTER_DATA_LEN)
				{
					DigitalKeyboard.Str[DigitalKeyboard.len++] = (tmp_key & 0x00ff)+0x30;
					DigitalKeyboard.Str[DigitalKeyboard.len] = 0;
				}
		} 
		else if (tmp_key == KEY_DIGITAL_POINT)
		{
				if(DigitalKeyboard.len < MAX_ENTER_DATA_LEN)
				{
					if(DigitalKeyboard.len == 0)
					{
						DigitalKeyboard.Str[DigitalKeyboard.len++] = '0';
						DigitalKeyboard.Str[DigitalKeyboard.len++] = '.';
						DigitalKeyboard.Str[DigitalKeyboard.len] = 0;
					}
					else
					{
						DigitalKeyboard.Str[DigitalKeyboard.len++] = '.';
						DigitalKeyboard.Str[DigitalKeyboard.len] = 0;
					}
				}
		} 
		else if (tmp_key == KEY_DIGITAL_CE)
		{
				if(DigitalKeyboard.len < MAX_ENTER_DATA_LEN)
				{
					if(DigitalKeyboard.len > 0)
					{
						DigitalKeyboard.Str[--DigitalKeyboard.len] = 0;
					}
				}
		} 	
		else if (tmp_key == KEY_DIGITAL_SIGN)
		{
				int i=0;
				if(DigitalKeyboard.Str[0] == '-')
				{
					for(i=0;i<DigitalKeyboard.len;i++)
					{
						DigitalKeyboard.Str[i] = DigitalKeyboard.Str[i+1];
					}
					DigitalKeyboard.len--;
				}
				else
				{
					for(i=DigitalKeyboard.len;i>=0;i--)
					{
						DigitalKeyboard.Str[i+1] = DigitalKeyboard.Str[i];
					}
					DigitalKeyboard.Str[0] = '-';
					DigitalKeyboard.len++;
				}
		}
		else if (tmp_key == KEY_DIGITAL_ENTER)
		{
			ExitDigitalKeyBoard();
		}
	}
}

#define HZ_PAGE_NUM			6

/**
 * @brief 绘制字母键盘的语言选择标志。
 * @return 无返回值。
 * @note 根据 AlphabetKeyboard.NameType 选择中文或英文图标，并绘制在当前子窗口层级的位置。
 */
void Child_AlphabetKeyboardW_Init(void)
{
	uint16_t xpos=0;
	uint16_t ypos=0;

	if (ColorWm.IsChildFlag == 1U)
	{
		xpos = ColorWm.ChildWin_Xpos;
		ypos = ColorWm.ChildWin_Ypos;
	}
	else if (ColorWm.IsChildFlag == 2U)
	{
		xpos = ColorWm.SecondChildWin_Xpos;
		ypos = ColorWm.SecondChildWin_Ypos;
	}

	if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
	{
		ColorLcdDispIcon_Ext1(ICON_ALPH_CN,
								KEY_ALPH_EN_CN_BLACK->left + xpos,
								KEY_ALPH_EN_CN_BLACK->top + ypos);
			
	}
	else
	{
		ColorLcdDispIcon_Ext1(ICON_ALPH_EN,
								KEY_ALPH_EN_CN_BLACK->left + xpos,
								KEY_ALPH_EN_CN_BLACK->top + ypos);
	}

}

/**
 * @brief 绘制文本输入内容；中文模式下同时绘制当前拼音候选项。
 * @return 无返回值。
 * @note 中文模式下绘制候选文本和拼音，英文模式下绘制英文输入字符串。
 */
void Child_AlphabetKeyboardW(void)
{

	uint8_t i=0;
	if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
	{
		ColorLcdSendString(ColorWm.ChildWin_Xpos+20,	ColorWm.ChildWin_Ypos+20,
							COLOR_LCD_LIB_24_24,
							FCOLOR_ON|BCOLOR_ON|GB2312,0X02,
							COLOR_BLUE,
							COLOR_WHITE,
							(uint8_t*)"                                    ");

		for(i=0;i<HZ_PAGE_NUM;i++)
		{
			if(i+AlphabetKeyboard.DispPage*HZ_PAGE_NUM>= AlphabetKeyboard.HZ_num)
			{
				break;
			}
			AlphabetKeyboard.DispBuff[i*5] = i+'1';
			AlphabetKeyboard.DispBuff[i*5+1] = '.';
			AlphabetKeyboard.DispBuff[i*5+2] = AlphabetKeyboard.HZ_String[(AlphabetKeyboard.DispPage*HZ_PAGE_NUM+i)*2];
			AlphabetKeyboard.DispBuff[i*5+3] = AlphabetKeyboard.HZ_String[(AlphabetKeyboard.DispPage*HZ_PAGE_NUM+i)*2+1];
			AlphabetKeyboard.DispBuff[i*5+4] = ' ';
			AlphabetKeyboard.DispBuff[i*5+5] = '\0';
		}
		
		if(AlphabetKeyboard.HZ_num != 0)
		{
			ColorLcdSendString(ColorWm.ChildWin_Xpos+104,	ColorWm.ChildWin_Ypos+20,
							COLOR_LCD_LIB_24_24,
							FCOLOR_ON|BCOLOR_ON|GB2312,0X02,
							COLOR_BLUE,
							COLOR_WHITE,
							(uint8_t*)AlphabetKeyboard.DispBuff);//
		}
		ColorLcdSendString(ColorWm.ChildWin_Xpos+20,	ColorWm.ChildWin_Ypos+20,
						COLOR_LCD_LIB_24_24,
						FCOLOR_ON|BCOLOR_ON|GB2312,0X02,
						COLOR_BLUE,
						COLOR_WHITE,
						(uint8_t*)AlphabetKeyboard.PinYinStr);//

		
		
	}
	else
	{
		ColorLcdSendString(ColorWm.ChildWin_Xpos+20,	ColorWm.ChildWin_Ypos+20,
							COLOR_LCD_LIB_24_24,
							FCOLOR_ON|BCOLOR_ON|GB2312,0X02,
							COLOR_BLUE,
							COLOR_WHITE,
							(uint8_t*)"                                    ");
		ColorLcdSendString(ColorWm.ChildWin_Xpos+20,	ColorWm.ChildWin_Ypos+20,
						COLOR_LCD_LIB_24_24,
						FCOLOR_ON|BCOLOR_ON|GB2312,0X02,
						COLOR_BLUE,
						COLOR_WHITE,
						(uint8_t*)AlphabetKeyboard.EnStr);//
	}

}
/**
 * @brief 处理文本输入、语言切换、拼音候选、编辑及提交操作。
 * @return 无返回值。
 * @note 根据触摸事件编辑中英文输入、翻页或选择候选字、处理编辑按键，并按需要发送确认或取消结果。
 */
void Child_AlphabetKeyboardK(void)
{

	
	uint32_t tmp_key = 0U;
	if(TouchKey.Trg== 1)
	{
		uint16_t xpos;
		uint16_t ypos;

		if (ColorWm.IsChildFlag == 1U)
		{
			xpos = ColorWm.ChildWin_Xpos;
			ypos = ColorWm.ChildWin_Ypos;
		}
		else if (ColorWm.IsChildFlag == 2U)
		{
			xpos = ColorWm.SecondChildWin_Xpos;
			ypos = ColorWm.SecondChildWin_Ypos;
		}
		else
		{
			return;
		}

		taskENTER_CRITICAL();
		tmp_key = TouchKey.back_keyassignments;
		taskEXIT_CRITICAL();
		if (tmp_key == KEY_ALPHABET_EN_CN)
		{
			if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
			{
				AlphabetKeyboard.NameType = ENGLISH_LANGUAGE;
				AlphabetKeyboard.HZ_num = 0;
				AlphabetKeyboard.Enlen = 0;
				AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = '\0';
				AlphabetKeyboard.DispPage = 0;
				ColorLcdDispIcon_Ext1(ICON_ALPH_EN,
										TouchKey.p->left + xpos,
										TouchKey.p->top + ypos);
					
			}
			else
			{
				AlphabetKeyboard.NameType = CHINESE_LANGUAGE;
				AlphabetKeyboard.HZ_num = 0;
				AlphabetKeyboard.PinYinlen = 0;
				AlphabetKeyboard.PinYinStr[AlphabetKeyboard.PinYinlen] = '\0';
				AlphabetKeyboard.DispPage = 0;
				ColorLcdDispIcon_Ext1(ICON_ALPH_CN,
										TouchKey.p->left + xpos,
										TouchKey.p->top + ypos);
			}
			
			
			
		}
	}
	else if (TouchKey.Release == 1)
	{
 		tmp_key = TouchKey.back_keyassignments;
		
		if (tmp_key >= KEY_ALPHABET_0 && tmp_key <= KEY_ALPHABET_9)
		{
			if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.DispPage*HZ_PAGE_NUM +(tmp_key - KEY_ALPHABET_0)   <= AlphabetKeyboard.HZ_num)
				{
					if(AlphabetKeyboard.len < AlphabetKeyboard.MaxLen - 1)
					{
						AlphabetKeyboard.Str[AlphabetKeyboard.len++] = AlphabetKeyboard.HZ_String[(AlphabetKeyboard.DispPage*HZ_PAGE_NUM+tmp_key - KEY_ALPHABET_1)*2];
						AlphabetKeyboard.Str[AlphabetKeyboard.len++] = AlphabetKeyboard.HZ_String[(AlphabetKeyboard.DispPage*HZ_PAGE_NUM+tmp_key - KEY_ALPHABET_1)*2+1];
						AlphabetKeyboard.Str[AlphabetKeyboard.len] = '\0';
						
						AlphabetKeyboard.PinYinlen = 0;
						AlphabetKeyboard.PinYinStr[AlphabetKeyboard.PinYinlen] = '\0';
						AlphabetKeyboard.HZ_num = 0;
						AlphabetKeyboard.DispBuff[0] = '\0';
					}
					
				}
			}
			else
			{
				if(AlphabetKeyboard.len < AlphabetKeyboard.MaxLen)
				{
					AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen++] = (tmp_key - KEY_ALPHABET_0)+'0';
					AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = '\0';
					AlphabetKeyboard.Str[AlphabetKeyboard.len++] = tmp_key - KEY_ALPHABET_0 + '0';
					AlphabetKeyboard.Str[AlphabetKeyboard.len] =  '\0';
				}
			}
		} 
		else if (tmp_key >= KEY_ALPHABET_A && tmp_key <= KEY_ALPHABET_Z)
		{
			if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.PinYinlen < 6)
				{
					AlphabetKeyboard.PinYinStr[AlphabetKeyboard.PinYinlen++] = (tmp_key - KEY_ALPHABET_A)+'a';
					AlphabetKeyboard.PinYinStr[AlphabetKeyboard.PinYinlen] = 0;
					AlphabetKeyboard.DispPage = 0;
					
					AlphabetKeyboard.HZ_String = (uint8_t*)py_ime(AlphabetKeyboard.PinYinStr);
					AlphabetKeyboard.HZ_num = strlen((const char*)AlphabetKeyboard.HZ_String)/2;
				}
			}
			else
			{
				if(AlphabetKeyboard.len < AlphabetKeyboard.MaxLen)
				{
					if(AlphabetKeyboard.CapsType == CAPITAL_LETTER)
					{
						AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen++] = (tmp_key - KEY_ALPHABET_A)+'A';
						AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = 0;
						AlphabetKeyboard.Str[AlphabetKeyboard.len++] = (tmp_key - KEY_ALPHABET_A)+'A';
						AlphabetKeyboard.Str[AlphabetKeyboard.len] = 0;
					}
					else
					{
						AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen++] = (tmp_key - KEY_ALPHABET_A)+'a';
						AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = 0;
						AlphabetKeyboard.Str[AlphabetKeyboard.len++] = (tmp_key - KEY_ALPHABET_A)+'a';
						AlphabetKeyboard.Str[AlphabetKeyboard.len] = 0;
					}

					AlphabetKeyboard.DispPage = 0;
				}
			}
		} 
		else if (tmp_key == KEY_ALPHABET_UP)
		{
			if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.DispPage>0)
				{
					AlphabetKeyboard.DispPage--;
				}
			}
			else
			{
				AlphabetKeyboard.CapsType = CAPITAL_LETTER;
			}
		}
		else if (tmp_key == KEY_ALPHABET_DOWN)
		{
			if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
			{
				if((AlphabetKeyboard.DispPage+1)*HZ_PAGE_NUM < AlphabetKeyboard.HZ_num)
				{
					AlphabetKeyboard.DispPage++;
				}
			}
			else
			{
				AlphabetKeyboard.CapsType = SMALL_LETTER;
			}
		
		}
		else if (tmp_key == KEY_ALPHABET_POINT)
		{
			if(AlphabetKeyboard.NameType != CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.len < AlphabetKeyboard.MaxLen)
				{
					AlphabetKeyboard.Str[AlphabetKeyboard.len++] = '.';
					AlphabetKeyboard.Str[AlphabetKeyboard.len] = 0;
				}
			}
		} 
		else if (tmp_key == KEY_ALPHABET_CE)
		{
			if(AlphabetKeyboard.NameType == CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.PinYinlen > 0)
				{
					AlphabetKeyboard.PinYinStr[--AlphabetKeyboard.PinYinlen] = 0;
					AlphabetKeyboard.HZ_String = (uint8_t*)py_ime(AlphabetKeyboard.PinYinStr);
					AlphabetKeyboard.HZ_num = strlen((const char*)AlphabetKeyboard.HZ_String)/2;
				}
				else 
				{
					if(AlphabetKeyboard.len >= 2)
					{
						if(AlphabetKeyboard.Str[AlphabetKeyboard.len-2]>=0x80)
						{
							AlphabetKeyboard.Str[--AlphabetKeyboard.len] = 0;
							AlphabetKeyboard.Str[--AlphabetKeyboard.len] = 0;
						}
						else
						{
							AlphabetKeyboard.Str[--AlphabetKeyboard.len] = 0;
						}
					}
					else if(AlphabetKeyboard.len >0)
					{
						AlphabetKeyboard.Str[--AlphabetKeyboard.len] = 0;
					}
				}
			}
			else
			{
				if(AlphabetKeyboard.len > 0)
				{
					AlphabetKeyboard.Str[--AlphabetKeyboard.len] = 0;
					AlphabetKeyboard.EnStr[--AlphabetKeyboard.Enlen] = 0;
				}
			}
		} 	
		else if (tmp_key == KEY_ALPHABET_DOWN_LINE)
		{
			if(AlphabetKeyboard.NameType != CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.len < AlphabetKeyboard.MaxLen)
				{
					AlphabetKeyboard.Str[AlphabetKeyboard.len++] = '_';
					AlphabetKeyboard.Str[AlphabetKeyboard.len] = 0;
					AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen++] = '_';
					AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = 0;
				}
			}
		}
		else if (tmp_key == KEY_ALPHABET_SPACE)
		{
			if(AlphabetKeyboard.NameType != CHINESE_LANGUAGE)
			{
				if(AlphabetKeyboard.len < AlphabetKeyboard.MaxLen)
				{
					AlphabetKeyboard.Str[AlphabetKeyboard.len++] = ' ';
					AlphabetKeyboard.Str[AlphabetKeyboard.len] = 0;
					AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen++] = ' ';
					AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = 0;
				}
			}
		}
		else if (tmp_key == KEY_ALPHABET_ENTER)
		{
			uint8_t msg_id;
			uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
			osStatus_t status;
			uint8_t messageLength;

			AlphabetKeyboard.PinYinlen = 0;
			AlphabetKeyboard.PinYinStr[AlphabetKeyboard.PinYinlen] = '\0';
			AlphabetKeyboard.HZ_num = 0;
			AlphabetKeyboard.DispBuff[0] = '\0';
			AlphabetKeyboard.DispPage = 0;
			AlphabetKeyboard.Enlen = 0;
			AlphabetKeyboard.EnStr[AlphabetKeyboard.Enlen] = '\0';
//			AlphabetKeyboard.len = 0;
//			AlphabetKeyboard.Str[AlphabetKeyboard.len] = '\0';



			if (AlphabetKeyboard.len > (COLOR_WM_MESSAGE_SIZE - 2U))
			{
				printf("Alphabet keyboard result is too long\r\n");
				TouchKey.Release = 0;
				return;
			}

			msg_id = GetCurrentTaskPrio();
			message[0] = MSG_CONFIRM;
			messageLength = (uint8_t)(AlphabetKeyboard.len + 1U);
			memcpy(&message[1], AlphabetKeyboard.Str, messageLength);
			status = ColorWM_WindowMessageSend(msg_id, message);
			if (status != osOK)
			{
				printf("Failed to post alphabet keyboard result: %ld\r\n",
				       (long)status);
				TouchKey.Release = 0;
				return;
			}
			ExitAlphabetKeyBoard();
		}
		else if (tmp_key == KEY_ALPHABET_CLOSE)
		{
			uint8_t msg_id = GetCurrentTaskPrio();
			uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};
			osStatus_t status;

			message[0] = MSG_CANCLE;
			status = ColorWM_WindowMessageSend(msg_id, message);
			if (status != osOK)
			{
				printf("Failed to post alphabet keyboard cancel: %ld\r\n",
				       (long)status);
				TouchKey.Release = 0;
				return;
			}
			ExitAlphabetKeyBoard();
		}
		TouchKey.Release = 0;

	}
	
}




/**
 * @brief 处理消息框的显示时长及事件等待完成状态。
 * @return 无返回值。
 * @note 事件等待型消息框轮询指定事件位，定时消息框递减显示时长；满足结束条件后发送 MSG_CONFIRM 并请求关闭消息框。
 */
void Child_MsgBoxW(void)
{
	uint8_t msg_id = GetCurrentTaskPrio();
	uint8_t message[COLOR_WM_MESSAGE_SIZE] = {0U};

	if(MsgBox.Type == 1)
	{
		EventBits_t uxBits = 0;

		if(MyOsFlag == NULL)
		{
			MyOsFlag_Init();
		}

		if(MyOsFlag != NULL && MsgBox.WaitFlag != 0)
		{
			uxBits = xEventGroupWaitBits(MyOsFlag,
			                             (EventBits_t)MsgBox.WaitFlag,
			                             pdTRUE,
			                             pdTRUE,
			                             pdMS_TO_TICKS(10));
			if((uxBits & (EventBits_t)MsgBox.WaitFlag) == (EventBits_t)MsgBox.WaitFlag)
			{
				message[0] = MSG_CONFIRM;
				ColorWM_WindowMessageSend(msg_id, message);
				HideMsgBox();
			}
		}
	}
	else if(MsgBox.Type == 0)
	{
		if(MsgBox.SurviveTime > 0)
		{
			MsgBox.SurviveTime--;
		}
		else
		{
			message[0] = MSG_CONFIRM;
			ColorWM_WindowMessageSend(msg_id, message);
			HideMsgBox();
		}
	}
	else if(MsgBox.Type == 2)
	{
	}

}


/**
 * @brief 处理消息框按键释放时的附加操作。
 * @return 无返回值。
 * @note 检查基线测量返回键；在指定的零点设置状态下请求紧急停止，并清除按键释放标志。
 */
void Child_MsgBoxK(void)
{
	uint32_t tmp_key = 0;
	
	if(TouchKey.Release== 1)
	{
		tmp_key = TouchKey.back_keyassignments;
		if (tmp_key == KEY_GUANGPU_BASE_LINE)
		{
			if(ColorWm.FatherWindowNumber == WIN_NUM_GUPU_CESHI && ColorWm.FatherMenuNumber == 1)
			{
				if(SysFlag.ZeroSetStep == 2)
				{
					SysFlag.EmergercyStopFlag = 1;
				}
			}
		}
		TouchKey.Release = 0;
	}
	
	

}

/**
 * @brief 绘制消息框文本或其对应图标。
 * @return 无返回值。
 * @note 未配置图标时显示 MsgBox.msg；配置了图标时在消息框位置绘制 MsgBox.MsgIcon。
 */
void Child_MsgBoxW_Init(void)
{
	if(MsgBox.MsgIcon ==(const SIMPLE_ICON*)0)
	{
		ColorLcdSendString(ColorWm.ChildWin_Xpos+50,ColorWm.ChildWin_Ypos+80,
											COLOR_LCD_LIB_24_24,FCOLOR_ON|BCOLOR_ON|GB2312,0X02,COLOR_BLUE,0XD69A,
											(uint8_t*)MsgBox.msg);
	}
	else
	{	
		ColorLcdDispIcon_Ext1(MsgBox.MsgIcon,ColorWm.ChildWin_Xpos+50,ColorWm.ChildWin_Ypos+80);
	}
}










void RedrawGuangPuLine(_DrawStruct tmpDraw)
{
		uint16_t points =0;
		uint16_t j=0;
//		uint8_t cnt = 0;
		uint16_t tWave=0;

		const uint16_t addr =SCAN_LINE1_BUF_ADDR;
		const uint16_t color =  COLOR_RED;

	
		ColorLcdDispResetAtFrame((_FRAME*)&WL_FRAME);
		
		points = (tmpDraw.right- tmpDraw.left)/tmpDraw.Interval+1;

		tWave = tmpDraw.left;
		PIXEL_POINT pt = CalcCoordinate(tWave, &tmpDraw,(_FRAME*)&WL_FRAME,GuangPuScanLine[tWave-MIN_WAVELEN]);
		LineDispBuf.pixel[0].x = pt.x;
		LineDispBuf.pixel[0].y = pt.y;
		LineDispBuf.pixel[1].x = pt.x;
		LineDispBuf.pixel[1].y = pt.y;
		LineDispBuf.num = 0;


		for(j=0;j<points;j++)
		{
			tWave = tmpDraw.left + j*tmpDraw.Interval;
			
			PIXEL_POINT pt = CalcCoordinate(tWave, &tmpDraw,(_FRAME*)&WL_FRAME,GuangPuScanLine[tWave-MIN_WAVELEN]);
			if(pt.x > LineDispBuf.pixel[1].x)
			{
				LineDispBuf.pixel[0].x  =  LineDispBuf.pixel[1].x;
				LineDispBuf.pixel[0].y  =  LineDispBuf.pixel[1].y;
				LineDispBuf.pixel[1].x  =  pt.x;
				LineDispBuf.pixel[1].y  =  pt.y;
				LineDispBuf.num++;
				ColorLcdWriteOneLineToBuff(addr+LineDispBuf.num*5,
											LineDispBuf.pixel[0].x,	LineDispBuf.pixel[0].y,
											LineDispBuf.pixel[1].x,	LineDispBuf.pixel[1].y,
											color);
				if(LineDispBuf.num % 5 == 0 )
				{
					ColorLcdDispBuffLines(addr,LineDispBuf.num);
				}
			}
		}
		LineDispBuf.scan_sum = LineDispBuf.num;
		ColorLcdDispBuffLines(addr,LineDispBuf.scan_sum);

}







// void RedrawDongLiXueLine(_DrawStruct tmpDraw)
// {

// 		uint16 points =0;
// 		uint16 j=0;


// 		const uint16 color =  COLOR_RED;

	
// 		ColorLcdDispResetAtFrame((_FRAME*)&TM_FRAME);
// 		points = (tmpDraw.right- tmpDraw.left)/tmpDraw.Interval+1;

// 		PIXEL_POINT pt = CalcCoordinate(0, &tmpDraw,(_FRAME*)&TM_FRAME,DongLiXueScanLine[0]);
// 		LineDispBuf.pixel[0].x = pt.x;
// 		LineDispBuf.pixel[0].y = pt.y;
// 		LineDispBuf.pixel[1].x = pt.x;
// 		LineDispBuf.pixel[1].y = pt.y;
// 		LineDispBuf.num = 0;

	
// 		points = (tmpDraw.right- tmpDraw.left)/tmpDraw.Interval+1;

// 		for(j=0;j<points;j++)
// 		{
// 			PIXEL_POINT pt = CalcCoordinate(j*tmpDraw.Interval, &tmpDraw,(_FRAME*)&TM_FRAME,DongLiXueScanLine[j]);
// 			if(pt.x > LineDispBuf.pixel[1].x)
// 			{
// 				LineDispBuf.pixel[0].x  =  LineDispBuf.pixel[1].x;
// 				LineDispBuf.pixel[0].y  =  LineDispBuf.pixel[1].y;
// 				LineDispBuf.pixel[1].x  =  pt.x;
// 				LineDispBuf.pixel[1].y  =  pt.y;

// 				ColorLcdWriteOneLineToBuff(TIME_LINE1_BUF_ADDR+LineDispBuf.num*5,
// 											LineDispBuf.pixel[0].x,	LineDispBuf.pixel[0].y,
// 											LineDispBuf.pixel[1].x,	LineDispBuf.pixel[1].y,
// 											color);
// 				LineDispBuf.num++;
// 				if(LineDispBuf.num % 5 == 0 )
// 				{
// 					ColorLcdDispBuffLines(TIME_LINE1_BUF_ADDR,LineDispBuf.num);
// 				}
// 			}
// 		}


// 		ColorLcdDispBuffLines(TIME_LINE1_BUF_ADDR,LineDispBuf.num);



	 

// }







// void CopyGuangPuDrawPara(void)
// {
// 	memcpy((uint8*)&GuPuCeShiStruct,(uint8*)&SysPara.GuPuPar,sizeof(GuPuCeShiStruct));
// 	gDraw.x_Width= GuPuCeShiStruct.WaveLenEnd - GuPuCeShiStruct.WaveLenStart;
	
// 	gDraw.y_Height = GuPuCeShiStruct.data_E_End - GuPuCeShiStruct.data_E_Start;
// 	gDraw.left = GuPuCeShiStruct.WaveLenStart;
// 	gDraw.right = GuPuCeShiStruct.WaveLenEnd;
// 	gDraw.top = GuPuCeShiStruct.data_E_End;
// 	gDraw.bottom = GuPuCeShiStruct.data_E_Start;
// 	gDraw.Interval = GuPuCeShiStruct.Interval;
// 	gDraw.x_cur = GuPuCeShiStruct.WaveLenStart;


// 	gDraw.left_reserve = gDraw.left;
// 	gDraw.right_reserve= gDraw.right;
// 	gDraw.top_reserve= gDraw.top;
// 	gDraw.bottom_reserve= gDraw.bottom;
// }




PIXEL_POINT CalcCoordinate(uint32_t wave,_DrawStruct* ds,_FRAME* pFrame,int16_t sl)
{
	uint32_t tmp1=0,tmp2=0;
	float f1=0,f2=0;
	PIXEL_POINT pt;
	//计算 x坐标
	tmp1 = (wave - ds->left) ;
	tmp2 = tmp1 * (pFrame->right - pFrame->left)/(ds->right- ds->left)+ pFrame->left;
	if(tmp2 > pFrame->right)
	{
		pt.x=pFrame->right;
	}
	else
	{
		pt.x = tmp2;
	}
	//计算Y坐标
	f1 = (sl - ds->bottom);
	f2 = pFrame->bottom - f1* (pFrame->bottom - pFrame->top)/(ds->top - ds->bottom);
	if(f1 < 0 )
	{
		pt.y = pFrame->bottom ;
	}
	else if (f2 < pFrame->top)
	{
		pt.y = pFrame->top;
	}
	else
	{
		pt.y = (f2);
	}
	
	
	return pt;

}


PIXEL_POINT CalcXY(uint32_t wave,uint16_t data,_DrawStruct ds,_FRAME* pFrame)
{
	uint32_t tmp1=0,tmp2=0;	
	float f1=0,f2=0;
	PIXEL_POINT pt;
	//计算 x坐标
	tmp1 = (wave - ds.left) ;
	tmp2 = tmp1 * (pFrame->right - pFrame->left)/(ds.right- ds.left)+ pFrame->left;
	if(tmp2 > pFrame->right)
	{
		pt.x=pFrame->right;
	}
	else
	{
		pt.x = tmp2;
	}
	//计算Y坐标
	f1 = (data - ds.bottom);
	f2 = pFrame->bottom - f1* (pFrame->bottom - pFrame->top)/(ds.top - ds.bottom);
	if(f1 < 0 )
	{
		pt.y = pFrame->bottom ;
	}
	else if (f2 < pFrame->top)
	{
		pt.y = pFrame->top;
	}
	else
	{
		pt.y = (f2);
	}

		pt.y -= 15;
	
	
	return pt;



}
