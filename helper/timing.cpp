#include "helper/timing.h"


#include <string>
#include <math.h>
#include <time.h>

#define CLOCKS_PER_MSEC			(CLOCKS_PER_SEC / MSECS_IN_SEC)
#define CLOCKS_PER_USEC			(CLOCKS_PER_SEC / USECS_IN_SEC)


/*
 * All implementations support the system-wide realtime clock, which is identified by CLOCK_REALTIME. 
 * Its time represents seconds and nanoseconds since the Epoch.
 *
 *
 */

static int64_t _sec = 0x00;
static int64_t _nsec = 0x00;

int clock_init(void)
{
	struct timespec t;
	if (clock_gettime(CLOCK_REALTIME_COARSE, &t))
		return -1;

	_sec = t.tv_sec;
	_nsec = t.tv_nsec;

	return 0;
}

static inline uint64_t getCurrentTimeUS(void)
{
	uint64_t sec, nsec;
	struct timespec t;
	if (clock_gettime(CLOCK_REALTIME_COARSE, &t))
		return -1;

	sec = t.tv_sec;
	nsec = t.tv_nsec;

	sec -= _sec;
	nsec -= _nsec;

	return ((sec << 20) + (nsec >> 10));
}

uint64_t millis(void)
{
	int64_t sec, nsec;
	struct timespec t;
	if (clock_gettime(CLOCK_REALTIME_COARSE, &t))
		return -1;

	sec = t.tv_sec;
	nsec = t.tv_nsec;

	sec -= _sec;
	nsec -= _nsec;

	return ((sec * 1000) + (nsec >> 20));
}

uint64_t micros(void)
{
	int64_t sec, nsec;
	struct timespec t;
	if (clock_gettime(CLOCK_REALTIME_COARSE, &t))
		return -1;

	sec = t.tv_sec;
	nsec = t.tv_nsec;

	sec -= _sec;
	nsec -= _nsec;

	return ((sec * (1000 * 1000)) + (nsec / 1000));
}

void delay(uint64_t ms)
{
	delay_ms(ms);
}

void delay_ms(uint64_t ms)
{
	uint64_t _ms = millis();
	while ((millis() - _ms) < ms);
}

void delay_us(uint64_t us)
{
	uint64_t _us = micros();
	while ((micros() - _us) < us);
}


//#define _MAIN_

#ifdef _MAIN_

#include <iostream>

int main()
{
	if (clock_init()) {
		std :: cerr << "Error" << std :: endl;
		return -1;
	}
	// delay(0);
	delay(1000);
	
	while (true) {

		std :: cout << "US: " << micros() << std :: endl;
		std :: cout << "MS: " << millis() << std :: endl;
		delay_us(1000);
	}

	return 0;
}

#endif

