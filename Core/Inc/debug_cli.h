#ifndef DEBUG_CLI_H
#define DEBUG_CLI_H

#include <stdint.h>

typedef enum
{
  DEBUG_CLI_OK = 0,
  DEBUG_CLI_ERROR_RTOS,
  DEBUG_CLI_ERROR_UART
} DebugCLI_Status;

extern volatile uint32_t DebugCLI_DroppedRxBytes;
extern volatile uint32_t DebugCLI_UartErrors;
extern volatile DebugCLI_Status DebugCLI_StartResult;

DebugCLI_Status DebugCLI_Start(void);

void DebugCLI_Write(const char *text);


#endif
