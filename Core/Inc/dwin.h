#ifndef DWIN_H
#define DWIN_H

#include <stdint.h>

#define DWIN_MAX_VP_WORDS 125U

typedef enum
{
  DWIN_OK = 0,
  DWIN_ERROR_ARGUMENT,
  DWIN_ERROR_RTOS,
  DWIN_ERROR_UART,
  DWIN_ERROR_TIMEOUT,
  DWIN_ERROR_NOT_INITIALIZED
} DWIN_Status;

typedef struct
{
  uint8_t command;
  uint8_t data_length;
  uint8_t data[255];
  uint8_t has_vp_data;
  uint16_t address;
  uint8_t word_count;
  uint16_t words[125];
} DWIN_Event;

/* The callback runs in the DWIN task; event data is valid only during it. */
typedef void (*DWIN_EventCallback)(const DWIN_Event *event);

extern volatile DWIN_Status DWIN_StartResult;
extern volatile uint32_t DWIN_DroppedRxBytes;
extern volatile uint32_t DWIN_InvalidFrames;
extern volatile uint32_t DWIN_UartErrors;

DWIN_Status DWIN_Init(void);
/* Transfer calls block for DMA completion and must run in task context. */
DWIN_Status DWIN_SendCommand(uint8_t command, const uint8_t *data,
                            uint8_t data_length);
void DWIN_SetEventCallback(DWIN_EventCallback callback);
/* Kept for source compatibility; DWIN communication does not use CRC. */

void DWIN_HandleRxEvent(uint16_t size);
void DWIN_HandleTxComplete(void);
void DWIN_HandleUartError(void);

#endif
