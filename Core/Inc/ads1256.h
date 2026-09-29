#ifndef ADS1256_H
#define ADS1256_H

#include <stdint.h>

#include "cmsis_os2.h"

#define ADS1256_REFERENCE_VOLTAGE      2.5f
#define ADS1256_DATA_RATE_SPS           100U

typedef enum
{
  ADS1256_OK = 0,
  ADS1256_ERROR_ARGUMENT,
  ADS1256_ERROR_RTOS,
  ADS1256_ERROR_SPI,
  ADS1256_ERROR_TIMEOUT,
  ADS1256_ERROR_CONFIG
} ADS1256_Status;

extern volatile int32_t ADS1256_LastRawSample;
extern volatile float ADS1256_LastVoltage;
extern volatile ADS1256_Status ADS1256_LastStatus;

ADS1256_Status ADS1256_Init(void);
osStatus_t ADS1256_WaitDataReady(uint32_t timeout);
ADS1256_Status ADS1256_ReadData(int32_t *sample);

#endif
