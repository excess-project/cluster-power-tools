
#include "cpufreq_user.h"
#include <stdio.h>
#include <string.h>
/*
 * Get cpu clock frequency by
 * reading the entire contents of /proc/cpuinfo into the buffer. 
 * @http://www.advancedlinuxprogramming.com/alp-folder/alp-ch07-proc-filesystem.pdf
 */
double get_cpu_clock_speed ()
{
  FILE* fp;
  char buffer[131072];
  size_t bytes_read;
  char* match;
  float clock_speed;
  
  fp = fopen("/proc/cpuinfo", "r");
  bytes_read = fread(buffer, 1, sizeof (buffer), fp);
  fclose (fp);/* Bail if read failed or if buffer isn’t big enough.  */
  
  if (bytes_read == 0  || bytes_read == sizeof (buffer))   return (double)-1.0;
   /* NUL-terminate the text.  */
  buffer[bytes_read] = '\0';
   /* Locate the line that starts with “cpu GHz”.  */
  match = strstr (buffer, "cpu MHz");
   if (match == NULL)   return (double)-2.0;
  /* Parse the line to extract the clock speed.  */
  sscanf (match, "cpu MHz : %f", &clock_speed);
  return (double)clock_speed;
}

unsigned long* cpufreq_get_available_cpufrequencies(unsigned int cpu, long* number_of_cpufrequencies)
{
  unsigned long* available_cpufrequencies;
  struct cpufreq_available_frequencies* cpufrequencies;
  struct cpufreq_available_frequencies* first_cpufrequencies;
  struct cpufreq_available_frequencies* current_cpufrequencies;
  long lnumber_of_cpufrequencies;
  long ii;

  available_cpufrequencies = Nullptr(unsigned long);
  cpufrequencies = cpufreq_get_available_frequencies(cpu);
  first_cpufrequencies  = cpufrequencies;
  current_cpufrequencies  = cpufrequencies;
  lnumber_of_cpufrequencies = 0;
  while(current_cpufrequencies!=Nullptr(struct cpufreq_available_frequencies))
  {
           lnumber_of_cpufrequencies++;
           current_cpufrequencies=current_cpufrequencies->next;
   }
  if(lnumber_of_cpufrequencies>0)
  {
          available_cpufrequencies = (unsigned long*)malloc(sizeof(unsigned long)*lnumber_of_cpufrequencies);
          
  }
  current_cpufrequencies  = first_cpufrequencies;
  lnumber_of_cpufrequencies = 0;
  while(current_cpufrequencies!=Nullptr(struct cpufreq_available_frequencies))
  {
           available_cpufrequencies[lnumber_of_cpufrequencies] = current_cpufrequencies->frequency;
           lnumber_of_cpufrequencies++;
           current_cpufrequencies=current_cpufrequencies->next;
   }
  cpufreq_put_available_frequencies(cpufrequencies);
  *number_of_cpufrequencies = lnumber_of_cpufrequencies;

  return available_cpufrequencies;
}

unsigned long* cpufreq_get_correct_cpufreq(unsigned long* available_cpufrequencies)
{
    long ii;
    
    unsigned long* correct_available_cpufrequencies = (unsigned long*) malloc(16*sizeof(unsigned long));
#ifdef CPU_I5_2500
    correct_available_cpufrequencies[15]=1600000;
    correct_available_cpufrequencies[14]=2300000;
    correct_available_cpufrequencies[13]=2400000;
    correct_available_cpufrequencies[12]=2500000;
    correct_available_cpufrequencies[11]=2600000;
    correct_available_cpufrequencies[10]=2700000;
    correct_available_cpufrequencies[9]=2800000;
    correct_available_cpufrequencies[8]=2900000;
    correct_available_cpufrequencies[7]=3000000;
    correct_available_cpufrequencies[6]=3100000;
    correct_available_cpufrequencies[5]=3200000;
    correct_available_cpufrequencies[4]=3300000;
    correct_available_cpufrequencies[3]=3400000;
    correct_available_cpufrequencies[2]=3500000;
    correct_available_cpufrequencies[1]=3600000;
    correct_available_cpufrequencies[0]=3700000;
#endif
#ifdef CPU_E5_2687W
    correct_available_cpufrequencies[0]=1200000;
    correct_available_cpufrequencies[1]=1300000;
    correct_available_cpufrequencies[2]=1500000;
    correct_available_cpufrequencies[3]=1600000;
    correct_available_cpufrequencies[4]=1700000;
    correct_available_cpufrequencies[5]=1900000;
    correct_available_cpufrequencies[6]=2000000;
    correct_available_cpufrequencies[7]=2100000;
    correct_available_cpufrequencies[8]=2300000;
    correct_available_cpufrequencies[9]=2400000;
    correct_available_cpufrequencies[10]=2500000;
    correct_available_cpufrequencies[11]=2700000;
    correct_available_cpufrequencies[12]=2800000;
    correct_available_cpufrequencies[13]=3000000;
    correct_available_cpufrequencies[14]=3100000;
    correct_available_cpufrequencies[15]=3101000;
#endif
#ifdef CPU_E5_2690V2
    correct_available_cpufrequencies[0]=1200000;
    correct_available_cpufrequencies[1]=1300000;
    correct_available_cpufrequencies[2]=1500000;
    correct_available_cpufrequencies[3]=1600000;
    correct_available_cpufrequencies[4]=1700000;
    correct_available_cpufrequencies[5]=1800000;
    correct_available_cpufrequencies[6]=2000000;
    correct_available_cpufrequencies[7]=2100000;
    correct_available_cpufrequencies[8]=2200000;
    correct_available_cpufrequencies[9]=2400000;
    correct_available_cpufrequencies[10]=2500000;
    correct_available_cpufrequencies[11]=2600000;
    correct_available_cpufrequencies[12]=2700000;
    correct_available_cpufrequencies[13]=2900000;
    correct_available_cpufrequencies[14]=3000000;
    correct_available_cpufrequencies[15]=3001000;
#endif
#ifdef CPU_E5_2680V3
    correct_available_cpufrequencies[0]=1200000;
    correct_available_cpufrequencies[1]=1300000;
    correct_available_cpufrequencies[2]=1400000;
    correct_available_cpufrequencies[3]=1500000;
    correct_available_cpufrequencies[4]=1600000;
    correct_available_cpufrequencies[5]=1700000;
    correct_available_cpufrequencies[6]=1800000;
    correct_available_cpufrequencies[7]=1900000;
    correct_available_cpufrequencies[8]=2000000;
    correct_available_cpufrequencies[9]=2100000;
    correct_available_cpufrequencies[10]=2200000;
    correct_available_cpufrequencies[11]=2300000;
    correct_available_cpufrequencies[12]=2400000;
    correct_available_cpufrequencies[13]=2500000;
    correct_available_cpufrequencies[14]=2510000;
#endif

   return correct_available_cpufrequencies;
    
}

unsigned long* cpufreq_get_correct_turbofreq(unsigned long* available_turbofreq)
{
#ifdef CPU_E5_2680V3
    available_turbofreq[0]=3300;
    available_turbofreq[1]=3300;
    available_turbofreq[2]=3100;
    available_turbofreq[3]=3000;
    available_turbofreq[4]=2900;
    available_turbofreq[5]=2900;
    available_turbofreq[6]=2900;
    available_turbofreq[7]=2900;
    available_turbofreq[8]=2900;
    available_turbofreq[9]=2900;
    available_turbofreq[10]=2900;
    available_turbofreq[11]=2900;
#endif
#ifdef CPU_E5_2690V2
    available_turbofreq[0]=3600;
    available_turbofreq[1]=3600;
    available_turbofreq[2]=3500;
    available_turbofreq[3]=3500;
    available_turbofreq[4]=3400;
    available_turbofreq[5]=3400;
    available_turbofreq[6]=3300;
    available_turbofreq[7]=3300;
    available_turbofreq[8]=3300;
    available_turbofreq[9]=3300;
#endif

   return available_turbofreq;
    
}

unsigned long cpufreq_current_frequency(unsigned int cpu)
{
        return cpufreq_get(cpu);
}

void cpufreq_print_available_cpufrequencies()
{
        unsigned long* available_cpufrequencies;
        long number_of_cpufrequencies;
        unsigned int ii;
        long jj;

        for(ii=0; ii<1; ii++)
        {
                fprintf(stderr,"\nindicies of the cpu frequencies\n");
                available_cpufrequencies = cpufreq_get_available_cpufrequencies(ii, &number_of_cpufrequencies );
                if(available_cpufrequencies!=Nullptr(unsigned long))
                {
                        for(jj=0; jj<number_of_cpufrequencies; jj++)
                                fprintf(stderr,"%3lu: %lu; \n",jj,available_cpufrequencies[jj]);
                }
                free(available_cpufrequencies);
        }
}

void cpufreq_print_available_governors()
{
  long ii;
  struct cpufreq_available_governors* all_governors;
  struct cpufreq_available_governors* current_governor;

  for(ii=0; ii<1; ii++)
  {
    fprintf(stderr,"possible governors:\n");
    all_governors = cpufreq_get_available_governors(ii);
    current_governor = all_governors->first;
    while(current_governor!=Nullptr(struct cpufreq_available_governors))
    {
      fprintf(stderr,"%s\n",current_governor->governor);
      current_governor = current_governor->next;
     } 
    cpufreq_put_available_governors(all_governors);
  }
}



int cpufreq_set_userspace_governor_policy(unsigned int cpu_id, unsigned long** cpufrequencies,  long* number_of_cpufrequencies)
{
  unsigned int ii;
  long lnumber_of_cpufrequencies;
  struct cpufreq_available_governors* all_governors;
  struct cpufreq_available_governors* current_governor;
  unsigned long* frequencies;
  int err;
  int flag;
  char* name;
  err = 0;
  frequencies = Nullptr(unsigned long);

  all_governors = cpufreq_get_available_governors(0);
  current_governor = all_governors->first;
  while(current_governor!=Nullptr(struct cpufreq_available_governors) && !flag)
  {
    name = current_governor->governor;
    flag = name[0]=='u' && name[1]=='s' && name[2]=='e' && name[3]=='r' && name[4]=='s' && name[5]=='p' && name[6]=='a' && name[7]=='c' && name[8]=='e' && name[9]==(char)0;
    if(flag==0)
      current_governor = all_governors->next;
  }
  if(frequencies == Nullptr(unsigned long))
  frequencies= cpufreq_get_available_cpufrequencies(0, &lnumber_of_cpufrequencies);

  if(cpu_id>CPUFREQ_NUMBER_OF_CORES-1)
  {
    for(ii=0; ii<CPUFREQ_NUMBER_OF_CORES; ii++)
    {
      err += cpufreq_modify_policy_governor(ii,"userspace");
      err += cpufreq_set_frequency(ii, frequencies[lnumber_of_cpufrequencies-1]);
    }
  }else
  {
    err += cpufreq_modify_policy_governor(cpu_id,"userspace");
    err += cpufreq_set_frequency(cpu_id, frequencies[lnumber_of_cpufrequencies-1]);
  }
  if(err>0)
  {
    fprintf(stderr,"could not set the governor userspace on the core %d:\n",(int)ii);
    cpufreq_put_available_governors(all_governors);
  }

  *number_of_cpufrequencies = lnumber_of_cpufrequencies;
  *cpufrequencies = frequencies;

  return err;
}

int cpufreq_set_ondemand_governor_policy(unsigned cpu_id)
{
  unsigned int ii;
  long lnumber_of_cpufrequencies;
  struct cpufreq_available_governors* all_governors;
  struct cpufreq_available_governors* current_governor;
  struct cpufreq_available_frequencies * available_frequencies;
  unsigned long* frequencies;
  int err;
  int flag;
  char* name;
  err = 0;
  frequencies = Nullptr(unsigned long);

  all_governors = cpufreq_get_available_governors(0);
  current_governor = all_governors->first;
  while(current_governor!=Nullptr(struct cpufreq_available_governors) && !flag)
  {
    name = current_governor->governor;
    flag = name[0]=='u' && name[1]=='s' && name[2]=='e' && name[3]=='r' && name[4]=='s' && name[5]=='p' && name[6]=='a' && name[7]=='c' && name[8]=='e' && name[9]==(char)0;
    if(flag==0)
      current_governor = all_governors->next;
  }
	if(frequencies == Nullptr(unsigned long))
	 	frequencies= cpufreq_get_available_cpufrequencies(0, &lnumber_of_cpufrequencies);

  if(cpu_id>CPUFREQ_NUMBER_OF_CORES-1)
	{
    for(ii=0; ii<CPUFREQ_NUMBER_OF_CORES; ii++)
    {
      err += cpufreq_modify_policy_governor(ii,"ondemand");
      err += cpufreq_modify_policy_min(ii, frequencies[lnumber_of_cpufrequencies-1]);
      err += cpufreq_modify_policy_max(ii, frequencies[0]);
    }
	}else
	{
      err += cpufreq_modify_policy_governor(cpu_id,"ondemand");
      err += cpufreq_modify_policy_min(cpu_id, frequencies[lnumber_of_cpufrequencies-1]);
      err += cpufreq_modify_policy_max(cpu_id, frequencies[0]);
	}
	
	if(err>0)
  {
    fprintf(stderr,"could not set the governor ondemand on the core %lu:\n",cpu_id);
    cpufreq_put_available_governors(all_governors);
    free(frequencies);
  }
        return err;
}

int cpufreq_set_userspace_cpufrequency(unsigned int cpu_id, unsigned long cpufrequency)
{
  unsigned int ii;
  int err;
  err = 0;
  if(cpu_id >CPUFREQ_NUMBER_OF_CORES-1)
  {
    for(ii=0; ii<CPUFREQ_NUMBER_OF_CORES; ii++)
	  {
  	  err += cpufreq_set_frequency(ii, cpufrequency);
    }
  }else
  {
    err += cpufreq_set_frequency(cpu_id, cpufrequency);
  }
  return err;
}


void cpufreq_print_copyright()
{
  fprintf(stdout,"\n#*\n");
  fprintf(stdout,"#*\tHW-Threads Frequency Tool\n");
  fprintf(stdout,"#*\tHigh Performance Computing Center Stuttgart (HLRS)\n");
  fprintf(stdout,"#*\thttp://www.hlrs.de/\n");
  fprintf(stdout,"#*\tUniversity of Stuttgart\n");
  fprintf(stdout,"#*\tmail bugs to: khabi@hlrs.de\n");
  fprintf(stdout,"#*\tbased on cpufreq kernel modul\n");
  fprintf(stdout,"#*\n\n");
}

void cpufreq_print_options_help()
{
	int ii;
  fprintf(stdout,"options include:\n");
  fprintf(stdout,"\t-p  <int>\t: cpu id (default 0)\n");
  fprintf(stdout,"\t-c  <int>\t: command id(see below)\n");
  fprintf(stdout,"\t\t0 - do nothing (nop)\n");
  fprintf(stdout,"\t\t1 - print possible governor\n");
  fprintf(stdout,"\t\t2 - print possible frequencies\n");
  fprintf(stdout,"\t\t3 - set \"ondemand\" governor\n");
  fprintf(stdout,"\t\t4 - set \"userspace\" governor\n");
  fprintf(stdout,"\t\t5 - print current frequency\n");
  fprintf(stdout,"\t\t6 - print current governor  (not implemented; use cat /sys/devices/system/cpu/...)\n");
  fprintf(stdout,"\n\t-f  <int>\t: set frequency by its index (use -c 4 to show the indices for desirable frequency)\n"); 
  fprintf(stdout,"\n\t-h           \t: show this help text and exit (do nothing)\n");
  fprintf(stdout,"\n\texamples:\n");
  fprintf(stdout,"\n\t\t -p 0 -c 4\t: set \"userspace\" governor on the first cpu\n");
  fprintf(stdout,"\n\t\t -p 0 -f 0\t: set the lowest freqquency on the first cpu\n");
  fprintf(stdout,"\n\t\t -p 0 -c 4\t: set \"ondemand\" governor on the first cpu\n");
}

int main_hide ( int argc, char ** argv )
{
        unsigned int ii;
        unsigned long* frequencies;
        long number_of_cpufrequencies;
        int err;
	unsigned int cpu_id;
        int command_id;
        int frequency_index;
        char opt;
	unsigned long current_frequency_indx;

	cpu_id = CPUFREQ_NUMBER_OF_CORES+1;
        command_id=-1;
        frequency_index=-1;
        frequencies = Nullptr(unsigned long);
  while (( opt = (char)(getopt( argc, argv, "c:f:h:p:?:"))) != -1 )
  {
    switch ( opt )
    {
      case 'c' : command_id = atoi( optarg ); break;
      case 'f' : frequency_index = atoi( optarg ); break;
      case 'p' : cpu_id = atoi( optarg ); break;
      case 'h':
      case '?':
        cpufreq_print_copyright();
      	cpufreq_print_options_help();
        return 0;
      default :
        printf("%c - udefined option;\n", opt);
      return 1;
    }
  }


  if(command_id>=0)
  {
    switch(command_id)
    {
      case 0: 
        ;
        break;
      case 1: 
        cpufreq_print_available_governors();
        break;
      case 2: 
        cpufreq_print_available_cpufrequencies();
        break;
      case 3: 
        err = cpufreq_set_ondemand_governor_policy(cpu_id);
        if(err!=0)
          fprintf(stderr,"ondemand governor was set(?) with errors (root rights - sudo?)\n");
      break;
      case 4: 
        err = cpufreq_set_userspace_governor_policy(cpu_id, &frequencies, &number_of_cpufrequencies);
        if(err!=0)
          fprintf(stderr,"userspace governor was set(?) with errors (root rights - sudo?) \n");
        free(frequencies);
      break;
      case 5:
        if(cpu_id > CPUFREQ_NUMBER_OF_CORES -1)
        {
          for(ii=0;ii<CPUFREQ_NUMBER_OF_CORES;ii++)
            fprintf(stderr,"cpu:%lu; freq:%lu\n",ii,cpufreq_current_frequency(ii));
        }else
        {
          current_frequency_indx = cpufreq_current_frequency(cpu_id);
          fprintf(stderr,"current cpufreq: %lu \n",current_frequency_indx);
        }
				break;
			default :
				printf("%d - udefined command;\n", command_id);
				return 1;
			break;
    }
        
  }

  if(frequency_index>=0)
  {

  frequencies = cpufreq_get_available_cpufrequencies(0, &number_of_cpufrequencies);
  if(frequency_index<number_of_cpufrequencies)
  {
   err = cpufreq_set_userspace_cpufrequency(cpu_id,frequencies[frequency_index]);
  }else
    err = -1;
    free(frequencies);
  if(err!=0)
    fprintf(stderr,"frequency was set(?) with errors (root rights - sudo?)\n");
  }


  return err;
}

