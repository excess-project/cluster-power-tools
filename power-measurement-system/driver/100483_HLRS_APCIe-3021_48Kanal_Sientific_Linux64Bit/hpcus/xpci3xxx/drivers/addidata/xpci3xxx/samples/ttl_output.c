/* demonstrates how to use the TTL I/O
 *
 * it configures the bidirectional TTLs as output
 * and interactively allows to clear or set I/O
 *
 * */
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdlib.h>

#include <xpci3xxx.h>
#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"
#include "input.h"

//--------------------------------------------------------------------------------
int output_ttl(int fd)
{
	uint32_t inputs = 0;
	if( call_ioctl(fd, CMD_xpci3xxx_Get32DigitalOutputStatus, &inputs) )
		return -1;
	printf("TTL state: %X\n",(inputs>>16));
	return 0;
}
//--------------------------------------------------------------------------------
int set_ttl(int fd, int action, int which)
{
	uint32_t bitmask = (1 << which) << 16;

	printf("%X\n",bitmask);

	if(action==0) /* clear */
	{
		if( call_ioctl(fd, CMD_xpci3xxx_Set32DigitalOutputsOff,&bitmask) )
			return -1;
	}
	else	/* set */
	{
		if( call_ioctl(fd, CMD_xpci3xxx_Set32DigitalOutputsOn,&bitmask) )
			return -1;
	}
	return 0;
}

//--------------------------------------------------------------------------------
int run_ttl_test(int board, int fd)
{
	/* configures all 8 bidirectional TTL as output */
	{
		uint8_t arg = 1;
		if(call_ioctl(fd,CMD_xpci3xxx_SetTTLPortConfiguration,&arg))
			return -1;
	}

	/* activates digital output memory */
	if( call_ioctl(fd, CMD_xpci3xxx_SetDigitalOutputMemoryOn, NULL) )
		return -1;

	/* clears all TTL outputs */
	{
		uint32_t bitmask = 0xFFFF0000;
		if( call_ioctl(fd, CMD_xpci3xxx_Set32DigitalOutputsOff,&bitmask) )
			return -1;
	}
	/* goes in a loop, ask user */
	while(1)
	{
		int action = 0; /* 0: clear | 1: set */
		int which = 0; /* TTL I/O index : 0 .. 15 */

		printf("\n");
		output_ttl(fd);

		printf("\n0: clear TTL\n");
		printf("1: set TTL\n");
		printf("choice");
		action = get_integer(0,1);

		printf("index of TTL I/O ");
		which = get_integer(0,15);

		if( set_ttl(fd, action, which) )
			return -1;
	}
	return 0;
}

//--------------------------------------------------------------------------------
int main(void)
{
	int * xpci3xxx_card_fd; /* array of file descriptors */
	unsigned int xpci3xxx_card_number = apci_find_boards("xpci3xxx", &xpci3xxx_card_fd);
	if (xpci3xxx_card_number==-1)
		exit(1);

	if (xpci3xxx_card_number==0)
	{
		printf("no board found\n");
		exit(1);
	}
	{
		unsigned int choice;

		if(xpci3xxx_card_number==1)
		{
			choice = 0;
		}
		else
		{
			printf("board to test? ");
			choice = get_integer(0,xpci3xxx_card_number-1);
		}

		return run_ttl_test(choice,xpci3xxx_card_fd[choice]);

	}
}
