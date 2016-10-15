#include <errormsg.h>
#include <errno.h>

static const char SUCCESS[] = "SUCCESS";
static const char UNKNOWN_ERROR[] = "unknown error code";
static const char WRONG_BOARD_HANDLE[] = "wrong handle parameter of the board (driver internal error)"; /* this error doesn't relate to the filedescriptor */
static const char WRONG_NUMBER_OF_CHANNELS[] = "Wrong number of channels";
static const char WRONG_CHANNEL_NUMBER[] = "Wrong channel number";
static const char WRONG_GAIN_FACTOR[] = "Wrong gain factor";
static const char WRONG_POLARITY[] = "Wrong polarity";
static const char WRONG_DMA[] = "Wrong use of DMA";
static const char WRONG_NUMBER_OF_SEQUNCES[] = "Wrong number of sequence for each interrupt";
static const char WRONG_SINGLEDIFF[] = "Wrong single diff parameter (not 0 or 1)";
static const char WRONG_DELAY_MODE[] = "Wrong delay mode parameter";
static const char WRONG_DELAY_TIMEUNIT[] = "Wrong delay time unit parameter";
static const char WRONG_DELAY_TIME[] = "Wrong delay time parameter";
static const char CONVERSION_STARTED[] = "A conversion is already started";
static const char CONVERSION_NOT_STARTED[] = "No conversion is started";
static const char NO_AUTORESFRESH[] = "Auto refresh not initialized";
static const char NO_SEQUENCE[] = "Sequence not initialized";
static const char INVALID_SINGLEDIFF[] = "Requested single/differential mode not supported by the board";
static const char DMA_NOT_SUPPORTED[] = "DMA mode not supported";
static const char NO_MEMORY_SPACE_AVAILABLE[] = "No memory space available";
static const char DMA_INITIALISATION_ERROR[] = "Error by initializing the DMA";
static const char DMA_RELEASING_ERROR[] = "Error by releasing DMA";
static const char WRONG_INTERRUPT_FLAG[] = "Wrong interrupt flag";
static const char WRONG_TIMER[] = "Wrong timer index";
static const char WRONG_TIMER_MODE[] = "Wrong timer mode";
static const char WRONG_TIMER_UNIT[] = "Wrong timer unit";
static const char WRONG_TIMER_VALUE[] = "Wrong timer value";
static const char TIMER_AS_COUNTER_ALREADY_USED[] = "Timer is configured as counter and is already in use";
static const char TIMER_AS_WATCHDOG_ALREADY_USED[] = "Timer is configured as watchdog and is already in use";
static const char TIMER_NOT_INITIALIZED[] = "Timer not initialized";
static const char INTERRUPT_CALLBACK_NOT_INSTALLED[] = "Interrupt Callback not installed";

const char* msg_CMD_xpci3xxx_InitTimer(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return WRONG_TIMER;
		case 3:
			return TIMER_AS_COUNTER_ALREADY_USED;
		case 4:
			return TIMER_AS_WATCHDOG_ALREADY_USED;
		case 5:
			return WRONG_TIMER_MODE;
		case 6:
			return WRONG_TIMER_UNIT;
		case 7:
			return WRONG_TIMER_VALUE;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_EnableDisableTimerInterrupt(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return TIMER_NOT_INITIALIZED;
		case 3:
			return WRONG_TIMER;
		case 4:
			return TIMER_AS_COUNTER_ALREADY_USED;
		case 5:
			return TIMER_AS_WATCHDOG_ALREADY_USED;
		case 6:
			return WRONG_INTERRUPT_FLAG;
		case 7:
			return INTERRUPT_CALLBACK_NOT_INSTALLED;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_EnableDisableTimerHardwareTrigger(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return TIMER_NOT_INITIALIZED;
		case 3:
			return WRONG_TIMER;
		case 4:
			return TIMER_AS_COUNTER_ALREADY_USED;
		case 5:
			return TIMER_AS_WATCHDOG_ALREADY_USED;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_StartTimer(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return TIMER_NOT_INITIALIZED;
		case 3:
			return WRONG_TIMER;
		case 4:
			return TIMER_AS_COUNTER_ALREADY_USED;
		case 5:
			return TIMER_AS_WATCHDOG_ALREADY_USED;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_TestInterrupt(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case -EFAULT:
			return "Fail to exchange value with user mode";
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_InitAnalogInputSequence(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return WRONG_NUMBER_OF_CHANNELS;
		case 3:
			return WRONG_CHANNEL_NUMBER;
		case 4:
			return WRONG_GAIN_FACTOR;
		case 5:
			return WRONG_POLARITY;
		case 6:
			return WRONG_SINGLEDIFF;
		case 7:
			return WRONG_DMA;
		case 8:
			return WRONG_NUMBER_OF_SEQUNCES;
		case 9:
			return WRONG_DELAY_MODE;
		case 10:
			return WRONG_DELAY_TIMEUNIT;
		case 11:
			return WRONG_DELAY_TIME;
		case 12:
			return CONVERSION_STARTED;
		case 13:
			return DMA_NOT_SUPPORTED;
		case 14:
			return NO_MEMORY_SPACE_AVAILABLE;
		case 15:
			return DMA_INITIALISATION_ERROR;
		case 16:
			return INVALID_SINGLEDIFF;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_ReleaseAnalogInputSequence(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return NO_SEQUENCE;
		case 3:
			return CONVERSION_STARTED;
		case 4:
			return DMA_RELEASING_ERROR;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_StartAnalogInputSequence(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return NO_SEQUENCE;
		case 3:
			return "Wrong convert time unit";
		case 4:
			return "Wrong convert time";
		case 5:
			return CONVERSION_STARTED;
		case 6:
			return DMA_RELEASING_ERROR;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_StopAnalogInputSequence(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return NO_SEQUENCE;
		case 3:
			return CONVERSION_NOT_STARTED;
		case 4:
			return DMA_RELEASING_ERROR;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_InitAnalogInputAutoRefresh(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return WRONG_NUMBER_OF_CHANNELS;
		case 3:
			return WRONG_CHANNEL_NUMBER;
		case 4:
			return WRONG_GAIN_FACTOR;
		case 5:
			return WRONG_POLARITY;
		case 6:
			return WRONG_SINGLEDIFF;
		case 7:
			return WRONG_DELAY_MODE;
		case 8:
			return WRONG_DELAY_TIMEUNIT;
		case 9:
			return WRONG_DELAY_TIME;
		case 10:
			return CONVERSION_STARTED;
		case 11:
			return INVALID_SINGLEDIFF;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_ReleaseAnalogInputAutoRefresh(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return "Auto refresh not initialised";
		case 3:
			return CONVERSION_STARTED;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_StartAnalogInputAutoRefresh(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return NO_AUTORESFRESH;
		case 3:
			return "Wrong convert time unit";
		case 4:
			return "Wrong convert time";
		case 5:
			return CONVERSION_STARTED;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_ReadAnalogInputAutoRefreshValueAndCounter(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return NO_AUTORESFRESH;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_StopAnalogInputAutoRefresh(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return NO_AUTORESFRESH;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_ReadAnalogOutputBit(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_InitAnalogOutput(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return WRONG_CHANNEL_NUMBER;
		case 3:
			return WRONG_SINGLEDIFF;
		case 4:
			return CONVERSION_STARTED;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_WriteAnalogOutputValue(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return WRONG_CHANNEL_NUMBER;
		case 3:
			return "write value out of range";
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_SetTTLPortConfiguration(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return "TTL functionality not supported by the board";
		default:
			return UNKNOWN_ERROR;
	}
}
const char* msg_CMD_xpci3xxx_Read32DigitalInputs(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_SetDigitalOutputMemoryOn(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		default:
			return UNKNOWN_ERROR;
	}
}
const char* msg_CMD_xpci3xxx_Set32DigitalOutputsOff(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		case 2:
			return "Digital output memory not active";
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_Set32DigitalOutputsOn(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		default:
			return UNKNOWN_ERROR;
	}
}

const char* msg_CMD_xpci3xxx_Get32DigitalOutputStatus(int nb)
{
	switch(nb)
	{
		case 0:
			return SUCCESS;
		case 1:
			return WRONG_BOARD_HANDLE;
		default:
			return UNKNOWN_ERROR;
	}
}
