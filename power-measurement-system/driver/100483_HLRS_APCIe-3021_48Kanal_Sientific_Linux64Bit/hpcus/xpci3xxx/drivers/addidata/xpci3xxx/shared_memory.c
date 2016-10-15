/** @file shared_memory.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* Kernel functions for the shared memory management.
*
*/

/** @par LICENCE
* @verbatim
 Copyright (C) 2004,2005  ADDI-DATA GmbH for the source code and the documentation.
        
        ADDI-DATA GmbH
        Dieselstrasse 3 
        D-77833 Ottersweier
        Tel: +19(0)7223/9493-0
        Fax: +49(0)7223/9493-92
        http://www.addi-data-com
        info@addi-data.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details. 
        
    You shoud find the complete GPL in the COPYING file accompanying 
    this source code. 
* @endverbatim
*/

#include "xpci3xxx-private.h"

EXPORT_SYMBOL(i_xpci3xxx_AllocateSharedMemory);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseSharedMemory);

/** 
 * 
 * Allocate a shared memory space.
 * 
 * @param[in] pdev 		: PCI Device struct                
 * @param[in] dw_Size	: Size of the memory to reserve in byte.
 * 
 * @return	0	No error.\n
 *      	1	No memory space available.\n
 * 
 */	
int i_xpci3xxx_AllocateSharedMemory (struct pci_dev * pdev, uint32_t dw_Size)
{
   unsigned long virt_addr;


      /* Allocate the Shared Memory : always add 2*PAGE_SIZE to the size to allocate */
      XPCI3XXX_PRIVDATA(pdev)->kmalloc_ptr = kmalloc(dw_Size+2*PAGE_SIZE,GFP_KERNEL);

      if (XPCI3XXX_PRIVDATA(pdev)->kmalloc_ptr == NULL)
         return 1;

      /* Define the pointer to the shared memory on the kernel module side */
      XPCI3XXX_PRIVDATA(pdev)->kmalloc_area = (char *)(((unsigned long)XPCI3XXX_PRIVDATA(pdev)->kmalloc_ptr + PAGE_SIZE));

      /* Cause the page to become reserved
       * A reserved page is reserved from any futher consideration by the linux kernel,
       * meaning it is not scanned as potentially pageable, or available for page allocation.
       */
      for ( virt_addr=(unsigned long)XPCI3XXX_PRIVDATA(pdev)->kmalloc_area; virt_addr<((unsigned long)XPCI3XXX_PRIVDATA(pdev)->kmalloc_area+dw_Size); virt_addr+=PAGE_SIZE)
      /* 2.4 only : remap_page_range is deprecated in 2.6 */
      #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
            mem_map_reserve (virt_to_page(virt_addr));
      #else
			SetPageReserved (virt_to_page(virt_addr));
      #endif // 2.4

      /* Initialize the memory to 0 */
      memset (XPCI3XXX_PRIVDATA(pdev)->kmalloc_area, 0, (dw_Size+2*PAGE_SIZE));

      return (0);
}

/** 
 * 
 * Release a shared memory space.
 * 
 * @param[in] pdev 		: PCI Device struct                
 * @param[in] dw_Size	: Size of the memory to release in byte.
 * 
 * @return	0	No error.\n
 * 
 */
int i_xpci3xxx_ReleaseSharedMemory (struct pci_dev * pdev, uint32_t dw_Size)
{
   unsigned long virt_addr;

      /* Cause the page to become unreserved */
      for (virt_addr=(unsigned long)XPCI3XXX_PRIVDATA(pdev)->kmalloc_area; virt_addr<((unsigned long)XPCI3XXX_PRIVDATA(pdev)->kmalloc_area+dw_Size); virt_addr+=PAGE_SIZE)
      /* 2.4 only : remap_page_range is deprecated in 2.6 */
      #if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
			mem_map_unreserve(virt_to_page(virt_addr));
      #else
			ClearPageReserved (virt_to_page(virt_addr));
      #endif // 2.4

      /* Free the shared memory */
      kfree(XPCI3XXX_PRIVDATA(pdev)->kmalloc_ptr);

      return 0;
}




