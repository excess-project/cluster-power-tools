/** @file hpcus.c
 *
 * @brief This is the main code of the High Performance Computing application.
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

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <xpci3xxx.h>

#include "hpc.h"
#include "hpc_config.h"

#include "hpc_utils.h"
#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

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
int hpc_initialize_jitter_measure (int fd)
{
	uint32_t arg[4];

	arg[0] = 0;		// Timer 0
	arg[1] = 2;		// Mode 2
	arg[2] = 1;		// Unit us
	arg[3] = 10000;	// Timer could count up to 10 ms

	if (call_ioctl (fd, CMD_xpci3xxx_InitTimer ,arg))
		goto error;

	arg[1] = 0;		// Disable interrupt

	if (call_ioctl (fd, CMD_xpci3xxx_EnableDisableTimerInterrupt ,arg))
		goto error;

	arg[1] = 1;		// Enable hardware trigger
	arg[2] = 1;		// Trigger on low level

	if (call_ioctl (fd, CMD_xpci3xxx_EnableDisableTimerHardwareTrigger ,arg))
		goto error;

	if (call_ioctl (fd, CMD_xpci3xxx_StartTimer ,arg))
		goto error;

	return 0;

error:

	call_ioctl (fd, CMD_xpci3xxx_StopTimer ,arg);
	/* Commented because we don't want to reload the FPGA here */
	// call_ioctl (fd, CMD_xpci3xxx_ReleaseTimer ,arg);

	return 1;
}

//------------------------------------------------------------------------------

/** Release the jitter measure.
 *
 * @param [in] fd: File descriptor of the board.
 *
 * @retval 0: No error.
 */
int hpc_release_jitter_measure (int fd)
{
	uint32_t arg[4];

	arg[0] = 0;		// Timer 0

	//call_ioctl (fd, CMD_xpci3xxx_StopTimer ,arg);
	//call_ioctl (fd, CMD_xpci3xxx_ReleaseTimer ,arg);

	return 0;
}

//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

