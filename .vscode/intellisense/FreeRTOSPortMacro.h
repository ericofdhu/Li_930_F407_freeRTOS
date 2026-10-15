#ifndef FREERTOS_INTELLISENSE_PORTMACRO_H
#define FREERTOS_INTELLISENSE_PORTMACRO_H

/* IntelliSense-only stubs; the Keil project continues to use the real port. */
#include <stdint.h>

typedef uint32_t StackType_t;
typedef long BaseType_t;
typedef unsigned long UBaseType_t;
typedef uint32_t TickType_t;

#define portMAX_DELAY ((TickType_t)0xffffffffUL)
#define portTICK_TYPE_IS_ATOMIC 1
#define portSTACK_GROWTH (-1)
#define portTICK_PERIOD_MS ((TickType_t)1000 / configTICK_RATE_HZ)
#define portBYTE_ALIGNMENT 8

#define portYIELD() ((void)0)
#define portEND_SWITCHING_ISR(xSwitchRequired) ((void)(xSwitchRequired))
#define portYIELD_FROM_ISR(x) portEND_SWITCHING_ISR(x)
#define portDISABLE_INTERRUPTS() ((void)0)
#define portENABLE_INTERRUPTS() ((void)0)
#define portENTER_CRITICAL() ((void)0)
#define portEXIT_CRITICAL() ((void)0)
#define portSET_INTERRUPT_MASK_FROM_ISR() 0U
#define portCLEAR_INTERRUPT_MASK_FROM_ISR(x) ((void)(x))
#define portTASK_FUNCTION_PROTO(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portTASK_FUNCTION(vFunction, pvParameters) void vFunction(void *pvParameters)
#define portNOP()
#define portINLINE inline
#define portFORCE_INLINE inline

#ifndef __align
#define __align(n) __attribute__((aligned(n)))
#endif

#endif
