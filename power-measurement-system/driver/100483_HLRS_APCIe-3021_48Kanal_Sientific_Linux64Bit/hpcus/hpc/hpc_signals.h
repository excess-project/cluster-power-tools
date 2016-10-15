/** @file hpc_signals.h
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

#ifndef SERVER_SIGNALS_H_
#define SERVER_SIGNALS_H_

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void signal_handler(int sig, siginfo_t * info, void * context);

//---------------------------------------------------------------------------------------

/** Set a signal handler to catch defines signals.
 *
 * @param [in] sig_handler: Signal handler.
 *
 * @retval 0 No error.
 * @retval -1 Error.
 */
int hpc_set_signals (void (*sig_handler)(int, siginfo_t *, void *));

//---------------------------------------------------------------------------------------

/** Request an asynchronous SIGIO to catch the interrupt signal.
 *
 * @param [in] fd: File descriptor of the board.
 * @param [in] sig_handler: Signal handler.
 *
 * @retval 0 No error.
 * @retval -1 Error.
 */
int hpc_set_interrupt_signal (int fd, void (*sig_handler)(int));

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#endif /* SERVER_SIGNALS_H_ */
