#ifndef SYSTEM_RUNTIME_H
#define SYSTEM_RUNTIME_H

#include <stdint.h>

typedef struct
{
  volatile uint8_t En;
  volatile uint32_t cnt;
} BeepCtrl_t;

extern volatile BeepCtrl_t Beep;

void SysRun_SetBeep(uint32_t ticks);
void SysRun_GetBeep(uint8_t *enabled, uint32_t *ticks);

#endif
