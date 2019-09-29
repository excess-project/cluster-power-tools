#ifndef _timer_profile_h
#define _timer_profile_h

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif

#ifndef __USE_POSIX199309
#define __USE_POSIX199309
#include <time.h>
#undef __USE_POSIX199309
#else
#include <time.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
void timer_get_time_stamp(long long* seconds,long long* nanoseconds);
#ifdef __MIC__
double timer_get_tsc_rate();
#endif
unsigned long long timer_read_tsc_register(void);
double timer_get_time(void);
double timer_calcTscCycle(double accuracy,double maximumDuration, int output_on);
#endif
