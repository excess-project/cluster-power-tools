/** @file vtable.c
*
* @author Sylvain Nahas
*
* @date 05.01.06
* 
* @version  $LastChangedRevision$
* @version $LastChangedDate$
* 
* A vtable is an area of pointers to ioctl-implementing functions
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
#include "vtable.h"

/**@def EXPORT_NO_SYMBOLS
 * Function in this file are not exported.
 */
EXPORT_NO_SYMBOLS;

//------------------------------------------------------------------------------

vtable_t xpci3xxx_vtable __cacheline_aligned; 

//------------------------------------------------------------------------------
/** Affect an vtable entry.
 * 
 * @return 0 if OK, -1 if problem 
 */
int __xpci3xxx_DECLARE_IOCTL_HANDLER(vtable_t vtable ,unsigned int cmd__,  __do_fcn * handler__ptr) 
{
	
	/* better to be paranoid - catches bugs at run time */
	if (!vtable)
	{
		printk("%s: ioctl %d: vtable is NULL\n",__FUNCTION__,_IOC_NR(cmd__));
		return -1;
	}
	
	/* check boundaries - vtable size is known at compile time - 
	 * using it avoid code dependancy on macro definition */
	if ( _IOC_NR(cmd__) > (VTABLE_ELEMENT_NB(vtable_t)-1) )
	{
		printk("%s: ioctl %d: vtable entry index is over table boundaries (%lu elements)\n",__FUNCTION__,_IOC_NR(cmd__),VTABLE_ELEMENT_NB(vtable_t) );
		return -1;		
	}		
	
	/* vtable entries must be set to xpci3xxx_do_dummy before affectation with clear_vtable */
	
	vtable[_IOC_NR(cmd__)] = handler__ptr;
	
	return 0;
}

//------------------------------------------------------------------------------
/** This function initialise a vtable (set all entry to dummy)
 * 
 * @param vtable VTABLE to intialise
 * 
 * */
void xpci3xxx_dummyze_vtable(vtable_t vtable, __do_fcn * dummyfcn )
{
	int i;
	for ( i=0; i< VTABLE_ELEMENT_NB(vtable_t) ; i++ )
	{
		vtable[i] = dummyfcn;
	}
}
