/* demonstrates how to use the TTL I/O
 *
 * it configures the bidirectional TTL as input
 * and print the values on the standard output
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
int ttl_sync(int fd)
{
	{
		uint32_t inputs = 0;
		if( call_ioctl(fd, CMD_xpci3xxx_Read32DigitalInputs, &inputs) )
			return -1;
	}
	return 0;
}
//--------------------------------------------------------------------------------
int run_ttl_test(int board, int fd, unsigned int howmany)
{
	/* configure all 8 bidirectional TTL as input */
	{
		uint8_t arg = 0x0;
		if(call_ioctl(fd,CMD_xpci3xxx_SetTTLPortConfiguration,&arg))
			return -1;
	}
	/* "fake" read to synchronize TTL input register, needed if it was configured as output */
	ttl_sync(fd);
	ttl_sync(fd);

	/* goes in a loop, reading up to howmany */
	{
		int i = 0;
		while(1)
		{
			uint32_t inputs = 0;
			if( call_ioctl(fd, CMD_xpci3xxx_Read32DigitalInputs, &inputs) )
				return -1;

			printf("%X\n",(inputs>>16));

			if( howmany && i>=howmany)
				break;

			i++;
		}
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
		unsigned int howmany;

		if(xpci3xxx_card_number==1)
		{
			choice = 0;
		}
		else
		{
			printf("board to test? ");
			choice = get_integer(0,xpci3xxx_card_number-1);
		}

		/* number of read */
		printf("how many (0 for infinity)? ");
		howmany = get_integer(0,100000);

		return run_ttl_test(choice,xpci3xxx_card_fd[choice],howmany);

	}
}
