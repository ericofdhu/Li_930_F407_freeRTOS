#ifndef __TOUCH_KEY_8INCH_H
#define __TOUCH_KEY_8INCH_H

#include "ColorLcd.h"
#include "cmsis_os2.h"
#include <stdint.h>
#include "TouchKey_8inch.h"
#include "ColorWm_8inch.h"
#include "system_runtime.h"




_TOUCH_KEY     			TouchKey;
_DigitalKeyboard		DigitalKeyboard;
_AlphabetKeyboard		AlphabetKeyboard;
TOUCH_POINT     		KeyBoardPosition;



const TOUCH_KEY_SQUARE WindowWelcom_key_table[]=
{
{270,		385,		270+270,	385+110,			KEY_WELCOME_DIS_CONNECT,		0,		480,	186,	(SIMPLE_ICON*)ICON_BLANK},
{0xffff,	0xffff,		0xffff,		0xffff,			0xffff,							0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

const TOUCH_KEY_SQUARE WindowWarmUp_key_table[]=
{
{110,		325,		320,		395,			KEY_WARM_UP_YES,							0,		480,	186,	(SIMPLE_ICON*)ICON_BLANK},
{465,		325,		675,		395,			KEY_WARM_UP_NO,								0,		480,	186,	(SIMPLE_ICON*)ICON_BLANK},
#if DEVICE_TYPE == DEVICE_TYPE_L9	
{700,		0,			800,		100,			KEY_WARM_UP_DEBUG,						0,		480,	186,	(SIMPLE_ICON*)ICON_BLANK},
#endif


{0xffff,	0xffff,		0xffff,		0xffff,			0xffff,								0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};



const TOUCH_KEY_SQUARE WindowInit_key_table[]=
{
{0,		0,		100,		100,		KEY_BACK_DOOR,			0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK},//T/ABSת��
{0xffff,0xffff,	0xffff,	0xffff,	0xffff,							0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};


//���˵�
const TOUCH_KEY_SQUARE WindowMainMenu_key_table[]=
{

{125,	235,		220,	75,		KEY_MAIN_MENU_GUANG_PU,					0,0,0,								(SIMPLE_ICON*)ICON_BLANK},
{125,	330,		220,	75,		KEY_MAIN_MENU_DING_LIANG,				0,0,0,								(SIMPLE_ICON*)ICON_BLANK},
{465,	235,		220,	75,		KEY_MAIN_MENU_DONG_LI_XUE,				0,0,0,								(SIMPLE_ICON*)ICON_BLANK},
{460,	330,		220,	75,		KEY_MAIN_MENU_SYSTEM_CONFIG,				0,0,0,								(SIMPLE_ICON*)ICON_BLANK},



{0xffff,0xffff,0xffff,0xffff,0xffff,0,0,0,(SIMPLE_ICON*)ICON_BLANK}


};



const TOUCH_KEY_SQUARE WindowGuangPu_key_table[]=
{
{0,		0,			70,		70,			KEY_GUANGPU_MAIN_MENU,							2,		ICON_CUT_PIC_NUM_7,		0,			(SIMPLE_ICON*)ICON_BLANK},
{568,	0,			65,		65,			KEY_GUANGPU_SET_PARA,								2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{709,	0,			65,		65,			KEY_GUANGPU_CLEAR,									2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{639,	0,			65,		65,			KEY_GUANGPU_SAVE_LOAD,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{533,	525,		180,	65,			KEY_GUANGPU_TEST,										2,		ICON_CUT_PIC_NUM_7,		ICON_CUT_PIC_NUM_8,		(SIMPLE_ICON*)ICON_BLANK},
{97,	525,		180,	65,			KEY_GUANGPU_FIND_PEAK,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{109,	118,		600,	373,		KEY_GUANGPU_CUR_CURSOR,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{715,	450,		37,		37,			KEY_GUANGPU_CURSOR_LEFT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
{757,	450,		37,		37,			KEY_GUANGPU_CURSOR_RIGHT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
{717,	410,		75,		38,			KEY_GUANGPU_M1_M2,								0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{0,		526,		70,		70,			KEY_GUANGPU_TO_CORRECT_WAVE,					0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{292,	535,		75,		55,			KEY_GUANGPU_SENS_SENSITIVE_LEVEL,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};







const TOUCH_KEY_SQUARE WindowGuangPuPar_key_table[]=
{

{202,	77,				100,	35,		KEY_SET_GUPU_SPEED_SLOW,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{378,	77,				100,	35,		KEY_SET_GUPU_SPEED_MIDDLE,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{552,	77,				100,	35,		KEY_SET_GUPU_SPEED_FAST,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{202,	119,			100,	35,		KEY_SET_GUPU_INTERVAL_01,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{378,	119,			100,	35,		KEY_SET_GUPU_INTERVAL_10,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{202,	165,			100,	35,		KEY_SET_GUPU_WL_START,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{389,	165,			100,	35,		KEY_SET_GUPU_WL_END,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{202,	207,			100,	35,		KEY_SET_GUPU_DATA_START,		0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{389,	207,			100,	35,		KEY_SET_GUPU_DATA_END,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{202,	285,			100,	35,		KEY_SET_GUPU_EMIT_LIGHT_1,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{317,	285,			100,	35,		KEY_SET_GUPU_EMIT_LIGHT_2,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{431,	285,			100,	35,		KEY_SET_GUPU_EMIT_LIGHT_3,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{546,	285,			100,	35,		KEY_SET_GUPU_EMIT_LIGHT_4,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},


{202,	375,			100,	35,		KEY_SET_GUPU_PMT_ON,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{385,	375,			100,	35,		KEY_SET_GUPU_PMT_OFF,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{202,	421,			100,	35,		KEY_SET_GUPU_PMT_VOLTAGE,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},


{202,	487,			100,	35,		KEY_SET_GUPU_GAIN_1,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{297,	487,			100,	35,		KEY_SET_GUPU_GAIN_2,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{390,	487,			100,	35,		KEY_SET_GUPU_GAIN_4,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{485,	487,			100,	35,		KEY_SET_GUPU_GAIN_8,				0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},


{286,	534,			108,	42,		KEY_SET_GUPU_CONFIRM,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{432,	534,			108,	42,		KEY_SET_GUPU_CANCLE,			0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},




{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

const TOUCH_KEY_SQUARE WindowMontainValley_key_table[] = 
{
{630,	62,		710,	107,		KEY_MONTAIN_VALLEY_CLOSE,						0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},
{125,	485,	125+70,	485+40,		KEY_MONTAIN_VALLEY_SENSITIVE1,					0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},
{230,	485,	230+70,	485+40,		KEY_MONTAIN_VALLEY_SENSITIVE2,					0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},
{340,	485,	340+70,	485+40,		KEY_MONTAIN_VALLEY_SENSITIVE3,					0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},
{450,	485,	450+70,	485+40,		KEY_MONTAIN_VALLEY_SENSITIVE4,					0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},
{555,	485,	555+70,	485+40,		KEY_MONTAIN_VALLEY_SENSITIVE5,					0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},
{670,	135,	702,	287,		KEY_MONTAIN_VALLEY_UP,							1,		670,		135,			(SIMPLE_ICON*)ICON_VALLEY_UP_BLACK},
{670,	300,	702,	452,		KEY_MONTAIN_VALLEY_DOWN,						1,		670,		300,			(SIMPLE_ICON*)ICON_VALLEY_DOWN_BLACK},
//{718,	447,	792,	520,		KEY_MONTAIN_VALLEY_PRINT,						1,		718,	447,		(SIMPLE_ICON*)ICON_PRINT_GUANGPU_BLACK},
{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}

};











/////////////////////////////
//����MPϵ����  ���Խ��水��
const TOUCH_KEY_SQUARE WindowDingLiangCeShi_key_table[]=
{
{510,	100,			53,	100,		KEY_DING_LIANG_CESHI_LAST_PAGE,				0,		520,	100,						(SIMPLE_ICON*)ICON_BLANK},
{510,	353,			53,	100,		KEY_DING_LIANG_CESHI_NEXT_PAGE,				0,		520,	353,						(SIMPLE_ICON*)ICON_BLANK},

{601,	394,			183,	73,		KEY_DING_LIANG_CESHI_TEST,					2,		ICON_CUT_PIC_NUM_15,	0,						(SIMPLE_ICON*)ICON_BLANK},

{0,		0,			70,		70,			KEY_DING_LIANG_CESHI_MAIN_MENU,							2,		ICON_CUT_PIC_NUM_7,		0,			(SIMPLE_ICON*)ICON_BLANK},
{568,	0,			65,		65,			KEY_DING_LIANG_CESHI_SET_PAR,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{709,	0,			65,		65,			KEY_DING_LIANG_CESHI_CLEAR,								2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{639,	0,			65,		65,			KEY_DING_LIANG_CESHI_SAVELOAD,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{638,	539,		105,	61,			KEY_DING_LIANG_CESHI_CALIB,							0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK},
{640,	90,			111,	53,			KEY_DING_LIANG_CESHI_SET_WAVE,							0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK},

{601,	152,			183,	73,		KEY_DING_LIANG_CESHI_READ_BASE,		2,		ICON_CUT_PIC_NUM_15,	0,						(SIMPLE_ICON*)ICON_BLANK},
{746,	228,			40,		40,		KEY_DING_LIANG_CESHI_EN_BASE,			0,		0,	0,						(SIMPLE_ICON*)ICON_BLANK},



{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};


//	const uint16 Ytable[] = {135,164,193,221,249,277,305,333,361,389,417,445,473,501};


//����MPϵ����  �궨
const TOUCH_KEY_SQUARE WindowDingLiangCeShiBiaoDing_key_table[]=
{
	
{94,	114,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_1,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	152,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_2,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	190,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_3,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	228,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_4,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	266,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_5,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	304,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_6,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	342,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_7,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	380,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_8,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	418,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_9,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	456,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_SEL_10,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},


{295,	114,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_1,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	152,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_2,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	190,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_3,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	228,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_4,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	266,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_5,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	304,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_6,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	342,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_7,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	380,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_8,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	418,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_9,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	456,			190,	35,		KEY_DING_LIANG_CESHI_BIAODING_DATA_10,			0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},


{636,	90,			111,	53,			KEY_DING_LIANG_CESHI_BIAODING_SET_WAVE,			0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK},
{602,	155,			183,	67,		KEY_DING_LIANG_CESHI_BIAODING_READ_BASE,		2,		ICON_CUT_PIC_NUM_17,	0,						(SIMPLE_ICON*)ICON_BLANK},
{746,	228,			40,		40,		KEY_DING_LIANG_CESHI_BIAODING_EN_BASE,			0,		ICON_CUT_PIC_NUM_17,	0,						(SIMPLE_ICON*)ICON_BLANK},


{639,	0,			65,	65,	KEY_DING_LIANG_CESHI_BIAODING_SAVE_LOAD,				2,		ICON_CUT_PIC_NUM_17,	0,						(SIMPLE_ICON*)ICON_BLANK},
{709,	0,			65,	65,	KEY_DING_LIANG_CESHI_BIAODING_CLR_DATA,					2,		ICON_CUT_PIC_NUM_17,	0,						(SIMPLE_ICON*)ICON_BLANK},

{599,	396,			183,	67,		KEY_DING_LIANG_CESHI_BIAODING_TEST,					2,		ICON_CUT_PIC_NUM_17,	0,						(SIMPLE_ICON*)ICON_BLANK},

{630,	545,			156,	51,		KEY_DING_LIANG_CESHI_BIAODING_DISP_CURVE,			0,		630,	545,							(SIMPLE_ICON*)ICON_DISP_CURVE_MP_BLACK},
{498,	540,			111,	54,		KEY_DING_LIANG_CESHI_BIAODING_CREATE_CURVE,			0,		498,	540,						(SIMPLE_ICON*)ICON_FINISH_MP_BLACK},
{0,		0,				70,		70,		KEY_DING_LIANG_CESHI_BIAODING_BACK,					0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},



{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

//����MPϵ����  �������ð���
const TOUCH_KEY_SQUARE WindowDingLiangMPPar_key_table[]=
{
{180,	130,			370,	175,		KEY_DING_LIANG_MPPAR_SET_WAVE,					0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},

{190,	214,			300,	255,		KEY_DING_LIANG_MPPAR_UNIT_NONE,					0,		718,	256,						(SIMPLE_ICON*)ICON_BLANK},
{310,	214,			420,	255,		KEY_DING_LIANG_MPPAR_UNIT_PERCENT,				0,		718,	365,						(SIMPLE_ICON*)ICON_BLANK},
{445,	214,			555,	255,		KEY_DING_LIANG_MPPAR_UNIT_PPB,					0,		718,	447,						(SIMPLE_ICON*)ICON_BLANK},
{561,	214,			672,	255,		KEY_DING_LIANG_MPPAR_UNIT_ML,					0,		14,		546,						(SIMPLE_ICON*)ICON_BLANK},

{190,	280,			300,	325,		KEY_DING_LIANG_MPPAR_UNIT_GL,					0,		470,	546,						(SIMPLE_ICON*)ICON_BLANK},
{310,	280,			420,	325,		KEY_DING_LIANG_MPPAR_UNIT_MGML,					0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},
{445,	280,			555,	325,		KEY_DING_LIANG_MPPAR_UNIT_UGML,					0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},
{561,	280,			672,	325,		KEY_DING_LIANG_MPPAR_UNIT_NGML,					0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},

{465,	345,			590,	390,		KEY_DING_LIANG_MPPAR_BIAO_DING,					1,		465,	345,						(SIMPLE_ICON*)ICON_BLANK},
{503,	480,			600,	520,		KEY_DING_LIANG_MPPAR_CONFIRM,					0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},
{604,	480,			702,	520,		KEY_DING_LIANG_MPPAR_CANCLE,					0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},


{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

//����MPϵ����  ��ʵ����
const TOUCH_KEY_SQUARE WindowDingLiangMPCurve_key_table[]=
{
//{101,	549,		220,	592,		KEY_DING_LIANG_MP_CURVE_FINISH,						1,		101,	549,		(SIMPLE_ICON*)ICON_FINISH_MP_BLACK},
//{225,	549,		345,	592,		KEY_DING_LIANG_MP_CURVE_DISP_DATA,				1,		225,	549,		(SIMPLE_ICON*)ICON_DISP_DATA_MP_BLACK},
//{718,	365,		792,	440,		KEY_DING_LIANG_MP_CURVE_SAVE_DATA,				0,		101,	549,		(SIMPLE_ICON*)ICON_BLANK},
{532,	525,		186,	63,			KEY_DING_LIANG_MP_CURVE_BACK,						0,		718,	447,		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};




//����Ȳ���?
const TOUCH_KEY_SQUARE WindowSNRCeShi_key_table[]=
{
	
{568,	0,			65,		65,			KEY_SNR_CESHI_SET_PARA,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{709,	0,			65,		65,			KEY_SNR_CESHI_CLEAR,								2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{639,	0,			65,		65,			KEY_SNR_CESHI_SAVE_LOAD,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{533,	525,		180,	65,			KEY_SNR_CESHI_TEST,								2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{0,		0,			70,		70,			KEY_SNR_CESHI_MAIN_MENU,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},

{109,	118,		600,	373,		KEY_SNR_CESHI_CUR_CURSOR,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{715,	450,		37,		37,			KEY_SNR_CESHI_CURSOR_LEFT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
{757,	450,		37,		37,			KEY_SNR_CESHI_CURSOR_RIGHT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},



{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};





//����ѧ����
const TOUCH_KEY_SQUARE WindowDongLiXueCeshi_key_table[]=
{
	
{568,	0,			65,		65,			KEY_DONG_LI_XUE_CESHI_SET_PARA,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{709,	0,			65,		65,			KEY_DONG_LI_XUE_CESHI_CLEAR,								2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{639,	0,			65,		65,			KEY_DONG_LI_XUE_CESHI_SAVE_LOAD,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{533,	525,		180,	65,			KEY_DONG_LI_XUE_CESHI_TEST,								2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},
{0,		0,			70,		70,			KEY_DONG_LI_XUE_CESHI_MAIN_MENU,							2,		ICON_CUT_PIC_NUM_7,		0,		(SIMPLE_ICON*)ICON_BLANK},

{109,	118,		600,	373,		KEY_DONG_LI_XUE_CESHI_CUR_CURSOR,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{715,	450,		37,		37,			KEY_DONG_LI_XUE_CESHI_CURSOR_LEFT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
{757,	450,		37,		37,			KEY_DONG_LI_XUE_CESHI_CURSOR_RIGHT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},



{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};




//����ѧ���� ��������
const TOUCH_KEY_SQUARE WindowDongLiXuePar_key_table[]=
{
{292,	72,			130,	32,		KEY_DONG_LI_XUE_PAR_SET_WAVE,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{292,	113,		130,	32,		KEY_DONG_LI_XUE_PAR_SET_TIME,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{292,	155,		130,	32,		KEY_DONG_LI_XUE_PAR_UNIT_MINUTE,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{457,	155,		130,	32,		KEY_DONG_LI_XUE_PAR_UNIT_SECOND,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{292,	199,		130,	32,		KEY_DONG_LI_XUE_PAR_DATA_START,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{457,	199,		130,	32,		KEY_DONG_LI_XUE_PAR_DATA_END,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{292,	243,		130,	32,		KEY_DONG_LI_XUE_PAR_SCAN_INTERVAL,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{224,	309,		100,	40,		KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_1,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{337,	309,		100,	40,		KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_2,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{452,	309,		100,	40,		KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_3,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{566,	309,		100,	40,		KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_4,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},

{221,	475,		80,		40,		KEY_DONG_LI_XUE_PAR_GAIN_1,								0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{316,	475,		80,		40,		KEY_DONG_LI_XUE_PAR_GAIN_2,								0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{411,	475,		80,		40,		KEY_DONG_LI_XUE_PAR_GAIN_4,								0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{506,	475,		80,		40,		KEY_DONG_LI_XUE_PAR_GAIN_8,								0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},


{223,	374,		140,	40,		KEY_DONG_LI_XUE_PAR_PMT_ON,								0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{405,	374,		140,	40,		KEY_DONG_LI_XUE_PAR_PMT_OFF,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{223,	423,		130,	32,		KEY_DONG_LI_XUE_PAR_PMT_VOLTAGE,						0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},


{524,	534,		180,	65,		KEY_DONG_LI_XUE_PAR_CONFIRM,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{0,		0,			70,		70,		KEY_DONG_LI_XUE_PAR_CANCLE,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};


const TOUCH_KEY_SQUARE WindowWaveCorrect_key_table[]=
{

{94,	114,			190,	35,		KEY_WAVE_CORRECT_STEPS_1,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	152,			190,	35,		KEY_WAVE_CORRECT_STEPS_2,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	190,			190,	35,		KEY_WAVE_CORRECT_STEPS_3,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	228,			190,	35,		KEY_WAVE_CORRECT_STEPS_4,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	266,			190,	35,		KEY_WAVE_CORRECT_STEPS_5,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	304,			190,	35,		KEY_WAVE_CORRECT_STEPS_6,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	342,			190,	35,		KEY_WAVE_CORRECT_STEPS_7,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	380,			190,	35,		KEY_WAVE_CORRECT_STEPS_8,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	418,			190,	35,		KEY_WAVE_CORRECT_STEPS_9,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{94,	456,			190,	35,		KEY_WAVE_CORRECT_STEPS_10,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},


{295,	114,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_1,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	152,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_2,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	190,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_3,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	228,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_4,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	266,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_5,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},	
{295,	304,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_6,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	342,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_7,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	380,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_8,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	418,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_9,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},
{295,	456,			190,	35,		KEY_WAVE_CORRECT_REAL_WAVE_10,				0,		718,	173,						(SIMPLE_ICON*)ICON_BLANK},

{639,	0,				65,		65,		KEY_WAVE_CORRECT_SAVE,					1,	700,	0,							(SIMPLE_ICON*)ICON_BLANK},
{568,	0,				65,		65,		KEY_WAVE_CORRECT_LOAD_DEFAULT,				0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},
{0,		0,				70,		70,		KEY_WAVE_CORRECT_CONFIRM,				0,		700,	0,							(SIMPLE_ICON*)ICON_BLANK},
// {613,	394,			183,	73,		KEY_WAVE_CORRECT_CALC,					1,		465,	345,						(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};


//const TOUCH_KEY_SQUARE WindowSnScan_key_table[]=
//{

//{86,	100,		600,	187,		KEY_SN_SCAN_WAVE_CUR_CURSOR,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{86,	344,		600,	156,		KEY_SN_SCAN_TIME_CUR_CURSOR,							0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{385,	543,		45,		45,			KEY_SN_SCAN_CURSOR_LEFT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{439,	543,		45,		45,			KEY_SN_SCAN_CURSOR_RIGHT,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{757,	450,		37,		37,			KEY_SN_SCAN_START,						2,		ICON_CUT_PIC_NUM_7,	0,		(SIMPLE_ICON*)ICON_BLANK},
//};




const TOUCH_KEY_SQUARE WindowSaveLoad_key_table[] = 
{

{112,	103,	530,	24,		KEY_SAVE_LOAD_FILE0,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	131,	530,	24,		KEY_SAVE_LOAD_FILE1,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	159,	530,	24,		KEY_SAVE_LOAD_FILE2,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	189,	530,	24,		KEY_SAVE_LOAD_FILE3,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	218,	530,	24,		KEY_SAVE_LOAD_FILE4,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	247,	530,	24,		KEY_SAVE_LOAD_FILE5,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	276,	530,	24,		KEY_SAVE_LOAD_FILE6,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	306,	530,	24,		KEY_SAVE_LOAD_FILE7,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	335,	530,	24,		KEY_SAVE_LOAD_FILE8,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	364,	530,	24,		KEY_SAVE_LOAD_FILE9,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	393,	530,	24,		KEY_SAVE_LOAD_FILE10,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	422,	530,	24,		KEY_SAVE_LOAD_FILE11,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	451,	530,	24,		KEY_SAVE_LOAD_FILE12,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},
{112,	480,	530,	24,		KEY_SAVE_LOAD_FILE13,		  0,		0,	0,		(SIMPLE_ICON*)ICON_BLANK},


{15,	527,	180,	65,		KEY_SAVE_LOAD_SAVE,				0,		700,	150,		(SIMPLE_ICON*)ICON_BLANK},
{215,	527,	180,	65,		KEY_SAVE_LOAD_LOAD,				0,		700,	238,		(SIMPLE_ICON*)ICON_BLANK},
{416,	527,	180,	65,		KEY_SAVE_LOAD_RENAME,			0,		700,	505,		(SIMPLE_ICON*)ICON_BLANK},
{612,	527,	180,	65,		KEY_SAVE_LOAD_DELETE,			0,		700,	417,		(SIMPLE_ICON*)ICON_BLANK},

{0,		0,		90,		70,		KEY_SAVE_LOAD_CLOSE,			0,		630,	62, 		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}

};









const TOUCH_KEY_SQUARE WindowSystemConfig_key_table[]=
{
//{270,	85,				380,	140,		KEY_SYSTEM_CONFIG_WU_ON,					0,		270,	85,		(SIMPLE_ICON*)ICON_AUTO_ZERO_DING_LIANG_BLACK},
//{420,	85,				530,	140,		KEY_SYSTEM_CONFIG_WU_OFF,					0,		420,	85,		(SIMPLE_ICON*)ICON_TEST_GUANGPU_BLACK},
//{270,	150,			380,	205,		KEY_SYSTEM_CONFIG_D2_ON,					0,		270,	150,	(SIMPLE_ICON*)ICON_PRINT_GUANGPU_BLACK},
//{420,	150,			530,	205,		KEY_SYSTEM_CONFIG_D2_OFF,					0,		420,	150,	(SIMPLE_ICON*)ICON_MULTI_ML_T_A_BLACK},

//{275,	270,			585,	360,		KEY_SYSTEM_CONFIG_WAVE_CORRECT,				0,		470,	546,	(SIMPLE_ICON*)ICON_BLANK},
//{330,	370,			650,	460,		KEY_SYSTEM_CONFIG_CHANGE_LAMP_WAVE,		0,		700,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{20,	530,			190,	590,		KEY_SYSTEM_CONFIG_BLACK_CURRENT,			1,		20,		530,	(SIMPLE_ICON*)ICON_SYSTEM_CONFIG_BALCK_CURRENT_BALCK},
//#if		ENGLISH_ONLY > 0 
//{200,	530,			350,	590,		KEY_SYSTEM_CONFIG_LANGUAGE_CHOOSE,		0,		200,	530,	(SIMPLE_ICON*)ICON_SYSTEM_CONFIG_LANGUAGE_CHOOSE_BALCK},
//#else
//{200,	530,			350,	590,		KEY_SYSTEM_CONFIG_LANGUAGE_CHOOSE,		1,		200,	530,	(SIMPLE_ICON*)ICON_SYSTEM_CONFIG_LANGUAGE_CHOOSE_BALCK},
//#endif
//#if	DEVICE_TYPE == DEVICE_TYPE_L9
//{360,	530,			510,	590,		KEY_SYSTEM_CONFIG_SLIT_CHOOSE,				1,		360,	530,	(SIMPLE_ICON*)ICON_SYSTEM_CONFIG_SLIT_BALCK},
//#endif

//{179,	0,				354,	49,		  KEY_SYSTEM_CONFIG_TO_TIME,					0,		700,	0,		(SIMPLE_ICON*)ICON_BLANK},

//{610,	530,			785,	590,		KEY_SYSTEM_CONFIG_INIT,								1,		610,	530,	(SIMPLE_ICON*)ICON_SYSTEM_CONFIG_INIT_BALCK},
//{700,	0,				792,	100,		KEY_SYSTEM_CONFIG_MAIN_MENU,					0,		700,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{0,		0,				100,	100,		KEY_BACK_DOOR,												0,		700,	0,		(SIMPLE_ICON*)ICON_BLANK},
//{0,		270,			60,		330,		KEY_SYSTEM_CONFIG_UDATE1,					0,		230,	320,		(SIMPLE_ICON*)ICON_BLANK},
//{740,	270,			800,	330,		KEY_SYSTEM_CONFIG_UDATE2,					0,		230,	320,		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};


const TOUCH_KEY_SQUARE WindowTimeConfig_key_table[]=
{
{180,	155,			180+100,	155+50,		KEY_TIME_CONFIG_DAY,					0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},
{390,	155,			390+100,	155+50,		KEY_TIME_CONFIG_MONTH,				0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},
{600,	155,			600+100,	155+50,		KEY_TIME_CONFIG_YEAR,					0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},

{180,	345,			180+100,	345+50,		KEY_TIME_CONFIG_HOUR,					0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},
{390,	345,			390+100,	345+50,		KEY_TIME_CONFIG_MINUTE,				0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},
{600,	345,			600+100,	345+50,		KEY_TIME_CONFIG_SECOND,				0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},

{0,		0,				170,			50,				KEY_TIME_CONFIG_TO_SYS,				0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},
{700,	0,			  792,			100,			KEY_TIME_CONFIG_MAIN_MENU,		0,		0,	0,	(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

const TOUCH_KEY_SQUARE WindowFastInit_key_table[]=
{
	
{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};







//�Ӵ��ڰ�����
const TOUCH_KEY_SQUARE Child_W_34Mode_Select_key_table[]=
{
{500-455,	81-16,		525-455,	106-16,		KEY_34MODE_SELECT_3,			0,		14,		546,		(SIMPLE_ICON*)ICON_BLANK},
{500-455,	124-16,		525-455,	149-16,		KEY_34MODE_SELECT_4,			0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},
{708-455,	16-16,		785-455,	60-16,		KEY_34MODE_SELECT_CLOSE,	0,		263,	546,		(SIMPLE_ICON*)ICON_BLANK},
{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

const TOUCH_KEY_SQUARE Child_W_Set_Slit_key_table[]=
{
{58-23,		355-294,	193-23,	395-294,		KEY_SLIT_05NM,			0,		14,		546,		(SIMPLE_ICON*)ICON_BLANK},
{237-23,	355-294,	372-23,	395-294,		KEY_SLIT_10NM,			0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},
{416-23,	355-294,	551-23,	395-294,		KEY_SLIT_20NM,			0,		263,	546,		(SIMPLE_ICON*)ICON_BLANK},
{58-23,		415-294,	193-23,	455-294,		KEY_SLIT_40NM,			0,		14,		546,		(SIMPLE_ICON*)ICON_BLANK},
{237-23,	415-294,	372-23,	455-294,		KEY_SLIT_50NM,			0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},

{415-23,	453-294,	515-23,	493-294,		KEY_SLIT_CONFIRM,		0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},
{520-23,	453-294,	620-23,	493-294,		KEY_SLIT_CANCLE,		0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,		0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};

const TOUCH_KEY_SQUARE Child_W_Language_key_table[]=
{
{30-14,		165-117,	185-14,	215-117,		KEY_LANGUAGE_CHINESE,		0,		14,		546,		(SIMPLE_ICON*)ICON_BLANK},
{200-14,	165-117,	355-14,	215-117,		KEY_LANGUAGE_ENGLISH,		0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},
{30-14,		225-117,	185-14,	275-117,		KEY_LANGUAGE_SPAIN,			0,		14,		546,		(SIMPLE_ICON*)ICON_BLANK},
{186-14,	225-117,	355-14,	275-117,		KEY_LANGUAGE_PORTUGAL,	0,		139,	546,		(SIMPLE_ICON*)ICON_BLANK},
{290-14,	120-117,	380-14,	160-117,		KEY_LANGUAGE_CLOSE,			0,		263,	546,		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}
};




const TOUCH_KEY_SQUARE Child_MsgBox_key_table[]=
{
//{593-200,	545-260,		715-200,	595-260,		KEY_GUANGPU_BASE_LINE,							0,		593,	545,		(SIMPLE_ICON*)ICON_BASE_LINE_GUANGPU_BLACK},
//{715-200,	170-260,		795-200,	250-260,		KEY_GUANGPU_BASE_LINE,							0,		715,	170,		(SIMPLE_ICON*)ICON_BLANK},


{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}

};


const TOUCH_KEY_SQUARE Child_W_DigitalKeyboard_key_table[]=
{
{557-549,	90-31,			65,	35,		KEY_DIGITAL_1,		0,		513-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_0_BLACK},
{630-549,	90-31,			65,	35,		KEY_DIGITAL_2,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},
{703-549,	90-31,			65,	35,		KEY_DIGITAL_3,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},

{557-549,	132-31,			65,	35,		KEY_DIGITAL_4,		0,		513-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_0_BLACK},
{630-549,	132-31,			65,	35,		KEY_DIGITAL_5,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},
{703-549,	132-31,			65,	35,		KEY_DIGITAL_6,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},

{557-549,	175-31,			65,	35,		KEY_DIGITAL_7,		0,		513-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_0_BLACK},
{630-549,	175-31,			65,	35,		KEY_DIGITAL_8,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},
{703-549,	175-31,			65,	35,		KEY_DIGITAL_9,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},

{557-549,	217-31,			65,	35,		KEY_DIGITAL_POINT,	0,		513-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_0_BLACK},
{630-549,	217-31,			65,	35,		KEY_DIGITAL_0,		0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},
{703-549,	217-31,			65,	35,		KEY_DIGITAL_SIGN,	0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},

{557-549,	259-31,			65,	35,		KEY_DIGITAL_CE,		0,		513-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_0_BLACK},
{630-549,	259-31,			138,35,		KEY_DIGITAL_ENTER,	0,		618-110,	527-55,		(SIMPLE_ICON*)KEY_DIGITAL_POINT_BLACK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}

};


const TOUCH_KEY_SQUARE Child_W_AlphabetKeyboard_key_table[]=
{
{320-30,	76-28,			40,	40,		KEY_ALPHABET_1,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_1_BLACK},
{365-30,	76-28,			40,	40,		KEY_ALPHABET_2,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_2_BLACK},
{412-30,	76-28,			40,	40,		KEY_ALPHABET_3,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_3_BLACK},	

{320-30,	121-28,			40,	40,		KEY_ALPHABET_4,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_4_BLACK},
{365-30,	121-28,			40,	40,		KEY_ALPHABET_5,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_5_BLACK},
{412-30,	121-28,			40,	40,		KEY_ALPHABET_6,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_6_BLACK},

{320-30,	166-28,			40,	40,		KEY_ALPHABET_7,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_7_BLACK},
{365-30,	166-28,			40,	40,		KEY_ALPHABET_8,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_8_BLACK},
{412-30,	166-28,			40,	40,		KEY_ALPHABET_9,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_9_BLACK},

{320-30,	212-28,			40,	40,		KEY_ALPHABET_POINT,		0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_POINT_BLACK},
{365-30,	212-28,			40,	40,		KEY_ALPHABET_0,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_0_BLACK},
{412-30,	212-28,			40,	40,		KEY_DIGITAL_SIGN,		0,		0,	0,		(SIMPLE_ICON*)KEY_DIGITAL_SIGN_BLACK},


{320-30,	258-28,			40,	40,		KEY_ALPHABET_CE,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Q_BLACK},
{365-30,	258-28,			87,	40,		KEY_ALPHABET_ENTER,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_W_BLACK},


{43-30,		76-28,			40,	40,		KEY_ALPHABET_A,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Q_BLACK},
{88-30,		76-28,			40,	40,		KEY_ALPHABET_B,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_W_BLACK},
{134-30,	76-28,			40,	40,		KEY_ALPHABET_C,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_E_BLACK},
{181-30,	76-28,			40,	40,		KEY_ALPHABET_D,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_R_BLACK},
{227-30,	76-28,			40,	40,		KEY_ALPHABET_E,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_T_BLACK},
{273-30,	76-28,			40,	40,		KEY_ALPHABET_F,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Y_BLACK},

{43-30,		121-28,			40,	40,		KEY_ALPHABET_G,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Q_BLACK},
{88-30,		121-28,			40,	40,		KEY_ALPHABET_H,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_W_BLACK},
{134-30,	121-28,			40,	40,		KEY_ALPHABET_I,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_E_BLACK},
{181-30,	121-28,			40,	40,		KEY_ALPHABET_J,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_R_BLACK},
{227-30,	121-28,			40,	40,		KEY_ALPHABET_K,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_T_BLACK},
{273-30,	121-28,			40,	40,		KEY_ALPHABET_L,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Y_BLACK},

{43-30,		167-28,			40,	40,		KEY_ALPHABET_M,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Q_BLACK},
{88-30,		167-28,			40,	40,		KEY_ALPHABET_N,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_W_BLACK},
{134-30,	167-28,			40,	40,		KEY_ALPHABET_O,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_E_BLACK},
{181-30,	167-28,			40,	40,		KEY_ALPHABET_P,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_R_BLACK},
{227-30,	167-28,			40,	40,		KEY_ALPHABET_Q,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_T_BLACK},
{273-30,	167-28,			40,	40,		KEY_ALPHABET_R,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Y_BLACK},

{43-30,		213-28,			40,	40,		KEY_ALPHABET_S,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Q_BLACK},
{88-30,		213-28,			40,	40,		KEY_ALPHABET_T,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_W_BLACK},
{134-30,	213-28,			40,	40,		KEY_ALPHABET_U,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_E_BLACK},
{181-30,	213-28,			40,	40,		KEY_ALPHABET_V,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_R_BLACK},
{227-30,	213-28,			40,	40,		KEY_ALPHABET_W,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_T_BLACK},
{273-30,	213-28,			40,	40,		KEY_ALPHABET_X,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Y_BLACK},

{43-30,		258-28,			40,	40,		KEY_ALPHABET_Y,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Q_BLACK},
{88-30,		258-28,			40,	40,		KEY_ALPHABET_Z,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_W_BLACK},
{134-30,	258-28,			40,	40,		KEY_ALPHABET_CAP,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_E_BLACK},
{181-30,	258-28,			40,	40,		KEY_ALPHABET_SLASH,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_R_BLACK},
{227-30,	258-28,			40,	40,		KEY_ALPHABET_UP,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_T_BLACK},
{273-30,	258-28,			40,	40,		KEY_ALPHABET_DOWN,			0,		0,	0,		(SIMPLE_ICON*)KEY_ALPH_Y_BLACK},






//{675-504,	474-305,			773-504,	572-305,		KEY_DIGITAL_ENTER,	0,		675-504,	474-305,		(SIMPLE_ICON*)KEY_DIGITAL_ENTER_BLACK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}

};






const TOUCH_KEY_SQUARE Child_W_Set_Scale_key_table[]=
{
{168-23,	115-63,			308-23,	154-63,		KEY_SET_SCALE_XSTART,			0,		513-504,	527-305,		(SIMPLE_ICON*)ICON_BLANK},
{168-23,	168-63,			308-23,	207-63,		KEY_SET_SCALE_XEND,				0,		618-504,	527-305,		(SIMPLE_ICON*)ICON_BLANK},
{455-23,	115-63,			594-23,	154-63,		KEY_SET_SCALE_YSTART,			0,		513-504,	474-305,		(SIMPLE_ICON*)ICON_BLANK},
{455-23,	168-63,			594-23,	207-63,		KEY_SET_SCALE_YEND,				0,		565-504,	474-305,		(SIMPLE_ICON*)ICON_BLANK},

{420-23,	222-63,			516-23,	265-63,		KEY_SET_SCALE_CONFIRM,			0,		618-504,	474-305,		(SIMPLE_ICON*)ICON_BLANK},
{520-23,	222-63,			616-23,	265-63,		KEY_SET_SCALE_CANCLE,			0,		513-504,	420-305,		(SIMPLE_ICON*)ICON_BLANK},

{0xffff,0xffff,0xffff,0xffff,0xffff,0,		0,		0,		(SIMPLE_ICON*)ICON_BLANK}

};








#define TOUCH_KEY_EVENT_QUEUE_LENGTH 1U

static osMessageQueueId_t TouchKeyEventQueue;

osStatus_t TouchKeyEventQueue_Init(void)
{
	if (TouchKeyEventQueue != NULL)
	{
		return osOK;
	}

	TouchKeyEventQueue = osMessageQueueNew(
		TOUCH_KEY_EVENT_QUEUE_LENGTH, sizeof(uint8_t), NULL);
	return (TouchKeyEventQueue != NULL) ? osOK : osErrorNoMemory;
}

osStatus_t TouchKeyEventQueue_Get(uint8_t *event, uint32_t timeout)
{
	if ((event == NULL) || (TouchKeyEventQueue == NULL))
	{
		return osErrorParameter;
	}

	return osMessageQueueGet(TouchKeyEventQueue, event, NULL, timeout);
}

static osStatus_t TouchKey_PostEvent(uint8_t event)
{
	osStatus_t status;
	uint8_t pendingEvent;

	if (TouchKeyEventQueue == NULL)
	{
		return osErrorParameter;
	}

	status = osMessageQueuePut(TouchKeyEventQueue, &event, 0U, 0U);
	if (status == osErrorResource)
	{
		/* Keep the newest touch update when input arrives faster than rendering. */
		status = osMessageQueueGet(TouchKeyEventQueue, &pendingEvent,
								   NULL, 0U);
		if ((status != osOK) && (status != osErrorResource))
		{
			return status;
		}
		status = osMessageQueuePut(TouchKeyEventQueue, &event, 0U, 0U);
	}

	return status;
}

void TouchKey_Init(void)
{
	TouchKey.back_keyassignments = 0xffff;
	TouchKey.current_keyassignments = 0xffff;
	TouchKey.Trg = 0;

	TouchKey.Press = 0;
	TouchKey.Release = 0;
	AlphabetKeyboard.NameType = 0;

}


uint8_t Get_TouchKeyAssignments(uint8_t* msg)
{
	TOUCH_POINT pt;
	if (msg == NULL)
	{
		return 1U;
	}

	if(msg[0] == 0xaa)
	{
		if (msg[1] == 0x73)//��������
		{
			pt.x = msg[2]*256+msg[3];
			pt.y = msg[4]*256+msg[5];
			TouchKey.x = pt.x;
			TouchKey.y = pt.y;	
			TouchKey.p = AnalyseKeyCodate(pt);
			
			if(TouchKey.p == 0)
			{
				TouchKey.back_keyassignments = 0xffff;
				TouchKey.current_keyassignments = 0xffff;
				TouchKey.Trg = 0;
				TouchKey.Press = 0;
			}
			else
			{
				TouchKey.current_keyassignments = TouchKey.p->code;
			}
			if(TouchKey.current_keyassignments != 0xffff)
			{
				if(TouchKey.Press == 0)
				{
					TouchKey.back_p = (TOUCH_KEY_SQUARE*)TouchKey.p;
					TouchKey.back_keyassignments = TouchKey.back_p ->code;//���浱ǰ���µİ�����ֻ�е���һ�ΰ������²Ż����?
					BEEP_ONCE();
					if(TouchKey.p->flag == 1)
					{
						if(ColorWm.IsChildFlag == 0)
						{
							ColorLcdDispIcon(
													TouchKey.p->tICON->cut_num,
													TouchKey.p->tICON->left,
													TouchKey.p->tICON->top,
													TouchKey.p->tICON->left + TouchKey.p->tICON->width,
													TouchKey.p->tICON->top + TouchKey.p->tICON->height,
													TouchKey.p->paint_x,
													TouchKey.p->paint_y);
						}
						else if(ColorWm.IsChildFlag == 1)
						{
							ColorLcdDispIcon1(
													TouchKey.p->tICON->cut_num,
													TouchKey.p->tICON->left+ColorWm.ChildWinIcon->left,
													TouchKey.p->tICON->top+ColorWm.ChildWinIcon->top,
													TouchKey.p->tICON->left+ColorWm.ChildWinIcon->left+TouchKey.p->tICON->width,
													TouchKey.p->tICON->top+ColorWm.ChildWinIcon->top+TouchKey.p->tICON->height,
													TouchKey.p->paint_x + ColorWm.ChildWin_Xpos,
													TouchKey.p->paint_y + ColorWm.ChildWin_Ypos);
						}
						else if(ColorWm.IsChildFlag == 2)
						{
							ColorLcdDispIcon1(
													TouchKey.p->tICON->cut_num,
													TouchKey.p->tICON->left+ColorWm.SecondChildWinIcon->left,
													TouchKey.p->tICON->top+ColorWm.SecondChildWinIcon->top,
													TouchKey.p->tICON->left+ColorWm.SecondChildWinIcon->left+TouchKey.p->tICON->width,
													TouchKey.p->tICON->top+ColorWm.SecondChildWinIcon->top+TouchKey.p->tICON->height,
													TouchKey.p->paint_x + ColorWm.SecondChildWin_Xpos,
													TouchKey.p->paint_y + ColorWm.SecondChildWin_Ypos);
						}
						
					}
					else if(TouchKey.p->flag == 2)
					{
						ColorLcdDispIcon(
													TouchKey.p->paint_x,//���水����Ӧ��ͼƬ��
													TouchKey.p->left,
													TouchKey.p->top,
													TouchKey.p->left+TouchKey.p->width,
													TouchKey.p->top+TouchKey.p->height,
													TouchKey.p->left,
													TouchKey.p->top);
					}
					TouchKey.Trg = 1;//�������±�־��λ
					TouchKey.Press = 1;//������ס��־��λ
					TouchKey.Release = 0;//�����ſ���־����
					if (TouchKey_PostEvent(
							(ColorWm.IsChildFlag == 0U) ?
							TOUCH_KEY_EVENT_FATHER :
							((ColorWm.IsChildFlag == 1U) ?
							 TOUCH_KEY_EVENT_CHILD :
							 TOUCH_KEY_EVENT_SECOND_CHILD)) != osOK)
					{
						return 1U;
					}
				}
				else
				{
					TouchKey.Trg = 0;
					TouchKey.Press = 1;
					TouchKey.Release = 0;
					if (TouchKey_PostEvent(
							(ColorWm.IsChildFlag == 0U) ?
							TOUCH_KEY_EVENT_FATHER :
							((ColorWm.IsChildFlag == 1U) ?
							 TOUCH_KEY_EVENT_CHILD :
							 TOUCH_KEY_EVENT_SECOND_CHILD)) != osOK)
					{
						return 1U;
					}
				}
			}
			else
			{
				//TouchKey.back_keyassignments = 0xffff;
				TouchKey.current_keyassignments = 0xffff;
				TouchKey.Trg = 0;
				TouchKey.Press = 0;
			}
				
		}
		else if (msg[1] == 0x72)//�����ſ�
		{
			//pt.x = msg[2]*256+msg[3];
			//pt.y = msg[4]*256+msg[5];
			//TouchKey.p = AnalyseKeyCodate(pt,wm.WindowNumber,wm.instr_ext);
			//TouchKey.p = TouchKey.back_p ;
			if((TouchKey.back_keyassignments != 0xffff) && (TouchKey.back_p->flag >0)) 
			{
				if(ColorWm.IsChildFlag == 0)
				{
					if(TouchKey.back_p->flag == 1)//�а�ѹЧ����
					{
						ColorLcdDispIcon(		ColorWm.FatherPicNumber,
											TouchKey.back_p->paint_x ,
											TouchKey.back_p->paint_y ,
											TouchKey.back_p->paint_x + TouchKey.back_p->tICON->width,
											TouchKey.back_p->paint_y + TouchKey.back_p->tICON->height,
											TouchKey.back_p->paint_x ,
											TouchKey.back_p->paint_y );
					}
					else if(TouchKey.back_p->flag == 2)//�а�ѹЧ������Ҫ�л���һ��״̬
					{
						ColorLcdDispIcon(
													ColorWm.FatherPicNumber,
													TouchKey.p->left,
													TouchKey.p->top,
													TouchKey.p->left+TouchKey.p->width,
													TouchKey.p->top+TouchKey.p->height,
													TouchKey.p->left,
													TouchKey.p->top);
					}
					else if(TouchKey.back_p->flag == 3)//�а�ѹЧ������Ҫ�л���һ��״̬
					{
						ColorLcdDispIcon(
													TouchKey.p->paint_y,//���水����Ӧ��ͼƬ��
													TouchKey.p->left,
													TouchKey.p->top,
													TouchKey.p->left+TouchKey.p->width,
													TouchKey.p->top+TouchKey.p->height,
													TouchKey.p->left,
													TouchKey.p->top);
					}
				}
				else if (ColorWm.IsChildFlag == 1) 
				{
					ColorLcdDispIcon1(		ColorWm.ChildWinIcon->cut_num ,
											TouchKey.back_p->paint_x + ColorWm.ChildWinIcon->left,
											TouchKey.back_p->paint_y + ColorWm.ChildWinIcon->top,
											TouchKey.back_p->paint_x + ColorWm.ChildWinIcon->left+ TouchKey.back_p->tICON->width,
											TouchKey.back_p->paint_y + ColorWm.ChildWinIcon->top + TouchKey.back_p->tICON->height,
											TouchKey.back_p->paint_x + ColorWm.ChildWin_Xpos,
											TouchKey.back_p->paint_y + ColorWm.ChildWin_Ypos);
				}
				else if (ColorWm.IsChildFlag == 2) 
				{
					ColorLcdDispIcon1(		ColorWm.SecondChildWinIcon->cut_num ,
											TouchKey.back_p->paint_x + ColorWm.SecondChildWinIcon->left,
											TouchKey.back_p->paint_y + ColorWm.SecondChildWinIcon->top,
											TouchKey.back_p->paint_x + ColorWm.SecondChildWinIcon->left+ TouchKey.back_p->tICON->width,
											TouchKey.back_p->paint_y + ColorWm.SecondChildWinIcon->top + TouchKey.back_p->tICON->height,
											TouchKey.back_p->paint_x + ColorWm.SecondChildWin_Xpos,
											TouchKey.back_p->paint_y + ColorWm.SecondChildWin_Ypos);
				}
			}
			else 
			{
					TouchKey.back_p->flag = 0;
			}
			TouchKey.current_keyassignments =0XFFFF;
			TouchKey.Trg = 0;
			TouchKey.Press = 0;
			TouchKey.Release = 1;
			if (TouchKey_PostEvent(
					(ColorWm.IsChildFlag == 0U) ?
					TOUCH_KEY_EVENT_FATHER :
					((ColorWm.IsChildFlag == 1U) ?
					 TOUCH_KEY_EVENT_CHILD :
					 TOUCH_KEY_EVENT_SECOND_CHILD)) != osOK)
			{
				return 1U;
			}
			
		}
	}
	return 0U;
}




const TOUCH_KEY_SQUARE* AnalyseKeyCodate(TOUCH_POINT point)
{
	const TOUCH_KEY_SQUARE*  p=0;
	const TOUCH_KEY_SQUARE* ret= 0;
	FATHER_WINDOWS_CONTROL_TABLE* 	p_father_win;
	CHILD_WINDOWS_CONTROL_TABLE* 		p_child_win;

	p_father_win = (FATHER_WINDOWS_CONTROL_TABLE*)FatherWinTbl;
	p_child_win = (CHILD_WINDOWS_CONTROL_TABLE*)ChildWinTbl;


	
	if(	ColorWm.IsChildFlag == 0)
	{

		while(p_father_win->win_num != 0xff)
		{
			if(p_father_win->win_num == ColorWm.FatherWindowNumber)
			{
				p = p_father_win->tks;
				break;
			}
			else
			{
				p_father_win++;
			}
		}

		if(p_father_win->win_num == 0xff )
		{
			return ret;
		}

		while(p->code != 0xffff)
		{
			if((point.x >= p->left) &&(point.x <= p->left+ p->width) &&  (point.y >= p->top) &&(point.y <= p->top+ p->height) )
			{
				ret = p;
				break;
			}
			else
			{
				p++;
			}
				
		}
	}
	else if(	ColorWm.IsChildFlag == 1)
	{

		while(p_child_win->win_num != 0xff)
		{
			if(p_child_win->win_num == ColorWm.ChildWindowNumber)
			{
				p = p_child_win->tks;
				break;
			}
			else
			{
				p_child_win++;
			}
		}

		if(p_child_win->win_num == 0xff )
		{
			return ret;
		}
//=================================
		point.x -= ColorWm.ChildWin_Xpos;
		point.y -= ColorWm.ChildWin_Ypos;
		while(p->code != 0xffff)
		{
			if((point.x >= p->left) &&(point.x <= p->left+ p->width) &&  (point.y >= p->top) &&(point.y <= p->top+ p->height) )
			{
				ret = p;
				break;
			}
			else
			{
				p++;
			}
				
		}
	}
	else if(	ColorWm.IsChildFlag == 2)
	{

		while(p_child_win->win_num != 0xff)
		{
			if(p_child_win->win_num == ColorWm.SecondChildWindowNumber)
			{
				p = p_child_win->tks;
				break;
			}
			else
			{
				p_child_win++;
			}
		}

		if(p_child_win->win_num == 0xff )
		{
			return ret;
		}
//=================================
		point.x -= ColorWm.SecondChildWin_Xpos;
		point.y -= ColorWm.SecondChildWin_Ypos;
		while(p->code != 0xffff)
		{
			if((point.x >= p->left) &&(point.x <= p->left+ p->width) &&  (point.y >= p->top) &&(point.y <= p->top+ p->height) )
			{
				ret = p;
				break;
			}
			else
			{
				p++;
			}
				
		}
	}

	return ret;
}


#endif




