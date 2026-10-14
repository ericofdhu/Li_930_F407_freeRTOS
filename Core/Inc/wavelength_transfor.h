#ifndef __WAVE_LENGTH_H
#define __WAVE_LENGTH_H

#include "ff.h"


typedef struct {
	 int wl;
	 int pos;

}WL_TRANS_TABLE;

/**
 * @brief ����������Ͻ���ṹ��
 * 
 * ��ϵõ��Ķ��κ���Ϊ: y = a*x^2 + b*x + c
 */
typedef struct {
    float a;  // x?��ϵ��
    float b;  // x��ϵ��
    float c;  // ������
} QuadraticFitResult_t;


extern WL_TRANS_TABLE gNew_wl_table[10];

FRESULT wl_table_init(void);
FRESULT wl_table_save(const WL_TRANS_TABLE* new_wl_table);
void wl_table_load(WL_TRANS_TABLE* new_wl_table);





int PositionToWavelength( int pos);
int WavelengthToPosition( int wl);
int wl_quadratic_fit(const WL_TRANS_TABLE* data, int count, QuadraticFitResult_t* result);

#endif


