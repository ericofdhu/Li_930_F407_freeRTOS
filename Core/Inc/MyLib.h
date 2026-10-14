#ifndef __MY_LIB_H
#define __MY_LIB_H

#include <stdint.h>







void _delay_us(int32_t n);


uint8_t Bcd2Hex(uint8_t bcd);
uint8_t Hex2Bcd(uint8_t hex);   

int32_t EnterData_ASCIItoHEX(uint8_t *pASCII,int8_t type);
void SortMinToMax(uint32_t* p,uint16_t num);
void GetEverageDataT(uint32_t dat1,uint32_t dat2,uint32_t* ret1,uint32_t* ret2);
void FindMaxMin(uint32_t* p,uint8_t num,uint32_t* retMax,uint32_t* retMin); 




#endif
