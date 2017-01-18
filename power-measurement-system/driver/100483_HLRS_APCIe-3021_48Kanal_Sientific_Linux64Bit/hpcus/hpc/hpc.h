/**
 * hpc.h - some base definitions.
 *
 * This is a free Software.
 * Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * Modifications of the original source code were done by
 * the High Performance Computing Center Stuttgart (HLRS)
 * University of Stuttgart
 * Nobelstraße 19
 * 70569 Stuttgart
 * Germany
 * Bugs to:khabi@hlrs.de
 **/



/** @file hpc.h
 *
 * @brief Contains hpc defines.
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

#include "hpc_config.h"

#ifndef HPC_H_
#define HPC_H_
#define SUPREMUM_WATT 1.0E3 
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#define DRIVER_FIFO_STATUS					0
#define DRIVER_FIFO_SOURCE					0
#define ANALOG_INPUT_WITH_DMA				1
#define ANALOG_INPUT_WITHOUT_DMA			2
#define ANALOG_INPUT_FIFO_EMPTY				0xfffffffe
#define ANALOG_INPUT_FIFO_OVERFLOW			0xffffffff

#define ARRAY_SIZE							(1 + ADDIDATA_MAX_AI + 1 + ADDIDATA_MAX_INTERRUPT_ANALOG_INPUT_VALUE)
#define NUMBER_OF_ANALOG_INPUT_DMA_BUFFER	(1 + ADDIDATA_MAX_AI + 1)
#define ANALOG_INPUT_DMA_BUFFER_SIZE		(1 + ADDIDATA_MAX_AI + 1 + 1)
#define ANALOG_INPUT_DMA_BUFFER_POINTER		(1 + ADDIDATA_MAX_AI + 1 + 2)

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/* Set the maximal number of boards to use */
#define NUMBER_OF_BOARDS	4
#define NUMBER_OF_CHANNELS_ON_BOARD 8
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/**@struct hpc_t
 * A global structure that contains data to share
 * between the main and the interrupt signal.
 */
struct hpc_t
{
	int nBoards;				/**< Number of boards to use (max. NUMBER_OF_BOARDS) */
	void * sharedMem [4];	/**< A pointer on the DMA shared memory */
	int sharedMemSize [4];		/**< Size of the DMA shared memory */
	int *node;					/**< File descriptors on board nodes */
	struct iniConfig_t config;	/**< Configuration (from the INI file) */
};

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/**@struct fileHeader_t
 * Header of the raw binary file.
 */
struct fileHeader_t
{
	uint32_t size;					/**< Size of the data in bytes without this header */
	uint8_t board;					/**< Index of the board (0, 1, 2, 3) */
	struct timespec timestamp;		/**< Timestamp */
	struct iniConfig_t config;		/**< INI configuration informations */
};

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#define APPLICATION_RUN		1
#define APPLICATION_QUIT	0

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/* This a global variable necessary to catch SIGINT (Ctrl + C) signal to close properly the application. */
extern int stop;
extern pthread_mutex_t stop_mutex;

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

#endif /*HPC_H_*/
