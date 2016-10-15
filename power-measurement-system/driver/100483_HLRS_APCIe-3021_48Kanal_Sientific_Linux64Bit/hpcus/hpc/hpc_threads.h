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

#ifndef SERVER_THREADS_H_
#define SERVER_THREADS_H_

#include "hpc.h"

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

struct threadParams_t
{
	int board;
	struct hpc_t hpc;
};

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

void *data_management (void *args);

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#endif /* SERVER_THREADS_H_ */
