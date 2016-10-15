#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/sysmacros.h> /* major() minor() */
#include <signal.h>
#include <errno.h>
#include <limits.h>

#include "findboards.h"

#define SZ_DEVICE_DIR (255)
#define SZ_PROC_PATH_TEMPLATE (10)

//------------------------------------------------------------------------------
/** Filter the "." and ".." entries in the struct dirent ** area set up by the scandir() function (utility function).*/
static int get_real_dir_entry (const struct dirent * ent)
{
    if (! strcmp(ent->d_name,"."))
            return 0;
    if (! strcmp(ent->d_name,".."))
            return 0;

    return 1;
}
//--------------------------------------------------------------------------------
/* @retval boolean 0 if path does NOT exists, boolean 1 if path exists */
static int path_exists(const char* path)
{
	struct stat file_stat;
	return ( stat(path,&file_stat) == 0);
}
//--------------------------------------------------------------------------------
/** @retval 0 if path exists - set device_dir and proc_path_template */
static int check_path(char device_dir[SZ_DEVICE_DIR], char proc_path_template[SZ_PROC_PATH_TEMPLATE], const char* path, const char* template)
{
	if (path_exists(path))
	{
		memset(device_dir,0,SZ_DEVICE_DIR);
		memset(proc_path_template,0,SZ_PROC_PATH_TEMPLATE);
		strncpy(device_dir, path, SZ_DEVICE_DIR);
		strncpy(proc_path_template, template, SZ_PROC_PATH_TEMPLATE);
		return 0;
	}
	return -1;
}
//--------------------------------------------------------------------------------
/** @retval 0 if dir. rootpath/boardname exists - set device_dir and proc_path_template */
static int check_device_directory(const char* boardname, const char* rootpath, const char* template, char device_dir[SZ_DEVICE_DIR], char proc_path_template[SZ_PROC_PATH_TEMPLATE])
{
	char tmpbuff[strlen(rootpath)+strlen(boardname)+2];
	strcpy(tmpbuff, rootpath);
	strcat(tmpbuff, boardname);
	strcat(tmpbuff, "/");

	return check_path(device_dir, proc_path_template, tmpbuff, template);
}
//--------------------------------------------------------------------------------
/** autodetect where devices are present.
 *
 * first search in /proc/sys/, then /dev.
 *
 * @param boardname Name of the board (such as apcixxx)
 * @param device_dir directory where devices were found
 * @param proc_path_template template to apply when searching for a board
 *
 * @retval 0 on success, non zero on failure
 */
static int autodetect_device_directory(const char* boardname, char device_dir[255], char proc_path_template[10])
{
	if(0==check_device_directory(boardname,"/proc/sys/","%s%s/ctrl",device_dir,proc_path_template))
		return 0;

	if(0==check_device_directory(boardname,"/dev/","%s%s",device_dir,proc_path_template))
		return 0;

	return -1;
}
//--------------------------------------------------------------------------------
/** @retval 0 on success, non zero otherwise */
static int open_and_store(int ** boards, char proc_path_template[SZ_PROC_PATH_TEMPLATE], char device_dir[SZ_DEVICE_DIR], const char* d_name)
{
	int fd;
	char tmpname[255];

	snprintf(tmpname,sizeof(tmpname),proc_path_template,device_dir,d_name);

	if ( ( fd = open(tmpname, O_RDWR)) == -1 )
	{
		perror(tmpname);
		return 1;
	}

	{
		struct stat file_stat;
		if ( fstat(fd,&file_stat) )
		{
			perror("fstat");
			goto on_error;
		}
		if (!S_ISCHR(file_stat.st_mode))
		{
			/* should never happen ! */
			goto on_error;
		}

		//printf("%s, major:%d minor:%d, fd=%d\n",name, major(file_stat.st_rdev),minor(file_stat.st_rdev), fd);
		/* device file descriptor are indexed by minor number */
		(*boards)[minor(file_stat.st_rdev)] = fd;
	}
	return 0;

	on_error:
		close(fd);
		return -1;
}
//--------------------------------------------------------------------------------
int apci_find_boards(const char* boardname, int ** boards)
{

	struct dirent **eps;
	int cnt = 0; // number of card found
	char device_dir[SZ_DEVICE_DIR]={0};
	char proc_path_template[SZ_PROC_PATH_TEMPLATE]={0};

	if( autodetect_device_directory(boardname, device_dir,proc_path_template) )
	{
		perror("autodetect_device_directory");
		return 0; /* no board found */
	}

	/* scan directory for card entry */
	cnt = scandir (device_dir, &eps, get_real_dir_entry , alphasort);

	switch(cnt)
	{
		case -1:
			perror("scandir");
			return -1; /* no board found */
		case 0:
			free(eps);
			return 0;
	}

	/* allocate cards area */
	if ( ! ( (*boards) = malloc( cnt * sizeof(int) ) ) )
	{
		perror("malloc");
		goto on_error;
	}

	/* for each card entry, open it and store file descriptor in cards[] */
	/* device file descriptor are indexed by minor number */
	{
		int i;
		for (i=0; i< cnt; i++)
		{
			if( open_and_store(boards, proc_path_template,device_dir,eps[i]->d_name) )
				goto on_error;
		}
	}
	return cnt;

	on_error:
		free(eps);
		return -1;
}
