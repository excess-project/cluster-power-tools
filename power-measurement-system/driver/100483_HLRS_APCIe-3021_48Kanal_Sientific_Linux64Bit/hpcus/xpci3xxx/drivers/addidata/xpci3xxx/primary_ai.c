/** @file primary_ai.c
*
* @author Rodolphe Hilpert
*
* @date 05.01.06
*
* @version  $LastChangedRevision$
* @version $LastChangedDate$
*
* Primary functions (hardware access) for the analog input functionality of the board.
*
*/

/** @par LICENCE
* @verbatim
* @endverbatim
*/

#include "xpci3xxx-private.h"
#include "iomap/analog_input.h"

EXPORT_SYMBOL(i_xpci3xxx_InitAnalogInput);
EXPORT_SYMBOL(i_xpci3xxx_StartAnalogInput);
EXPORT_SYMBOL(i_xpci3xxx_ReadAnalogInputBit);
EXPORT_SYMBOL(i_xpci3xxx_ReadAnalogInputValue);

EXPORT_SYMBOL(i_xpci3xxx_InitAnalogInputAutoRefresh);
EXPORT_SYMBOL(i_xpci3xxx_StartAnalogInputAutoRefresh);
EXPORT_SYMBOL(i_xpci3xxx_StopAnalogInputAutoRefresh);
EXPORT_SYMBOL(i_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter);
EXPORT_SYMBOL(i_xpci3xxx_FastReadAnalogInputAutoRefreshValueAndCounter);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseAnalogInputAutoRefresh);

EXPORT_SYMBOL(i_xpci3xxx_InitAnalogInputSequence);
EXPORT_SYMBOL(i_xpci3xxx_StartAnalogInputSequence);
EXPORT_SYMBOL(i_xpci3xxx_StopAnalogInputSequence);
EXPORT_SYMBOL(i_xpci3xxx_ReleaseAnalogInputSequence);

EXPORT_SYMBOL(i_xpci3xxx_GetNumberOfAnalogInputChannels);

//------------------------------------------------------------------------------
static inline int is_boolean(int val)
{
	switch(val)
	{
	case ADDIDATA_ENABLE:
	case ADDIDATA_DISABLE:
		return 1;
	default:
		return 0;
	}
}
//------------------------------------------------------------------------------
/** returns the number of channels the board has
*
* if mode SingleDiff is not supported, the number of channels is always the same.
* If not, it depends on the selected mode.
*
*/
static uint8_t get_number_of_channel(struct pci_dev *pdev, int SingleDiff)
{
	if ( XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSeDiffConfigurationSupported == ADDIDATA_DISABLE )
	{
		return XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput;
	}
	else
	{
		switch(SingleDiff)
		{
		case ADDIDATA_DIFFERENTIAL:
			return (XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput/2);
		default:
			return XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput;
		}
	}
}
//------------------------------------------------------------------------------
/* Tests the single diff flag
 *
 *  @retval 1 if OK 0 otherwise
 */
static int singlediff_is_valid(int val)
{
	switch(val)
	{
	case ADDIDATA_DIFFERENTIAL:
	case ADDIDATA_SINGLE:
		return 1;
	default:
		return 0;
	}
}
//------------------------------------------------------------------------------
/* Tests the gain flag
 *
 *  @retval 1 if OK 0 otherwise
 */
static int gain_is_valid(int val)
{
	switch(val)
	{
	case ADDIDATA_1_GAIN:
	case ADDIDATA_2_GAIN:
	case ADDIDATA_5_GAIN:
	case ADDIDATA_10_GAIN:
		return 1;
	default:
		return 0;
	}
}
//------------------------------------------------------------------------------
/* Tests the polarity flag
 *
 *  @retval 1 if OK 0 otherwise
 */
static int polarity_is_valid(int val)
{
	switch(val)
	{
	case ADDIDATA_UNIPOLAR:
	case ADDIDATA_BIPOLAR:
		return 1;
	default:
		return 0;
	}
}
//------------------------------------------------------------------------------
/** Tests the convert time unit flag
*
* @retval 1 if OK
* @retval 0 otherwise
*/
static int converttimeunit_is_valid(int unit)
{
	switch(unit)
	{
	case ADDIDATA_NANO_SECOND:
	case ADDIDATA_MICRO_SECOND:
	case ADDIDATA_MILLI_SECOND:
		return 1;
	default:
		return 0;
	}
}
//------------------------------------------------------------------------------
/** Convert Time Unit supported by board?
* @retval 1 if OK
* @retval 0 otherwise
*/
static int converttimeunit_is_supported(struct pci_dev *pdev, int unit)
{
	switch(unit)
	{
	case ADDIDATA_NANO_SECOND:
		if ( (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MaxConvertTimeForNanoSecond) && (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MinConvertTimeForNanoSecond) )
			return 0;
		break;
	case ADDIDATA_MICRO_SECOND:
		if ( (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MaxConvertTimeForMicroSecond) && (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MinConvertTimeForMicroSecond) )
			return 0;
		break;
	case ADDIDATA_MILLI_SECOND:
		if ( (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MaxConvertTimeForMilliSecond) && (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MinConvertTimeForMilliSecond) )
			return 0;
		break;
	default:
		/* internal invariant error ! */
		return 0;
	}

	return 1;
}
//------------------------------------------------------------------------------
/** Convert Time valid for the given unit?
* @retval 1 if OK
* @retval 0 otherwise
*/
static int converttime_is_valid(struct pci_dev *pdev, int unit, int value)
{
	switch(unit)
	{
	case ADDIDATA_NANO_SECOND:
		if ( (value < XPCI3XXX_PRIVDATA(pdev)->dw_MinConvertTimeForNanoSecond) || (value > XPCI3XXX_PRIVDATA(pdev)->dw_MaxConvertTimeForNanoSecond) )
			return 0;
		break;
	case ADDIDATA_MICRO_SECOND:
		if ( (value < XPCI3XXX_PRIVDATA(pdev)->dw_MinConvertTimeForMicroSecond) || (value > XPCI3XXX_PRIVDATA(pdev)->dw_MaxConvertTimeForMicroSecond) )
			return 0;
		break;
	case ADDIDATA_MILLI_SECOND:
		if ( (value < XPCI3XXX_PRIVDATA(pdev)->dw_MinConvertTimeForMilliSecond) || (value > XPCI3XXX_PRIVDATA(pdev)->dw_MaxConvertTimeForMilliSecond) )
			return 0;
		break;
	default:
		/* internal invariant error ! */
		return 0;
	}

	return 1;
}

//------------------------------------------------------------------------------
/** Tests the delay time unit flag
*
* @retval 1 if OK
* @retval 0 otherwise
*/
static int delaytimeunit_is_valid(int unit)
{
	switch(unit)
	{
	case ADDIDATA_MICRO_SECOND:
	case ADDIDATA_MILLI_SECOND:
	case ADDIDATA_SECOND:
		return 1;
	}
	return 0;
}
//------------------------------------------------------------------------------
/** Is the delay time unit supported by the board?
*
* @retval 1 if OK
* @retval 0 otherwise
*/
static int delaytimeunit_is_supported(struct pci_dev *pdev, int unit)
{
	switch(unit)
	{
	case ADDIDATA_MICRO_SECOND:
		if ( (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MaxDelayTimeForMicroSecond) && (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MinDelayTimeForMicroSecond) )
			return 0;
		break;
	case ADDIDATA_MILLI_SECOND:
		if ( (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MaxDelayTimeForMilliSecond) && (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MinDelayTimeForMilliSecond) )
			return 0;
		break;
	case ADDIDATA_SECOND:
		if( (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MaxDelayTimeForSecond) && (0 == XPCI3XXX_PRIVDATA(pdev)->dw_MinDelayTimeForSecond) )
			return 0;
		break;
	default:
		/* invariant break ! */
		return 0;
	}
	return 1;
}
//------------------------------------------------------------------------------
/** Delay Time valid for the given unit?
* @retval 1 if OK
* @retval 0 otherwise
*/
static int delaytime_is_valid(struct pci_dev *pdev, int unit, int value)
{
	switch(unit)
	{
	case ADDIDATA_MICRO_SECOND:
		if ( (value < XPCI3XXX_PRIVDATA(pdev)->dw_MinDelayTimeForMicroSecond) || (value > XPCI3XXX_PRIVDATA(pdev)->dw_MaxDelayTimeForMicroSecond) )
			return 0;
		break;
	case ADDIDATA_MILLI_SECOND:
		if ( (value < XPCI3XXX_PRIVDATA(pdev)->dw_MinDelayTimeForMilliSecond) || (value > XPCI3XXX_PRIVDATA(pdev)->dw_MaxDelayTimeForMilliSecond) )
			return 0;
		break;
	case ADDIDATA_SECOND:
		if ( (value < XPCI3XXX_PRIVDATA(pdev)->dw_MinDelayTimeForSecond) || (value > XPCI3XXX_PRIVDATA(pdev)->dw_MaxDelayTimeForSecond) )
			return 0;
		break;
	default:
		/* invariant break ! */
		return 0;
	}
	return 1;
}
//------------------------------------------------------------------------------
/** check if single/diff parameter is valid relative to the board
 *
 * if the board doesn't support configuration of this parameter,
 * then parameter must be the same as the board
 *
 * @retval 1 if OK
 * @retval 0 otherwise
 *
||  b_SingleDiff ||  b_AnalogInputSeDiffConfigurationSupported || b_AnalogInputSeDiffConfiguration || == || result ||
|| 0 ||  0 ||  0 || 1 || 1 ||
|| 0 ||  0 || 1 || 0 || 0 ||
|| 0 || 1 || 0 || - || 1 ||
|| 0 || 1 || 1 || - || 1 ||
|| 1 ||  0 || 0 || 0 || 0 ||
|| 1 ||  0 || 1 || 1 || 1 ||
|| 1 || 1 || 0 || - || 1 ||
|| 1 || 1 || 1 || - || 1 ||
 */
static int singlediff_supported(struct pci_dev *pdev, int val)
{
	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSeDiffConfigurationSupported == ADDIDATA_ENABLE)
		return 1;
	return ( XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSeDiffConfiguration == val );
}
//------------------------------------------------------------------------------
/** Get the number of analog input channels.
*
* @param [in] pdev				: The device to use.
* @param [in] b_SingleDiff		: The mode of the input channels ADDIDATA_DIFFERENTIAL or ADDIDATA_SINGLE.
*
* @param [out] b_NbrChannels	: The number of analog input channels.
*
* @retval 0: No error.
* @retval 1: The handle parameter of the board is wrong.
* @retval 2: Wrong single/diff value
* @retval 3: Requested single diff parameter not supported by the board
*/
int i_xpci3xxx_GetNumberOfAnalogInputChannels (struct pci_dev *pdev, uint8_t b_SingleDiff, uint8_t *b_NbrChannels)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	/* If no input channels */
	if (!XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput)
	{
		*b_NbrChannels = 0;
		return 0;
	}

	if ( !singlediff_is_valid(b_SingleDiff) )
		return 2;

	if( !singlediff_supported(pdev, b_SingleDiff) )
		return 3;

	*b_NbrChannels = get_number_of_channel(pdev, b_SingleDiff);

	return 0;
}
//------------------------------------------------------------------------------
/**
*
* Initializes one analog input channel
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in]   b_Channel      : channel number of the input    \n
*                                              to be initialise               \n
* @param[in]   b_SingleDiff   : 0 : Single ended HW config     \n
*                                              1 : Differential HW config     \n
* @param[in]   b_Gain         : Gain factor of the analog input\n
*                                              - ADDIDATA_1_GAIN : Gain =  1  \n
*                                              - ADDIDATA_2_GAIN : Gain =  2  \n
*                                              - ADDIDATA_5_GAIN : Gain =  5  \n
*                                              - ADDIDATA_10_GAIN: Gain = 10  \n
* @param[in]   b_Polarity     : Polarity of the analog input:  \n
*                                              - ADDIDATA_UNIPOLAR:  0V - +10V\n
*                                              - ADDIDATA_BIPOLAR: -10V - +10V\n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Wrong channel number
* @retval 3 Wrong single/diff value
* @retval 4 Wrong gain
* @retval 5 Wrong polarity
* @retval 6 A conversion is already started
* @retval 7 Requested single/diff parameter not supported by the board
*/
int i_xpci3xxx_InitAnalogInput (struct pci_dev * pdev,
		uint8_t   b_Channel,
		uint8_t   b_SingleDiff,
		uint8_t   b_Gain,
		uint8_t   b_Polarity)
{
	if (!pdev)
		return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if( !singlediff_is_valid(b_SingleDiff) )
		return 3;

	if( !singlediff_supported(pdev, b_SingleDiff) )
		return 7;

	/* check b_Channel */

	/* constraint: if the channels are not in differential mode, b_Channel : 0 .. b_NumberOfAnalogInput -1*/
	/* constraint: if the channels are in differential mode, b_Channel : 0 .. b_NumberOfAnalogInput/2 -1*/

	if( b_Channel >= get_number_of_channel(pdev, b_SingleDiff) )
		return 2;

	/* Tests the gain */
	if( !gain_is_valid(b_Gain) )
		return 4;

	/* Tests the polarity */
	if( !polarity_is_valid(b_Polarity) )
		return 5;

	if( reg_ai_is_acquisition_started(pdev) )
		return 6;

	/* save channel configuration */

	/* clear SCAN and SEQ bits, set CHANNEL INDEX */
	write_reg_ai_sequence_control(pdev, 0x21FF, b_Channel);

	/* set configuration of the channel currently selected */
	write_reg_ai_channel_config(pdev, 0xFF, (uint32_t)b_Gain | ((uint32_t)b_Polarity << 6) | ((uint32_t)b_SingleDiff << 7));

	/* save channel in sequence configuration array */

	/* clear SCAN bit, set SEQ bits, set CHANNEL INDEX */
	write_reg_ai_sequence_control(pdev, 0x20FF, 0x100 | (uint32_t)b_Channel);

	/* set channel index in the sequence array at the same index currently selected */
	write_reg_ai_channel_config(pdev, 0xFF, b_Channel);

	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSeDiffConfiguration = b_SingleDiff;
	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputChannelInitialised[b_Channel] = ADDIDATA_ENABLE;

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Starts the conversion of one analog input channel
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in]   b_Channel      : channel number of the input    \n
*                                              to be started                  \n
* @param[in]   b_ConvertTimeUnit : Convert time unit for the   \n
*                                              analog input:                  \n
*                                              - ADDIDATA_NANO_SECOND  :nanos \n
*                                              - ADDIDATA_MICRO_SECOND :micros\n
*                                              - ADDIDATA_MILLI_SECOND :ms    \n
* @param[in]   b_ConvertTime : Convert time value for the  \n
*                                              analog input conversion        \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Wrong channel number
* @retval 3 Channel not initialised
* @retval 4 Wrong convert time unit
* @retval 5 Wrong convert time
* @retval 6 A conversion is already started
*/
int i_xpci3xxx_StartAnalogInput (struct pci_dev * pdev,
		uint8_t   b_Channel,
		uint8_t   b_ConvertTimeUnit,
		uint16_t   w_ConvertTime)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if ( b_Channel >= get_number_of_channel( pdev, XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSeDiffConfiguration) )
		return 2;

	if ( XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputChannelInitialised[b_Channel] != ADDIDATA_ENABLE)
		return 3;

	if ( ! converttimeunit_is_valid( b_ConvertTimeUnit ) )
		return 4;

	if ( ! converttimeunit_is_supported( pdev, b_ConvertTimeUnit) )
		return 4;

	if ( ! converttime_is_valid( pdev, b_ConvertTimeUnit, w_ConvertTime) )
		return 5;

	if( reg_ai_is_acquisition_started(pdev) )
		return 6;

	write_reg_ai_conversion_time_value(pdev, w_ConvertTime);

	write_reg_ai_conversion_time_unit(pdev, b_ConvertTimeUnit);

	/* clear ALR bit, set SEQ bits, set CHANNEL INDEX */
	/* why not clear SCAN bit ? */
	write_reg_ai_sequence_control(pdev, 0x2FF, 0x100 | (uint32_t)b_Channel);

	/* set channel index in the sequence array at the same index currently selected */
	write_reg_ai_channel_config(pdev, 0xFF, b_Channel);

	/* clear SEQ bit, set CHANNEL INDEX */
	write_reg_ai_sequence_control(pdev, 0x1FF, b_Channel);

	/* Clear the FIFO = set bit CLEAR INDEX, write all other bits back */
	write_reg_ai_clear_index(pdev, 0, 0x10000);

	/* Set the number of sequence to 1 */
	write_reg_ai_sequence_count(pdev, 1);

	/* set START SEQUENCE and STOP SEQUENCE to selected channel, set START bit */
	write_reg_ai_sequence_config(pdev, 0xFFFFFFFFUL, ( 0x80000 | ( ((uint32_t)b_Channel) << 8) | (uint32_t)b_Channel) );

	return 0;
}
//------------------------------------------------------------------------------
/**
*
* Reads the EOC Bit of the analog input conversion
*
* @param[in]  pdev : PCI Device struct              \n
*
* @param[out] pb_AnalogInputConversionFinished:                \n
*                                   0: no analog input conversion finished    \n
*                                   1: analog input conversion finished       \n
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
*
*/
int i_xpci3xxx_ReadAnalogInputBit (struct pci_dev * pdev, uint8_t* pb_AnalogInputConversionFinished)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	/* Read the EOC Status bit */
	/* EOC is B0 of dw_ReadValueDWord ? dw_ReadValueDWord & 0x1 */

	*pb_AnalogInputConversionFinished = ( readl( GET_MEM_BAR3(pdev) + REG_AI_ACQUISITION_STATUS ) & ADDIDATA_EOC) == ADDIDATA_EOC;

	return 0;
}
//------------------------------------------------------------------------------
/**
*
* Reads the value of the analog input channel
*
* @param[in]  pdev : PCI Device struct              \n
*
* @param[out] pdw_ReadValue : Value of the analog input       \n
*                                             channel                         \n
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
*/
int i_xpci3xxx_ReadAnalogInputValue (struct pci_dev * pdev,
		uint32_t* pdw_ReadValue)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	*pdw_ReadValue = readl( GET_MEM_BAR3(pdev) + REG_AI_CHANNEL_VALUE );

	return 0;
}
//------------------------------------------------------------------------------
static void initialize_delay(struct pci_dev * pdev, uint8_t b_DelayMode, uint8_t b_DelayTimeUnit, uint16_t w_DelayTime)
{
	if (b_DelayMode == ADDIDATA_DISABLE)
	{
		/* Disable the delay = clear bit DELAY; write all other bits back*/
		write_reg_ai_sequence_config(pdev, 0x40000, 0);
		return;
	}

	/* Set the delay mode = clear SHORT-CIRCUIT and DELAY MODE, set bit DELAY MODE; write all other bits back  */
	write_reg_ai_sequence_control(pdev, 0x1D000, ((uint32_t)(b_DelayMode - 1) << 14));

	write_reg_ai_delay_timer_unit(pdev, b_DelayTimeUnit);

	write_reg_ai_delay_timer_value(pdev, w_DelayTime);

	/* Enable the delay = set bit DELAY, write all other bits back */
	write_reg_ai_sequence_config(pdev, 0x0, 0x40000);
}
//------------------------------------------------------------------------------
/* there is internally in the PLD two configuration arrays:
 * - one that holds the channel configuration (gain/polarity/(single/diff)mode) for each channel
 * - one that holds the acquisition sequence
 *
 * The sequence acquisition channel is an array of 16 entries that must contain the index of the channels
 * to acquire.
 * During the acquisition this array is run through and the relevant channel is acquired.
 *
 * This means that it is legal to specify twice or more the same value.
 */
static void initialize_channels(struct pci_dev * pdev, uint8_t b_NumberOfChannels, uint8_t* pb_ChannelList, uint8_t* pb_Gain, uint8_t* pb_Polarity,	uint8_t b_SingleDiff)
{
	uint8_t	b_ChannelCounter = 0;

	for (b_ChannelCounter = 0 ; b_ChannelCounter < b_NumberOfChannels ; b_ChannelCounter++)
	{
		/* save channel configuration */

		/* clear SCAN and SEQ bits, set CHANNEL INDEX, write other bits back */
		write_reg_ai_sequence_control(pdev, 0x21FF, b_ChannelCounter);

		/* set configuration of the channel currently selected , write other bits back */
		write_reg_ai_channel_config(pdev, 0xFF, (uint32_t)pb_Gain[b_ChannelCounter] | ((uint32_t)pb_Polarity[b_ChannelCounter] << 6) | ((uint32_t)b_SingleDiff << 7));

		/* save channel in sequence configuration array */

		/* clear SCAN bit, set SEQ bits, set CHANNEL INDEX, write other bits back */
		write_reg_ai_sequence_control(pdev, 0x20FF, 0x100 | b_ChannelCounter);

		/* set channel index in the sequence array at the same index currently selected, write other bits back */
		write_reg_ai_channel_config(pdev, 0xFF, pb_ChannelList[b_ChannelCounter]);
	}
}
//------------------------------------------------------------------------------
/** test the validity of NumberOfChannel parameter */
/* @retval 0 if OK, 1 otherwise */
static int test_NumberOfChannel(struct pci_dev * pdev, unsigned char b_NumberOfChannels, uint8_t b_SingleDiff)
{
	/* constraint: 0 < NumberOfChannel <= ADDIDATA_MAX_AI */
	switch(b_NumberOfChannels)
	{
	case 1 ... ADDIDATA_MAX_AI:
	break; /* OK */
	default:
		return 1;
	}

	if(	b_NumberOfChannels > get_number_of_channel(pdev,b_SingleDiff) )
		return 1;

	return 0;
}
//------------------------------------------------------------------------------
/* Tests content of pb_ChannelList */

/* @retval 0 if OK, 1 otherwise */
static int test_pb_ChannelList(struct pci_dev * pdev, uint8_t b_SingleDiff, uint8_t b_NumberOfChannels, uint8_t* pb_ChannelList)
{
	uint8_t channelnb = get_number_of_channel(pdev,b_SingleDiff);

	{
		int i;
		for (i = 0 ; i < b_NumberOfChannels ; i++)
		{
			/* constraint: pb_ChannelList[i] : ( 0 .. channelnb) */
			if ( (uint8_t)pb_ChannelList[i] < channelnb )
				continue; /* OK */
			return 2;
		}
	}

	return 0;
}
//------------------------------------------------------------------------------
/**  AUTOREFRESH  **/
//------------------------------------------------------------------------------
/**
*
* Initialise an analog input auto refresh conversion
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in]    b_NumberOfChannels : Number of channel to      \n
*                                              initialise (<=ADDIDATA_MAX_AI) \n
* @param[in]  pb_ChannelList : List of the channels           \n
*                                                  to initialise              \n
* @param[in]  pb_Gain     : Gain factor array of the channels \n
*                                          - ADDIDATA_1_GAIN : Gain =  1      \n
*                                          - ADDIDATA_2_GAIN : Gain =  2      \n
*                                          - ADDIDATA_5_GAIN : Gain =  5      \n
*                                          - ADDIDATA_10_GAIN: Gain = 10      \n
* @param[in]  pb_Polarity   : Polarity array of the channels: \n
*                                            - ADDIDATA_UNIPOLAR:  0V - +10V  \n
*                                            - ADDIDATA_BIPOLAR: -10V - +10V  \n
* @param[in]   b_SingleDiff : 0 : Single ended HW config      \n
*                                             1 : Differential HW config      \n
* @param[in]  dw_NumberOfSequence :   0: continuous mode      \n
*                                                   <>0: single mode          \n
* @param[in]   b_DelayMode     : Delay mode for the            \n
*                                               analog input:                 \n
*                                              - ADDIDATA_DELAY_MODE_1: Mode 1\n
*                                              - ADDIDATA_DELAY_MODE_2: Mode 2\n
* @param[in]   b_DelayTimeUnit : Delay time unit for the       \n
*                                              analog input:                  \n
*                                              - ADDIDATA_MICRO_SECOND :micros\n
*                                              - ADDIDATA_MILLI_SECOND :ms    \n
*                                              - ADDIDATA_SECOND :s           \n
* @param[in]   w_DelayTime : Delay time value for the          \n
*                                              analog input conversion        \n
*
*
* @retval 0  No error
* @retval 1  The handle parameter of the board is wrong
* @retval 2  Wrong number of channels
* @retval 3  Wrong channel number
* @retval 4  Wrong gain factor
* @retval 5  Wrong polarity
* @retval 6  Wrong single diff parameter (not 0 or 1)
* @retval 7  Wrong delay mode parameter
* @retval 8  Wrong delay time unit parameter
* @retval 9  Wrong delay time parameter
* @retval 10 A conversion is already started
* @retval 11 requested single diff parameter not supported by the board
*
* @note
*
* <b>More on b_NumberOfChannels</b>\n \n
*
* b_NumberOfChannels describes the sequence to follow during acquisition. It must contain indexes of valid channels.
* Which index is valid depends on the connection mode of the board (single or differential).
*
* The order of the index in this array have no influence on the order of the values
* returned after an acquisition.
*
* <b>Constraints on parameter NumberOfChannel</b>\n\n
* let NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if
* - 0 < NumberOfChannel <= ADDIDATA_MAX_AI
* - if the board is in differential mode, NumberOfChannel < (b_NumberOfAnalogInput/2)
* - if the board is in single mode, NumberOfChannel < b_NumberOfAnalogInput
*
*
* <b>Constraints on parameter pb_ChannelList</b>\n\n
* let NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if:
* - if the board is in differencial mode, for each value in pb_ChannelList, value < (b_NumberOfAnalogInput/2)
* - if the board is in single mode, for each value in pb_ChannelList, value < b_NumberOfAnalogInput
*/
int i_xpci3xxx_InitAnalogInputAutoRefresh (struct pci_dev * pdev,
		uint8_t   b_NumberOfChannels,
		uint8_t*  pb_ChannelList,
		uint8_t*  pb_Gain,
		uint8_t*  pb_Polarity,
		uint8_t    b_SingleDiff,
		uint32_t  dw_NumberOfSequence,
		uint8_t    b_DelayMode,
		uint8_t    b_DelayTimeUnit,
		uint16_t    w_DelayTime)
{
	if ( pdev == NULL ) return 1;

	{

		XPCI3XXX_DEBUG_FCN();
		XPCI3XXX_DEBUG_INTARRAY("pb_ChannelList",0, b_NumberOfChannels,pb_ChannelList);
		XPCI3XXX_DEBUG_INTARRAY("pb_Gain",0, b_NumberOfChannels,pb_Gain);
		XPCI3XXX_DEBUG_INTARRAY("pb_Polarity",0, b_NumberOfChannels,pb_Polarity);
		XPCI3XXX_DEBUG("b_SingleDiff: %u\n",b_SingleDiff);
		XPCI3XXX_DEBUG("dw_NumberOfSequence: %u\n",dw_NumberOfSequence);
		XPCI3XXX_DEBUG("b_DelayMode: %u\n",b_DelayMode);
		XPCI3XXX_DEBUG("b_DelayTimeUnit: %u\n",b_DelayTimeUnit);
		XPCI3XXX_DEBUG("w_DelayTime: %u\n",w_DelayTime);
	}

	/* Tests the single diff flag */
	if( !singlediff_is_valid(b_SingleDiff) )
		return 6;

	if( !singlediff_supported(pdev, b_SingleDiff) )
		return 11;

	/* Tests the number of channels */
	if(test_NumberOfChannel(pdev, b_NumberOfChannels, b_SingleDiff))
		return 3;

	/* Tests content of pb_ChannelList */
	if( test_pb_ChannelList(pdev, b_SingleDiff, b_NumberOfChannels, pb_ChannelList) )
		return 2;

	{
		uint8_t   b_ChannelCounter = 0;
		for (b_ChannelCounter = 0 ; b_ChannelCounter < b_NumberOfChannels ; b_ChannelCounter++)
		{
			/* Tests the gain */
			if(!gain_is_valid(pb_Gain[b_ChannelCounter]))
				return 4;

			if(!polarity_is_valid(pb_Polarity[b_ChannelCounter]))
				return 5;
		}
	}

	/* Tests the delay mode flag */
	if ((b_DelayMode != ADDIDATA_DISABLE) && (b_DelayMode != ADDIDATA_DELAY_MODE_1) && (b_DelayMode != ADDIDATA_DELAY_MODE_2))
		return 7;

	if (b_DelayMode != ADDIDATA_DISABLE)
	{
		if ( ! delaytimeunit_is_valid(b_DelayTimeUnit) )
			return 8;

		if ( ! delaytimeunit_is_supported( pdev, b_DelayTimeUnit) )
			return 8;

		if ( ! delaytime_is_valid( pdev, b_DelayTimeUnit, w_DelayTime) )
			return 9;
	}

	if( reg_ai_is_acquisition_started(pdev) )
		return 10;

	initialize_delay(pdev, b_DelayMode, b_DelayTimeUnit, w_DelayTime);
	initialize_channels(pdev, b_NumberOfChannels, pb_ChannelList, pb_Gain, pb_Polarity,b_SingleDiff);

	/* Set the first and last sequence index */
	/* first is 0, last is b_NumberOfChannels - 1 (acquisition on all channels) */

	/* set START SEQUENCE to 0, STOP SEQUENCE to selected channel */
	write_reg_ai_sequence_config(pdev, 0xFFFF, ( ( (uint32_t)b_NumberOfChannels - 1) << 8)  );

	/* Test if continuous mode */
	if (dw_NumberOfSequence == 0)
	{
		/* Set the continuous mode and disable the DMA = clear SEQ/SCAN Mode and DMA bit, set field SEQ/SCAN Mode to 01 (continous) */
		write_reg_ai_sequence_config(pdev, 0x230000, 0x10000 );

		/* Clear the sequence counter */
		write_reg_ai_sequence_count(pdev, 0);
	}
	else
	{
		/* Set the single mode and disable the DMA = clear SEQ/SCAN Mode and DMA bit */
		write_reg_ai_sequence_config(pdev, 0x230000, 0x0 );

		/* Set the sequence counter */
		write_reg_ai_sequence_count(pdev, dw_NumberOfSequence);
	}

	/* Clear the DMA sequence counter */
	write_reg_ai_number_dma_transfer(pdev, 0);

	/* Clear the sequence index = set CLEAR INDEX bit */
	write_reg_ai_clear_index(pdev, 0, 0x10000);

	flush_reg_ai(pdev);

	/* Complete the sequence structure */
	XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAutoRefreshChannels = b_NumberOfChannels;
	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputAutoRefreshInitialised = ADDIDATA_ENABLE;

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Start an analog input auto refresh conversion
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in]   b_ConvertTimeUnit : Convert time unit for the   \n
*                                              analog input:                  \n
*                                              - ADDIDATA_NANO_SECOND  :nanos \n
*                                              - ADDIDATA_MICRO_SECOND :micros\n
*                                              - ADDIDATA_MILLI_SECOND :ms    \n
* @param[in]   w_ConvertTime : Convert time value for the      \n
*                                              analog input conversion        \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Auto refresh not initialised
* @retval 3 Wrong convert time unit
* @retval 4 Wrong convert time
* @retval 5 A conversion is already started
*/
int i_xpci3xxx_StartAnalogInputAutoRefresh (struct pci_dev * pdev,
		uint8_t   b_ConvertTimeUnit,
		uint16_t   w_ConvertTime)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
		XPCI3XXX_DEBUG("b_ConvertTimeUnit: %u\n",b_ConvertTimeUnit);
		XPCI3XXX_DEBUG("w_ConvertTime: %u\n",w_ConvertTime);
	}

	if ( XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputAutoRefreshInitialised != ADDIDATA_ENABLE )
		return 2;

	if ( ! converttimeunit_is_valid(b_ConvertTimeUnit) )
		return 3;

	if ( ! converttimeunit_is_supported(pdev, b_ConvertTimeUnit) )
		return 3;

	/* Tests the convert time */
	if ( ! converttime_is_valid( pdev, b_ConvertTimeUnit, w_ConvertTime) )
		return 4;

	if( reg_ai_is_acquisition_started(pdev) )
		return 5;

	/* Clear all DMA interrupt source */
	/* NS: why not clear bit EOS as well ? */
	write_reg_ai_interrupt_status(pdev, 0x35);

	/* Set the conversion time base */
	write_reg_ai_conversion_time_value(pdev, w_ConvertTime);

	/* Set the conversion time unit */
	write_reg_ai_conversion_time_unit(pdev, b_ConvertTimeUnit);

	/* clear DMA and IRQ bits, set START bit */
	write_reg_ai_sequence_config(pdev, 0x300000, 0x80000);

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Stop an analog input auto refresh conversion
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Auto refresh not initialised
*/
int i_xpci3xxx_StopAnalogInputAutoRefresh (struct pci_dev * pdev)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputAutoRefreshInitialised != ADDIDATA_ENABLE)
		return 2;

	/* clear DMA, IRQ and START bits,  */
	write_reg_ai_sequence_config(pdev, 0x380000, 0);

	/* Clear all DMA interrupt source */
	/* NS: why not clear bit EOS as well ? */
	write_reg_ai_interrupt_status(pdev, 0x35);

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Read the auto refresh values
*
* @param[in] pdev PCI Device struct
* @param[in] pdw_ReadValue a buffer of 32 bits unsigned integer of size ADDIDATA_MAX_AI, containing the acquired data.
* @param[in] pdw_CycleIndex current cycle index
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Auto refresh not initialised
*/
int i_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter (struct pci_dev * pdev,
		uint32_t pdw_ReadValue[ADDIDATA_MAX_AI],
		uint32_t* pdw_CycleIndex)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputAutoRefreshInitialised != ADDIDATA_ENABLE)
		return 2;

	/* Read the number of sequence */
	*pdw_CycleIndex = readl( GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_COUNT );

	/* Read the digital value of the input */
	/* Value is B0?B31 dw_ReadValueDWord ? dw_ReadValueDWord & 0xFFFFFFFF */

	/* whatever is the actual b_NumberOfAnalogInput, the memory read by INPDW_MEM is correct */
	{
		unsigned char i;
		for (i = 0 ; i < ADDIDATA_MAX_AI ; i ++)
		{
			pdw_ReadValue[i] = readl( GET_MEM_BAR3(pdev) + 128 + (i * 4)  );
		}
	}
	return 0;
}
//------------------------------------------------------------------------------
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
		uint32_t *pdw_CycleIndex)
{
	int i = 0;

	/* Read the number of sequence */
	*pdw_CycleIndex = readl( GET_MEM_BAR3(pdev) + REG_AI_SEQUENCE_COUNT );

	for (i=0; i<dw_NbrOfChannels; i++)
		pdw_ReadValue[i] = readl( GET_MEM_BAR3(pdev) + 128 + (i * 4) );

	return 0;
}
//------------------------------------------------------------------------------
/**
*
* Release an analog input auto refresh conversion
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @retval 0 : No error
* @retval 1 : The handle parameter of the board is wrong
* @retval 2 : Auto refresh not initialised
* @retval 3 : A conversion is already started
*/
int i_xpci3xxx_ReleaseAnalogInputAutoRefresh (struct pci_dev * pdev)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputAutoRefreshInitialised != ADDIDATA_ENABLE)
		return 2;

	if( reg_ai_is_acquisition_started(pdev) )
		return 3;

	/* Complete the sequence structure */
	/* TODO use atomic write ! */
	XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAutoRefreshChannels = 0;
	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputAutoRefreshInitialised = ADDIDATA_DISABLE;

	return 0;
}
//------------------------------------------------------------------------------
/**  SEQUENCE  **/
//------------------------------------------------------------------------------
/**
*
* Initialise an analog input sequence conversion
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in]    b_NumberOfChannels : Number of channel to      \n
*                                              initialise (<=ADDIDATA_MAX_AI) \n
* @param[in]  pb_ChannelList : List of the channels           \n
*                                                  to initialise              \n
* @param[in]  pb_Gain     : Gain factor array of the channels \n
*                                          - ADDIDATA_1_GAIN : Gain =  1      \n
*                                          - ADDIDATA_2_GAIN : Gain =  2      \n
*                                          - ADDIDATA_5_GAIN : Gain =  5      \n
*                                          - ADDIDATA_10_GAIN: Gain = 10      \n
* @param[in]  pb_Polarity   : Polarity array of the channels: \n
*                                            - ADDIDATA_UNIPOLAR:  0V - +10V  \n
*                                            - ADDIDATA_BIPOLAR: -10V - +10V  \n
* @param[in]   b_SingleDiff : 0 : Single ended HW config      \n
*                                             1 : Differential HW config      \n
* @param[in]  dw_NumberOfSequence :   if DMA not used:        \n
*                                               - 0: continuous mode          \n
*                                               - <>0: single mode            \n
*                                                     if DMA used:            \n
*                                               - define the number of DMA    \n
*                                                 transfer                    \n
* @param[in]    b_UseDMA : ADDIDATA_ENABLE  : Use DMA          \n
*                                         ADDIDATA_DISABLE : Don't use DMA    \n
* @param[in]  dw_NumberOfSequenceForEachInterrupt : number    \n
*                                               of sequence to do before      \n
*                                               generating an interrupt       \n
* @param[in]   b_DelayMode     : Delay mode for the            \n
*                                               analog input:                 \n
*                                              - ADDIDATA_DELAY_MODE_1: Mode 1\n
*                                              - ADDIDATA_DELAY_MODE_2: Mode 2\n
* @param[in]   b_DelayTimeUnit : Delay time unit for the       \n
*                                              analog input:                  \n
*                                              - ADDIDATA_MICRO_SECOND :micros\n
*                                              - ADDIDATA_MILLI_SECOND :ms    \n
*                                              - ADDIDATA_SECOND :s           \n
* @param[in]   w_DelayTime : Delay time value for the          \n
*                                              analog input conversion        \n
*
*
* @retval 0  No error
* @retval 1  The handle parameter of the board is wrong
* @retval 2  Wrong number of channels
* @retval 3  Wrong channel number
* @retval 4  Wrong gain factor
* @retval 5  Wrong polarity
* @retval 6  Wrong single diff parameter (not 0 or 1)
* @retval 7  Wrong use DMA parameter
* @retval 8  Wrong number of sequence for each interrupt parameter
* @retval 9  Wrong delay mode parameter
* @retval 10 Wrong delay time unit parameter
* @retval 11 Wrong delay time parameter
* @retval 12 A conversion is already started
* @retval 13 DMA mode not supported
* @retval 14 No memory space available
* @retval 15 Error by initialising the DMA
* @retval 16 requested single diff parameter not supported by the board
* @retval 17 b_NumberOfChannels, dw_NumberOfSequence or dw_NumberOfSequenceForEachInterrupt wrong
*
* @note
*
* <b>More on b_NumberOfChannels</b>\n \n
*
* b_NumberOfChannels describes the sequence to follow during acquisition. It must contain indexes of valid channels.
* Which index is valid depends on the connection mode of the board (single or differential).
*
* The order of the index in this array is the order of the values returned after the acquisition.
*
* <b>Constraints on parameter NumberOfChannel</b>\n\n
* let b_NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if
* - 0 < NumberOfChannel <= ADDIDATA_MAX_AI
* - if the board is in differential mode, NumberOfChannel < (b_NumberOfAnalogInput/2)
* - if the board is in single mode, NumberOfChannel < b_NumberOfAnalogInput
*
*
* <b>Constraints on parameter pb_ChannelList</b>\n\n
* let b_NumberOfAnalogInput be the number of analog inputs (4,8,16) of the board, then NumberOfChannel is correct if:
* - if the board is in differencial mode, for each value in pb_ChannelList, value < (b_NumberOfAnalogInput/2)
* - if the board is in single mode, for each value in pb_ChannelList, value < b_NumberOfAnalogInput
*
*/
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
		                                uint16_t    w_DelayTime)
{
	uint8_t   b_AcquisitionMode = 0;

	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	/* Tests the single diff flag */
	if( !singlediff_is_valid(b_SingleDiff) )
		return 6;

	if( !singlediff_supported(pdev, b_SingleDiff) )
		return 16;

	/* Tests the number of channels */
	if( test_NumberOfChannel(pdev, b_NumberOfChannels, b_SingleDiff) )
		return 3;

	if( test_pb_ChannelList(pdev, b_SingleDiff, b_NumberOfChannels, pb_ChannelList) )
		return 2;

	{
		uint8_t   b_ChannelCounter = 0;
		for (b_ChannelCounter = 0 ; b_ChannelCounter < b_NumberOfChannels ; b_ChannelCounter++)
		{
			/* Tests the gain */
			if ( (pb_Gain[b_ChannelCounter] != ADDIDATA_1_GAIN) && (pb_Gain[b_ChannelCounter] != ADDIDATA_2_GAIN) && (pb_Gain[b_ChannelCounter] != ADDIDATA_5_GAIN) && (pb_Gain[b_ChannelCounter] != ADDIDATA_10_GAIN) )
				return 4;

			/* Tests the polarity */
			if ( (pb_Polarity[b_ChannelCounter] != ADDIDATA_UNIPOLAR) && (pb_Polarity[b_ChannelCounter] != ADDIDATA_BIPOLAR) )
				return 5;
		}
	}

	/* Tests the use DMA flag */
	if( !is_boolean(b_UseDMA) )
		return 7;

	/* Tests the number of sequence for each interrupt */
	if ((b_UseDMA == ADDIDATA_ENABLE) && (dw_NumberOfSequenceForEachInterrupt == 0))
		return 8;

	/* Test the acquisition combination in DMA mode */
    if ((b_UseDMA == ADDIDATA_ENABLE) && (((((dw_NumberOfSequence != 0) && (dw_NumberOfSequence < dw_NumberOfSequenceForEachInterrupt)) || (dw_NumberOfSequenceForEachInterrupt == 0)) ||
    	((((dw_NumberOfSequenceForEachInterrupt * b_NumberOfChannels) / 2) * 2) != (dw_NumberOfSequenceForEachInterrupt * b_NumberOfChannels)) ||
         ((dw_NumberOfSequence != 0) && (((dw_NumberOfSequence / dw_NumberOfSequenceForEachInterrupt) * dw_NumberOfSequenceForEachInterrupt) != dw_NumberOfSequence)))))
    	return 17;

	/* Tests the delay mode flag */
	if ((b_DelayMode != ADDIDATA_DISABLE) && (b_DelayMode != ADDIDATA_DELAY_MODE_1) && (b_DelayMode != ADDIDATA_DELAY_MODE_2))
		return 9;

	if (b_DelayMode != ADDIDATA_DISABLE)
	{
		if ( ! delaytimeunit_is_valid(b_DelayTimeUnit) )
			return 10;

		if ( ! delaytimeunit_is_supported( pdev, b_DelayTimeUnit) )
			return 10;

		if ( ! delaytime_is_valid( pdev, b_DelayTimeUnit, w_DelayTime) )
			return 11;
	}

	if( reg_ai_is_acquisition_started(pdev) )
		return 12;

	if (b_UseDMA == ADDIDATA_ENABLE)
	{
		if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMASupported != ADDIDATA_ENABLE)
			return 13;

		if (dw_NumberOfSequence == 0)
			b_AcquisitionMode = 1; // continuous mode
		else
			b_AcquisitionMode = 0; // single mode

		if (  i_xpci3xxx_InitAnalogInputDMA(pdev, b_NumberOfChannels, dw_NumberOfSequence, dw_NumberOfSequenceForEachInterrupt, b_AcquisitionMode) != 0)
		{
			/* Error by initialising the DMA */
			i_xpci3xxx_ReleaseAnalogInputDMA(pdev);
			return 15;
		}

	}

	initialize_delay(pdev, b_DelayMode, b_DelayTimeUnit, w_DelayTime);
	initialize_channels(pdev, b_NumberOfChannels, pb_ChannelList, pb_Gain, pb_Polarity,b_SingleDiff);

	/* set START SEQUENCE to 0, STOP SEQUENCE to selected channel */
	write_reg_ai_sequence_config(pdev, 0xFFFF, ( ( (uint32_t)b_NumberOfChannels - 1) << 8)  );

	/* Test if DMA used */
	if (b_UseDMA == ADDIDATA_ENABLE)
	{
		/* set CONTINUOUS mode */
		write_reg_ai_sequence_config( pdev, 0x030000, 0x010000 );

		/* Clear the sequence counter */
		write_reg_ai_sequence_count( pdev, 0 );
	}
	else
	{
		/* Test if continuous mode */
		if (dw_NumberOfSequence == 0)
		{
			/* Set CONTINUEOUS mode, clear DMA bit */
			write_reg_ai_sequence_config( pdev, 0x230000, 0x010000 );

			/* Clear the sequence counter */
			write_reg_ai_sequence_count( pdev, 0 );
		}
		else
		{
			/* Set SINGLE mode, clear DMA bit */
			write_reg_ai_sequence_config( pdev, 0x230000, 0x0 );

			/* Set the sequence counter */
			write_reg_ai_sequence_count( pdev, dw_NumberOfSequence );
		}
	}

	/* Set or Clear the DMA sequence counter */
	if (b_UseDMA == ADDIDATA_ENABLE)
		write_reg_ai_number_dma_transfer( pdev, (dw_NumberOfSequence * b_NumberOfChannels) );
	else
		write_reg_ai_number_dma_transfer( pdev, 0 );

	/* set CLEAR INDEX bit */
	write_reg_ai_clear_index(pdev, 0, 0x10000);

	flush_reg_ai(pdev);

	/* Complete the sequence structure */
	XPCI3XXX_PRIVDATA(pdev)->b_NumberOfSequenceChannels = b_NumberOfChannels;
	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceInitialised = ADDIDATA_ENABLE;

	if (b_UseDMA == ADDIDATA_ENABLE)
	{
		XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMA = ADDIDATA_ENABLE;
		XPCI3XXX_PRIVDATA(pdev)->dw_LastScatterGatherBuffer = 0;
		XPCI3XXX_PRIVDATA(pdev)->dw_NumberOfSequenceForEachInterrupt = dw_NumberOfSequenceForEachInterrupt;
	}
	else
		XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMA = ADDIDATA_DISABLE;

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Start an analog input sequence conversion
*
* @param[in]  pdev : PCI Device struct              \n
* @param[in]   b_ConvertTimeUnit : Convert time unit for the   \n
*                                              analog input:                  \n
*                                              - ADDIDATA_NANO_SECOND  :nanos \n
*                                              - ADDIDATA_MICRO_SECOND :micros\n
*                                              - ADDIDATA_MILLI_SECOND :ms    \n
* @param[in]   w_ConvertTime : Convert time value for the      \n
*                                              analog input conversion        \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Sequence not initialised
* @retval 3 Wrong convert time unit
* @retval 4 Wrong convert time
* @retval 5 A conversion is already started
* @retval 6 Error by starting DMA
*/
int i_xpci3xxx_StartAnalogInputSequence (struct pci_dev * pdev,
		                                 uint8_t   b_ConvertTimeUnit,
		                                 uint16_t   w_ConvertTime)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceInitialised != ADDIDATA_ENABLE)
		return 2;

	if( ! converttimeunit_is_valid(b_ConvertTimeUnit) )
		/* constraint: b_ConvertTimeUnit : ( ADDIDATA_NANO_SECOND,ADDIDATA_MICRO_SECOND,ADDIDATA_MILLI_SECOND ) */
		return 3;

	if ( ! converttimeunit_is_supported(pdev, b_ConvertTimeUnit) )
		return 3;

	if ( ! converttime_is_valid( pdev, b_ConvertTimeUnit, w_ConvertTime) )
		return 4;

	if ( reg_ai_is_acquisition_started(pdev) )
		return 5;

	if ( XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMA == ADDIDATA_ENABLE)
	{
		if ( i_xpci3xxx_StartAnalogInputDMA(pdev) != 0)
		{
			i_xpci3xxx_StopAnalogInputDMA(pdev);
			return 6;
		}
	}

	/* Clear all DMA interrupt source */
	/* NS: why not clear bit EOS as well ? */
	write_reg_ai_interrupt_status(pdev, 0x35);

	/* Set the conversion time base */
	write_reg_ai_conversion_time_value(pdev, w_ConvertTime);

	/* Set the conversion time unit */
	write_reg_ai_conversion_time_unit(pdev, b_ConvertTimeUnit);

	/* Start the acquisition */
	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMA == ADDIDATA_ENABLE)
	{
		/* set DMA, IRQ and START bits */
		write_reg_ai_sequence_config(pdev, 0x400000, 0x380000);
	}
	else
	{
		/* clear DMA bit, set IRQ and START bits */
		write_reg_ai_sequence_config(pdev, 0x200000, 0x180000);
	}

	flush_reg_ai(pdev);

	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceStarted = ADDIDATA_ENABLE;

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Stop an analog input sequence conversion
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Auto refresh not initialised
* @retval 3 Sequence not started
* @retval 4 Error by stoping DMA
*/
int i_xpci3xxx_StopAnalogInputSequence (struct pci_dev * pdev)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceInitialised != ADDIDATA_ENABLE)
		return 2;

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceStarted != ADDIDATA_ENABLE)
		return 3;

	/* clear the DMA, IRQ and START bits */
	write_reg_ai_sequence_config(pdev, 0x80000, 0);
	write_reg_ai_sequence_config(pdev, 0x300000, 0);
	write_reg_ai_sequence_config(pdev, 0x300000, 0);

	/* Clear all DMA interrupt source */
	write_reg_ai_interrupt_status( pdev, 0x35);

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMA == ADDIDATA_ENABLE)
	{
		if (i_xpci3xxx_StopAnalogInputDMA(pdev) != 0)
			return 4;
	}

	flush_reg_ai(pdev);

	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceStarted = ADDIDATA_DISABLE;

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Release an analog input sequence conversion
*
* @param[in]  pdev : PCI Device struct              \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Auto refresh not initialised
* @retval 3 A conversion is already started
* @retval 4 Error by releasing DMA
*/
int i_xpci3xxx_ReleaseAnalogInputSequence (struct pci_dev * pdev)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceInitialised != ADDIDATA_ENABLE)
		return 2;

	if (XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceStarted != ADDIDATA_DISABLE)
		return 3;

	if( reg_ai_is_acquisition_started(pdev) )
		return 3;

	if ( i_xpci3xxx_ReleaseAnalogInputDMA(pdev) != 0)
		return 4;

	flush_reg_ai(pdev);

	/* Complete the sequence structure */
	XPCI3XXX_PRIVDATA(pdev)->b_NumberOfSequenceChannels = 0;
	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputSequenceInitialised = ADDIDATA_DISABLE;
	XPCI3XXX_PRIVDATA(pdev)->b_AnalogInputDMA = ADDIDATA_DISABLE;
	XPCI3XXX_PRIVDATA(pdev)->dw_LastScatterGatherBuffer = 0;

	XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterWrite = 0;
	XPCI3XXX_PRIVDATA(pdev)->dw_EventCounterRead = 0;

	return 0;
}
//------------------------------------------------------------------------------
/**
* @retval 0 OK
* @retval <> incorrect
*/
static int TriggerAction_is_valid(int TriggerAction)
{
	switch(TriggerAction)
	{
		case ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION:
		case ADDIDATA_ONE_SHOT_TRIGGER:
		case ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES:
		case ADDIDATA_TRIGGER_START_A_SINGLE_SEQUENCE:
		case ADDIDATA_TRIGGER_START_A_SCAN_SERIES:
		case ADDIDATA_TRIGGER_START_A_SINGLE_SCAN:
		case ADDIDATA_TRIGGER_START_A_AUTO_REFRESH_SERIES:
		case ADDIDATA_TRIGGER_START_A_SINGLE_AUTO_REFRESH:
			return 1;
		default:
			return 0;
	}
}
//------------------------------------------------------------------------------
/**
*
* Enable, disable and initialise the analog input hardware trigger
*
* @param[in]  pdev : PCI Device struct              			\n
* @param[in]   b_HardwareTrigger : 0 : disable the hardware trigger 	\n
*                      						   1 : enable the hardware trigger 	\n
* @param[in]   b_HardwareTriggerLevel : ADDIDATA_LOW: If the hardware   \n
*              	                                	  trigger is used, it triggers  \n
*            	      	                    	          from "1" to "0"               \n
*              	        	                          ADDIDATA_HIGH: If the         \n
*                  	        	                      hardware trigger is used, it  \n
*                      	        	                  triggers from "0" to "1"      \n
*                          	        	              ADDIDATA_LOW_HIGH: If the     \n
*                              	        	          hardware trigger is used, it  \n
*                                  	        	      triggers from "0" to "1" or   \n
*                                      	        	  from "1" to "0"               \n
* @param[in]   b_HardwareTriggerAction : Trigger action selection       \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION	  0 \n
*            			            	ADDIDATA_ONE_SHOT_TRIGGER					  1 \n
*            			            	ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES	  2 \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_SEQUENCE	  3 \n
*            			            	ADDIDATA_TRIGGER_START_A_SCAN_SERIES	      6 \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_SCAN  		  7 \n
*            			            	ADDIDATA_TRIGGER_START_A_AUTO_REFRESH_SERIES 10 \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_AUTO_REFRESH 11 \n
* @param[in] dw_HardwareTriggerCount  : Hardware trigger counter.      \n
*                  		                              Define the number of trigger  \n
*                          		                      events before the action      \n
*                                  		              occur (> 0)                   \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Wrong hardware trigger parameter
* @retval 3 Wrong hardware trigger level parameter
* @retval 4 Wrong hardware trigger action parameter
* @retval 5 Wrong hardware trigger count parameter
* @retval 6 No analog input functionality available
* @retval 7 A conversion is already started
*/
int i_xpci3xxx_EnableDisableAnalogInputHardwareTrigger (struct pci_dev * pdev,
		uint8_t   b_HardwareTrigger,
		uint8_t     b_HardwareTriggerLevel,
		uint8_t     b_HardwareTriggerAction,
		uint32_t    dw_HardwareTriggerCount)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	/* Tests the hardware trigger flag */
	if( !is_boolean(b_HardwareTrigger) )
		return 2;

	if( !TriggerAction_is_valid(b_HardwareTriggerAction) )
		return 3;

	if ((b_HardwareTriggerLevel != ADDIDATA_LOW) && (b_HardwareTriggerLevel != ADDIDATA_HIGH) && (b_HardwareTriggerLevel != ADDIDATA_LOW_HIGH) && (b_HardwareTrigger == ADDIDATA_ENABLE))
		return 4;

	if ( (b_HardwareTrigger == ADDIDATA_ENABLE) && ((dw_HardwareTriggerCount > 65535) || (dw_HardwareTriggerCount == 0)) )
		return 5;

	if (0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput)
		return 6;

	if (b_HardwareTrigger != ADDIDATA_DISABLE)
	{
		if( reg_ai_is_acquisition_started(pdev) )
			return 7;
	}

	switch(b_HardwareTriggerAction)
	{
		case ADDIDATA_ONE_SHOT_TRIGGER:
			b_HardwareTriggerAction = 0;
			break;
		case ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION:
			b_HardwareTriggerAction = 1;
			break;
		default:
			/* does nothing ? */
			break;
	}

	/* clear all bits, including ENABLE */
	write_reg_ai_hw_trigger_config( pdev, 0xFFFFFFFF, 0x0 );

	if (b_HardwareTrigger == ADDIDATA_ENABLE)
	{
		/* encode parameters  */
		uint32_t setmask = (((uint32_t) b_HardwareTriggerLevel & 1) << 2) | (((uint32_t) b_HardwareTriggerLevel & 2) << 0) | (((uint32_t) (b_HardwareTriggerAction & 3)) << 3) | (((uint32_t) dw_HardwareTriggerCount) << 16);
		write_reg_ai_hw_trigger_config( pdev, 0xFFFF007E, setmask );

		/* set ENABLE bit */
		write_reg_ai_hw_trigger_config( pdev, 0, 0x1 );
	}

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Returns the status (occur or not), the state from input
*
* @param[in]  pdev : PCI Device struct              			\n
*
* @param[out]   pb_HardwareTriggerStatus : 0: Hardware trigger          \n
*                                                            did not occur             \n
*                                                         1: Hardware trigger          \n
*                                                            occurred                  \n
* @param[out] pdw_HardwareTriggerCount  : Number of pulse that         \n
*                                                         fail before the next         \n
*                                                         trigger occur                \n
* @param[out]   pb_HardwareTriggerState  : 0: Hardware trigger          \n
*                                                            input is not              \n
*                                                            active                    \n
*                                                            (Low state)               \n
*                                                         1: Hardware trigger          \n
*                                                            input is active           \n
*                                                            (High state)              \n
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 No analog input functionality available
*/
int i_xpci3xxx_GetAnalogInputHardwareTriggerStatus (struct pci_dev * pdev,
		uint8_t*   pb_HardwareTriggerStatus,
		uint32_t* pdw_HardwareTriggerCount,
		uint8_t*   pb_HardwareTriggerState)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if ( 0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput )
		return 2;

	{
		uint32_t dw_ReadValueDWord = 0;
		dw_ReadValueDWord = readl( GET_MEM_BAR3(pdev) + REG_AI_HW_TRIGGER_STATUS ) & 0xFFFF0003UL;
		*pb_HardwareTriggerStatus = (uint8_t) ((dw_ReadValueDWord >> 0) & 1);
		*pb_HardwareTriggerState  = (uint8_t) ((dw_ReadValueDWord >> 1) & 1);
		*pdw_HardwareTriggerCount = (dw_ReadValueDWord >> 16) & 0xFFFFUL;
	}

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Enable, disable and initialise the analog input software trigger
*
* @param[in]  pdev : PCI Device struct              			\n
* @param[in]   b_SoftwareTrigger : 0 : disable the software trigger 	\n
*                      						   1 : enable the software trigger 	\n
*                      b_SoftwareTriggerAction : Trigger action selection              \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_CONVERSION	  0 \n
*            			            	ADDIDATA_ONE_SHOT_TRIGGER					  1 \n
*            			            	ADDIDATA_TRIGGER_START_A_SEQUENCE_SERIES	  2 \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_SEQUENCE	  3 \n
*            			            	ADDIDATA_TRIGGER_START_A_SCAN_SERIES	      6 \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_SCAN  		  7 \n
*            			            	ADDIDATA_TRIGGER_START_A_AUTO_REFRESH_SERIES 10 \n
*            			            	ADDIDATA_TRIGGER_START_A_SINGLE_AUTO_REFRESH 11 \n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Wrong software trigger parameter
* @retval 3 Wrong software trigger action parameter
* @retval 4 No analog input functionality available
* @retval 5 A conversion is already started
*/
int i_xpci3xxx_EnableDisableAnalogInputSoftwareTrigger (struct pci_dev * pdev,
		uint8_t   b_SoftwareTrigger,
		uint8_t   b_SoftwareTriggerAction)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	/* Tests the hardware trigger flag */
	if ( ! is_boolean(b_SoftwareTrigger) )
		return 2;

	if ( ! TriggerAction_is_valid(b_SoftwareTriggerAction) )
		return 3;

	if ( 0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput )
		return 4;

	if ( reg_ai_is_acquisition_started(pdev) )
		return 5;

	/* clear all bits, including ENABLE  */
	write_reg_ai_sw_trigger_config( pdev, 0xFFFFFFFF, 0 );

	if ( b_SoftwareTrigger == ADDIDATA_ENABLE )
	{
		/* set SW TRIGGER MODE */
		write_reg_ai_sw_trigger_config( pdev, 0, ( (b_SoftwareTriggerAction & 3) << 1) );

		/* set ENABLE bit */
		write_reg_ai_sw_trigger_config( pdev, 0, 0x1 );
	}

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Trigger the analog input conversion
*
* @param[in]  pdev : PCI Device struct              			\n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 No analog input functionality available
*/
int i_xpci3xxx_AnalogInputSoftwareTrigger (struct pci_dev * pdev)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput)
		return 2;

	/* set SOFT TRG bit */
	write_reg_ai_sw_trigger_config( pdev, 0, 0x00000020 );
	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Returns the status (occur or not) from software trigger
*
* @param[in]  pdev : PCI Device struct              			\n
*
* @param[out]   pb_SoftwareTriggerStatus : 0: Software trigger          \n
*                                                            did not occur             \n
*                                                         1: Software trigger          \n
*                                                            occurred                  \n
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 No analog input functionality available
*/
int i_xpci3xxx_GetAnalogInputSoftwareTriggerStatus (struct pci_dev * pdev,
		uint8_t*   pb_SoftwareTriggerStatus)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput)
		return 2;

	*pb_SoftwareTriggerStatus = readl( GET_MEM_BAR3(pdev) + REG_AI_SW_TRIGGER_STATUS ) & 1;

	return 0;
}

//------------------------------------------------------------------------------
/**
*
* Enable, disable and initialise the analog input hardware gate
*
* @param[in]  pdev : PCI Device struct              			\n
* @param[in]   b_HardwareGate : 0 : disable the hardware gate       	\n
*                      						   1 : enable the hardware gate 		\n
*                      b_HardwareGateLevel     : ADDIDATA_LOW: If the hardware			\n
*                                                gate is used, it is active to			\n
*                                                "0"                          			\n
*                                                ADDIDATA_HIGH:If the hardware			\n
*                                                gate is used, it is active to			\n
*                                                "1"                          			\n
*
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 Wrong hardware gate parameter
* @retval 3 Wrong hardware gate level parameter
* @retval 4 No analog input functionality available
* @retval 5 A conversion is already started
*
* @warning NOT SUPPORTED BY THIS BOARD FAMILY!
* @todo REMOVE!
*/
int i_xpci3xxx_EnableDisableAnalogInputHardwareGate (struct pci_dev * pdev,
		uint8_t   b_HardwareGate,
		uint8_t     b_HardwareGateLevel)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	/* Tests the hardware gate flag */
	if( !is_boolean(b_HardwareGate) )
		return 2;

	if ((b_HardwareGateLevel != ADDIDATA_LOW) && (b_HardwareGateLevel != ADDIDATA_HIGH))
		return 3;

	if (0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput)
		return 4;

	if( reg_ai_is_acquisition_started(pdev) )
		return 5;

	if (b_HardwareGate == ADDIDATA_ENABLE)
	{
		/* set INPUT GATE ENABLE bit, set or clear LOW LEVEL bit */
		write_reg_ai_gate_input_config(pdev, 0x00000001 | ( (uint32_t)((b_HardwareGateLevel - 1) & 1) << 1) );
	}
	else
	{
		/* clear INPUT GATE ENABLE and LOW LEVEL bits */
		write_reg_ai_gate_input_config(pdev, 0 );
	}

	return 0;
}

//------------------------------------------------------------------------------
/** Returns the status the state from input (active or not)
*
*
*
* @param[in]  pdev : PCI Device struct              			\n
*
* @param[out]   pb_HardwareGateStatus  : 0: Hardware gate input         \n
*                                                          is not active               \n
*                                                          (Low state)                 \n
*                                                       1: Hardware gate input         \n
*                                                          is active                   \n
*                                                          (High state)                \n
*
* @retval 0 No error
* @retval 1 The handle parameter of the board is wrong
* @retval 2 No analog input functionality available
*
* @warning NOT SUPPORTED BY THIS BOARD FAMILY!
* @todo REMOVE!
*/
int i_xpci3xxx_GetAnalogInputHardwareGateStatus (struct pci_dev * pdev,
		uint8_t*   pb_HardwareGateStatus)
{
	if ( pdev == NULL ) return 1;

	{
		XPCI3XXX_DEBUG_FCN();
	}

	if (0 == XPCI3XXX_PRIVDATA(pdev)->b_NumberOfAnalogInput)
		return 2;

	*pb_HardwareGateStatus  = ( (( readl( GET_MEM_BAR3(pdev) + REG_AI_GATE_INPUT_CONFIG )  & 0x4 ) >> 2) & 1);

	return 0;
}
