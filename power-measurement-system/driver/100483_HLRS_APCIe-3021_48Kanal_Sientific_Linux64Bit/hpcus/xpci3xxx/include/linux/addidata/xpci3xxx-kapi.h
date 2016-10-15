#ifndef __xpci3xxx_KAPI__
#define __xpci3xxx_KAPI__


#ifndef __KERNEL__
	#error This header is intended for use in kernel context only
#endif

/* SHARED MEMORY MANAGEMENT */
int i_xpci3xxx_AllocateSharedMemory (struct pci_dev * pdev, uint32_t dw_Size);

int i_xpci3xxx_ReleaseSharedMemory (struct pci_dev * pdev, uint32_t dw_Size);

int i_xpci3xxx_GetSharedMemoryPointer (struct pci_dev * pdev, uint32_t dw_Size, void **pdw_Value);

/* TTL + DIGITAL INPUT + DIGITAL OUTPUT */
int  i_xpci3xxx_SetTTLPortConfiguration(struct pci_dev * pdev, uint8_t b_TTLPortConfiguration);

int i_xpci3xxx_SetDigitalOutputRegister (struct pci_dev * pdev, uint32_t dw_DigitalOutputRegister);

int  i_xpci3xxx_SetDigitalOutputPort(struct pci_dev * pdev);

int  i_xpci3xxx_SetTTLOutputPort(struct pci_dev * pdev);

int  i_xpci3xxx_GetDigitalOutputPort(struct pci_dev * pdev, uint32_t* pdw_DigitalOutputRegister);

int  i_xpci3xxx_GetTTLOutputPort(struct pci_dev * pdev, uint32_t* pdw_TTLOutputRegister);

int  i_xpci3xxx_ReadDigitalInputPort(struct pci_dev * pdev, uint32_t* pdw_DigitalInputRegister);

int  i_xpci3xxx_ReadTTLInputPort(struct pci_dev * pdev, uint32_t* pdw_TTLInputRegister);

int i_xpci3xxx_InitDigitalInputModuleFilter (struct pci_dev * pdev,
                                                uint8_t                                    b_FilterFlag,
                                                uint32_t                                  dw_ReloadValue);

/* ANALOG INPUT */
int  i_xpci3xxx_InitAnalogInput (struct pci_dev * pdev,
                                	uint8_t   b_Channel,
                                	uint8_t   b_SingleDiff,
                                	uint8_t   b_Gain,
                                	uint8_t   b_Polarity);

int  i_xpci3xxx_StartAnalogInput (struct pci_dev * pdev,
	                                uint8_t   b_Channel,
	  		                		uint8_t   b_ConvertTimeUnit,
			                		uint16_t   w_ConvertTime);

int i_xpci3xxx_ReadAnalogInputBit (struct pci_dev * pdev, uint8_t* pb_AnalogInputConversionFinished);

int i_xpci3xxx_ReadAnalogInputValue (struct pci_dev * pdev,
                                     uint32_t* pdw_ReadValue);

int i_xpci3xxx_InitAnalogInputAutoRefresh (struct pci_dev * pdev,
                                            uint8_t    b_NumberOfChannels,
                                            uint8_t*  pb_ChannelList,
			                                uint8_t*  pb_Gain,
			                                uint8_t*  pb_Polarity,
			                                uint8_t    b_SingleDiff,
                                            uint32_t  dw_NumberOfSequence,
                                            uint8_t    b_DelayMode,
                                            uint8_t    b_DelayTimeUnit,
                                            uint16_t    w_DelayTime);

int i_xpci3xxx_StartAnalogInputAutoRefresh (struct pci_dev * pdev,
												uint8_t   b_ConvertTimeUnit,
                                                uint16_t   w_ConvertTime);

int i_xpci3xxx_StopAnalogInputAutoRefresh (struct pci_dev * pdev);

int i_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter (struct pci_dev * pdev,
			                                                uint32_t pdw_ReadValue[ADDIDATA_MAX_AI],
			                                                uint32_t* pdw_CycleIndex);

/** Read the specified numbers of auto refresh values.
 *
 * This function doesn't check parameters validity.
 * It is written to have a short execution time.
 * Use it only if you are sure of what you are doing.
 *
 * @param[in] pdev				: PCI Device struct.
 * @param[in] dw_NbrOfChannels	: Specified the channel number up to which is to be read (eg.: 0 to 15 dw_NbrOfChannels = 15).
 * @param[out] pdw_ReadValue	: A buffer of 32 bits unsigned integer of size ADDIDATA_MAX_AI, containing the acquired data.
 * @param[out] pdw_CycleIndex	: Current cycle index.
 *
 * @retval 0 Always 0.
 */
int i_xpci3xxx_FastReadAnalogInputAutoRefreshValueAndCounter (struct pci_dev * pdev,
															uint32_t dw_NbrOfChannels,
			                                                uint32_t *pdw_ReadValue,
			                                                uint32_t *pdw_CycleIndex);

int i_xpci3xxx_ReleaseAnalogInputAutoRefresh (struct pci_dev * pdev);

int i_xpci3xxx_InitAnalogInputSequence (struct pci_dev * pdev,
                                            uint8_t    b_NumberOfChannels,
                                            uint8_t*  pb_ChannelList,
			                                uint8_t*  pb_Gain,
			                                uint8_t*  pb_Polarity,
			                                uint8_t    b_SingleDiff,
                                            uint32_t  dw_NumberOfSequence,
											uint8_t    b_UseDMA,
											uint32_t  dw_NumberOfSequenceForEachInterrupt,
                                            uint8_t    b_DelayMode,
                                            uint8_t    b_DelayTimeUnit,
                                            uint16_t    w_DelayTime);

int i_xpci3xxx_StartAnalogInputSequence (struct pci_dev * pdev,
												uint8_t   b_ConvertTimeUnit,
                                                uint16_t   w_ConvertTime);

int i_xpci3xxx_StopAnalogInputSequence (struct pci_dev * pdev);

int i_xpci3xxx_ReleaseAnalogInputSequence (struct pci_dev * pdev);

int i_xpci3xxx_InitAnalogInputDMA (struct pci_dev * pdev,
                                     uint32_t                    dw_NbrOfChannel,
									 uint32_t                    dw_SequenceCounter,
									 uint32_t                    dw_InterruptSequenceCounter,
									 uint8_t					   b_AcquisitionMode);

int i_xpci3xxx_StartAnalogInputDMA (struct pci_dev * pdev);

int i_xpci3xxx_StopAnalogInputDMA (struct pci_dev * pdev);

int i_xpci3xxx_ReleaseAnalogInputDMA (struct pci_dev * pdev);

//void v_xpci3xxx_Free9054DescriptorList (struct pci_dev * pdev);
//
//void v_xpci3xxx_Set9054DescriptorList (struct pci_dev * pdev, uint8_t b_DMAChannel);
//
void v_xpci3xxx_Reset9054DescriptorList (struct pci_dev * pdev);

//void v_xpci3xxx_Enable9054DMATransfer (struct pci_dev * pdev);
//
//void v_xpci3xxx_Start9054DMATransfer (struct pci_dev * pdev);
//
void v_xpci3xxx_Pause9054DMATransfer (struct pci_dev * pdev);

//void v_xpci3xxx_Continue9054DMATransfer
//      (struct pci_dev * pdev,
//       pstr_ScatterGatherInterruptDescriptor ps_ScatterGatherDescriptor);
//
void v_xpci3xxx_Abort9054DMATransfer (struct pci_dev * pdev);

//void v_xpci3xxx_Enable9054DMATransferInterrupt
//      (struct pci_dev * pdev,
//       pstr_ScatterGatherInterruptDescriptor ps_ScatterGatherDescriptor);
//
void v_xpci3xxx_Disable9054DMATransferInterrupt (struct pci_dev * pdev);

int i_xpci3xxx_EnableDisableAnalogInputHardwareTrigger (struct pci_dev * pdev,
							                                uint8_t   b_HardwareTrigger,
															uint8_t     b_HardwareTriggerLevel,
															uint8_t     b_HardwareTriggerAction,
															uint32_t    dw_HardwareTriggerCount);

int i_xpci3xxx_GetAnalogInputHardwareTriggerStatus (struct pci_dev * pdev,
										              uint8_t*   pb_HardwareTriggerStatus,
										              uint32_t* pdw_HardwareTriggerCount,
										              uint8_t*   pb_HardwareTriggerState);

int i_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger (struct pci_dev * pdev,
							                                uint8_t   b_SoftwareTrigger,
											                uint8_t   b_SoftwareTriggerAction);

int i_xpci3xxx_AnalogInputSoftwareTrigger (struct pci_dev * pdev);

int i_xpci3xxx_GetAnalogInputSoftwareTriggerStatus (struct pci_dev * pdev,
										              uint8_t*   pb_SoftwareTriggerStatus);

int i_xpci3xxx_EnableDisableAnalogInputHardwareGate (struct pci_dev * pdev,
							                                uint8_t   b_HardwareGate,
															uint8_t     b_HardwareGateLevel);

int i_xpci3xxx_GetAnalogInputHardwareGateStatus (struct pci_dev * pdev,
										              uint8_t*   pb_HardwareGateStatus);

/* ANALOG OUTPUT */
int i_xpci3xxx_InitAnalogOutput (struct pci_dev * pdev,
                                uint8_t   b_Channel,
                                uint8_t   b_VoltageMode);

int i_xpci3xxx_ReadAnalogOutputBit (struct pci_dev * pdev, uint8_t* pb_AnalogOutputConversionFinished);

int i_xpci3xxx_WriteAnalogOutputValue (struct pci_dev * pdev,
										uint8_t   b_Channel,
										uint32_t dw_WriteValue);

/** Writes the value of the analog output channel.
 *
 * This function doesn't check parameters validity.
 * It is written to have a short execution time.
 * Use it only if you are sure of what you are doing.
 *
 * @param[in] pdev			: PCI Device struct.
 * @param[in] b_Channel		: channel number of the output to be initialise.
 * @param[in] dw_WriteValue	: Value to write on the analog output channel.

 * @retval 0 Always 0.
 */
int i_xpci3xxx_FastWriteAnalogOutputValue (struct pci_dev * pdev, uint8_t b_Channel, uint32_t dw_WriteValue);

/* TIMER */
int  i_xpci3xxx_EnableDisableTimerHardwareTrigger (struct pci_dev *pdev, uint8_t b_TimerNumber, uint8_t b_HardwareTriggerFlag, uint8_t b_HardwareTriggerLevel);

int  i_xpci3xxx_InitTimer                      (struct pci_dev *pdev,
                                                         uint8_t   b_TimerNumber,
                                                         uint8_t   b_TimerMode,
                                                         uint8_t   b_TimerTimeUnit,
                                                         uint32_t dw_ReloadValue);

int  i_xpci3xxx_ReleaseTimer                    (struct pci_dev *pdev,
                                                         uint8_t   b_TimerNumber);

int  i_xpci3xxx_StartTimer                    (struct pci_dev *pdev,
                                                         uint8_t   b_TimerNumber);

int  i_xpci3xxx_StartAllTimers                    (struct pci_dev *pdev);

int  i_xpci3xxx_TriggerTimer                    (struct pci_dev *pdev,
                                                         uint8_t   b_TimerNumber);

int  i_xpci3xxx_TriggerAllTimers                    (struct pci_dev *pdev);

int  i_xpci3xxx_StopTimer                    (struct pci_dev *pdev,
                                                         uint8_t   b_TimerNumber);

int  i_xpci3xxx_StopAllTimers                    (struct pci_dev *pdev);

int  i_xpci3xxx_ReadTimerValue  (struct pci_dev *pdev,
                                    uint8_t     b_TimerNumber,
                                    uint32_t* pdw_TimerValue);

int  i_xpci3xxx_ReadTimerStatus  (struct pci_dev *pdev,
                                     uint8_t     b_TimerNumber,
                                     uint8_t* pb_TimerStatus,
                                     uint8_t* pb_SoftwareTriggerStatus,
                                     uint8_t* pb_HardwareTriggerStatus);

int  i_xpci3xxx_EnableDisableTimerInterrupt          (struct pci_dev *pdev,
                                                         uint8_t   b_TimerNumber,
                                                         uint8_t   b_InterruptFlag);

int  i_xpci3xxx_EnableDisableTimerHardwareOutput     (struct pci_dev *pdev,
                                                        uint8_t   b_TimerNumber,
														uint8_t   b_OutputFlag,
                                                        uint8_t   b_OutputLevel);

int  i_xpci3xxx_GetTimerHardwareOutputStatus  (struct pci_dev *pdev,
			                                    uint8_t     b_TimerNumber,
			                                    uint8_t*    pb_HardwareOutputStatus);

/* COUNTER */
int  i_xpci3xxx_InitCounter                      (struct pci_dev *pdev,
						                                uint8_t     b_CounterNumber,
														uint8_t   b_DirectionSelection,
														uint8_t   b_LevelSelection,
														uint32_t dw_ReloadValue);

int  i_xpci3xxx_ReleaseCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber);

int  i_xpci3xxx_StartCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber);

int  i_xpci3xxx_StartAllCounters                    (struct pci_dev *pdev);

int  i_xpci3xxx_TriggerCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber);

int  i_xpci3xxx_TriggerAllCounters                    (struct pci_dev *pdev);

int  i_xpci3xxx_StopCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber);

int  i_xpci3xxx_StopAllCounters                    (struct pci_dev *pdev);

int  i_xpci3xxx_ClearCounter                    (struct pci_dev *pdev,
                                                         uint8_t   b_CounterNumber);

int  i_xpci3xxx_ReadCounterValue  (struct pci_dev *pdev,
									uint8_t   b_CounterNumber,
                                    uint32_t* pdw_CounterValue);

int  i_xpci3xxx_ReadCounterStatus  (struct pci_dev *pdev,
										uint8_t   b_CounterNumber,
										uint8_t* pb_CounterStatus,
										uint8_t* pb_SoftwareTriggerStatus,
										uint8_t* pb_HardwareTriggerStatus,
										uint8_t* pb_SoftwareClearStatus);

int  i_xpci3xxx_EnableDisableCounterInterrupt          (struct pci_dev *pdev,
															uint8_t   b_CounterNumber,
															uint8_t   b_InterruptFlag);

int  i_xpci3xxx_EnableDisableCounterHardwareOutput     (struct pci_dev *pdev,
                                                        uint8_t   b_CounterNumber,
														uint8_t   b_OutputFlag,
                                                        uint8_t   b_OutputLevel);

int  i_xpci3xxx_GetCounterHardwareOutputStatus  (struct pci_dev *pdev,
			                                    uint8_t     b_CounterNumber,
			                                    uint8_t*    pb_HardwareOutputStatus);

/* WATCHDOG */
int  i_xpci3xxx_InitWatchdog                      (struct pci_dev *pdev,
													uint8_t   b_WatchdogNumber,
													uint8_t   b_WatchdogTimeUnit,
													uint32_t dw_ReloadValue);

int  i_xpci3xxx_ReleaseWatchdog                    (struct pci_dev *pdev,
													uint8_t   b_WatchdogNumber);

int  i_xpci3xxx_StartWatchdog                    (struct pci_dev *pdev,
													uint8_t   b_WatchdogNumber);

int  i_xpci3xxx_StartAllWatchdogs                    (struct pci_dev *pdev);

int  i_xpci3xxx_TriggerWatchdog                    (struct pci_dev *pdev,
													uint8_t   b_WatchdogNumber);

int  i_xpci3xxx_TriggerAllWatchdogs                    (struct pci_dev *pdev);

int  i_xpci3xxx_StopWatchdog                    (struct pci_dev *pdev,
													uint8_t   b_WatchdogNumber);

int  i_xpci3xxx_StopAllWatchdogs                    (struct pci_dev *pdev);

int  i_xpci3xxx_ReadWatchdogValue  (struct pci_dev *pdev,
										uint8_t   b_WatchdogNumber,
										uint32_t* pdw_WatchdogValue);

int  i_xpci3xxx_ReadWatchdogStatus  (struct pci_dev *pdev,
										uint8_t   b_WatchdogNumber,
										uint8_t* pb_WatchdogStatus,
										uint8_t* pb_SoftwareTriggerStatus,
										uint8_t* pb_HardwareTriggerStatus);

int  i_xpci3xxx_EnableDisableWatchdogInterrupt          (struct pci_dev *pdev,
															uint8_t   b_WatchdogNumber,
															uint8_t   b_InterruptFlag);

int  i_xpci3xxx_EnableDisableWatchdogHardwareOutput     (struct pci_dev *pdev,
                                                        uint8_t   b_WatchdogNumber,
														uint8_t   b_OutputFlag,
                                                        uint8_t   b_OutputLevel);

int  i_xpci3xxx_GetWatchdogHardwareOutputStatus  (struct pci_dev *pdev,
			                                    uint8_t     b_WatchdogNumber,
			                                    uint8_t*    pb_HardwareOutputStatus);



/** Get interrupt value.
 *
 * @param [in]	mask	Interrupt source has to be given to select the right value management.
 *
 * @param [out]	pdev	The handle of the board that generates the interrupt.
 * @param [out]	mask	The board minor number.<br>
 * 						If 0x7ffffffe, fifo is empty.
 * @param [out]	value	Number of buffer and number of value for each buffer, buffer address.
 * @param [out]	shm		A pointer on the shared memory.
 *
 * @retval	0	No error.
 */
int i_xpci3xxx_TestInterrupt(struct pci_dev * pdev, uint32_t *mask, uint32_t *value, uint16_t **shm);

int xpci3xxx_SetIntCallback( void(*InterruptCallback) (struct pci_dev * pdev, uint32_t dw_InterruptSource));

int xpci3xxx_ResetIntCallback(void);

/** Get the number of analog input channels.
 *
 * @param [in] pdev				: The device to use.
 * @param [in] b_SingleDiff		: The mode of the input channels ADDIDATA_DIFFERENTIAL or ADDIDATA_SINGLE.
 *
 * @param [out] b_NbrChannels	: The number of analog input channels.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */
int i_xpci3xxx_GetNumberOfAnalogInputChannels (struct pci_dev *pdev, uint8_t b_SingleDiff, uint8_t *b_NbrChannels);

/** Get the number of analog output channels.
 *
 * @WARNING:	To get the real number of channels of the APCI-3501<br>
 * 				you have to edit _makefile:<br>
 * 				echo "//#define USE_ADDIEEPROM	1" >> xpci3xxx-options.h; \
 * 				and remove // in the previous line.
 * 				This add functions to read the board header.
 * 				Once USE_ADDIEEPROM is defined you have to load
 * 				AddiEEPROM module before the xpci3xxx module.
 *
 * @param [in] pdev				: The device to use.
 * @param [in] b_NbrChannels	: The number of analog output channels.
 *
 * @retval 0: No error.
 * @retval 1: The handle parameter of the board is wrong.
 */
int i_xpci3xxx_GetNumberOfAnalogOutputChannels (struct pci_dev *pdev, uint8_t *b_NbrChannels);

/** find a xpci3xxx device of a given index in the system PCI device list.
 * @param index The index (!!minor number!!) to lookup
 * @return A pointer to the device or NULL
 *
 * This function is used to map a minor number to an actual device.
 */
extern struct pci_dev * xpci3xxx_lookup_board_by_index(unsigned int index);

//----------------------------------------------------------------------------
/** return a pointer to the lock protecting the board */
extern spinlock_t * xpci3xxx_get_lock(struct pci_dev *pdev);
//----------------------------------------------------------------------------
/** lock the board using spin_lock_irqsave(), disabling local software and hardware interrupts
 * @param[in] pdev The device to acquire.
 * @param[out] flags interuption flag used with unlock()
 *
 * This function is to be used before calling any kAPI function; BUT not in user interrupt handler where the board is already acquired.
 *
 * @warning lock aren't reentrant, that means that you can not nest call to lock()
 *
 */
static inline void xpci3xxx_lock(struct pci_dev *pdev, unsigned long * flags)
{
    spin_lock_irqsave(xpci3xxx_get_lock(pdev), *flags);
}

//----------------------------------------------------------------------------
/** unlock the board using spin_lock_irqrestore()
 * @param[in] pdev The device to acquire.
 * @param[out] flags interuption flag initialised by lock()
 *
 * This function is to be used before calling any kAPI function; BUT not in user interrupt handler where the board is already acquired.
 *
 *
 *
 */
static inline void xpci3xxx_unlock(struct pci_dev *pdev, unsigned long flags)
{
	spin_unlock_irqrestore(xpci3xxx_get_lock(pdev), flags);
}


#endif

