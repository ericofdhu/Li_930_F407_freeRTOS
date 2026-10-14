
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TMC_5130_H
#define __TMC_5130_H

//#include "stm32f10x.h"

#define TMC5130_NORMAL_MOVE_SPEED				3000
#define TMC5130_HOMING_MOVE_SPEED				500


#define TMC5130_MAX_MOVE_SPEED				5000
#define TMC5130_MID_MOVE_SPEED				100
#define TMC5130_MIN_MOVE_SPEED				50




/* TMC5130�Ĵ�����ַ���� */
#define TMC5130_GCONF        0x00
#define TMC5130_GSTAT        0x01
#define TMC5130_IFCNT        0x02
#define TMC5130_SLAVECONF    0x03
#define TMC5130_IOIN         0x04
#define TMC5130_X_COMPARE    0x05
#define TMC5130_IHOLD_IRUN   0x10
#define TMC5130_TPOWERDOWN   0x11
#define TMC5130_TPWMTHRS	   0x13
#define TMC5130_PWMCONF		   0x70

#define TMC5130_XACTUAL      0x21
#define TMC5130_VACTUAL      0x22
#define TMC5130_VSTART	     0x23
#define TMC5130_A1           0x24
#define TMC5130_V1           0x25
#define TMC5130_AMAX         0x26
#define TMC5130_VMAX         0x27
#define TMC5130_DMAX         0x28
#define TMC5130_D1           0x2A
#define TMC5130_VSTOP        0x2B
#define TMC5130_TZEROWAIT    0x2C
#define TMC5130_XTARGET      0x2D
#define TMC5130_RAMPMODE     0x20
#define TMC5130_CHOPCONF     0x6C
#define TMC5130_COOLCONF     0x6D
#define TMC5130_DRV_STATUS   0x6F
#define TMC5130_SW_MODE      0x34

/* SW_MODE�Ĵ���λ���� - �ο��������� */
#define SW_MODE_STOP_L_ENABLE    (1UL << 0)   /* ��ο�����ֹͣʹ�� */
#define SW_MODE_STOP_R_ENABLE    (1UL << 1)   /* �Ҳο�����ֹͣʹ�� */
#define SW_MODE_POL_STOP_L       (1UL << 2)   /* ��ο����ؼ��� */
#define SW_MODE_POL_STOP_R       (1UL << 3)   /* �Ҳο����ؼ��� */
#define SW_MODE_SWAP_LR          (1UL << 4)   /* ���Ҳο����ؽ��� */
#define SW_MODE_LATCH_L_ACT      (1UL << 5)   /* ������ο����ض��� */
#define SW_MODE_LATCH_R_ACT      (1UL << 6)   /* �����Ҳο����ض��� */
#define SW_MODE_LATCH_L_INACT    (1UL << 7)   /* ������ο����طǶ��� */
#define SW_MODE_LATCH_R_INACT    (1UL << 8)   /* �����Ҳο����طǶ��� */
#define SW_MODE_EN_SOFTSTOP      (1UL << 9)   /* ʹ����ֹͣ */
#define SW_MODE_EN_SOFTSTART     (1UL << 10)  /* ʹ�������� */

/* �ο��������ú� */
#define REFL_ENABLE              SW_MODE_STOP_L_ENABLE
#define REFR_ENABLE              SW_MODE_STOP_R_ENABLE
#define REFL_DISABLE             0
#define REFR_DISABLE             0
#define REFL_POL_NORMAL          0             /* �������� */
#define REFL_POL_INVERTED        SW_MODE_POL_STOP_L  /* ��ת���� */
#define REFR_POL_NORMAL          0
#define REFR_POL_INVERTED        SW_MODE_POL_STOP_R


/* �˶�ģʽ���� */
#define RAMP_MODE_POSITION   0
#define RAMP_MODE_VELOCITY   1
#define RAMP_MODE_HOLD       2

/* �˶�״̬���� */
#define STATUS_IDLE          		0
#define STATUS_ACCELERATING  		1
#define STATUS_CRUISING     		2
#define STATUS_DECELERATING  		3
#define STATUS_TARGET_REACHED 	4
#define STATUS_REACH_MAX_POS		5

/* �������� */
unsigned char TMC_5130_Init(void);
uint8_t TMC5130_WriteRegister(uint8_t address, int32_t data);
uint32_t TMC5130_ReadRegister(uint8_t address);
uint32_t TMC5130_ReadRegister_stat(uint8_t address,uint8_t* stat);

void TMC5130_SetCS(uint8_t state);
void TMC5130_SetEN(uint8_t state);

/* �ο��������ú��� */
void TMC5130_ConfigReferenceSwitches(uint8_t refL_enable, uint8_t refR_enable, uint8_t refL_polarity, uint8_t refR_polarity);
void TMC5130_EnableRefL(void);
void TMC5130_DisableRefL(void);
void TMC5130_EnableRefR(void);
void TMC5130_DisableRefR(void);
uint8_t TMC5130_GetRefLStatus(void);
uint8_t TMC5130_GetRefRStatus(void);

/* ���Ժ��� */
void TMC5130_SelfTest(void);
void TMC5130_TestCommunication(void);
void TMC5130_TestMotion(uint32_t test_distance);
void TMC5130_TestReferenceSwitches(void);
void TMC5130_PrintStatus(void);
void TMC5130_TestCurrentControl(void);
void TMC5130_TestComplete(void);

/* �˶����ƺ��� */

void TMC5130_Homing(int32_t vmax);

void TMC5130_Set_Zero_Pos(void);

void TMC5130_MoveToPosition(int32_t target_position);
void TMC5130_MoveToPositionWithSpeed(int32_t target_position, uint32_t speed);
void TMC5130_SetRampMode(uint8_t mode);
void TMC5130_Stop(void);
void TMC5130_EmergencyStop(void);
int32_t TMC5130_GetActualPosition(void);
uint8_t TMC5130_GetMotionStatus(void);
void TMC5130_WaitForMotionComplete(void);
void TMC5130_SetMaxSpeed(uint32_t max_speed);
void TMC5130_SetAcceleration(uint32_t acceleration);
void TMC5130_SetDeceleration(uint32_t deceleration);


void TMC5130_Set_Scan_para(uint32_t speed);


#endif


