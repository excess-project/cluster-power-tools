
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <xpci3xxx.h>
#include "errorlib.h"
#include "findboards.h"
#include "errormsg.h"

int wait_status(int fd, unsigned int ui_FirmwareAddress)
{
	uint32_t dw_Status = 0;
	struct timeval tv_start;
	struct timeval tv_now;

	gettimeofday(&tv_start, NULL);
	tv_start.tv_sec += 1;

	do
	{
		dw_Status = ui_FirmwareAddress;
		if (call_ioctl (fd, CMD_xpci3xxx_INPORTDW, &dw_Status))
			return 1;

		if (gettimeofday(&tv_now, NULL) < 0)
		{
			printf ("Timeout management error\n");
			return 2;
		}

		if (tv_start.tv_sec < tv_now.tv_sec)
		{
			printf ("Wait status timeout\n");
			return 2;
		}

	} while (((dw_Status) & 0x10) == 0x10);

	return 0;
}

int i_WriteRBF (int fd, unsigned int ui_FirmwareAddress, LONG l_RBFFileLength, unsigned char * pb_FlashData)
{
	uint32_t dw_Status = 0;
	long l_AdrCpt = 0;
	uint8_t b_ReadFlashData;
	int i_Wait = 0;
	uint32_t arg[2] = {0};

	dw_Status = ui_FirmwareAddress + 0x4;

	if (call_ioctl (fd, CMD_xpci3xxx_INPORTDW, &dw_Status))
		goto error;

	if (dw_Status != 0x41646469 )
		return 0;

    printf ("Erase the PLD: ");
           
    arg[0] = ui_FirmwareAddress;
    arg[1] = 3;
	if (call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg))
		goto error;
           
	if (wait_status (fd, ui_FirmwareAddress))
		goto error;

	printf ("done\n");

   /* Enable the write command */
    arg[0] = ui_FirmwareAddress;
    arg[1] = 5;
	if (call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg))
		goto error;
           
	if (wait_status (fd, ui_FirmwareAddress))
		goto error;

	for (l_AdrCpt = 0; l_AdrCpt < l_RBFFileLength; l_AdrCpt++)
	{
		/* Write the data */
	    arg[0] = ui_FirmwareAddress + 0x4;
	    arg[1] = pb_FlashData[l_AdrCpt];
		if (call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg))
			goto error;

		if (wait_status (fd, ui_FirmwareAddress))
			goto error;

		printf ("Write the PLD: %i %%\r", (int)((100.0 / (l_RBFFileLength - 1)) * l_AdrCpt));
	}

	/* Disable the write command */
	arg[0] = ui_FirmwareAddress;
	arg[1] = 1;
	if (call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg))
		goto error;
           
	if (wait_status (fd, ui_FirmwareAddress))
		goto error;

	/* Enable the read command */
	arg[0] = ui_FirmwareAddress;
	arg[1] = 9;
	if (call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg))
		goto error;
           
	if (wait_status (fd, ui_FirmwareAddress))
		goto error;

	printf ("\n");

	for (l_AdrCpt = 0; l_AdrCpt < l_RBFFileLength; l_AdrCpt++)
	{
		dw_Status = ui_FirmwareAddress + 0x4;
		if (call_ioctl (fd, CMD_xpci3xxx_INPORTDW, &dw_Status))
			goto error;

		b_ReadFlashData = (uint8_t) (dw_Status & 0xFF);

		if (wait_status (fd, ui_FirmwareAddress))
			goto error;

		if (b_ReadFlashData != pb_FlashData[l_AdrCpt])
		{
			printf ("\nFlash address %li data error\n", l_AdrCpt);
			break;
		}

		printf ("Verify the PLD: %i %%\r", (int)((100.0 / (l_RBFFileLength - 1)) * l_AdrCpt));
	}

	/* Disable the write command */
	arg[0] = ui_FirmwareAddress;
	arg[1] = 1;
	if (call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg))
		goto error;
           
	if (l_AdrCpt != l_RBFFileLength)
    {
		printf ("\nFail to write the whole RBF");
		goto error;
    }

	printf ("\nLoad the PLD: ");

	/* Enable the read command */
	arg[0] = ui_FirmwareAddress;
	arg[1] = 0x21;
	call_ioctl (fd, CMD_xpci3xxx_OUTPORTDW, arg);
	sleep(1);

	do
	{
		dw_Status = ui_FirmwareAddress + 0x4;
		if (call_ioctl (fd, CMD_xpci3xxx_INPORTDW, &dw_Status))
			goto error;

		sleep(1);
		i_Wait = i_Wait + 1;

		if (i_Wait == 5)
		{
			printf ("The computer has to be restarted\n");
			break;
		}
	} while (dw_Status != 0x41646469 );
	printf ("done");

error:
	return 0;
}

int main (int argc, char **argv)
{
	uint32_t dw_FirmwareRevision = 0;
	unsigned char * pb_RBFFileData = NULL;
	int xpci3xxxfd = 0;
	int rbffd = 0;
	struct stat sbuf;

	if (argc < 3)
	{
		printf ("hpc_update file.rbf /dev/xpci3xxx_x\n");
		return EXIT_FAILURE;
	}

	if ((xpci3xxxfd = open(argv[2], O_RDWR)) == -1)
	{
		perror("Fail to open the xpci3xxx");
		return EXIT_FAILURE;
	}

	/* Open RBF File */
	rbffd = open (argv[1], O_RDONLY, S_IREAD);

	if (rbffd == -1)
	{
		perror ("Fail to open the RBF file (opne)");
		goto error;
	}

	if (fstat (rbffd, &sbuf) < -1)
	{
		perror ("Fail to get RBF file informations (fstat)");
		goto error;
	}

	if (sbuf.st_size <= 0)
	{
		printf ("Bad RBF file (size <= 0)\n");
		goto error;
	}

	pb_RBFFileData = (unsigned char *) malloc (sbuf.st_size);
	if (pb_RBFFileData == NULL)
	{
		perror ("Fail to allocate memory (malloc)");
		goto error;
	}

	if (read (rbffd, pb_RBFFileData, sbuf.st_size) != sbuf.st_size)
	{
		printf ("Fail to read the RBF file\n");
		goto error;
	}

    close (rbffd);
                 
	/* Get the firmware revision */
    dw_FirmwareRevision = 0;
	if (call_ioctl (xpci3xxxfd, CMD_xpci3xxx_INPORTDW, &dw_FirmwareRevision))
		goto error;
           
    printf ("\nFirmware Revision = %c%c%c%c\n", (char) ((dw_FirmwareRevision >> 25) & 0x7F),
    									  (char) ((dw_FirmwareRevision >> 18) & 0x7F),
    									  (char) ((dw_FirmwareRevision >> 11) & 0x7F),
    									  (char) ((dw_FirmwareRevision >> 4) & 0x7F));
           
    /* Write the firmware */
    i_WriteRBF (xpci3xxxfd, 16, sbuf.st_size, pb_RBFFileData);
           
	/* Get the firmware revision */
    dw_FirmwareRevision = 0;
	if (call_ioctl (xpci3xxxfd, CMD_xpci3xxx_INPORTDW, &dw_FirmwareRevision))
		goto error;
           
    printf ("\nNew firmware Revision = %c%c%c%c\n", (char) ((dw_FirmwareRevision >> 25) & 0x7F),
    									  (char) ((dw_FirmwareRevision >> 18) & 0x7F),
    									  (char) ((dw_FirmwareRevision >> 11) & 0x7F),
    									  (char) ((dw_FirmwareRevision >> 4) & 0x7F));

error:

	if (rbffd != 0)
		close (rbffd);

	if (xpci3xxxfd != 0)
		close (xpci3xxxfd);

	return EXIT_FAILURE;
}
