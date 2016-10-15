/** @file server_utils.h
 *
 * @author J. Krauth
 *
 * @par LICENCE
 * @verbatim

   Copyright (C) 2009  ADDI-DATA GmbH for the source code of this module.

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

#ifndef SERVER_UTILS_H_
#define SERVER_UTILS_H_

#include <sys/time.h>
#include <stdio.h>


//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#undef parsing_error_str
#define parsing_error_str(a) "parsing_error_str - undefined"
/**@def __DO_INFO_SYSLOG_
 *
 * When defined, some debug information are printed in the kernel log file.
 */
#ifdef __DO_INFO_SYSLOG_
	/**@def loginfo_info
	 *
	 * A macro function that permit to print messages.
	 * __DO_INFO_SYSLOG_ has to be defined.
	 */
	#define loginfo_info(format, arg...) printf (format "\n" , ## arg)
#else
	/**@def loginfo_info
	 *
	 * The same as before, but is silent.
	 * __DO_INFO_SYSLOG_ has not to be defined.
	 */
	#define loginfo_info(format, arg...) {}
#endif

/**@def __DO_DEBUG_SYSLOG_
 *
 * When defined, some debug information are printed in the kernel log file.
 */
#ifdef __DO_DEBUG_SYSLOG_
	/**@def loginfo_info
	 *
	 * A macro function that permit to print messages.
	 * __DO_DEBUG_SYSLOG_ has to be defined.
	 */
	#define loginfo_debug(format, arg...) printf ("[PID: %i %s: %i] %s: "format "\n" , getpid(), __FILE__ , __LINE__ , __FUNCTION__ , ## arg)
#else
	/**@def loginfo_info
	 *
	 * The same as before, but is silent.
	 * __DO_DEBUG_SYSLOG_ has not to be defined.
	 */
	#define loginfo_debug(format, arg...) {}
#endif

/**@def loginfo_error
 *
 * A macro function that permit to print error messages.
 */
#define loginfo_error(format, arg...) printf ("[PID: %i %s: %i] %s: "format "\n" , getpid(), __FILE__ , __LINE__ , __FUNCTION__ , ## arg)

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#endif /* SERVER_UTILS_H_ */
