
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include "hpc_timespec.h"

/** Substract two timespec struct.
 *
 * @param[out] result: Result of (x - y).
 * @param[in] x: Timespec struct from which to subtract y.
 * @param[in] y: Timespec struct to subtract from x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_substract (struct timespec *result, struct timespec *x, struct timespec *y)
{
	/* Check if x <= y */
	if ((x->tv_sec < y->tv_sec) || ((x->tv_sec == y->tv_sec) && (x->tv_nsec <= y->tv_nsec)))
	{
		result->tv_sec = 0;
		result->tv_nsec = 0;

		return 1;
	}
	else
	{
		/* When x > y */
		result->tv_sec = (x->tv_sec - y->tv_sec);

		if (x->tv_nsec < y->tv_nsec)
		{
			result->tv_nsec = (x->tv_nsec + 1000000000L - y->tv_nsec);
			result->tv_sec--;
		}
		else
		{
			result->tv_nsec = x->tv_nsec - y->tv_nsec;
		}
	}

    return 0;
}
/** Add two timespec struct.
 *
 * @param[out] result: Result of (x + y).
 * @param[in] x: Timespec struct y.
 * @param[in] y: Timespec struct x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_add (struct timespec *result, struct timespec *x, struct timespec *y)
{

    result->tv_nsec = (x->tv_nsec + y->tv_nsec);
    result->tv_sec = (x->tv_sec + y->tv_sec);
    if(result->tv_nsec >= 1000000000L)
    {
        result->tv_sec++;
        result->tv_nsec -= 1000000000L;
    }
    return 0;
}
/** Substract microseconds from timespec struct.
 *
 * @param[out] result: Result of (x - y_micro_sec).
 * @param[in] x: Timespec struct from which to subtract y_micro_sec.
 * @param[in] y_micro_sec: Microseconds to subtract from x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_substract_micro_seconds (struct timespec *result, struct timespec *x, uint32_t y_micro_sec)
{
	uint32_t seconds;
    uint32_t nanoseconds;
    struct timespec y;
    int ret = 0;
    
    seconds = (y_micro_sec/1.0e06);
    nanoseconds = (y_micro_sec*1.0e03) - seconds*(10.e09);
   // printf("seconds:%i; nanoseconds:%i\n",seconds,nanoseconds);
    y.tv_sec = seconds;
    y.tv_nsec = nanoseconds;
	
    ret = timespec_substract (result, x, &y);

    return ret;
}

/** Add microseconds to timespec struct.
 *
 * @param[out] result: Result of (x + y_micro_sec).
 * @param[in] x: Timespec struct 
 * @param[in] y_micro_sec: Microseconds to add to x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_add_micro_seconds (struct timespec *result, struct timespec *x, uint32_t y_micro_sec)
{
	uint32_t seconds;
    uint32_t nanoseconds;
    struct timespec y;
    int ret = 0;
    
    seconds = (y_micro_sec/1.0e06);
    nanoseconds = (y_micro_sec*1.0e03) - seconds*(10.e09);

    y.tv_sec = seconds;
    y.tv_nsec = nanoseconds;
	
    ret = timespec_add (result, x, &y);

    return ret;
}
