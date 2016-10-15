/** @file hpc_threads.h
 *
 *
 * @author J. Krauth
 *
 * @par LICENCE
 * @verbatim

   Copyright (C) 2014  ADDI-DATA GmbH for the source code of this module.

   ADDI-DATA GmbH
   Airpark Business Center
   Airport Boulevard B210
   77836 Rheinmuenster
   Germany
   Tel: +49(0)7229/1847-0
   Fax: +49(0)7229/1847-200
   http://www.addi-data-com
   info@addi-data.com

 * @endverbatim
 *
 */

#ifndef HPC_JITTER_H_
#define HPC_JITTER_H_

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/** Initialize the jitter measure.
 *
 * Timer 0 is used to measure the jitter time in microseconds
 * between the hardware interrupt and the call of the software
 * interrupt routine. The jitter time is used to improve the
 * time-stamp measure accuracy.
 *
 * @param [in] fd: File descriptor of the board.
 *
 * @retval 0: No error.
 * @retval != 0: Error
 */
int hpc_initialize_jitter_measure (int fd);

//---------------------------------------------------------------------------------------

/** Release the jitter measure.
 *
 * @param [in] fd: File descriptor of the board.
 *
 * @retval 0: No error.
 */
int hpc_release_jitter_measure (int fd);

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#endif /* HPC_JITTER_H_ */
