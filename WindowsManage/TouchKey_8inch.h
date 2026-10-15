#ifndef __TOUCH_KEY_8_INCH_H
#define __TOUCH_KEY_8_INCH_H


#include <stdint.h>
#include "cmsis_os2.h"
#include "icon_8inch.h"





#define KEY_NULL									0xffff





#define KEY_YES		 										0XAC00
#define KEY_NO		 										0XAC01
#define KEY_CHINESE		 									0XAC02
#define KEY_ENGLISH		 									0XAC03
	




#define KEY_WELCOME_DIS_CONNECT 							0X0001
#define KEY_BACK_DOOR													0X0101
#define KEY_WARM_UP_YES												0x0102
#define KEY_WARM_UP_NO												0x0103
#define KEY_WARM_UP_DEBUG												0x0104


//���˵�����
#define KEY_MAIN_MENU_GUANG_PU									0x0300
#define KEY_MAIN_MENU_DING_LIANG							0x0301
#define KEY_MAIN_MENU_SYSTEM_CONFIG						0x0302
#define KEY_MAIN_MENU_HELP										0x0303
#define KEY_MAIN_MENU_DONG_LI_XUE							0x0304

//��Ȳ���������
#define KEY_GUANGDU_T_ABS		 							0X0501
#define KEY_GUANGDU_CELL_LEFT		 						0X0502
#define KEY_GUANGDU_CELL_RIGHT		 						0X0503
#define KEY_GUANGDU_CELL_TO_R			 					0X0504
#define KEY_GUANGDU_AUTO_ZERO		 						0X0505
#define KEY_GUANGDU_TEST				 					0X0506
#define KEY_GUANGDU_PRINT				 					0X0507
#define KEY_GUANGDU_TO_GUANGPU		 						0X0508
#define KEY_GUANGDU_TO_DINGLIANG		 					0X0509
#define KEY_GUANGDU_TO_DONGLIXUE		 					0X050A
#define KEY_GUANGDU_DOWN_ARROR		 						0X050B
#define KEY_GUANGDU_SET_WAVE		 						0X050C
#define KEY_GUANGDU_DATA			 						0X050D
#define KEY_GUANGDU_DISP_MODE								0X050E
#define KEY_GUANGDU_MAIN_MENU								0X050F
#define KEY_GUANGDU_SET_SLIT								0X0510


//���ײ���������
#define KEY_GUANGPU_SET_PARA	 							0X0701
#define KEY_GUANGPU_FIND_PEAK			 					0X0702
#define KEY_GUANGPU_DISP_MONTAIN_VALLY				 		0X0703
#define KEY_GUANGPU_TEST	 								0X0704
#define KEY_GUANGPU_SAVE_LOAD		 						0X0705
#define KEY_GUANGPU_PRINT	 								0X0706
#define KEY_GUANGPU_MAIN_MENU		 						0X0707
#define KEY_GUANGPU_BASE_LINE		 						0X0708
#define KEY_GUANGPU_CUR_CURSOR	 							0X0709
#define KEY_GUANGPU_CURSOR_LEFT	 							0X070A
#define KEY_GUANGPU_CURSOR_RIGHT	 						0X070B
#define KEY_GUANGPU_CLEAR	 								0X070C
#define KEY_GUANGPU_M1_M2	 								0X070E
#define KEY_GUANGPU_SET_SCALE	 							0X070F
#define KEY_GUANGPU_TO_CORRECT_WAVE	 						0X0710
#define KEY_GUANGPU_SENS_SENSITIVE_LEVEL		 			0X0711





//���ײ�������
#define KEY_SET_GUPU_DATA_START					 			0X0901
#define KEY_SET_GUPU_DATA_END				 					0X0902
#define KEY_SET_GUPU_WL_START					 				0X0903
#define KEY_SET_GUPU_WL_END					 					0X0904
#define KEY_SET_GUPU_SPEED_SLOW					 			0X0905
#define KEY_SET_GUPU_SPEED_MIDDLE					 		0X0906
#define KEY_SET_GUPU_SPEED_FAST					 			0X0907
#define KEY_SET_GUPU_INTERVAL_01					 		0X0908
#define KEY_SET_GUPU_INTERVAL_10					 		0X0909

#define KEY_SET_GUPU_EMIT_LIGHT_1				 			0X090A
#define KEY_SET_GUPU_EMIT_LIGHT_2				 			0X090B
#define KEY_SET_GUPU_EMIT_LIGHT_3				 			0X090C
#define KEY_SET_GUPU_EMIT_LIGHT_4				 			0X090D

#define KEY_SET_GUPU_GAIN_1				 						0X090E
#define KEY_SET_GUPU_GAIN_2				 						0X090F
#define KEY_SET_GUPU_GAIN_4				 						0X0910
#define KEY_SET_GUPU_GAIN_8				 						0X0911

#define KEY_SET_GUPU_PMT_ON				 						0X0912
#define KEY_SET_GUPU_PMT_OFF				 					0X0913
#define KEY_SET_GUPU_PMT_VOLTAGE				 			0X0914

#define KEY_SET_GUPU_CONFIRM					 			0X0915
#define KEY_SET_GUPU_CANCLE					 				0X0916


//��ȼ��
#define KEY_MONTAIN_VALLEY_CLOSE		 					0X0c01
#define KEY_MONTAIN_VALLEY_SENSITIVE1						0X0c02
#define KEY_MONTAIN_VALLEY_SENSITIVE2						0X0c03
#define KEY_MONTAIN_VALLEY_SENSITIVE3						0X0c04
#define KEY_MONTAIN_VALLEY_SENSITIVE4						0X0c05
#define KEY_MONTAIN_VALLEY_SENSITIVE5						0X0c06
#define KEY_MONTAIN_VALLEY_UP								0X0c07
#define KEY_MONTAIN_VALLEY_DOWN								0X0c08
#define KEY_MONTAIN_VALLEY_PRINT							0X0c09


//��������Kϵ����   ���Խ���
#define KEY_DING_LIANG_KTEST_AUTO_ZERO						0X0e00
#define KEY_DING_LIANG_KTEST_TEST							0X0e01
#define KEY_DING_LIANG_KTEST_SAVELOAD						0X0e02
#define KEY_DING_LIANG_KTEST_PRINT							0X0e03
#define KEY_DING_LIANG_KTEST_SET_PAR						0X0e04
#define KEY_DING_LIANG_KTEST_SET_SLIT						0X0e05
#define KEY_DING_LIANG_KTEST_MAIN_MENU						0X0e06

#define KEY_DING_LIANG_KTEST_LAST_PAGE						0X0e07
#define KEY_DING_LIANG_KTEST_NEXT_PAGE						0X0e08
#define KEY_DING_LIANG_KTEST_TO_1P							0X0e09
#define KEY_DING_LIANG_KTEST_TO_MP							0X0e0A
#define KEY_DING_LIANG_KTEST_CLR_DATA						0X0e0B



//��������Kϵ����   �������ý���
#define KEY_DING_LIANG_KPAR_SET_WAVE						0X0f00
#define KEY_DING_LIANG_KPAR_UNIT_NONE						0X0f01
#define KEY_DING_LIANG_KPAR_UNIT_PERCENT					0X0f02
#define KEY_DING_LIANG_KPAR_UNIT_PPB						0X0f03
#define KEY_DING_LIANG_KPAR_UNIT_ML							0X0f04
#define KEY_DING_LIANG_KPAR_UNIT_GL							0X0f05
#define KEY_DING_LIANG_KPAR_UNIT_MGML						0X0f06
#define KEY_DING_LIANG_KPAR_UNIT_UGML						0X0f07
#define KEY_DING_LIANG_KPAR_UNIT_NGML						0X0f08
#define KEY_DING_LIANG_KPAR_SET_K							0X0f09
#define KEY_DING_LIANG_KPAR_SET_B							0X0f0A
#define KEY_DING_LIANG_KPAR_CONFIRM							0X0f0B
#define KEY_DING_LIANG_KPAR_CANCLE							0X0f0C
#define KEY_DING_LIANG_KPAR_AUTO_ZERO						0X0f0d


//��������1Pϵ����   ���Խ���
#define KEY_DING_LIANG_1PTEST_AUTO_ZERO									0X1200
#define KEY_DING_LIANG_1PTEST_TEST										0X1201
#define KEY_DING_LIANG_1PTEST_SAVELOAD									0X1202
#define KEY_DING_LIANG_1PTEST_PRINT										0X1203
#define KEY_DING_LIANG_1PTEST_SET_PAR									0X1204
#define KEY_DING_LIANG_1PTEST_SET_SLIT									0X1205
#define KEY_DING_LIANG_1PTEST_MAIN_MENU									0X1206
#define KEY_DING_LIANG_1PTEST_LAST_PAGE									0X1207
#define KEY_DING_LIANG_1PTEST_NEXT_PAGE									0X1208
#define KEY_DING_LIANG_1PTEST_TO_K										0X1209
#define KEY_DING_LIANG_1PTEST_TO_MP										0X120A
#define KEY_DING_LIANG_1PTEST_CLR_DATA									0X120B



//��������1Pϵ����   �������ý���
#define KEY_DING_LIANG_1PPAR_SET_WAVE									0X1300
#define KEY_DING_LIANG_1PPAR_UNIT_NONE									0X1301
#define KEY_DING_LIANG_1PPAR_UNIT_PERCENT								0X1302
#define KEY_DING_LIANG_1PPAR_UNIT_PPB									0X1303
#define KEY_DING_LIANG_1PPAR_UNIT_ML									0X1304
#define KEY_DING_LIANG_1PPAR_UNIT_GL									0X1305
#define KEY_DING_LIANG_1PPAR_UNIT_MGML									0X1306
#define KEY_DING_LIANG_1PPAR_UNIT_UGML									0X1307
#define KEY_DING_LIANG_1PPAR_UNIT_NGML									0X1308
#define KEY_DING_LIANG_1PPAR_SET_C										0X1309
#define KEY_DING_LIANG_1PPAR_BIAO_DING									0X130A
#define KEY_DING_LIANG_1PPAR_CONFIRM									0X130B
#define KEY_DING_LIANG_1PPAR_CANCLE										0X130C
#define KEY_DING_LIANG_1PPAR_AUTO_ZERO									0X130D


//��������ϵ����   ���Խ���
#define KEY_DING_LIANG_CESHI_TEST										0X1400
#define KEY_DING_LIANG_CESHI_CLEAR										0X1401
#define KEY_DING_LIANG_CESHI_SET_PAR									0X1402
#define KEY_DING_LIANG_CESHI_SAVELOAD									0X1403
#define KEY_DING_LIANG_CESHI_CALIB										0X1404
#define KEY_DING_LIANG_CESHI_MAIN_MENU									0X1405
#define KEY_DING_LIANG_CESHI_LAST_PAGE									0X1406
#define KEY_DING_LIANG_CESHI_NEXT_PAGE									0X1407
#define KEY_DING_LIANG_CESHI_SET_WAVE									0X1408
#define KEY_DING_LIANG_CESHI_READ_BASE									0X1409
#define KEY_DING_LIANG_CESHI_EN_BASE									0X140a





//��������MPϵ����   �������ý���
#define KEY_DING_LIANG_MPPAR_SET_WAVE									0X1500
#define KEY_DING_LIANG_MPPAR_UNIT_NONE									0X1501
#define KEY_DING_LIANG_MPPAR_UNIT_PERCENT								0X1502
#define KEY_DING_LIANG_MPPAR_UNIT_PPB									0X1503
#define KEY_DING_LIANG_MPPAR_UNIT_ML									0X1504
#define KEY_DING_LIANG_MPPAR_UNIT_GL									0X1505
#define KEY_DING_LIANG_MPPAR_UNIT_MGML									0X1506
#define KEY_DING_LIANG_MPPAR_UNIT_UGML									0X1507
#define KEY_DING_LIANG_MPPAR_UNIT_NGML									0X1508
#define KEY_DING_LIANG_MPPAR_BIAO_DING									0X1509
#define KEY_DING_LIANG_MPPAR_CONFIRM									0X150a
#define KEY_DING_LIANG_MPPAR_CANCLE										0X150b


//��������MPϵ����   �궨
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_1								0X1600
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_2								0X1601
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_3								0X1602
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_4								0X1603
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_5								0X1604
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_6								0X1605
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_7								0X1606
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_8								0X1607
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_9								0X1608
#define KEY_DING_LIANG_CESHI_BIAODING_SEL_10							0X1609


#define KEY_DING_LIANG_CESHI_BIAODING_DATA_1								0X1610
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_2								0X1611
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_3								0X1612
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_4								0X1613
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_5								0X1614
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_6								0X1615
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_7								0X1616
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_8								0X1617
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_9								0X1618
#define KEY_DING_LIANG_CESHI_BIAODING_DATA_10								0X1619

#define KEY_DING_LIANG_CESHI_BIAODING_READ_BASE							0X1620
#define KEY_DING_LIANG_CESHI_BIAODING_SET_WAVE							0X1621
#define KEY_DING_LIANG_CESHI_BIAODING_EN_BASE								0X1622


#define KEY_DING_LIANG_CESHI_BIAODING_TEST									0X1630
#define KEY_DING_LIANG_CESHI_BIAODING_SAVE_LOAD								0X1631
#define KEY_DING_LIANG_CESHI_BIAODING_CLR_DATA								0X1632
#define KEY_DING_LIANG_CESHI_BIAODING_CREATE_CURVE							0X1633
#define KEY_DING_LIANG_CESHI_BIAODING_DISP_CURVE							0X1634
#define KEY_DING_LIANG_CESHI_BIAODING_BACK									0X1635



#define KEY_DING_LIANG_CESHI_BIAODING_AUTO_ZERO							0X1632
#define KEY_DING_LIANG_CESHI_BIAODING_PRINT								0X1637
//��������MPϵ����   ����
// #define KEY_DING_LIANG_MP_CURVE_DISP_DATA								0X1701
// #define KEY_DING_LIANG_MP_CURVE_FINISH									0X1702
// #define KEY_DING_LIANG_MP_CURVE_SAVE_DATA								0X1703
// #define KEY_DING_LIANG_MP_CURVE_PRINT										0X1704
#define KEY_DING_LIANG_MP_CURVE_BACK										0X1705



//����Ȳ���
#define KEY_SNR_CESHI_SET_PARA	 										0X1800
#define KEY_SNR_CESHI_SAVE_LOAD		 									0X1801
#define KEY_SNR_CESHI_CLEAR			 									0X1802
#define KEY_SNR_CESHI_TEST	 											0X1803
#define KEY_SNR_CESHI_MAIN_MENU		 									0X1804
#define KEY_SNR_CESHI_CUR_CURSOR		 								0X1805
#define KEY_SNR_CESHI_CURSOR_LEFT		 								0X1806
#define KEY_SNR_CESHI_CURSOR_RIGHT		 								0X1807





//����ѧ���� ���
#define KEY_DONG_LI_XUE_CESHI_SET_PARA	 										0X1800
#define KEY_DONG_LI_XUE_CESHI_SAVE_LOAD		 									0X1801
#define KEY_DONG_LI_XUE_CESHI_CLEAR			 									0X1802
#define KEY_DONG_LI_XUE_CESHI_TEST	 											0X1803
#define KEY_DONG_LI_XUE_CESHI_MAIN_MENU		 									0X1804
#define KEY_DONG_LI_XUE_CESHI_CUR_CURSOR		 								0X1805
#define KEY_DONG_LI_XUE_CESHI_CURSOR_LEFT		 								0X1806
#define KEY_DONG_LI_XUE_CESHI_CURSOR_RIGHT		 								0X1807

//����ѧ���� ��������
#define KEY_DONG_LI_XUE_PAR_SET_WAVE												0X1900
#define KEY_DONG_LI_XUE_PAR_SET_TIME												0X1901
#define KEY_DONG_LI_XUE_PAR_UNIT_MINUTE												0X1902
#define KEY_DONG_LI_XUE_PAR_UNIT_SECOND												0X1903
#define KEY_DONG_LI_XUE_PAR_SCAN_INTERVAL											0X1904
#define KEY_DONG_LI_XUE_PAR_DATA_START												0X1905
#define KEY_DONG_LI_XUE_PAR_DATA_END												0X1906
#define KEY_DONG_LI_XUE_PAR_CONFIRM													0X1907
#define KEY_DONG_LI_XUE_PAR_CANCLE													0X1908
#define KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_1				 							0X1909
#define KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_2				 							0X190A
#define KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_3				 							0X190B
#define KEY_DONG_LI_XUE_PAR_EMIT_LIGHT_4				 							0X190C

#define KEY_DONG_LI_XUE_PAR_GAIN_1				 									0X190D
#define KEY_DONG_LI_XUE_PAR_GAIN_2				 									0X190E
#define KEY_DONG_LI_XUE_PAR_GAIN_4				 									0X190F
#define KEY_DONG_LI_XUE_PAR_GAIN_8				 									0X1910

#define KEY_DONG_LI_XUE_PAR_PMT_ON				 									0X0911
#define KEY_DONG_LI_XUE_PAR_PMT_OFF				 									0X0912
#define KEY_DONG_LI_XUE_PAR_PMT_VOLTAGE				 								0X0913




#define KEY_WAVE_CORRECT_STEPS_1												0X1A02
#define KEY_WAVE_CORRECT_STEPS_2												0X1A03
#define KEY_WAVE_CORRECT_STEPS_3												0X1A04
#define KEY_WAVE_CORRECT_STEPS_4												0X1A05
#define KEY_WAVE_CORRECT_STEPS_5												0X1A06
#define KEY_WAVE_CORRECT_STEPS_6												0X1A07
#define KEY_WAVE_CORRECT_STEPS_7												0X1A08
#define KEY_WAVE_CORRECT_STEPS_8												0X1A09
#define KEY_WAVE_CORRECT_STEPS_9												0X1A0A
#define KEY_WAVE_CORRECT_STEPS_10												0X1A0B

#define KEY_WAVE_CORRECT_REAL_WAVE_1											0X1A0C
#define KEY_WAVE_CORRECT_REAL_WAVE_2											0X1A0D
#define KEY_WAVE_CORRECT_REAL_WAVE_3											0X1A0E
#define KEY_WAVE_CORRECT_REAL_WAVE_4											0X1A0F
#define KEY_WAVE_CORRECT_REAL_WAVE_5											0X1A10
#define KEY_WAVE_CORRECT_REAL_WAVE_6											0X1A11
#define KEY_WAVE_CORRECT_REAL_WAVE_7											0X1A12
#define KEY_WAVE_CORRECT_REAL_WAVE_8											0X1A13
#define KEY_WAVE_CORRECT_REAL_WAVE_9											0X1A14
#define KEY_WAVE_CORRECT_REAL_WAVE_10											0X1A15

#define KEY_WAVE_CORRECT_CONFIRM												0X1A16
#define KEY_WAVE_CORRECT_SAVE													0X1A17
#define KEY_WAVE_CORRECT_CALC													0X1A18
#define KEY_WAVE_CORRECT_LOAD_DEFAULT											0X1A19


#define KEY_SN_SCAN_WAVE_CUR_CURSOR												0X1B00
#define KEY_SN_SCAN_TIME_CUR_CURSOR												0X1B01
#define KEY_SN_SCAN_CURSOR_LEFT													0X1B02
#define KEY_SN_SCAN_CURSOR_RIGHT												0X1B03
#define KEY_SN_SCAN_START														0X1B04



//�洢
#define KEY_SAVE_LOAD_CLOSE												0Xa800
#define KEY_SAVE_LOAD_SAVE												0Xa801
#define KEY_SAVE_LOAD_LOAD												0Xa802
#define KEY_SAVE_LOAD_TO_UDISK										0Xa823
#define KEY_SAVE_LOAD_DELETE											0Xa803
#define KEY_SAVE_LOAD_RENAME											0Xa804
#define KEY_SAVE_LOAD_FILE0												0Xa805
#define KEY_SAVE_LOAD_FILE1												0Xa806
#define KEY_SAVE_LOAD_FILE2												0Xa807
#define KEY_SAVE_LOAD_FILE3												0Xa808
#define KEY_SAVE_LOAD_FILE4												0Xa809
#define KEY_SAVE_LOAD_FILE5												0Xa80A
#define KEY_SAVE_LOAD_FILE6												0Xa80B
#define KEY_SAVE_LOAD_FILE7												0Xa80C
#define KEY_SAVE_LOAD_FILE8												0Xa80D
#define KEY_SAVE_LOAD_FILE9												0Xa80E
#define KEY_SAVE_LOAD_FILE10											0Xa80F
#define KEY_SAVE_LOAD_FILE11											0Xa810
#define KEY_SAVE_LOAD_FILE12											0Xa811
#define KEY_SAVE_LOAD_FILE13											0Xa812
#define KEY_SAVE_LOAD_FILE14											0Xa813
#define KEY_SAVE_LOAD_FILE15											0Xa814
#define KEY_SAVE_LOAD_FILE16											0Xa815
#define KEY_SAVE_LOAD_FILE17											0Xa816
#define KEY_SAVE_LOAD_FILE18											0Xa817
#define KEY_SAVE_LOAD_FILE19											0Xa818
#define KEY_SAVE_LOAD_FILE20											0Xa819
#define KEY_SAVE_LOAD_FILE21											0Xa81A
#define KEY_SAVE_LOAD_FILE22											0Xa81B
#define KEY_SAVE_LOAD_FILE23											0Xa81C
#define KEY_SAVE_LOAD_FILE24											0Xa81D
#define KEY_SAVE_LOAD_FILE25											0Xa81E
#define KEY_SAVE_LOAD_FILE26											0Xa81F
#define KEY_SAVE_LOAD_FILE27											0Xa820
#define KEY_SAVE_LOAD_FILE28											0Xa821
#define KEY_SAVE_LOAD_FILE29											0Xa822



#define KEY_MULTI_WL_TEST_AUTO_ZERO														0X1B00
#define KEY_MULTI_WL_TEST_TEST															0X1B01
#define KEY_MULTI_WL_TEST_PRINT															0X1B02
#define KEY_MULTI_WL_TEST_T_ABS															0X1B03
#define KEY_MULTI_WL_TEST_LAST_PAGE														0X1B04
#define KEY_MULTI_WL_TEST_NEXT_PAGE														0X1B05
#define KEY_MULTI_WL_TEST_SET_SLIT														0X1B06
#define KEY_MULTI_WL_TEST_MAIN_MENU														0X1B07
#define KEY_MULTI_WL_TEST_WL_1															0X1B08
#define KEY_MULTI_WL_TEST_WL_2															0X1B09
#define KEY_MULTI_WL_TEST_WL_3															0X1B0A
#define KEY_MULTI_WL_TEST_WL_4															0X1B0B
#define KEY_MULTI_WL_TEST_WL_5															0X1B0C
#define KEY_MULTI_WL_TEST_CLR_DATA														0X1B0D


#define KEY_SYSTEM_CONFIG_D2_ON															0x1C00
#define KEY_SYSTEM_CONFIG_D2_OFF														0x1C01
#define KEY_SYSTEM_CONFIG_WU_ON															0x1C02
#define KEY_SYSTEM_CONFIG_WU_OFF														0x1C03
#define KEY_SYSTEM_CONFIG_WAVE_CORRECT											0x1C04
#define KEY_SYSTEM_CONFIG_CHANGE_LAMP_WAVE									0x1C05
#define KEY_SYSTEM_CONFIG_BLACK_CURRENT											0x1C06
#define KEY_SYSTEM_CONFIG_LANGUAGE_CHOOSE										0x1C07
#define KEY_SYSTEM_CONFIG_INIT															0x1C08
#define KEY_SYSTEM_CONFIG_MAIN_MENU													0x1c09
#define KEY_SYSTEM_CONFIG_SLIT_CHOOSE												0x1c0A
#define KEY_SYSTEM_CONFIG_TO_TIME												    0x1c0B
#define KEY_SYSTEM_CONFIG_UDATE1														0x1c0c
#define KEY_SYSTEM_CONFIG_UDATE2														0x1c0d


#define KEY_TIME_CONFIG_DAY																	0x1D00
#define KEY_TIME_CONFIG_MONTH																0x1D01
#define KEY_TIME_CONFIG_YEAR																0x1D02
#define KEY_TIME_CONFIG_HOUR																0x1D03
#define KEY_TIME_CONFIG_MINUTE															0x1D04
#define KEY_TIME_CONFIG_SECOND															0x1D05
#define KEY_TIME_CONFIG_TO_SYS															0x1D06
#define KEY_TIME_CONFIG_MAIN_MENU														0x1D07

//////////////////////////////////////////////////////////////////////////////////////////////
//�Ӵ��ڽ���


//////////////////////////////////////////////////////////////////////////////////////////////
//����3,4λС������
#define KEY_34MODE_SELECT_3		 												0Xa000
#define KEY_34MODE_SELECT_4		 												0Xa001
#define KEY_34MODE_SELECT_CLOSE													0Xa002



//�������Խ���
#define KEY_LANGUAGE_CHINESE	 												0Xa900
#define KEY_LANGUAGE_ENGLISH		 											0Xa901
#define KEY_LANGUAGE_SPAIN		 												0Xa902
#define KEY_LANGUAGE_PORTUGAL		 											0Xa903
#define KEY_LANGUAGE_CLOSE														0Xa904


//�����������
#define KEY_SLIT_05NM					 										0Xa500
#define KEY_SLIT_10NM		 													0Xa501
#define KEY_SLIT_20NM															0Xa502
#define KEY_SLIT_40NM															0Xa503
#define KEY_SLIT_50NM															0Xa504
#define KEY_SLIT_CONFIRM													0Xa505
#define KEY_SLIT_CANCLE														0Xa506

//���ױ������
#define KEY_SET_SCALE_XSTART					 								0Xa300
#define KEY_SET_SCALE_XEND					 									0Xa301
#define KEY_SET_SCALE_YSTART					 								0Xa302
#define KEY_SET_SCALE_YEND					 									0Xa303
#define KEY_SET_SCALE_CONFIRM					 								0Xa304
#define KEY_SET_SCALE_CANCLE					 								0Xa305


//���ּ���
#define KEY_DIGITAL_0					 										0Xa200
#define KEY_DIGITAL_1					 										0Xa201
#define KEY_DIGITAL_2					 										0Xa202
#define KEY_DIGITAL_3					 										0Xa203
#define KEY_DIGITAL_4					 										0Xa204
#define KEY_DIGITAL_5					 										0Xa205
#define KEY_DIGITAL_6					 										0Xa206
#define KEY_DIGITAL_7					 										0Xa207
#define KEY_DIGITAL_8					 										0Xa208
#define KEY_DIGITAL_9					 										0Xa209
#define KEY_DIGITAL_CE					 										0Xa20A
#define KEY_DIGITAL_SIGN					 									0Xa20B
#define KEY_DIGITAL_ENTER					 									0Xa20C
#define KEY_DIGITAL_POINT					 									0Xa20D



//��ĸ����
#define KEY_ALPHABET_0					 										0Xa700
#define KEY_ALPHABET_1					 										0Xa701
#define KEY_ALPHABET_2					 										0Xa702
#define KEY_ALPHABET_3					 										0Xa703
#define KEY_ALPHABET_4					 										0Xa704
#define KEY_ALPHABET_5					 										0Xa705
#define KEY_ALPHABET_6					 										0Xa706
#define KEY_ALPHABET_7					 										0Xa707
#define KEY_ALPHABET_8					 										0Xa708
#define KEY_ALPHABET_9					 										0Xa709

#define KEY_ALPHABET_A					 										0Xa70A
#define KEY_ALPHABET_B					 										0Xa70B
#define KEY_ALPHABET_C					 										0Xa70C
#define KEY_ALPHABET_D					 										0Xa70D
#define KEY_ALPHABET_E					 										0Xa70E
#define KEY_ALPHABET_F					 										0Xa70F
#define KEY_ALPHABET_G					 										0Xa710
#define KEY_ALPHABET_H					 										0Xa711
#define KEY_ALPHABET_I					 										0Xa712
#define KEY_ALPHABET_J					 										0Xa713
#define KEY_ALPHABET_K					 										0Xa714
#define KEY_ALPHABET_L					 										0Xa715
#define KEY_ALPHABET_M					 										0Xa716
#define KEY_ALPHABET_N					 										0Xa717
#define KEY_ALPHABET_O					 										0Xa718
#define KEY_ALPHABET_P					 										0Xa719
#define KEY_ALPHABET_Q					 										0Xa71A
#define KEY_ALPHABET_R					 										0Xa71B
#define KEY_ALPHABET_S					 										0Xa71C
#define KEY_ALPHABET_T					 										0Xa71D
#define KEY_ALPHABET_U					 										0Xa71E
#define KEY_ALPHABET_V					 										0Xa71F
#define KEY_ALPHABET_W					 										0Xa720
#define KEY_ALPHABET_X					 										0Xa721
#define KEY_ALPHABET_Y					 										0Xa722
#define KEY_ALPHABET_Z					 										0Xa723

#define KEY_ALPHABET_POINT					 									0Xa724
#define KEY_ALPHABET_CE					 										0Xa725
#define KEY_ALPHABET_ENTER					 									0Xa726
#define KEY_ALPHABET_SPACE					 									0Xa727
#define KEY_ALPHABET_DOWN_LINE					 								0Xa728
#define KEY_ALPHABET_CLOSE							 							0Xa729
#define KEY_ALPHABET_UP							 								0Xa72A
#define KEY_ALPHABET_DOWN							 							0Xa72B
#define KEY_ALPHABET_EN_CN							 							0Xa72C
#define KEY_ALPHABET_CAP							 							0Xa72D
#define KEY_ALPHABET_SLASH							 							0Xa72E








typedef struct {
	uint16_t  left;     
	uint16_t   top;
	uint16_t   width;
	uint16_t   height;
	uint16_t code;
	uint16_t flag;
	uint16_t paint_x;
	uint16_t paint_y;
	SIMPLE_ICON*   tICON;
}TOUCH_KEY_SQUARE;



typedef struct {
	uint8_t win_num;
	TOUCH_KEY_SQUARE* tks;

}WIN_KEY;



typedef struct {
  uint8_t Str[15];
	uint8_t len;
	float KeyboardMax;
	float KeyboardMin;
	uint8_t* pRet;
	uint8_t RetDataType;
	uint8_t RetStatus;
	

}_DigitalKeyboard;

typedef struct {
	uint8_t Str[31];//�����ַ���
	uint8_t len;
	uint8_t MaxLen;
	
	uint8_t NameType;//���Ļ���Ӣ��
	uint8_t CapsType;//��д����Сд
	
	uint8_t EnStr[20];//����Ӣ��ʱ����ַ�������
	uint8_t Enlen;//����Ӣ�ĳ���

	uint8_t PinYinStr[10];
	uint8_t PinYinlen;
	uint8_t HZ_num;
	uint8_t DispPage;
	uint8_t DispBuff[55];//1.�� 2.�� 3.��........һ����ʾ10������
	uint8_t* HZ_String;
}_AlphabetKeyboard;




typedef struct {
	uint16_t   back_keyassignments;
	uint16_t   current_keyassignments;
	uint16_t x;
	uint16_t y;
	uint8_t 	Trg;
	uint8_t 	Press;
	uint8_t 	Release;
	uint32_t 	Cont;
	const TOUCH_KEY_SQUARE*  p;
	TOUCH_KEY_SQUARE*  back_p;


}_TOUCH_KEY;





typedef struct _TOUCH_POINT
{
	int16_t x;
	int16_t y;
}TOUCH_POINT;

extern TOUCH_POINT KeyBoardPosition;



extern _TOUCH_KEY TouchKey;	
extern _DigitalKeyboard		DigitalKeyboard;
extern _AlphabetKeyboard		AlphabetKeyboard;



// extern OS_EVENT      *TouchKeyFrameReceiveMbox;
// extern uint8        TouchKeyFrameReceiveMboxBuff[50]; 
// extern OS_EVENT      *TouchKeySem;
// extern OS_EVENT      *ChildTouchKey;
// extern OS_EVENT      *SecondChildTouchKey;

//TOUCH_KEY_8_INCH_EXT  uint8 TouchKeyQueenBuf[4][30];
//�����ڰ�����
extern const TOUCH_KEY_SQUARE WindowWelcom_key_table[];
extern const TOUCH_KEY_SQUARE WindowWarmUp_key_table[];

extern const TOUCH_KEY_SQUARE WindowInit_key_table[];
extern const TOUCH_KEY_SQUARE WindowMainMenu_key_table[];
extern const TOUCH_KEY_SQUARE WindowGuangDu_key_table[];

extern const TOUCH_KEY_SQUARE WindowGuangPu_key_table[];
extern const TOUCH_KEY_SQUARE WindowGuangPuPar_key_table[];
extern const TOUCH_KEY_SQUARE WindowMontainValley_key_table[]; 

extern const TOUCH_KEY_SQUARE WindowDingLiangCeShi_key_table[];
extern const TOUCH_KEY_SQUARE WindowDingLiangCeShiBiaoDing_key_table[];

extern const TOUCH_KEY_SQUARE WindowDingLiangMPPar_key_table[];
extern const TOUCH_KEY_SQUARE WindowDingLiangMPCurve_key_table[];

extern const TOUCH_KEY_SQUARE WindowDongLiXueCeshi_key_table[];
extern const TOUCH_KEY_SQUARE WindowDongLiXuePar_key_table[];
extern const TOUCH_KEY_SQUARE WindowSaveLoad_key_table[];


extern const TOUCH_KEY_SQUARE WindowWaveCorrect_key_table[];
extern const TOUCH_KEY_SQUARE WindowSNRCeShi_key_table[];



extern const TOUCH_KEY_SQUARE WindowMultiWlTest_key_table[];
extern const TOUCH_KEY_SQUARE WindowSystemConfig_key_table[];
extern const TOUCH_KEY_SQUARE WindowTimeConfig_key_table[];
extern const TOUCH_KEY_SQUARE WindowFastInit_key_table[];



//�Ӵ��ڰ�����
extern const TOUCH_KEY_SQUARE Child_W_34Mode_Select_key_table[];
extern const TOUCH_KEY_SQUARE Child_W_Set_Slit_key_table[];
extern const TOUCH_KEY_SQUARE Child_W_DigitalKeyboard_key_table[];
extern const TOUCH_KEY_SQUARE Child_W_AlphabetKeyboard_key_table[];
extern const TOUCH_KEY_SQUARE Child_W_Language_key_table[];


extern const TOUCH_KEY_SQUARE Child_W_Set_Scale_key_table[];
//extern const TOUCH_KEY_SQUARE Child_W_Set_GuPu_Par_key_table[];
extern const TOUCH_KEY_SQUARE Child_MsgBox_key_table[];


void TouchKey_Init(void);
uint8_t Get_TouchKeyAssignments(uint8_t	* msg);
osStatus_t TouchKeyEventQueue_Init(void);
osStatus_t TouchKeyEventQueue_Get(uint8_t eventType, uint8_t *event, uint32_t timeout);
const TOUCH_KEY_SQUARE* AnalyseKeyCodate(TOUCH_POINT point);

#define TOUCH_KEY_EVENT_FATHER       0U
#define TOUCH_KEY_EVENT_CHILD        1U
#define TOUCH_KEY_EVENT_SECOND_CHILD 2U

#endif




