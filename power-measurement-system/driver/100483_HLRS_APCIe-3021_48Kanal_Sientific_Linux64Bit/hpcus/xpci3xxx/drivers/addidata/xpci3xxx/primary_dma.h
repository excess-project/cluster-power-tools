#ifndef __XPCI3XXX_PRIMARY_DMA_H__
#define __XPCI3XXX_PRIMARY_DMA_H__

int i_xpci3xxx_AllocPhysMem (caddr_t *BigphysareaHandle,void** pv_DMABuffer, uint32_t * pdw_PhysicalAddress, uint32_t dw_AllocatedMemory);

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
void v_xpci3xxx_FreePhysMem  (caddr_t BigphysareaHandle, uint32_t dw_AllocatedMemory);

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
int i_xpci3xxx_CreateScatterGatherMemory 
					  (uint32_t                                   dw_DMATotalBufferSize,
					   uint8_t                                     b_AcquisitionMode,
					   uint32_t                                   dw_MaxBufferAllocationSize,
					   uint16_t                                     w_LocalBusWidth,
					   str_ScatterGatherPCIDMAInformation *    ps_ScatterGatherPCIDMAInformation);

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
void v_xpci3xxx_FreeScatterGatherMemory (str_ScatterGatherPCIDMAInformation * ps_ScatterGatherPCIDMAInformation);

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
int i_xpci3xxx_Create9054DescriptorList  
      (uint32_t                                   dw_SingleBufferSize,
       uint32_t                                   dw_DMATotalBufferSize,
       str_ScatterGatherPCIDMAInformation *    ps_ScatterGatherPCIDMAInformation,
       uint8_t                                     b_AcquisitionMode,
       uint8_t                                     b_TransferDirection,
       uint32_t                                   dw_LocalBusPhysicalAddress,
       uint16_t                                     w_LocalBusWidth,
       uint32_t*                                 pdw_MaxNumberOfBufferForOneInterrupt,
       pstr_ScatterGatherInterruptDescriptor * ps_ScatterGatherDescriptor);

/**
 *
 * Free the scatter gather description list for the 9054  chip       
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return  -                                                      \n
 */
void v_xpci3xxx_Free9054DescriptorList (struct pci_dev * pdev);

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
void v_xpci3xxx_Set9054DescriptorList  (struct pci_dev * pdev, uint8_t b_DMAChannel);

/**
 *
 * Reset the 0954 Descriptor list                         
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Reset9054DescriptorList  (struct pci_dev * pdev);

/**
 *
 * Enable the 9054 DMA transfer                           
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Enable9054DMATransfer(struct pci_dev * pdev);

/**
 *
 * Start the 9054 DMA transfer                            
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Start9054DMATransfer(struct pci_dev * pdev);

/**
 *
 * Pause the 9054 DMA transfer                            
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Pause9054DMATransfer (struct pci_dev * pdev);

/**
 *
 * Continue the 9054 DMA transfer                         
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Continue9054DMATransfer (struct pci_dev * pdev);

/**
 *
 * Abort the 9054 DMA transfer                            
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Abort9054DMATransfer (struct pci_dev * pdev);

/**
 *
 * Enable the 9054 DMA transfer interrupt                 
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Enable9054DMATransferInterrupt (struct pci_dev * pdev);
/**
 *
 * Disable the 9054 DMA transfer interrupt                
 *
 * @param[in]  pdev : PCI Device struct              \n
 *
 *
 * @return   -                                                     \n
 */
void v_xpci3xxx_Disable9054DMATransferInterrupt (struct pci_dev * pdev);

#endif // __XPCI3XXX_PRIMARY_DMA_H__
