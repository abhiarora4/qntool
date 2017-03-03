#ifndef _TIMING_H_
#define _TIMING_H_

#include <stdint.h>

#define MSECS_IN_SEC			(1000)
#define USECS_IN_SEC			(1000*1000)
#define NANOSECS_IN_SEC			(1000*1000*1000)

int clock_init(void);

uint64_t millis(void);
uint64_t micros(void);

void delay(uint64_t ms);
void delay_ms(uint64_t ms);
void delay_us(uint64_t us);

#endif