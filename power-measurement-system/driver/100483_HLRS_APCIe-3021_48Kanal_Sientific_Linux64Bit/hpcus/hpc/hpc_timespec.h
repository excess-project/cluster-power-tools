
#ifndef HPC_TIMESPEC_H_
#define HPC_TIMESPEC_H_

#include <unistd.h>
#include <time.h>
#include <stdint.h>

/** Substract two timespec struct.
 *
 * @param[out] result: Result of (x - y).
 * @param[in] x: Timespec struct from which to subtract y.
 * @param[in] y: Timespec struct to subtract from x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_substract (struct timespec *result, struct timespec *x, struct timespec *y);
/** Add two timespec struct.
 *
 * @param[out] result: Result of (x + y).
 * @param[in] x: Timespec struct y.
 * @param[in] y: Timespec struct x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_add (struct timespec *result, struct timespec *x, struct timespec *y);
/** Substract microseconds from timespec struct.
 *
 * @param[out] result: Result of (x - y_micro_sec).
 * @param[in] x: Timespec struct from which to subtract y_micro_sec.
 * @param[in] y_micro_sec: Microseconds to subtract from x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_substract_micro_seconds (struct timespec *result, struct timespec *x, uint32_t y_micro_sec);

/** Add microseconds to timespec struct.
 *
 * @param[out] result: Result of (x + y_micro_sec).
 * @param[in] x: Timespec struct 
 * @param[in] y_micro_sec: Microseconds to add to x.
 *
 * @retval 0: No error.
 * @retval 1: result is negative.
 */
int timespec_add_micro_seconds (struct timespec *result, struct timespec *x, uint32_t y_micro_sec);

#endif /* HPC_TIMESPEC_H_ */
