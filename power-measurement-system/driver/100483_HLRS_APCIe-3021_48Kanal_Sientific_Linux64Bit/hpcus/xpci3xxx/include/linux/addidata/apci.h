 /* @file apci.h
  * 
  * @brief Define common public utilities used in driver's public headers. 
  * 
  *
  */

/** @par LICENCE
 * 
 *  @verbatim
  Copyright (C) 2004,2005 ADDI-DATA GmbH for the source code of this module.
  
         ADDI-DATA GmbH
         Dieselstrasse 3
         D-77833 Ottersweier
         Tel: +19(0)7223/9493-0
         Fax: +49(0)7223/9493-92
         http://www.addi-data-com
         info@addi-data.com
 
 This library is free software; you can redistribute it and/or modify it under 
 the terms of the GNU Lesser General Public License as published by the 
 Free Software Foundation; either version 2.1 of the License, 
 or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
 or FITNESS FOR A PARTICULAR PURPOSE. 
 See the GNU Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public License 
 along with this library; if not, write to the Free Software Foundation, 
 Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 
 
 You also shoud find the complete LGPL in the LGPL.txt file accompanying 
 this source code.
 
 @endverbatim
**/

#ifndef __ADDI__APCI__H_
#define __ADDI__APCI__H_

#ifdef __KERNEL__
	#include <linux/types.h>
#else
	#include <inttypes.h>
	#include <sys/types.h>
#endif

// set this define to remove definition of legacy DOS types
#ifndef _ADDI_NO_WIN32_COMPAT

#define VOID           void
#define INT            int32_t
#define UINT           uint32_t
#define SHORT          int16_t
#define USHORT         uint16_t
#define CHAR           int8_t
#define BYTE           uint8_t
#define WORD           uint16_t
#define LONG           int32_t
#define ULONG          uint32_t
#define DWORD          uint32_t

#define PVOID          VOID *
#define PINT           INT *
#define PUINT          UINT *
#define PSHORT         SHORT *
#define PUSHORT        USHORT *
#define PCHAR          CHAR *
#define PBYTE          BYTE *
#define PWORD          WORD *
#define PLONG          LONG *
#define PULONG         ULONG *
#define PDWORD         PULONG

#define BYTE_          BYTE
#define WORD_          WORD
#define DWORD_         DWORD

#define PBYTE_         PBYTE
#define PWORD_         PWORD
#define PDWORD_        PDWORD


#define _INT_          INT
#define _VOID_         VOID

#define DWORD64        struct timeval
#define HANDLE         INT
#define PDWORD64       DWORD64 *
#define DWORD64_       DWORD64
#define PDWORD64_      PDWORD64

#endif //  ADDI_NO_WIN32_COMPAT

// #define LOBYTE(__val)      (BYTE)   ((__val) & 0xFF)
/** Returns the lower 8 bits of a 16 bits unsigned integer.
* 
* @arg  __val A 16 bits number
* @return The lower 8 bit
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint8_t LOBYTE( uint16_t __val)
{
	return (__val) & 0xFF;
}
//#define HIBYTE(__val)      (BYTE)   
/** Returns the upper 8 bits of a 16 bits unsigned integer.
* 
* @arg  __val A 16 bits number
* @return The upper 8 bit
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint8_t HIBYTE( uint16_t __val)
{
	return (((__val) >> 8) & 0xFF);
}
//#define MAKEWORD(__H,__L)  (USHORT) ((__L) | ((__H) << 8))
/** Packs two 8 bits unsigned integers in one 16 bits unsigned integer.
* 
* @arg  __H The upper 8 bit to pack.
* @arg  __L The lower 8 bit to pack.
* @return A 16 bits unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint16_t MAKEWORD( uint8_t __H, uint8_t __L) 
{
	return ((__L) | ((__H) << 8));
}

//#define MAKESHORT(__H,__L) ((__L) | ((__H) << 8))
/** Packs two 8 bits unsigned integers in one 16 bits unsigned integer.
* 
* @arg  __H The upper 8 bit to pack.
* @arg  __L The lower 8 bit to pack.
* @return A 16 bits unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint16_t MAKESHORT( uint8_t __H, uint8_t __L)
{
	return ((__L) | ((__H) << 8));
}

//#define MAKESHORT(__H,__L) ((__L) | ((__H) << 8))

// #define LOSHORT(__val)     ((__val) & 0xFFFF)
/** Returns the lower 16 bits of a 32 bits unsigned integer.
* 
* @arg  __val A 32 bits unsigned integer.
* @return The lower 16 bits as unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint16_t LOSHORT( uint32_t __val)
{
	return ((__val) & 0xFFFF);
}

//#define HISHORT(__val)     (((__val) >> 16) & 0xFFFF)
/** Returns the upper 16 bits of a 32 bits unsigned integer.
* 
* @arg  __val A 32 bits unsigned integer.
* @return The upper 16 bits as unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint16_t HISHORT( uint32_t __val)
{
	return (((__val) >> 16) & 0xFFFF);
}

//#define MAKELONG(__H,__L)  ((__L) | ((__H) << 16)
/** Packs two 16 bits unsigned integer in one 16 bits unsigned integer.
* 
* @arg  __H The upper 16 bit to pack.
* @arg  __L The lower 16 bit to pack.
* @return A 32 bits unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint32_t MAKELONG( uint16_t __H, uint16_t  __L )
{
	return (((__H) << 16) | (__L) );
}

//#define LOWORD(__val)      (USHORT) ((__val) & 0xFFFF)
/** Returns the lower 16 bits of a 32 bits unsigned integer.
* 
* @arg  __val A 32 bits unsigned integer.
* @return The lower 16 bits as unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint16_t LOWORD(uint32_t __val)
{
	return ((__val) & 0xFFFF);
}

//#define HIWORD(__val)      (USHORT) (((__val) >> 16) & 0xFFFF)
/** Returns the upper 16 bits of a 32 bits unsigned integer.
* 
* @arg  __val A 32 bits unsigned integer.
* @return The upper 16 bits as unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__ uint16_t HIWORD( uint32_t __val)
{
	return (((__val) >> 16) & 0xFFFF);
}

//#define MAKEDWORD(__H,__L) (UINT)((__L)|( (__H)<<16) )
/** Packs two 16 bits unsigned integer in one 16 bits unsigned integer.
* 
* @arg  __H The upper 16 bit to pack.
* @arg  __L The lower 16 bit to pack.
* @return A 32 bits unsigned integer.
*
* @note These names come from the i386 architecture, where  
* - a byte is 8 bits wide 
* - a word is 16 bits wide
* - a short is 16 bits wide
* - a long is 32 bits wide
* - a dword is 32 bits wide
*
* @note These function are intended for PCI access, where registers are of definite size. 
*/
static __inline__  uint32_t MAKEDWORD( uint16_t __H, uint16_t __L)
{
	 return ((__L)|( (__H)<<16) );
}

/* This structure is aimed at containing OS information about a PCI board.  
*
* @note Actually the reference data are in struct pci_dev. 
* 
* @warning The command CMD_APCI1710_GetHardwareInformation() is deprecated. 
* The type used for holding the information doesn't scale on recent architectures
* and it is possible to get the same information from the kernel using standard OS functions.
* It is here for backward compatibility reasons only
* 
*      
*/
typedef struct
{
   uint32_t ui_BaseAddress[5]; /**< The BAR addresses */
   uint8_t	  b_Interrupt; /**< Interrupt to which the bord is associated (if any)  */
   uint8_t   b_SlotNumber; /**< Slot number  */
}
str_BaseInformations;

/* internaly used by driver */

#ifdef __KERNEL__

	#include <linux/version.h>

	// COMPATIBILITY WITH 2.4 --------------------------------------------------
	
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,0)	
	
		#define EXPORT_NO_SYMBOLS
		#define MOD_INC_USE_COUNT
		#define MOD_DEC_USE_COUNT
	
	#endif

	//--------------------------------------------------------------------------
	/** clean cast from a void pointer to a struct pci_dev pointer */
	static __inline__ struct pci_dev * VOID_TO_PCIDEV(void * __ptr)
	{
		return (struct pci_dev *) __ptr;
	}
	
	/** prototype of IOCTL internal call implementation function 
	 * 
	 * @internal
	 * 
	 * */
	typedef  int (__do_fcn) (struct pci_dev* pdev,unsigned int cmd, unsigned long arg);
	
	/* same with identifier */
	typedef  int (__do_fcn_with_id) (struct pci_dev* pdev,unsigned int cmd, unsigned long arg, unsigned long id);

	/* function annotation to indicate that the function accesses driver private data and must by called in locked context  */
	#if defined (__needlock)
		#error conflict: __needlock is already defined.
	#else
		#ifdef __CHECKER__
		#define __needlock __attribute__((context(lock,1,1)))
		#else
		#define __needlock
		#endif
	#endif

#else // ! __KERNEL__
#define __needlock

#endif // __KERNEL__

#endif // __ADDI__APCI__H_
