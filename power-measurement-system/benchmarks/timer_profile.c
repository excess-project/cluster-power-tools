#include "timer_profile.h"
#include <sys/time.h>


//Get time since the 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970 (POSIX time):
//long long* seconds(OUT) - seconds
//long long* nanoseconds(OUT) - nanoseconds
void timer_get_time_stamp(long long* seconds,long long* nanoseconds)
{
    struct timespec time_now;

    clock_gettime(CLOCK_REALTIME,&time_now);
    *seconds=time_now.tv_sec;
    *nanoseconds=time_now.tv_nsec;
}

#ifdef __MIC__
double timer_get_tsc_rate()
{
        /*see Interval Timing for Measuring Performance on...__MIC__*/
        u_int64_t lfreq;
        size_t len = sizeof(lfreq);
        sysctlbyname("kern.timecounter.tc.TSC.frequency",&lfreq,&len,0L,0);
        return lfreq;
}
#endif

unsigned long long timer_read_tsc_register(void) {
#ifndef __MIC__
    unsigned lo, hi;
    __asm__ __volatile__ (
    "        xorl %%eax,%%eax \n"
    "        cpuid"  
    ::: "%rax", "%rbx", "%rcx", "%rdx");
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return (unsigned long long)hi << 32 | lo;
#else
    return __rdtsc();
#endif
}

double timer_get_time(void)
{
#ifndef __MIC__
    struct timespec time_now;
    double result_sec_now;

    clock_gettime(CLOCK_REALTIME,&time_now);
    result_sec_now=(time_now.tv_sec)+(time_now.tv_nsec)*1e-9;
    return result_sec_now;
#else
   return read_tsc_register()/(1050000000.0);
#endif
}


double timer_calcTscCycle(double accuracy,double maximumDuration, int output_on) {

   double timer1,timer2,duration;
   unsigned long long tics1,tics2,tics; 
   int ii;
   timer1=0.0;
   timer2=0.0;
   tics1=0;
   tics2=0;
   for( ii=0; ii<10;ii++)
   {
     tics1=timer_read_tsc_register();
     timer1=timer_get_time();
     usleep((unsigned long long)(1.0e6));
     tics2=timer_read_tsc_register();
     timer2=timer_get_time();
     duration+=timer2-timer1;
     tics+=tics2-tics1;
   }
   duration/= 10.0;
   tics/=10;
   if(output_on>0)
     printf("base_freq: %15.6g;",((double)tics)/duration);  

   return ((double)tics)/duration;
}

