/** @file primary_dma.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Primary functions (hardware access) for the dma functionality of the board.
*
*/

/** @par LICENCE
* @verbatim
* @endverbatim
*/

#include "xpci3xxx-private.h"
#include "xpci3xxx-options.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,19)
#include <linux/config.h>
#else
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,33)
		#include <linux/autoconf.h>
	#else
		#include <generated/autoconf.h>
	#endif
#endif

#ifndef CONFIG_XPCI3XXX_NO_DMA
#ifdef CONFIG_BIGPHYS_AREA
#include <linux/bigphysarea.h>
#else
#warning big phys area support not present in kernel
#endif //CONFIG_BIGPHYS_AREA
#endif //!CONFIG_XPCI3XXX_NO_DMA

#include <linux/time.h>

#include "primary_dma.h"

//#define DEBUG	printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

EXPORT_SYMBOL(i_xpci3xxx_InitAnalogInputDMA);
EXPORT_SYMBOL(i_xpci3xxx_StartAnalogInputDMA);
EXPORT_SYMBOL(i_xpci3xxx_StopAnalogInputDMA);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseAnalogInputDMA);

/* Scatter gater initialisation list struct */
typedef struct
{
	uint32_t   dw_BufferPhysicalAddress;      // DMA buffer physical address
	uint32_t   dw_LocalBusPhysicalAddress;    // Local bus physical address
	uint32_t   dw_BufferSize;                 // Acquisition size (byte)
	uint32_t   dw_NextDescriptorAddress;      // Next descriptor address
}str_PCI9054DMADescriptor,*pstr_PCI9054DMADescriptor;

/**
*
* Function which allocate the physical memory
*
* @param[in] dw_AllocatedMemory : DMA Buffer allocated memory \n
*
* @param[out] BigphysareaHandle : DMA Buffer bigphysarea address \n
* @param[out] pv_DMABuffer : DMA Buffer address               \n
* @param[out] pdw_PhysicalAddress : DMA Buffer physical address  \n
*
* @return  0: No error                                            \n
*/
#ifdef CONFIG_XPCI3XXX_NO_DMA
int i_xpci3xxx_AllocPhysMem (caddr_t *BigphysareaHandle,void** pv_DMABuffer, uint32_t * pdw_PhysicalAddress, uint32_t dw_AllocatedMemory)
{
	printk (KERN_INFO "%s: No DMA compiler option used\n",__DRIVER_NAME);
	*BigphysareaHandle = NULL;
	*pv_DMABuffer = NULL;
	*pdw_PhysicalAddress = 0;
	return -1;
}
#else //CONFIG_XPCI3XXX_NO_DMA
int i_xpci3xxx_AllocPhysMem (caddr_t *BigphysareaHandle,void** pv_DMABuffer, uint32_t * pdw_PhysicalAddress, uint32_t dw_AllocatedMemory)
{
#ifdef CONFIG_BIGPHYS_AREA
	int i_FreeMemorySize = 0;
	bigphysarea_kinfo(&i_FreeMemorySize);
	// printk("\n i_FreeMemorySize = %d\n",i_FreeMemorySize);
	if (dw_AllocatedMemory > i_FreeMemorySize)
	{
		printk("ERROR dw_AllocatedMemory = %d\n",dw_AllocatedMemory);
		return -1;
	}
	*BigphysareaHandle =  bigphysarea_alloc (dw_AllocatedMemory);
#else
	*BigphysareaHandle =  kmalloc(dw_AllocatedMemory, GFP_KERNEL | GFP_DMA);
#endif

	*pv_DMABuffer = *BigphysareaHandle;

	if (*pv_DMABuffer == NULL)
	{
		printk (KERN_ERR "%s: not enough memory\n", __DRIVER_NAME);
		*BigphysareaHandle = NULL;
		*pv_DMABuffer = NULL;
		*pdw_PhysicalAddress = 0;
		return -1;
	}
#ifdef CONFIG_MIPS
	//#warning "MIPS"
	/*The MIPS cache handling is not transparent for system softwares, whereas the x86's one is.*/
	dma_cache_wback_inv((unsigned long)*pv_DMABuffer,dw_AllocatedMemory);

	/*Map this address to the KSEG1 kernel segment (change cachability)*/
	*pv_DMABuffer = (void *)KSEG1ADDR(*pv_DMABuffer);
	#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		*pdw_PhysicalAddress = (uint32_t)PHYSADDR (*pv_DMABuffer);
	#else
		*pdw_PhysicalAddress = (uint32_t)CPHYSADDR (*pv_DMABuffer);
	#endif
	// the same address is returned by virt_to_phys(bigphysarea) or virt_to_bus
#else
	//not MIPS: x86, arm ...
	//#warning "x86"
	*pdw_PhysicalAddress = (uint32_t)virt_to_bus((volatile void *)(*pv_DMABuffer));
#endif

	return 0;
}
#endif //!CONFIG_XPCI3XXX_NO_DMA
/**
*
* Function which release allocated memory
*
* @param[in] pv_DMABuffer : DMA Buffer address                \n
* @param[in] dw_AllocatedMemory : DMA Buffer allocated memory \n
*
*
* @return  0: No error                                            \n
*/

#ifdef CONFIG_XPCI3XXX_NO_DMA
void v_xpci3xxx_FreePhysMem  (caddr_t BigphysareaHandle, uint32_t dw_AllocatedMemory)
{
}
#else // CONFIG_XPCI3XXX_NO_DMA
void v_xpci3xxx_FreePhysMem  (caddr_t BigphysareaHandle, uint32_t dw_AllocatedMemory)
{
	if ( (void *)BigphysareaHandle != NULL)
	{
#ifdef CONFIG_BIGPHYS_AREA
		bigphysarea_free(BigphysareaHandle, dw_AllocatedMemory);
#else
		kfree(BigphysareaHandle);
#endif
	}
}
#endif // !CONFIG_XPCI3XXX_NO_DMA


/* Memory management function                                           */

/**
*
* Get or create the DMA buffer memory allocation
*
* @param[in] dw_DMATotalBufferSize       : Number total bytes \n
*                                                          transfer           \n
*                                                          (Single mode)      \n
* @param[in]  b_AcquisitionMode           : 0 : Single mode    \n
*                                                          1 : Continuous mode\n
* @param[in] dw_MaxBufferAllocationSize  : Max size for one   \n
*                                                          scatter gather     \n
*                                                          buffer (byte)      \n
* @param[in]  w_LocalBusWidth             : 8 : 8-bit bus width\n
*                                                          16:16-bit bus width\n
*                                                          32:32-bit bus width\n
*
* @param[out] ps_ScatterGatherPCIDMAInformation : DMA allocated      \n
*                                                          buffer struct      \n
*
* @return   0 : OK                                                \n
*                      -1 : Memory allocation error                           \n
*/
int i_xpci3xxx_CreateScatterGatherMemory (uint32_t                                   dw_DMATotalBufferSize,
		                                  uint8_t                                     b_AcquisitionMode,
		                                  uint32_t                                   dw_MaxBufferAllocationSize,
		                                  uint16_t                                     w_LocalBusWidth,
		                                  str_ScatterGatherPCIDMAInformation *    ps_ScatterGatherPCIDMAInformation)
{

	/* Variable declaration */
	uint8_t     b_BufferArrayIndex     = 0;
	caddr_t  BigphysareaHandle	= NULL;
	uint8_t ** pb_Buffer               = NULL;
	uint32_t   dw_BufferIndex          = 0;
	uint32_t   dw_MemoryToAllocate     = 0;
	uint32_t   dw_ScatterGatherNbr [2] = {0,0};
	uint32_t   dw_AllocatedMemory      = 0xFFFFFFFFUL;
	uint32_t   dw_PhysicalAddress      = 0;
	uint32_t   dw_AllocationTest       = 0;

	/* Variable initialisation */
	dw_DMATotalBufferSize  = dw_DMATotalBufferSize * ((uint32_t) b_AcquisitionMode + 1UL);

	/* Clear the scatter gather DMA informations array */
	memset (ps_ScatterGatherPCIDMAInformation,
			0,
			sizeof (str_ScatterGatherPCIDMAInformation));

	/* Get the number of available buffer description */

	/* Get the number of buffer allocated */
	dw_ScatterGatherNbr[0] = 0;
	dw_ScatterGatherNbr[1] = 0;

	/* Allocate the rest of memory */
	for (b_BufferArrayIndex = 0; (b_BufferArrayIndex < 2) &&
	                             (ps_ScatterGatherPCIDMAInformation->ul_BufferSize != dw_DMATotalBufferSize) &&
			                     (dw_AllocationTest != ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER);
	     b_BufferArrayIndex ++)
	{
		for (dw_BufferIndex = dw_ScatterGatherNbr [b_BufferArrayIndex];	(dw_BufferIndex < DMA_MAX_SCATTER_GATHER_BUFFER) &&
		                                                                (ps_ScatterGatherPCIDMAInformation-> ul_BufferSize != dw_DMATotalBufferSize) &&
				                                                        (dw_AllocationTest != ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER);
		     dw_BufferIndex ++)
		{

			/* Get the buffer size to allocate */
			dw_MemoryToAllocate = dw_DMATotalBufferSize - ps_ScatterGatherPCIDMAInformation->
			                                              ul_BufferSize;

			// printk ("%s %i dw_MemoryToAllocate %u\n", __FUNCTION__, __LINE__, dw_MemoryToAllocate);

			/* Test the buffer size */
			if (dw_MemoryToAllocate > dw_MaxBufferAllocationSize)
				dw_MemoryToAllocate = dw_MaxBufferAllocationSize;

			// printk ("%s %i dw_MemoryToAllocate %u\n", __FUNCTION__, __LINE__, dw_MemoryToAllocate);

			/* Test if memory to allocate > last allocated memory */
			if (dw_MemoryToAllocate > dw_AllocatedMemory)
			{
				/* Set the memory to allocate to the last allocated memory */
				dw_MemoryToAllocate = dw_AllocatedMemory;
			}

			// printk ("%s %i dw_MemoryToAllocate %u\n", __FUNCTION__, __LINE__, dw_MemoryToAllocate);

			for (dw_AllocationTest  = 0,dw_AllocatedMemory = dw_MemoryToAllocate;dw_AllocationTest < ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER;
			     dw_AllocationTest ++, dw_AllocatedMemory = ((dw_MemoryToAllocate * ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER) - (dw_MemoryToAllocate * dw_AllocationTest)) / ADDIDATA_MAX_SCATTER_GATHER_SUB_BUFFER)
			{
				dw_AllocatedMemory = (dw_AllocatedMemory / (w_LocalBusWidth / 8)) * (w_LocalBusWidth / 8);

				// printk ("%s %i dw_AllocatedMemory %u\n", __FUNCTION__, __LINE__, dw_AllocatedMemory);
				/* Create the shared memory */
				if (i_xpci3xxx_AllocPhysMem (&BigphysareaHandle,
						                     (void**)&pb_Buffer,
						                     &dw_PhysicalAddress,
						                     dw_AllocatedMemory) == 0)
				{
					/* Save the allocated buffer information's */
					ps_ScatterGatherPCIDMAInformation->
					s_BufferInformations [b_BufferArrayIndex].
					s_ScatterGather [dw_BufferIndex].
					BigphysareaHandle = BigphysareaHandle;


					ps_ScatterGatherPCIDMAInformation->
					s_BufferInformations [b_BufferArrayIndex].
					s_ScatterGather [dw_BufferIndex].
					pv_DMABuffer = (void *) pb_Buffer;

					ps_ScatterGatherPCIDMAInformation->
					s_BufferInformations [b_BufferArrayIndex].
					s_ScatterGather [dw_BufferIndex].
					ul_PhysicalAddress = dw_PhysicalAddress;
					// printk("b_BufferArrayIndex %hu dw_BufferIndex %hu ul_PhysicalAddress : %d\n",b_BufferArrayIndex,dw_BufferIndex,dw_PhysicalAddress);

					ps_ScatterGatherPCIDMAInformation->
					s_BufferInformations [b_BufferArrayIndex].
					s_ScatterGather [dw_BufferIndex].
					ul_BufferSize = dw_AllocatedMemory;
					// printk("ul_BufferSize : %d\n",dw_AllocatedMemory);

					ps_ScatterGatherPCIDMAInformation->
					s_BufferInformations [b_BufferArrayIndex].
					ul_ScatterGatherNbr =   ps_ScatterGatherPCIDMAInformation->
					                        s_BufferInformations [b_BufferArrayIndex].
					                        ul_ScatterGatherNbr + 1;

					// printk ("%s %i ul_ScatterGatherNbr %u\n", __FUNCTION__, __LINE__, ps_ScatterGatherPCIDMAInformation->
					//		s_BufferInformations [b_BufferArrayIndex].
					//		ul_ScatterGatherNbr);

					ps_ScatterGatherPCIDMAInformation->
					ul_BufferSize = ps_ScatterGatherPCIDMAInformation->
						            ul_BufferSize + dw_AllocatedMemory;

					ps_ScatterGatherPCIDMAInformation->
					ul_BufferNbr = ps_ScatterGatherPCIDMAInformation->
				               	   ul_BufferNbr + 1;

					memset (pb_Buffer, 1, dw_AllocatedMemory);

					break;
				}
			}
		}
	}

	/* Test the memory allocation */

	/* Test if not all memory allocated */
	if (ps_ScatterGatherPCIDMAInformation-> ul_BufferSize != dw_DMATotalBufferSize)
	{
		/* Free all allocated buffer */
		for (b_BufferArrayIndex = 0; b_BufferArrayIndex < 2; b_BufferArrayIndex ++)
		{
			for (dw_BufferIndex = 0; dw_BufferIndex < DMA_MAX_SCATTER_GATHER_BUFFER; dw_BufferIndex ++)
			{
				v_xpci3xxx_FreePhysMem (ps_ScatterGatherPCIDMAInformation->
        						        s_BufferInformations [b_BufferArrayIndex].
        						        s_ScatterGather [dw_BufferIndex].
        						        BigphysareaHandle,
        						        ps_ScatterGatherPCIDMAInformation->
        						        s_BufferInformations [b_BufferArrayIndex].
        						        s_ScatterGather [dw_BufferIndex].
        						        ul_BufferSize);
			}
		}

		// printk("ps_ScatterGatherPCIDMAInformation-> ul_BufferSize != dw_DMATotalBufferSize %d!=%d\n",ps_ScatterGatherPCIDMAInformation-> ul_BufferSize,dw_DMATotalBufferSize);
		/* Memory allocation error */
		return (-1);
	}

	/* The buffer must be continuous -> only 1 buffer, because the shared memory with the user mode is continuous */
	if (ps_ScatterGatherPCIDMAInformation-> ul_BufferNbr > 1)
	{
		// printk ("%s %i ul_BufferNbr %u\n", __FUNCTION__, __LINE__, ps_ScatterGatherPCIDMAInformation-> ul_BufferNbr);
		for (b_BufferArrayIndex = 0; b_BufferArrayIndex < 2; b_BufferArrayIndex ++)
		{
			/* if several buffer : check that the buffer are continuous */
			for (dw_BufferIndex = 1; dw_BufferIndex < ps_ScatterGatherPCIDMAInformation-> ul_BufferNbr; dw_BufferIndex ++)
			{
				if((ps_ScatterGatherPCIDMAInformation->
        		    s_BufferInformations [b_BufferArrayIndex].
        		    s_ScatterGather [dw_BufferIndex - 1].ul_PhysicalAddress +
        		    ps_ScatterGatherPCIDMAInformation->
        		    s_BufferInformations [b_BufferArrayIndex].
        		    s_ScatterGather [dw_BufferIndex - 1].ul_BufferSize) != ps_ScatterGatherPCIDMAInformation->
																		   s_BufferInformations [b_BufferArrayIndex].
																		   s_ScatterGather [dw_BufferIndex].ul_PhysicalAddress)
				{
					/* Free all allocated buffer */
					for (b_BufferArrayIndex = 0; b_BufferArrayIndex < 2; b_BufferArrayIndex ++)
					{
						for (dw_BufferIndex = 0; dw_BufferIndex < DMA_MAX_SCATTER_GATHER_BUFFER; dw_BufferIndex ++)
						{
							v_xpci3xxx_FreePhysMem (ps_ScatterGatherPCIDMAInformation->
			        						        s_BufferInformations [b_BufferArrayIndex].
			        						        s_ScatterGather [dw_BufferIndex].
			        						        BigphysareaHandle,
			        						        ps_ScatterGatherPCIDMAInformation->
			        						        s_BufferInformations [b_BufferArrayIndex].
			        						        s_ScatterGather [dw_BufferIndex].
			        						        ul_BufferSize);
						}
					}

					// printk("ps_ScatterGatherPCIDMAInformation-> ul_BufferNbr > 1 %d\n",ps_ScatterGatherPCIDMAInformation-> ul_BufferNbr);
					/* Memory allocation error */
					return (-1);
				}
			}
		}
	}

	// printk ("%s %i %i\n", __FUNCTION__, __LINE__, ps_ScatterGatherPCIDMAInformation-> ul_BufferNbr);

	/* Memory allocation OK */
	return (0);
}

/**
*
* Free the DMA buffer memory allocation
*
* @param[in] ps_ScatterGatherPCIDMAInformation : DMA allocated      \n
*                                                          buffer struct      \n
*
*
* @return  -                                                      \n
*/
void v_xpci3xxx_FreeScatterGatherMemory (str_ScatterGatherPCIDMAInformation *    ps_ScatterGatherPCIDMAInformation)
{

	/* Variable declaration */
	uint8_t   b_BufferArrayIndex = 0;
	uint32_t dw_BufferIndex      = 0;

	/* Free all allocated buffer */
	for (b_BufferArrayIndex = 0; b_BufferArrayIndex < 2; b_BufferArrayIndex ++)
	{
		for (dw_BufferIndex = 0; dw_BufferIndex < DMA_MAX_SCATTER_GATHER_BUFFER; dw_BufferIndex ++)
		{
			v_xpci3xxx_FreePhysMem (ps_ScatterGatherPCIDMAInformation->
					                s_BufferInformations [b_BufferArrayIndex].
					                s_ScatterGather [dw_BufferIndex].
					                BigphysareaHandle,
					                ps_ScatterGatherPCIDMAInformation->
					                s_BufferInformations [b_BufferArrayIndex].
					                s_ScatterGather [dw_BufferIndex].
					                ul_BufferSize);
		}
	}

	memset (ps_ScatterGatherPCIDMAInformation,
			0,
			sizeof (str_ScatterGatherPCIDMAInformation));
}

/* 9054 chip function                                                   */

/**
*
* Create the scatter gather description list for the 9054 chip
*
* @param[in] dw_SingleBufferSize         : Number of byte     \n
*                                                          transfer for one   \n
*                                                          acquisition cycle  \n
* @param[in] dw_DMATotalBufferSize       : Number total bytes \n
*                                                          transfer           \n
*                                                          (Single mode)      \n
* @param[in] ps_ScatterGatherPCIDMAInformation : DMA allocated      \n
*                                                          memory informations\n
*                                                          struct             \n
*                                                          (ADDI_DMA.SYS)     \n
* @param[in] b_AcquisitionMode           : 0 : Single mode    \n
*                                                          1 : Continuous mode\n
* @param[in] b_TransferDirection         : 0 : Transfer from  \n
*                                                              the PCI bus to \n
*                                                              the local bus  \n
*                                                          1 : Transfer from  \n
*                                                              the local bus  \n
*                                                              to the PCI bus \n
* @param[in] dw_LocalBusPhysicalAddress  : Indicates from     \n
*                                                          where in Local     \n
*                                                          Memory space DMA   \n
*                                                          transfers start    \n
* @param[in] w_LocalBusWidth              : 8 : 8-bit bus width\n
*                                                          16:16-bit bus width\n
*                                                          32:32-bit bus width\n
*
* @param[out] pdw_MaxNumberOfBufferForOneInterrupt : Return the max  \n
*                                                             number of buffer\n
*                                                             for one sequence\n
*                                                             interrupt       \n
* @param[out] ps_ScatterGatherDescriptor  : Descriptor         \n
*                                                         struct. Contain the \n
*                                                         descriptor list     \n
*
* @return   0 : OK                                                \n
*                      -1 : Memory size error                                 \n
*                      -2 : Descriptor list memory allocation error           \n
*                      -3 : Descriptor interrupt struct memory allocation     \n
*                           error                                             \n
*/
int i_xpci3xxx_Create9054DescriptorList (uint32_t                                   dw_SingleBufferSize,
		                                 uint32_t                                   dw_DMATotalBufferSize,
		                                 str_ScatterGatherPCIDMAInformation *    ps_ScatterGatherPCIDMAInformation,
		                                 uint8_t                                     b_AcquisitionMode,
		                                 uint8_t                                     b_TransferDirection,
		                                 uint32_t                                   dw_LocalBusPhysicalAddress,
		                                 uint16_t                                     w_LocalBusWidth,
		                                 uint32_t*                                 pdw_MaxNumberOfBufferForOneInterrupt,
		                                 pstr_ScatterGatherInterruptDescriptor * ps_ScatterGatherDescriptor)
{

	/* Variable declaration */
	caddr_t				*p_BigphysareaHandle	= NULL;
	uint8_t				**pb_Buffer		= NULL;
	uint8_t				*pb_ScatterGather	= NULL;
	caddr_t				BigphysareaHandle	= NULL;
	pstr_PCI9054DMADescriptor	*ps_PCI9054DMADescriptor;//= NULL;
	uint8_t                       b_Interrupt                         = 0;
	uint8_t                       b_BufferArray                       = 0;
	uint8_t                       b_SingleDescriptor                  = ADDIDATA_DISABLE;
	uint32_t                   *pdw_DescriptorPhysicalAddress;
	uint32_t                     dw_FreeCpt                           = 0;
	uint32_t                     dw_AlignShift                        = 0;
	uint32_t                     dw_BufferIndex                       = 0;
	uint32_t                     dw_SelectedList                      = 0;
	uint32_t                     dw_DMABufferSize                     = 0;
	uint32_t                     dw_SubListCounter                    = 0;
	uint32_t                     dw_AllocatedMemory                   = 0;
	uint32_t                     dw_MemoryRemainder                   = 0;
	uint32_t                     dw_DescriptorCounter                 = 0;
	uint32_t                     dw_NextDMABufferIndex                = 0;
	uint32_t                     dw_LastDescriptorIndex               = 0;
	uint32_t                     dw_DescriptorListIndex               = 0;
	uint32_t                     dw_NumberOfDescriptors               = 0;
	uint32_t                     dw_MinNumberOfDescriptors            = 0;
	uint32_t                     dw_NumberOfDescriptorsList           = 0;
	uint32_t                     dw_ScatterGatherBufferIndex          = 0;
	uint32_t                     dw_ScattherGatherBufferSize          = 0;
	uint32_t                     dw_ScatterGatherBufferAddress        = 0;
	uint32_t                     dw_NextScatterGatherBufferIndex      = 0;
	uint32_t                     dw_NumberOfDescriptorsForOneBlock    = 0;
	uint32_t                     dw_TestNumberOfBufferForOneInterrupt = 0;
	int                       i_Error                             = 0;

	/* Variable initialisation */
	*ps_ScatterGatherDescriptor = NULL;

	/* Memory test dw_DMATotalBufferSize <= Alloctead memoty (ADDI_DMA.SYS) */
	dw_DMATotalBufferSize  = dw_DMATotalBufferSize * ((uint32_t) b_AcquisitionMode + 1UL);

	/* Test the allocated memory size */
	if (dw_DMATotalBufferSize > ps_ScatterGatherPCIDMAInformation->ul_BufferSize)
		return (-1);

	/* Get the number of descriptors */

	/* Test if for each interrupt one descriptor buffer can by used */

	/* Get the minimal number of descriptors */
	dw_NumberOfDescriptors = dw_DMATotalBufferSize / dw_SingleBufferSize;

	/* Test the descriptors number */
	if (dw_NumberOfDescriptors != (dw_DMATotalBufferSize / dw_SingleBufferSize))
		dw_NumberOfDescriptors = dw_NumberOfDescriptors + 1;

	// printk ("%s %i dw_NumberOfDescriptors %u dw_SingleBufferSize %u dw_DMATotalBufferSize %u\n", __FUNCTION__, __LINE__, dw_NumberOfDescriptors, dw_SingleBufferSize, dw_DMATotalBufferSize);

	/* Get the number of configurable descriptors for the allocated memory */

	/* Save the minimal number of descriptors */
	dw_MinNumberOfDescriptors = dw_NumberOfDescriptors;

	/* Complete the descriptor list */
	for (dw_DescriptorCounter = 0; ((dw_DescriptorCounter < dw_NumberOfDescriptors) && (b_AcquisitionMode == 0)) || (b_AcquisitionMode == 1); dw_DescriptorCounter++)
	{
		// printk ("%s %i dw_DMABufferSize %u dw_SingleBufferSize %u\n", __FUNCTION__, __LINE__, dw_DMABufferSize, dw_SingleBufferSize);

		/* Test the size */
		if (dw_SingleBufferSize > dw_DMABufferSize)
		{
			/* Search the next available memory allocated buffer */
			for (dw_ScatterGatherBufferIndex = dw_NextScatterGatherBufferIndex;
			     ((dw_SingleBufferSize > dw_DMABufferSize) &&
				  (dw_ScatterGatherBufferIndex < (ps_ScatterGatherPCIDMAInformation->
					                        	  s_BufferInformations [0].
					                        	  ul_ScatterGatherNbr +
					                        	  ps_ScatterGatherPCIDMAInformation->
					                        	  s_BufferInformations [1].
					                        	  ul_ScatterGatherNbr)));
			    dw_ScatterGatherBufferIndex ++)
			{

				// printk ("%s %i ul_ScatterGatherNbr %u\n", __FUNCTION__, __LINE__, ps_ScatterGatherPCIDMAInformation->
                //        s_BufferInformations [0].
                //        ul_ScatterGatherNbr);

				// printk ("%s %i dw_ScatterGatherBufferIndex %u\n", __FUNCTION__, __LINE__, dw_ScatterGatherBufferIndex);

				/* Test the buffer index */
				if (dw_ScatterGatherBufferIndex < ps_ScatterGatherPCIDMAInformation->
						                          s_BufferInformations [0].
						                          ul_ScatterGatherNbr)
				{

					/* Get the next allocated buffer index */
					b_BufferArray  = 0;
					dw_BufferIndex = dw_ScatterGatherBufferIndex;
				}
				else
				{

					/* Get the next allocated buffer index */
					b_BufferArray  = 1;
					dw_BufferIndex = dw_ScatterGatherBufferIndex -
					                 ps_ScatterGatherPCIDMAInformation->
					                 s_BufferInformations [0].
					                 ul_ScatterGatherNbr;
				}

				// printk ("%s %i dw_BufferIndex %u\n", __FUNCTION__, __LINE__, dw_BufferIndex);

				/* Get the DMA buffer allocated size */
				dw_DMABufferSize = ps_ScatterGatherPCIDMAInformation->
				                   s_BufferInformations [b_BufferArray].
				                   s_ScatterGather [dw_BufferIndex].
				                   ul_BufferSize;

				// printk ("%s %i dw_DMABufferSize %u\n", __FUNCTION__, __LINE__, dw_DMABufferSize);

				dw_NextScatterGatherBufferIndex = dw_ScatterGatherBufferIndex + 1;
			}
		}

		// printk ("%s %i dw_SingleBufferSize %u dw_DMABufferSize %u\n", __FUNCTION__, __LINE__, dw_SingleBufferSize, dw_DMABufferSize);

		/* Test the buffer size */
		if (dw_SingleBufferSize <= dw_DMABufferSize)
			dw_DMABufferSize = dw_DMABufferSize - dw_SingleBufferSize;
		else
		{
			// printk ("%s %i dw_DMABufferSize %u\n", __FUNCTION__, __LINE__, dw_DMABufferSize);
			break;
		}
	}

	// printk ("%s %i dw_NumberOfDescriptors %u dw_DescriptorCounter %u\n", __FUNCTION__, __LINE__, dw_NumberOfDescriptors, dw_DescriptorCounter);

	/* Test if continuous mode and more that the minimal of descriptors found */
	if ((b_AcquisitionMode == 1) &&	(dw_DescriptorCounter > dw_NumberOfDescriptors))
		/* Save the new descriptor number */
		dw_NumberOfDescriptors = dw_DescriptorCounter;

	// printk ("%s %i dw_NumberOfDescriptors %u dw_DescriptorCounter %u\n", __FUNCTION__, __LINE__, dw_NumberOfDescriptors, dw_DescriptorCounter);

	/* Test if can not create one descriptor for each interrupt and get the number of split descriptors */

	/* Test if not found the number of required descriptors */
	if (dw_DescriptorCounter < dw_NumberOfDescriptors)
	{
		// printk ("Test if not found the number of required descriptors\n"); // When problem it goes here

		/* Variable initialization */
		dw_AllocatedMemory     = 0;
		dw_NumberOfDescriptors = 0;
		dw_DMABufferSize       = 0;
		dw_NextDMABufferIndex  = 0;
		dw_MemoryRemainder     = dw_SingleBufferSize;

		do
		{

			/* Test the size */
			if (dw_DMABufferSize == 0)
			{

				/* Test the buffer index */
				if (dw_NextDMABufferIndex < ps_ScatterGatherPCIDMAInformation->
						                    s_BufferInformations [0].
						                    ul_ScatterGatherNbr)
				{

					/* Get the next allocated buffer index */
					b_BufferArray  = 0;
					dw_BufferIndex = dw_NextDMABufferIndex;
				}
				else
				{

					/* Get the next allocated buffer index */
					b_BufferArray  = 1;
					dw_BufferIndex = dw_NextDMABufferIndex -
					                 ps_ScatterGatherPCIDMAInformation->
					                 s_BufferInformations [0].
					                 ul_ScatterGatherNbr;
				}

				/* Get the DMA buffer size */
				dw_DMABufferSize = ps_ScatterGatherPCIDMAInformation->
				                   s_BufferInformations [b_BufferArray].
				                   s_ScatterGather [dw_BufferIndex].
				                   ul_BufferSize;

				/* Select the next DMA buffer */
				dw_NextDMABufferIndex = dw_NextDMABufferIndex + 1;
			}

			/* Test if DMA buffer size > single scatter gather buffer size */
			if (dw_DMABufferSize >= dw_MemoryRemainder)
			{

				dw_AllocatedMemory = dw_AllocatedMemory + dw_MemoryRemainder;
				dw_DMABufferSize = dw_DMABufferSize - dw_MemoryRemainder;
				dw_MemoryRemainder = dw_SingleBufferSize;
			}
			else
			{
				dw_AllocatedMemory = dw_AllocatedMemory + dw_DMABufferSize;
				dw_MemoryRemainder = dw_MemoryRemainder - dw_DMABufferSize;
				dw_DMABufferSize = 0;
			}

			// printk ("%s %i dw_AllocatedMemory %u\n", __FUNCTION__, __LINE__, dw_AllocatedMemory);
			// printk ("%s %i dw_MemoryRemainder %u\n", __FUNCTION__, __LINE__, dw_MemoryRemainder);

			/* Increment the number of descriptors */
			dw_NumberOfDescriptors = dw_NumberOfDescriptors + 1;

			// printk ("%s %i dw_NumberOfDescriptors %u\n", __FUNCTION__, __LINE__, dw_NumberOfDescriptors);

			if (dw_AllocatedMemory == dw_DMATotalBufferSize)
				/* Save the minimal number of descriptors */
				dw_MinNumberOfDescriptors = dw_NumberOfDescriptors;

			// printk ("%s %i dw_NumberOfDescriptors %u\n", __FUNCTION__, __LINE__, dw_NumberOfDescriptors);

			// printk ("%s %i ul_BufferSize %u\n", __FUNCTION__, __LINE__, ps_ScatterGatherPCIDMAInformation->ul_BufferSize);
			// printk ("%s %i dw_AllocatedMemory %u\n", __FUNCTION__, __LINE__, dw_AllocatedMemory);
			// printk ("%s %i dw_SingleBufferSize %u\n", __FUNCTION__, __LINE__, dw_SingleBufferSize);

		} while (((dw_AllocatedMemory != dw_DMATotalBufferSize) && (b_AcquisitionMode == 0)) ||
			     (((ps_ScatterGatherPCIDMAInformation->ul_BufferSize - dw_AllocatedMemory) > dw_SingleBufferSize) && (b_AcquisitionMode == 1)));
	}
	else
	{
		b_SingleDescriptor = ADDIDATA_ENABLE;
	}

	/* PCI DMA Descriptor list memory allocation */

	/* Get the number of descriptor list  */
	if (dw_NumberOfDescriptors <= ADDIDATA_MAX_DESCRIPTORS_LIST)
	{
		dw_NumberOfDescriptorsList = dw_NumberOfDescriptors;
		dw_NumberOfDescriptorsForOneBlock = 1;
		dw_LastDescriptorIndex = 0;
	}
	else
	{

		dw_NumberOfDescriptorsForOneBlock = dw_NumberOfDescriptors / ADDIDATA_MAX_DESCRIPTORS_LIST;

		dw_NumberOfDescriptorsList = ADDIDATA_MAX_DESCRIPTORS_LIST;

		if ((dw_NumberOfDescriptorsForOneBlock * ADDIDATA_MAX_DESCRIPTORS_LIST) != dw_NumberOfDescriptors)
		{
			dw_NumberOfDescriptorsForOneBlock = dw_NumberOfDescriptorsForOneBlock + 1;
		}

		dw_LastDescriptorIndex = (dw_NumberOfDescriptors - 1) % dw_NumberOfDescriptorsForOneBlock;
	}

	/* Memory allocation for saving the descriptor list informations */


	p_BigphysareaHandle		      = (caddr_t*)kmalloc (dw_NumberOfDescriptorsList * sizeof (caddr_t), GFP_KERNEL);
	pb_Buffer                     = (uint8_t**)kmalloc (dw_NumberOfDescriptorsList * sizeof (uint8_t *), GFP_KERNEL);
	ps_PCI9054DMADescriptor       = (pstr_PCI9054DMADescriptor*)kmalloc (dw_NumberOfDescriptorsList * sizeof (pstr_PCI9054DMADescriptor), GFP_KERNEL);
	pdw_DescriptorPhysicalAddress = (uint32_t*)kmalloc (dw_NumberOfDescriptorsList * sizeof (uint32_t), GFP_KERNEL);

	/* Test if memory allocation error */
	if (	(p_BigphysareaHandle		== NULL) ||
			(pb_Buffer			== NULL) ||
			(ps_PCI9054DMADescriptor	== NULL) ||
			(pdw_DescriptorPhysicalAddress	== NULL))
	{

		/* Test if memory allocated */
		if (p_BigphysareaHandle)
			/* Free the memory */
			kfree (p_BigphysareaHandle);


		/* Test if memory allocated */
		if (pb_Buffer != NULL)
			/* Free the memory */
			kfree (pb_Buffer);

		/* Test if memory allocated */
		if (ps_PCI9054DMADescriptor != NULL)
			/* Free the memory */
			kfree (ps_PCI9054DMADescriptor);

		/* Test if memory allocated */
		if (pdw_DescriptorPhysicalAddress != NULL)
			/* Free the memory */
			kfree (pdw_DescriptorPhysicalAddress);

		return (-2);
	}

	for (dw_SubListCounter = 0;	dw_SubListCounter < dw_NumberOfDescriptorsList; dw_SubListCounter ++)
	{

		/* Create the descriptor sub list */
		if (i_xpci3xxx_AllocPhysMem	(& (p_BigphysareaHandle[dw_SubListCounter]),
						             (void**)&pb_Buffer [dw_SubListCounter],
						             &pdw_DescriptorPhysicalAddress [dw_SubListCounter],
						             (sizeof(str_PCI9054DMADescriptor) *
								     dw_NumberOfDescriptorsForOneBlock) + 0x10) != 0)
		{

			/* Free all allocated descriptors list */
			for (dw_FreeCpt = 0; dw_FreeCpt < dw_SubListCounter; dw_FreeCpt ++)
			{
				v_xpci3xxx_FreePhysMem (p_BigphysareaHandle[dw_SubListCounter],
						                (sizeof(str_PCI9054DMADescriptor) *
								        dw_NumberOfDescriptorsForOneBlock) + 0x10);
			}

			/* Free the memory */
			kfree (p_BigphysareaHandle);
			kfree (pb_Buffer);
			kfree (ps_PCI9054DMADescriptor);
			kfree (pdw_DescriptorPhysicalAddress);

			return (-2);
		}
	}

	/* Interrupt descriptor array memory allocation */

	/* Create the shared memory */
	*ps_ScatterGatherDescriptor = kmalloc (sizeof (str_ScatterGatherInterruptDescriptor) +
			                               (sizeof (str_ScatterGatherSingleBuffer) *
					                       (dw_NumberOfDescriptors - 1)), GFP_KERNEL);

	/* Test if error occur */
	if (i_Error != 0)
	{
		for (dw_SubListCounter = 0;	dw_SubListCounter < dw_NumberOfDescriptorsList; dw_SubListCounter ++)
		{

			/* Free the allocated description list */
			v_xpci3xxx_FreePhysMem (p_BigphysareaHandle[dw_SubListCounter],
					                (sizeof(str_PCI9054DMADescriptor) *
							        dw_NumberOfDescriptorsForOneBlock) + 0x10);
		}

		/* Free the memory */
		kfree (p_BigphysareaHandle);
		kfree (pb_Buffer);
		kfree (ps_PCI9054DMADescriptor);
		kfree (pdw_DescriptorPhysicalAddress);

		return (-3);
	}
	else
	{

		/* Clear the struct */
		memset (*ps_ScatterGatherDescriptor,
				0,
				sizeof (str_ScatterGatherInterruptDescriptor) +
				(sizeof (str_ScatterGatherSingleBuffer) *
						(dw_NumberOfDescriptors - 1)));
	}

	/* Quadword alignement test */
	for (dw_SubListCounter = 0; dw_SubListCounter < dw_NumberOfDescriptorsList; dw_SubListCounter ++)
	{

		/* Save the address for free the scatter gather list */
		(*ps_ScatterGatherDescriptor)->
		pv_FreeDescriptorArrayAddress [dw_SubListCounter] = (void *) p_BigphysareaHandle[dw_SubListCounter];

		(*ps_ScatterGatherDescriptor)->
		dw_FreeDescriptorArraySize[dw_SubListCounter] = (sizeof(str_PCI9054DMADescriptor) * dw_NumberOfDescriptorsForOneBlock) + 0x10;

		if (pdw_DescriptorPhysicalAddress [dw_SubListCounter] & 0xF)
		{

			/* Verification that bits 0-3 are zero (quadword alignement) */
			dw_AlignShift = 0x10 - (pdw_DescriptorPhysicalAddress[dw_SubListCounter] & 0xF);
			pdw_DescriptorPhysicalAddress[dw_SubListCounter] = pdw_DescriptorPhysicalAddress[dw_SubListCounter] + dw_AlignShift;
		}
		else
		{

			dw_AlignShift = 0;
		}

		/* Get the descriptor list address  */
		/* 2.4 only : don't work with new version of gcc.
		 * Use of preprocessor condition so long
		 * as this modification is not tested.
		 */
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)
		(uint8_t *) ps_PCI9054DMADescriptor[dw_SubListCounter] = (pb_Buffer[dw_SubListCounter] + dw_AlignShift);
#else
		ps_PCI9054DMADescriptor[dw_SubListCounter] = (pstr_PCI9054DMADescriptor) (pb_Buffer[dw_SubListCounter] + dw_AlignShift);
#endif
	}

	/* Create the scatter gather descriptor list */

	/* Create the descriptor lits for interrupt generated = one descriptor  */
	if (b_SingleDescriptor == ADDIDATA_ENABLE)
	{

		// printk ("%s %i\n", __FUNCTION__, __LINE__);

		/* Variable initialisation */
		dw_DescriptorCounter                  = 0;
		dw_DMABufferSize                      = 0;
		dw_NextScatterGatherBufferIndex       = 0;
		*pdw_MaxNumberOfBufferForOneInterrupt = 1;

		/* Complete the descriptor list */
		for (dw_DescriptorCounter   = 0; dw_DescriptorCounter < dw_NumberOfDescriptors; dw_DescriptorCounter ++)
		{

			/* Get the DMA allocated buffer information's */

			/* Test the size */
			if (dw_SingleBufferSize > dw_DMABufferSize)
			{

				/* Seach the next available memory allocated buffer */
				for (dw_ScatterGatherBufferIndex = dw_NextScatterGatherBufferIndex;
				     ((dw_SingleBufferSize > dw_DMABufferSize) &&
					  (dw_ScatterGatherBufferIndex < (ps_ScatterGatherPCIDMAInformation->
						                          	  s_BufferInformations [0].
								                      ul_ScatterGatherNbr +
								                      ps_ScatterGatherPCIDMAInformation->
								                      s_BufferInformations [1].
								                      ul_ScatterGatherNbr)));
				    dw_ScatterGatherBufferIndex ++)
				{

					/* Test the buffer index */
					if (dw_ScatterGatherBufferIndex < ps_ScatterGatherPCIDMAInformation->
							                          s_BufferInformations [0].
							                          ul_ScatterGatherNbr)
					{

						/* Get the next allocated buffer index */
						b_BufferArray  = 0;
						dw_BufferIndex = dw_ScatterGatherBufferIndex;
					}
					else
					{

						/* Get the next allocated buffer index */
						b_BufferArray  = 1;
						dw_BufferIndex = dw_ScatterGatherBufferIndex -
						                 ps_ScatterGatherPCIDMAInformation->
						                 s_BufferInformations [0].
						                 ul_ScatterGatherNbr;
					}

					/* Get the DMA allocated buffer informations */
					dw_DMABufferSize              = ps_ScatterGatherPCIDMAInformation->
					                                s_BufferInformations [b_BufferArray].
					                                s_ScatterGather [dw_BufferIndex].
					                                ul_BufferSize;
					dw_ScatterGatherBufferAddress = ps_ScatterGatherPCIDMAInformation->
					                                s_BufferInformations [b_BufferArray].
					                                s_ScatterGather [dw_BufferIndex].
					                                ul_PhysicalAddress;
					pb_ScatterGather              = (uint8_t *) ps_ScatterGatherPCIDMAInformation->
					                                            s_BufferInformations [b_BufferArray].
					                                            s_ScatterGather [dw_BufferIndex].
					                                            pv_DMABuffer;

					BigphysareaHandle              = ps_ScatterGatherPCIDMAInformation->
					                                 s_BufferInformations [b_BufferArray].
					                                 s_ScatterGather [dw_BufferIndex].
					                                 BigphysareaHandle;
					// 					printk("%s %d:\tBigphysareaHandle %X\n",__FILE__,__LINE__,ps_ScatterGatherPCIDMAInformation->
					// 					                                    s_BufferInformations [b_BufferArray].
					// 					                                    s_ScatterGather [dw_BufferIndex].
					// 					                                    BigphysareaHandle);

				}
			}

			/* Save the single element information's */

			/* Save the buffer physical address */
			ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
			dw_BufferPhysicalAddress = dw_ScatterGatherBufferAddress;

			/* Save the local bus address */
			ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
			dw_LocalBusPhysicalAddress = dw_LocalBusPhysicalAddress;

			/* Save the buffer size */
			ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
			dw_BufferSize = dw_SingleBufferSize;

			/* Test if not the last descriptor */
			if ((dw_DescriptorCounter + 1) < dw_NumberOfDescriptors)
			{
				if ((dw_DescriptorListIndex + 1) < dw_NumberOfDescriptorsForOneBlock)
				{

					/* Save the next descriptor physical address */
					ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
					dw_NextDescriptorAddress = (pdw_DescriptorPhysicalAddress[dw_SelectedList] +
							                    (sizeof (str_PCI9054DMADescriptor) *
									             (dw_DescriptorListIndex + 1))) | 0x5 |
									            (b_TransferDirection << 3);

					/* Set the descriptor list index to the next element */
					dw_DescriptorListIndex = dw_DescriptorListIndex + 1;
				}
				else
				{

					/* Save the next descriptor physical address */
					ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
					dw_NextDescriptorAddress = pdw_DescriptorPhysicalAddress[dw_SelectedList + 1] |
					                           0x5 |
					                           (b_TransferDirection << 3);

					/* Select the next descriptot list */
					dw_SelectedList = dw_SelectedList + 1;

					/* Set the descriptor list index to the firt element */
					dw_DescriptorListIndex = 0;
				}
			}
			else
			{

				/* Set the last descriptor informations */
				ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
				dw_NextDescriptorAddress = 0x5 | (b_TransferDirection << 3);
			}

			/* Save single scatter gather buffer interrupt information's */
			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			dw_Size = dw_SingleBufferSize;

			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			dw_PhysicalAddress = dw_ScatterGatherBufferAddress;

			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			s_Buffer.
			pb_DMABuffer = pb_ScatterGather;

			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			BigphysareaHandle = BigphysareaHandle;


			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			b_Interrupt = 1;

			/* Set next DMA allocated buffer address */

			/* Get the next DMA information's */
			dw_ScatterGatherBufferAddress   = dw_ScatterGatherBufferAddress +
			                                  dw_SingleBufferSize;
			pb_ScatterGather                = pb_ScatterGather +
			                                  dw_SingleBufferSize;
			dw_DMABufferSize                = dw_DMABufferSize -
			                                  dw_SingleBufferSize;
			dw_NextScatterGatherBufferIndex = dw_ScatterGatherBufferIndex;
		}
	}

	/* Test if can not create one descriptor for each interrupt and get the number of slpit descriptors */

	/* Test if not found the number of requireds descriptors */
	if (b_SingleDescriptor == ADDIDATA_DISABLE)
	{

		/* Variable initialisation */
		dw_AllocatedMemory     = 0;
		dw_NextDMABufferIndex  = 0;
		dw_DescriptorCounter   = 0;
		dw_DMABufferSize       = 0;
		dw_MemoryRemainder     = dw_SingleBufferSize;

		// printk ("%s %i\n", __FUNCTION__, __LINE__);

		do
		{

			/* Get the DMA allocated buffer information's */

			/* Test the size */
			if (dw_DMABufferSize == 0)
			{

				/* Test the buffer index */
				if (dw_NextDMABufferIndex < ps_ScatterGatherPCIDMAInformation->
						                    s_BufferInformations [0].
						                    ul_ScatterGatherNbr)
				{
					/* Get the next allocated buffer index */
					b_BufferArray  = 0;
					dw_BufferIndex = dw_NextDMABufferIndex;
				}
				else
				{
					/* Get the next allocated buffer index */
					b_BufferArray  = 1;
					dw_BufferIndex = dw_NextDMABufferIndex -
					ps_ScatterGatherPCIDMAInformation->
					s_BufferInformations [0].
					ul_ScatterGatherNbr;
				}

				/* Get the DMA allocated buffer informations */
				dw_DMABufferSize              = ps_ScatterGatherPCIDMAInformation->
              				                    s_BufferInformations [b_BufferArray].
              				                    s_ScatterGather [dw_BufferIndex].
              				                    ul_BufferSize;
				dw_ScatterGatherBufferAddress = ps_ScatterGatherPCIDMAInformation->
              				                    s_BufferInformations [b_BufferArray].
              				                    s_ScatterGather [dw_BufferIndex].
              				                    ul_PhysicalAddress;
				pb_ScatterGather              = (uint8_t *) ps_ScatterGatherPCIDMAInformation->
              				                                s_BufferInformations [b_BufferArray].
              				                                s_ScatterGather [dw_BufferIndex].
              				                                pv_DMABuffer;
				BigphysareaHandle             = ps_ScatterGatherPCIDMAInformation->
              				                    s_BufferInformations [b_BufferArray].
              				                    s_ScatterGather [dw_BufferIndex].
              				                    BigphysareaHandle;
				// 				printk("%s %d:\tBigphysareaHandle %X\n",__FILE__,__LINE__,ps_ScatterGatherPCIDMAInformation->
				// 									s_BufferInformations [b_BufferArray].
				// 									s_ScatterGather [dw_BufferIndex].
				// 									BigphysareaHandle);


				/* Select the next DMA buffer */
				dw_NextDMABufferIndex = dw_NextDMABufferIndex + 1;

			}

			/* Test if DMA buffer size > single scatter gather buffer size */
			if (dw_DMABufferSize >= dw_MemoryRemainder)
			{
				dw_ScattherGatherBufferSize = dw_MemoryRemainder;

				dw_AllocatedMemory = dw_AllocatedMemory + dw_MemoryRemainder;

				dw_DMABufferSize = dw_DMABufferSize - dw_MemoryRemainder;

				dw_MemoryRemainder = dw_SingleBufferSize;

				b_Interrupt = 4;

				dw_TestNumberOfBufferForOneInterrupt = dw_TestNumberOfBufferForOneInterrupt + 1;

				if (dw_TestNumberOfBufferForOneInterrupt > *pdw_MaxNumberOfBufferForOneInterrupt)
					*pdw_MaxNumberOfBufferForOneInterrupt = dw_TestNumberOfBufferForOneInterrupt;

				dw_TestNumberOfBufferForOneInterrupt = 0;
			}
			else
			{
				dw_ScattherGatherBufferSize = dw_DMABufferSize;

				dw_AllocatedMemory = dw_AllocatedMemory + dw_DMABufferSize;

				dw_MemoryRemainder = dw_MemoryRemainder - dw_DMABufferSize;

				dw_DMABufferSize = 0;

				b_Interrupt = 0;

				dw_TestNumberOfBufferForOneInterrupt = dw_TestNumberOfBufferForOneInterrupt + 1;

				if (dw_TestNumberOfBufferForOneInterrupt > *pdw_MaxNumberOfBufferForOneInterrupt)
					*pdw_MaxNumberOfBufferForOneInterrupt = dw_TestNumberOfBufferForOneInterrupt;
			}

			/* Save the single element information's */

			/* Save the buffer physical address */
			ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
			dw_BufferPhysicalAddress = dw_ScatterGatherBufferAddress;

			/* Save the local bus address */
			ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
			dw_LocalBusPhysicalAddress = dw_LocalBusPhysicalAddress;

			/* Save the buffer size */
			ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
			dw_BufferSize = dw_ScattherGatherBufferSize;

			/* Test if not the last descriptor */
			if ((dw_DescriptorCounter + 1) < dw_NumberOfDescriptors)
			{
				if ((dw_DescriptorListIndex + 1) < dw_NumberOfDescriptorsForOneBlock)
				{

					/* Save the next descriptor physical address */
					ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
					dw_NextDescriptorAddress = (pdw_DescriptorPhysicalAddress[dw_SelectedList] +
							                   (sizeof (str_PCI9054DMADescriptor) *
									           (dw_DescriptorListIndex + 1))) | 0x5 |
									           (b_TransferDirection << 3);

					/* Set the descriptor list index to the next element */
					dw_DescriptorListIndex = dw_DescriptorListIndex + 1;
				}
				else
				{

					/* Save the next descriptor physical address */
					ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
					dw_NextDescriptorAddress = pdw_DescriptorPhysicalAddress[dw_SelectedList + 1] |
					                           0x5 |
					                           (b_TransferDirection << 3);

					/* Select the next descriptot list */
					dw_SelectedList = dw_SelectedList + 1;

					/* Set the descriptor list index to the firt element */
					dw_DescriptorListIndex = 0;
				}
			}
			else
			{

				/* Set the last descriptor informations */
				ps_PCI9054DMADescriptor[dw_SelectedList][dw_DescriptorListIndex].
				dw_NextDescriptorAddress = 0x5 | (b_TransferDirection << 3);
			}

			/* Save single scatter gather buffer interrupt information's */
			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			dw_Size = dw_ScattherGatherBufferSize;

			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			dw_PhysicalAddress = dw_ScatterGatherBufferAddress;

			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			s_Buffer.
			pb_DMABuffer = pb_ScatterGather;

			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			BigphysareaHandle= BigphysareaHandle;


			(*ps_ScatterGatherDescriptor)->
			s_ScatterGatherSingleBuffer [dw_DescriptorCounter].
			b_Interrupt = b_Interrupt >> 2;

			/* Set next DMA allocated buffer address */

			/* Get the next DMA information's */
			dw_ScatterGatherBufferAddress   = dw_ScatterGatherBufferAddress +
			                                  dw_ScattherGatherBufferSize;
			pb_ScatterGather                = pb_ScatterGather +
			                                  dw_ScattherGatherBufferSize;

			/* Increment the number of descriptors */
			dw_DescriptorCounter = dw_DescriptorCounter + 1;
		}
		while (dw_DescriptorCounter != dw_NumberOfDescriptors);
	}

	/* Test if single acquisition */
	if (b_AcquisitionMode == 0)
	{

		/* Set the end of chain for the last element */
		ps_PCI9054DMADescriptor[dw_NumberOfDescriptorsList - 1][dw_LastDescriptorIndex].
		dw_NextDescriptorAddress = (ps_PCI9054DMADescriptor[dw_NumberOfDescriptorsList - 1][dw_LastDescriptorIndex].
			                   	    dw_NextDescriptorAddress | 0x2) & 0xF;
	}
	else
	{

		/* Set the next element for the last element to the first element */
		ps_PCI9054DMADescriptor[dw_NumberOfDescriptorsList - 1][dw_LastDescriptorIndex].
		dw_NextDescriptorAddress = (pdw_DescriptorPhysicalAddress [0] & 0xFFFFFFF0) |
		                           (ps_PCI9054DMADescriptor[dw_NumberOfDescriptorsList - 1][dw_LastDescriptorIndex].
				                    dw_NextDescriptorAddress & 0xF);
	}

	/* Save global informations */

	/* Save the descriptor global informations */
	(*ps_ScatterGatherDescriptor)->
	dw_AquisitionTotalSize = dw_DMATotalBufferSize;

	(*ps_ScatterGatherDescriptor)->
	b_AcquisitionMode = b_AcquisitionMode;

	(*ps_ScatterGatherDescriptor)->
	dw_FisrtDescriptorAddress = pdw_DescriptorPhysicalAddress [0] |
	(b_TransferDirection << 3);

	// printk ("%s %i dw_NumberOfDescriptors %u\n", __FUNCTION__, __LINE__, dw_NumberOfDescriptors);

	(*ps_ScatterGatherDescriptor)->
	dw_NumberOfScatterGatherBuffer = dw_NumberOfDescriptors;

	{
		int i = 0;

		for (i=0; i<dw_NumberOfDescriptors; i++)
		{
			/* The maximal size of one scatter gather block has to be at most 8 MBytes */
			if ((*ps_ScatterGatherDescriptor)->s_ScatterGatherSingleBuffer [i].dw_Size >= 0x800000)
			{
				printk("%s %d:\tDescriptor pointer 0x%lx size: %u too big (has to be less than 8 MBytes)\n",__FILE__,__LINE__,
						(unsigned long)(*ps_ScatterGatherDescriptor)->s_ScatterGatherSingleBuffer [i].s_Buffer.pb_DMABuffer,
						(*ps_ScatterGatherDescriptor)->s_ScatterGatherSingleBuffer [i].dw_Size);

				return -1;
			}
		}
	}

	(*ps_ScatterGatherDescriptor)->
	b_LocalBusWidth = (uint8_t) w_LocalBusWidth;

	(*ps_ScatterGatherDescriptor)->
	b_NumberOfDescriptorsList = (uint8_t) dw_NumberOfDescriptorsList;

	/* Free the memory */
	kfree (p_BigphysareaHandle);
	kfree (pb_Buffer);
	kfree (ps_PCI9054DMADescriptor);
	kfree (pdw_DescriptorPhysicalAddress);

	return (0);
}

/**
*
* Free the scatter gather description list for the 9054  chip
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return  -                                                      \n
*/
void v_xpci3xxx_Free9054DescriptorList (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint8_t b_DecriptorCounter = 0;

	/* Test if descriptor allocated */
	if (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor != NULL)
	{
		for (b_DecriptorCounter = 0;
 		     b_DecriptorCounter < XPCI3XXX_PRIVDATA(pdev)->
		                          ps_ScatterGatherDescriptor->
		                          b_NumberOfDescriptorsList;
		     b_DecriptorCounter ++)
		{

			/* Test if descriptor list allocated */
			if (XPCI3XXX_PRIVDATA(pdev)->
				ps_ScatterGatherDescriptor->
				pv_FreeDescriptorArrayAddress [b_DecriptorCounter] != NULL)
			{

				/* Free the descriptor list */
				v_xpci3xxx_FreePhysMem (XPCI3XXX_PRIVDATA(pdev)->
					                 	ps_ScatterGatherDescriptor->
						                pv_FreeDescriptorArrayAddress [b_DecriptorCounter],
						                XPCI3XXX_PRIVDATA(pdev)->
						                ps_ScatterGatherDescriptor->
						                dw_FreeDescriptorArraySize[b_DecriptorCounter]);
			}
		}

		/* Free the descriptor struct */
		kfree (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor);
	}
}

/**
*
* Set the 0954 Descriptor list
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in] b_DMAChannel        : DMA channel selection       \n
*                                                    0 : DMA channel 0        \n
*                                                    1 : DMA channel 1        \n
*
*
* @return  -                                                      \n
*/
void v_xpci3xxx_Set9054DescriptorList (struct pci_dev * pdev,
		                               uint8_t b_DMAChannel)
{

	/* Variable declaration */
	uint32_t dw_DMAMode     = 0;
	uint32_t dw_DMADpr      = 0;
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Set the DMA mode                    */
	/* Set the access mode (D0 - D1)       */
	/* Enable the TA#/READY# Input (D6)    */
	/* Enable the scatter gather mode (D9) */
	/* Address fix (D11)                   */
	/* Enable the Demande Mode (D12)       */
	dw_DMAMode = 1UL << 6  |
	             1UL << 9  |
	             1UL << 11 |
	             1UL << 12 |
	             ((XPCI3XXX_PRIVDATA(pdev)->
	               ps_ScatterGatherDescriptor->
	               b_LocalBusWidth / 8) - 1);

	/* Get the first descript address and set descriptor location to PCI Address space */
	dw_DMADpr = XPCI3XXX_PRIVDATA(pdev)->
	            ps_ScatterGatherDescriptor->
	            dw_FisrtDescriptorAddress | 1 | 8;

	/* Set the DMA mode */
	dw_SetMask = dw_DMAMode;
	dw_ResetMask = 0x3UL;

	INPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (b_DMAChannel * 0x14), &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (b_DMAChannel * 0x14), dw_WriteValueDWord);

	/* Set the DMA descriptor pointer */
	dw_SetMask = dw_DMADpr;
	dw_ResetMask = 0xFFFFFFFFUL;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0x90 + (b_DMAChannel * 0x14), &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x90 + (b_DMAChannel * 0x14), dw_WriteValueDWord);


	/* Transfer disabled */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 1;

	/* Save the DMA channel */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_DMAChannel = b_DMAChannel;

}

/**
*
* Reset the 0954 Descriptor list
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Reset9054DescriptorList (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t dw_DMAMode     = 0;
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Reset the DMA mode                   */
	/* Reset the access mode (D0 - D1)      */
	/* Disable the TA#/READY# Input (D6)    */
	/* Disable the scatter gather mode (D9) */
	/* Disable the Demande Mode (D12)       */
	dw_DMAMode = 1UL << 6  |
                 1UL << 9  |
	             1UL << 12 |
	             ((XPCI3XXX_PRIVDATA(pdev)->
			       ps_ScatterGatherDescriptor->
			       b_LocalBusWidth / 8) - 1);

	/* Set the DMA mode */
	dw_SetMask = 0;
	dw_ResetMask = dw_DMAMode;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 0x14), &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 0x14), dw_WriteValueDWord);

	/* Transfer not initialised */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 0;

}

/**
*
* Enable the 9054 DMA transfer
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Enable9054DMATransfer (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Enable the DMA transfer */
	dw_SetMask = 1 << (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8);
	dw_ResetMask = 0;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, dw_WriteValueDWord);

	/* Transfer enabled */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 2;

}

/**
*
* Start the 9054 DMA transfer
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Start9054DMATransfer (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Start the DMA transfer */
	dw_SetMask = 2 << (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8);
	dw_ResetMask = 0;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, dw_WriteValueDWord);

	/* Transfer started */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 3;

}

/**
*
* Pause the 9054 DMA transfer
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Pause9054DMATransfer (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Pause the DMA transfer */
	dw_SetMask = 0;
	dw_ResetMask = 1 << (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8);
	INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, dw_WriteValueDWord);

	/* Transfer pause */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 4;

}

/**
*
* Continue the 9054 DMA transfer
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Continue9054DMATransfer (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Continue the DMA transfer */
	dw_SetMask = 3 << (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8);
	dw_ResetMask = 0;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, dw_WriteValueDWord);

	/* Transfer started */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 3;
}

/**
*
* Abort the 9054 DMA transfer
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Abort9054DMATransfer (struct pci_dev * pdev)
{
	uint32_t dw_Value;

	/* Variable declaration */
	uint32_t  	dw_ResetMask = 0;
	uint32_t  	dw_SetMask = 0;
	uint32_t  	dw_WriteValueDWord = 0;

	uint32_t 	dw_TimeOut = 0;
	uint8_t	b_OverFlow = 0;

	/* Abort the DMA transfer */
	dw_SetMask = 6 << (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8);
	dw_ResetMask = 1 << (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8);
	INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, dw_WriteValueDWord);

	dw_TimeOut = jiffies + HZ; // time out in 1 s
	if (jiffies > dw_TimeOut)
		b_OverFlow = 1;

	/* Wait the DMA done */
	do
	{
		if (((b_OverFlow == 0) && (dw_TimeOut > jiffies)) || ((b_OverFlow == 1) && ((dw_TimeOut > jiffies) || (jiffies <= 4294967295UL))))
		{
			INPDW_MEM(GET_MEM_BAR0(pdev), 0xA8, &dw_Value);
		}
		else
		{
			//				printk("%s:%d - Time out occur\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__
			break;
		}
	}
	while (((dw_Value >> (4 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 8))) & 1) != 1);


	/* Transfer list not selected */
	XPCI3XXX_PRIVDATA(pdev)->
	ps_ScatterGatherDescriptor->
	b_TransferStatus = 0;

}

/**
*
* Enable the 9054 DMA transfer interrupt
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Enable9054DMATransferInterrupt (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Enable the Local Interrupt Output (D16)               */
	/* Enable the Local DMA Channel X Interrupt (D18 or D19) */
	dw_SetMask = 1UL << 16 | 1UL << (18 + XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel);
	dw_ResetMask = 0;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0x68, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x68, dw_WriteValueDWord);

	/* Enable the Done Interrupt (D10) */
	dw_SetMask = 1UL << 10;
	dw_ResetMask = 0;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 0x14), &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 0x14), dw_WriteValueDWord);

}

/**
*
* Disable the 9054 DMA transfer interrupt
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return   -                                                     \n
*/
void v_xpci3xxx_Disable9054DMATransferInterrupt (struct pci_dev * pdev)
{

	/* Variable declaration */
	uint32_t  dw_ResetMask = 0;
	uint32_t  dw_SetMask = 0;
	uint32_t  dw_WriteValueDWord = 0;

	/* Disable the Local DMA Channel X Interrupt (D18 or D19)*/
	dw_SetMask = 0;
	dw_ResetMask = 1UL << (18 + XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel);
	INPDW_MEM(GET_MEM_BAR0(pdev), 0x68, &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x68, dw_WriteValueDWord);

	/* Disable the Done Interrupt (D10)*/
	dw_SetMask = 0;
	dw_ResetMask = 1UL << 10;
	INPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 0x14), &dw_WriteValueDWord);
	dw_WriteValueDWord = (dw_WriteValueDWord & (0xFFFFFFFFUL - dw_ResetMask)) | dw_SetMask;
	OUTPDW_MEM(GET_MEM_BAR0(pdev), 0x80 + (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->b_DMAChannel * 0x14), dw_WriteValueDWord);

}

/* Main functions                                      */

/**
*
* Init the analog input DMA.
*
* @param[in] pdev : PCI Device struct                \n
* @param[in] dw_NbrOfChannel   : Number of channel in the      \n
*                                               sequence.                     \n
* @param[in] dw_InterruptSequenceCounter : Number of sequences.      \n
*                                      A sequence is composed of the channels \n
* @param[in] dw_InterruptSequenceCounter : Number of sequence before \n
*                                                   a interrupt are generated \n
* @param[in] b_AcquisitionMode            : 0 : Single mode     \n
*                                                         1 : Continuous mode \n
*
*
* @return  0: No error                                            \n
*                      1: Error by memory allocation                          \n
*                      2: Error by descriptor list creation                   \n
*/
int i_xpci3xxx_InitAnalogInputDMA (struct pci_dev * pdev,
		                           uint32_t                    dw_NbrOfChannel,
		                           uint32_t                    dw_SequenceCounter,
		                           uint32_t                    dw_InterruptSequenceCounter,
		                           uint8_t					   b_AcquisitionMode)
{

	/* Variable declaration and initialisation */
	uint32_t dw_DMATotalBufferSize               = 0;
	uint32_t dw_MaxNumberOfBufferForOneInterrupt = 0;
	int      i_ReturnValue                      = 0;

	dw_InterruptSequenceCounter = 2 * dw_InterruptSequenceCounter * dw_NbrOfChannel;

	/* Get the sequence acquisition buffer size */
	if (dw_SequenceCounter == 0)
		dw_DMATotalBufferSize = dw_InterruptSequenceCounter;
	else
		dw_DMATotalBufferSize = 2 * dw_SequenceCounter * dw_NbrOfChannel;

	/* Create the shared memory */
	i_ReturnValue = i_xpci3xxx_CreateScatterGatherMemory (dw_DMATotalBufferSize, // Number total byte
			                                              b_AcquisitionMode, // 0 : Single mode / 1 : Continuous mode
			                                              0x1000000,// 0x800000, // Max byte size for one (8 MBytes)
			                                              32, // 32-bit bus
			                                              &XPCI3XXX_PRIVDATA(pdev)->s_ScatterGatherPCIDMAInformation);

	if (i_ReturnValue != 0)
	{
		printk("i_xpci3xxx_CreateScatterGatherMemory error : %d\n",i_ReturnValue);
		return 1;
	}

	if ((pdev->device == xpci3002_16_BOARD_DEVICE_ID) ||
		(pdev->device == xpci3002_8_BOARD_DEVICE_ID) ||
		(pdev->device == xpci3002_4_BOARD_DEVICE_ID) ||
		(pdev->device == xpci3003_BOARD_DEVICE_ID) ||
		(pdev->device == xpci3002_16_AUDI_BOARD_DEVICE_ID))
	{
		/* Create the scatter gather list */
		i_ReturnValue = i_xpci3xxx_Create9054DescriptorList (dw_InterruptSequenceCounter,
				                                             dw_DMATotalBufferSize,
				                                             &XPCI3XXX_PRIVDATA(pdev)->s_ScatterGatherPCIDMAInformation,
				                                             b_AcquisitionMode,
				                                             1,
				                                             448,
				                                             32,
				                                             &dw_MaxNumberOfBufferForOneInterrupt,
				                                             &XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor);
	}
	else
	{
		/* Create the scatter gather list */
		i_ReturnValue = i_xpci3xxx_Create9054DescriptorList (dw_InterruptSequenceCounter,
				                                             dw_DMATotalBufferSize,
				                                             &XPCI3XXX_PRIVDATA(pdev)->s_ScatterGatherPCIDMAInformation,
				                                             b_AcquisitionMode,
				                                             1,
				                                             340,
				                                             32,
				                                             &dw_MaxNumberOfBufferForOneInterrupt,
				                                             &XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor);
	}

	if (i_ReturnValue != 0)
	{
		printk("i_xpci3xxx_Create9054DescriptorList error : %d\n",i_ReturnValue);
		return 2;
	}

	// 	printk("XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor->s_ScatterGatherSingleBuffer [0].BigphysareaHandle = %X\n",XPCI3XXX_PRIVDATA(pdev)->													ps_ScatterGatherDescriptor->
	// 													s_ScatterGatherSingleBuffer [0].
	// 												//	s_Buffer.
	// 													BigphysareaHandle);
	XPCI3XXX_PRIVDATA(pdev)->kmalloc_area = (char*) XPCI3XXX_PRIVDATA(pdev)->
	                                                ps_ScatterGatherDescriptor->
	                                                s_ScatterGatherSingleBuffer [0].
	                                                BigphysareaHandle;

	return 0;
}

/**
*
* Start the analog input DMA.
*
* @param[in]  pdev : PCI Device struct               \n
*
*
* @return  0: No error                                            \n
*/
int i_xpci3xxx_StartAnalogInputDMA (struct pci_dev * pdev)
{

	//	printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

	/* Set the descriptor list */
	v_xpci3xxx_Set9054DescriptorList (pdev, 0);

	/* Enable the DMA transfer */
	v_xpci3xxx_Enable9054DMATransfer (pdev);

	/* Start the DMA transfer */
	v_xpci3xxx_Start9054DMATransfer (pdev);

	/* Enable the DMA transfer interrupt */
	v_xpci3xxx_Enable9054DMATransferInterrupt (pdev);

	return 0;

}

/**
*
* Stop the analog input DMA.
*
* @param[in] pdev : PCI Device struct               \n
*
*
* @return  0: No error                                            \n
*/
int i_xpci3xxx_StopAnalogInputDMA (struct pci_dev * pdev)
{

	if (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor != NULL)
	{

		//		printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

		/* Disable the DMA transfer interrupt */
		v_xpci3xxx_Disable9054DMATransferInterrupt  (pdev);

		/* Abort the DMA transfer */
		v_xpci3xxx_Abort9054DMATransfer  (pdev);

		/* Stop the DMA transfer */
		v_xpci3xxx_Pause9054DMATransfer  (pdev);

		/* Reset the descriptor list */
		v_xpci3xxx_Reset9054DescriptorList  (pdev);

	}

	return 0;
}


/**
*
* Release the analog input DMA.
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @return  0: No error                                            \n
*/
int i_xpci3xxx_ReleaseAnalogInputDMA (struct pci_dev * pdev)
{

	/* First disable DMA (stop) OK */

	if (XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor != NULL)
	{

		//		printk("%s:%d\n",__FUNCTION__,__LINE__); // __FILE__,__FUNCTION__,__LINE__

		/* Free the descriptor list */
		v_xpci3xxx_Free9054DescriptorList (pdev);

		/* Free the scatter gather memory */
		v_xpci3xxx_FreeScatterGatherMemory  (&XPCI3XXX_PRIVDATA(pdev)->s_ScatterGatherPCIDMAInformation);

		XPCI3XXX_PRIVDATA(pdev)->ps_ScatterGatherDescriptor = NULL;

		XPCI3XXX_PRIVDATA(pdev)->kmalloc_area = NULL;
	}

	return 0;
}

