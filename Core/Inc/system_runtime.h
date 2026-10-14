#ifndef SYSTEM_RUNTIME_H
#define SYSTEM_RUNTIME_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "event_groups.h"
typedef struct
{
  volatile uint8_t En;
  volatile uint32_t cnt;
} BeepCtrl_t;

extern volatile BeepCtrl_t Beep;

typedef struct _SYS_FLAG
{
	uint32_t MoveFlag				:1;
	uint32_t MoveDirFlag			:1;
	uint32_t ReadFreqFlag 		:1;
	uint32_t RunWaveScanFlag		:1;
	uint32_t RunBaseScanFlag		:1;
	uint32_t EmergercyStopFlag    :1;
	uint32_t OnePointMethodGetKFlag	:1;
	uint32_t MultiPointMehtodGetBiaoDing	:1;
	uint32_t RunTimeScanFlag			:1;
	uint32_t ZeroSetStep			:3;
	uint32_t KeyDebugFlag			:1;
	uint32_t ScanFileAvaliable			:1;
	uint32_t DuobochangZeroFlag		:1;
	uint32_t DuobochangTestFlag		:1;
	uint32_t PcConnectFlag				:1;
	uint32_t GetBlackCurrent			:1;
	uint32_t DeviceInitFlag				:2;
	uint32_t InitByPcFlag					:1;
	uint32_t StopInitFlag					:1;
	uint32_t PcCmdSolveFlag				:1;
	uint32_t RunSnrScanFlag				:1;
	
	uint32_t ReserveFlag				:8; 
}SYS_FLAG;

extern volatile SYS_FLAG SysFlag;


//ϵͳ����ȣ�͸����?
typedef struct {
	float data_A;
	float data_T;
	int16_t data_E;

}_SYS_ABS;
extern volatile _SYS_ABS MySysAbs ;

typedef struct {

	uint16_t Montain[30];
	uint16_t M_sum;
	uint16_t Vally[30];
	uint16_t V_sum;	
	uint16_t sensitive_level;		
}_MVStruct;
extern volatile _MVStruct MVStruct ;


//�޸ı�����ݽṹ
typedef struct {
		uint32_t left;
		uint32_t right;
		float   top;
		float   bottom;
}_SetScaleStruct;
extern volatile _SetScaleStruct SetScale;







extern EventGroupHandle_t MyInitFlag;
extern EventGroupHandle_t MyOsFlag;
extern QueueHandle_t g_ActionQueue;




typedef enum {FALSE = 0, TRUE = !FALSE} bool;

void SysRun_SetBeep(uint32_t ticks);
void SysRun_GetBeep(uint8_t *enabled, uint32_t *ticks);
EventGroupHandle_t MyInitFlag_Init(void);
EventGroupHandle_t MyOsFlag_Init(void);

extern int32_t volatile CurWaveLen; //��ǰ�����Ĳ���

#define BEEP_ONCE() SysRun_SetBeep(100)

//======================��ʼ����־λ���� (EventBits)==================
#define INIT_WL_GOOD_FLAG              (1UL << 0)
#define INIT_WL_ERR_FLAG               (1UL << 1)
#define INIT_WL_500_GOOD_FLAG          (1UL << 2)
#define INIT_WL_500_ERR_FLAG           (1UL << 3)
#define INIT_ALL_GOOD_FLAG             (1UL << 4)
#define SYS_MOVE_WL_FLAG                (1UL << 5)
#define SYS_SCAN_WL_FINISH_FLAG         (1UL << 6)




#define Unit_10nm						100
#define Unit_5nm						50
#define Unit_2nm						20
#define Unit_1nm						10
#define Unit_0_5nm						5
#define Unit_0_1nm						1




#define DATA_TYPE_E					3
#define DATA_TYPE_A					4
#define DATA_TYPE_T					3
#define DATA_TYPE_WAVELEN		1
#define DATA_TYPE_TIME			5
#define DATA_TYPE_NORMAL		6




//������Χ
#define  MAX_WAVELEN			8000ul  //��󲨳�??
#define  MIN_WAVELEN			2000ul  //��С����  
#define  WAVE_SCAN_LINE_BUF_SIZE				(MAX_WAVELEN- MIN_WAVELEN)
#define  TIME_SCAN_LINE_BUF_SIZE				3600




//ɨ���ٶ�
#define SCAN_SPEED_FAST		    2
#define SCAN_SPEED_MIDDLE	    1
#define SCAN_SPEED_SLOW		    0

// ������?��
#define     EMIT_LIGHT_1		0
#define     EMIT_LIGHT_2		1
#define     EMIT_LIGHT_3		2
#define     EMIT_LIGHT_4		3


// PMT����
#define     PMT_GAIN_1		0
#define     PMT_GAIN_2		1
#define     PMT_GAIN_4		2
#define     PMT_GAIN_8		3

// PMT����
#define     PMT_OFF		0
#define     PMT_ON		1


#define MAX_ENTER_DATA_LEN				8
#define MAX_K_METHOD_POINT				30
#define MAX_ONE_POINT_METHOD_POINT		30
#define MAX_MULTI_POINT_METHOD_POINT	30
#define MAX_MULTI_POINT_BIAO_DING_NUM	10





#endif
