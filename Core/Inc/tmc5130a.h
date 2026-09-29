#ifndef TMC5130A_H
#define TMC5130A_H

#include <stdint.h>

/* Tune these values for the motor, sense resistor, and mechanism. */
#define TMC5130A_IRUN                  8U
#define TMC5130A_IHOLD                 4U
#define TMC5130A_HOME_VMAX             500000U
#define TMC5130A_HOME_AMAX             10000U
#define TMC5130A_HOME_MAX_STEPS        20000L
#define TMC5130A_HOME_TIMEOUT_MS       60000U

typedef enum
{
  TMC5130A_HOME_NOT_STARTED = 0,
  TMC5130A_HOME_SUCCESS,
  TMC5130A_HOME_INIT_ERROR,
  TMC5130A_HOME_SPI_ERROR,
  TMC5130A_HOME_TRAVEL_LIMIT,
  TMC5130A_HOME_TIMEOUT
} TMC5130A_HomeResult;

extern volatile TMC5130A_HomeResult TMC5130A_HomingResult;

uint8_t TMC5130A_Init(void);
TMC5130A_HomeResult TMC5130A_FindHome(void);

#endif
