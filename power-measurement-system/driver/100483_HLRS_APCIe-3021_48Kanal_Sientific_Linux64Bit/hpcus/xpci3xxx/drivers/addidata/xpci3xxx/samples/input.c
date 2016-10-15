#include <stdio.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>

#include "input.h"

/* safely read an integer from standard input - loop until input is correct */
int get_integer(int min, int max)
{
	int val;
    do
    {
    	char buff[10];
        char * endptr = NULL;

        printf("(%d-%d)",min,max);
        fgets(buff,sizeof(buff)-1,stdin);
        errno=0;
        val = strtol(buff, &endptr, 10);
        if ((errno == ERANGE && (val == LONG_MAX || val == LONG_MIN)) || (errno != 0 && val == 0)) { perror("strtol") ; continue; }
        if (endptr == buff) { fprintf(stderr, "input is not a digit\n"); continue; }
        if ( (val >= min) && (val <= max) ) return val;
        fprintf(stderr,"input is out of range\n");
	}
    while(1);
}
