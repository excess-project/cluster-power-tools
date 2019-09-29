#include "affinity.h"
#include "omp.h"
#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 600
#endif
#include <stdio.h>
#ifndef __USE_GNU
#define __USE_GNU
#endif
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <errno.h>

void affinity_set_aff()
{
	int thread_num = omp_get_thread_num();
        int err;
        cpu_set_t processor_mask;
        cpu_set_t new_mask;
        unsigned int len = sizeof(new_mask);
	cpu_set_t cur_mask;
        int ret;
	pid_t p = 0;
        CPU_ZERO(&processor_mask);
        ret = sched_getaffinity(p, len, &cur_mask);
        CPU_SET(thread_num,&processor_mask);
	//printf("%d:current sched_getaffinity = %d, cur_mask = %08lx\n",thread_num, ret, processor_mask);
        err = sched_setaffinity( 0, sizeof(cpu_set_t), &processor_mask );
        //ret = sched_getaffinity(p, len, &new_mask);
	//printf("%d:new sched_getaffinity = %d, cur_mask = %08lx\n", thread_num,ret, new_mask);

        if(err<0)
        {
          printf("affinity_set_aff error:%s\n",strerror(errno));
        }
}
