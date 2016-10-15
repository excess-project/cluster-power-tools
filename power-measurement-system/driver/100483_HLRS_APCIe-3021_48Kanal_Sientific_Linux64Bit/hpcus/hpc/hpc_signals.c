/** @file hpc_signals.c
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

#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <syslog.h>
#include <string.h>
#include <fcntl.h>

#include "hpc.h"
#include "hpc_signals.h"
#include "hpc_utils.h"

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/** Signal handler.
 *
 */
void signal_handler(int sig, siginfo_t * info, void * context)
{
	switch (sig)
	{
		case SIGSEGV:
			/* For debugging: Critical error e.g.: segmentation fault */
			loginfo_info ("Signal = %d, subcode = %d, pid = %d, uid = %d, address = %p, errno = %d", sig, info->si_code, info->si_pid, info->si_uid, info->si_addr, info->si_errno);
			exit (1);
		break;

		case SIGCHLD:
		break;

		case SIGPIPE:
			loginfo_info ("SIGPIPE: Stop (PID %i).", info->si_pid);
		break;

		case SIGQUIT:
		break;

		case SIGTERM:
		break;

		case SIGALRM:		// Alarm
		case SIGINT:		// Ctrl + C
			loginfo_info ("Please wait the application will quit... (PID %i).", info->si_pid);
			stop = APPLICATION_QUIT;
		break;

		case SIGUSR1:
		break;
	}
}

//---------------------------------------------------------------------------------------

/** Set a signal handler to catch defines signals.
 *
 * @param [in] sig_handler: Signal handler.
 *
 * @retval 0 No error.
 * @retval -1 Error.
 */
int hpc_set_signals (void (*sig_handler)(int, siginfo_t *, void *))
{
	struct sigaction act;

	/* Set a handler "sig_handler" on some signals */
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	act.sa_sigaction = sig_handler;

	if (sigaction(SIGUSR1, &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGQUIT, &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGSEGV, &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGCHLD, &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGTERM, &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGINT , &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGPIPE , &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	if (sigaction(SIGALRM , &act, NULL))
	{
		loginfo_error ("sigaction: %s", strerror(errno));
		return -1;
	}

	return 0;
}

//---------------------------------------------------------------------------------------

/** Request an asynchronous SIGIO to catch the interrupt signal.
 *
 * @param [in] fd: File descriptor of the board.
 * @param [in] sig_handler: Signal handler.
 *
 * @retval 0 No error.
 * @retval -1 Error.
 */
int hpc_set_interrupt_signal (int fd, void (*sig_handler)(int))
{
	int oflags = fcntl (fd, F_GETFL);

	if (signal (SIGIO, sig_handler) == SIG_ERR)
	{
		loginfo_error ("Error by setting the interrupt signal (signal): %s\n", strerror(errno));
		return -1;
	}

	if (fcntl (fd, F_SETOWN, getpid()) == -1)
	{
		loginfo_error ("Error by setting the interrupt signal (fcntl): %s\n", strerror(errno));
		return -1;
	}

	if (fcntl (fd, F_SETFL, oflags | FASYNC) == -1)
	{
		loginfo_error ("Error by setting the interrupt signal (fcntl): %s\n", strerror(errno));
		return -1;
	}

	return 0;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

