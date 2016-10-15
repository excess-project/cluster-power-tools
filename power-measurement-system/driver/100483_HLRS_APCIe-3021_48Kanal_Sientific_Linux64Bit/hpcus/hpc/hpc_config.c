/**
 * hpc_config.c - The coefficients and relation of the measured channel 
 * are defined in the configuration files. This code helps
 * to read and interpret these.
 *
 * This is a free Software.
 * Code is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  
 *
 * Modifications of the original source code were done by
 * the High Performance Computing Center Stuttgart (HLRS)
 * University of Stuttgart
 * Nobelstraße 19
 * 70569 Stuttgart
 * Germany
 * Bugs to:khabi@hlrs.de
 **/


/** @file hpc_config.c
 *
 *
 * @author J. Krauth
 *
 * @par LICENCE
 * @verbatim

   Copyright (C) 2014  ADDI-DATA GmbH for the source code of this module.

   ADDI-DATA GmbH
   Airpark Business Center
   Airport Boulevard B210
   77836 Rheinmuenster
   Germany
   Tel: +49(0)7229/1847-0
   Fax: +49(0)7229/1847-200
   http://www.addi-data-com
   info@addi-data.com

 * @endverbatim
 *
 */

#include <errno.h>
#include <stdint.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <ini_config.h>

#include "hpc_config.h"
#include "hpc_utils.h"
#include "hpc.h"

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/** Read configuration informations from the config file.
 *
 * @param [in] config_file: Path to the configuration file.
 * @param [in] nBoards: The number of boards (structure array size).
 * @param [in,out] config: Configuration structure.
 *
 * @retval 0 No error.
 * @retval != 0 Error.
 */
int hpc_read_config_file (const char * config_file, struct iniConfig_t * config, int nBoards)
{
	struct collection_item * ini_config = NULL;
	struct collection_item * error_list = NULL;
	struct collection_item * item = NULL;
	uint32_t ui32val = 0;
	int ret = 0;

	ret = config_from_file ("hpc_measure", config_file, &ini_config, 0, &error_list);
	if (ret)
	{
		loginfo_error ("config_from_file error %i (%s)", ret, parsing_error_str (ret));
		goto error;
	}

	/* Read the config file channel informations */
	{
		char section[255] = {'0'};
		char name[255] = {'0'};
		uint32_t board = 0;
		uint32_t channel = 0;
		uint32_t nChannels = 0;
		double doubleval = 0.0;

		ret = get_config_item ("GLOBAL", "acquisition_time", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] acquisition_time, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_uint32_config_value [GLOBAL] acquisition_time, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].acquisition_time = ui32val;

		loginfo_debug ("acquisition_time\t= %u", ui32val);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "number_of_sequences_per_interrupt", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] number_of_sequences_per_interrupt, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_uint32_config_value [GLOBAL] number_of_sequences_per_interrupt, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].number_of_sequences_per_interrupt = ui32val;

		loginfo_debug ("number_of_sequences_per_interrupt\t= %u", ui32val);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "total_sequence_count", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] total_sequence_count, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_uint32_config_value [GLOBAL] total_sequence_count, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].total_sequence_count = ui32val;

		loginfo_debug ("total_sequence_count\t= %u", ui32val);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "duration_time", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] duration_time, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_uint32_config_value [GLOBAL] duration, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].duration_time = ui32val;

		loginfo_debug ("duration_time\t= %u", ui32val);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "trigger_on_off", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] trigger_on_off, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_uint32_config_value [GLOBAL] trigger_on_off, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].trigger_on_off = ui32val;

		loginfo_debug ("trigger_on_off\t= %u", ui32val);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "log_file_path", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] log_file_path, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		{
			const char *buff = get_const_string_config_value (item, &ret);
			strcpy (config[board].log_file_path, buff);
		}

		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] log_file_path, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		loginfo_debug ("log_file_path\t= %s", config[board].log_file_path);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "raw_file_path", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] raw_file_path, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		{
			const char *buff = get_const_string_config_value (item, &ret);
			strcpy (config[board].raw_file_path, buff);
		}

		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] raw_file_path, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		loginfo_debug ("raw_file_path\t= %s", config[board].raw_file_path);

		//-------------------------------------------------------------------------------------------------------------------

		ret = get_config_item ("GLOBAL", "ac_voltage_channel_id", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] ac_voltage_channel_id, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] ac_voltage_channel_id, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].ac_voltage_channel_id = ui32val;

		loginfo_debug ("ac_voltage_channel_id\t= %i", ui32val);

		//-------------------------------------------------------------------------------------------------------------------


		ret = get_config_item ("GLOBAL", "ac_num_periods", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] ac_num_periods, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] ac_num_periods, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		config[board].ac_num_periods = ui32val;

		loginfo_debug ("ac_num_periods\t= %i", ui32val);


		
		//-------------------------------------------------------------------------------------------------------------------

		for (board=0; (board<nBoards) ; board++)
		{
			sprintf (section, "BOARD_%i", board);

			/* New board */
			nChannels = 0;

			loginfo_debug ("[BOARD_%i]", board);

			//-------------------------------------------------------------------------------------------------------------------

			for (channel=0; (channel<8) ; channel++)
			{
				sprintf (name, "channel_%i_on_off", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_on_off, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				ui32val = get_uint32_config_value (item, 0, 0, &ret);
				if (ret)
				{
					loginfo_error ("get_uint32_config_value [BOARD_%i] channel_%i_on_off, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}
				config->board[board].is_on[nChannels] = ui32val;
				/* Read other values only if the channel is to be used */
				if (!ui32val)
					continue;

				//-------------------------------------------------------------------------------------------------------------------

				/* The channel is to be used */
				config->board[board].sequence[nChannels] = channel;

				loginfo_debug ("channel_%i_on_off\t= %u", channel, ui32val);

				//-------------------------------------------------------------------------------------------------------------------

				sprintf (name, "channel_%i_id", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_id, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				ui32val = get_uint32_config_value (item, 0, 0, &ret);
				if (ret)
				{
					loginfo_error ("get_uint32_config_value [BOARD_%i] channel_%i_id, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				config->board[board].id[nChannels] = ui32val;

				loginfo_debug ("channel_%i_id\t= %u", channel, ui32val);

				//-------------------------------------------------------------------------------------------------------------------
				sprintf (name, "channel_%i_label", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_label, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}
				{
					const char *buff = get_const_string_config_value (item, &ret);
					strcpy (config->board[board].label[nChannels], buff);
				}

				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_label, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				loginfo_debug ("channel_%i_label\t= %s", channel,config->board[board].label[nChannels]);

				//-------------------------------------------------------------------------------------------------------------------

				sprintf (name, "channel_%i_gain", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_gain, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				ui32val = get_uint32_config_value (item, 0, 0, &ret);
				if (ret)
				{
					loginfo_error ("get_uint32_config_value [BOARD_%i] channel_%i_gain, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				config->board[board].gain[nChannels] = ui32val;

				loginfo_debug ("channel_%i_gain\t= %u", channel, ui32val);

				//-------------------------------------------------------------------------------------------------------------------

				sprintf (name, "channel_%i_polarity", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_polarity, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				ui32val = get_uint32_config_value (item, 0, 0, &ret);
				if (ret)
				{
					loginfo_error ("get_uint32_config_value [BOARD_%i] channel_%i_polarity, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				config->board[board].polarity[nChannels] = ui32val;

				loginfo_debug ("channel_%i_polarity\t= %u", channel, ui32val);

				//-------------------------------------------------------------------------------------------------------------------

				sprintf (name, "channel_%i_coeff1", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_coeff1, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				doubleval = get_double_config_value (item, 0, 0, &ret);
				if (ret)
				{
					loginfo_error ("get_double_config_value [BOARD_%i] channel_%i_coeff1, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				config->board[board].coeff1[nChannels] = doubleval;

				loginfo_debug ("channel_%i_coeff1\t= %lf", channel, doubleval);

				//-------------------------------------------------------------------------------------------------------------------

				sprintf (name, "channel_%i_coeff2", channel);

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [BOARD_%i] channel_%i_coeff2, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				doubleval = get_double_config_value (item, 0, 0, &ret);
				if (ret)
				{
					loginfo_error ("get_double_config_value [BOARD_%i] channel_%i_coeff2, error %i (%s)", board, channel, ret, parsing_error_str (ret));
					goto error;
				}

				loginfo_debug ("channel_%i_coeff2\t= %lf", channel, doubleval);

				//-------------------------------------------------------------------------------------------------------------------

				config->board[board].coeff2[nChannels] = doubleval;

				/* Increase the number of channels in the sequence */
				nChannels++;
				config->nChannels++;

				/* Count the number of channel per board */
				config->board[board].nChannels++;
			}
		}
	}

error:

	if (ini_config)
		free_ini_config (ini_config);

	if (error_list)
		free_ini_config_errors (error_list);

	return ret;
}

//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
int hpc_read_power_config_file_fortran (const char * config_file, struct iniPowerConfig_t * config, struct components_t* components)
{
   config->components = components;
   return hpc_read_power_config_file (config_file, config);
}

int hpc_read_power_config_file (const char * config_file, struct iniPowerConfig_t * config)
{
	struct collection_item * ini_config = NULL;
	struct collection_item * error_list = NULL;
	struct collection_item * item = NULL;
	uint32_t ui32val = 0;
	int ret = 0;

	ret = config_from_file ("hpc_power_measure", config_file, &ini_config, 0, &error_list);
	if (ret)
	{
		loginfo_error ("config_from_file %s error %i (%s)",config_file, ret, parsing_error_str (ret));
		goto error;
	}

	/* Read the config file channel informations */
	{
		char section[255] = {'0'};
		char name[255] = {'0'};
		uint32_t nPowerComponents = 0;
	        uint32_t component = 0;
	        double doubleval;

		ret = get_config_item ("GLOBAL", "nPowerComponents", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] nPowerComponents, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		ui32val = get_uint32_config_value (item, 0, 0, &ret);
		if (ret)
		{
			loginfo_error ("get_uint32_config_value [GLOBAL] nPowerComponents, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}
		config->nPowerComponents = ui32val;
		nPowerComponents = config->nPowerComponents;
		loginfo_debug ("nPowerComponents\t= %i",config->nPowerComponents);
		ret = get_config_item ("GLOBAL", "mf_url", ini_config, &item);
		if (ret)
		{
			loginfo_error ("get_config_item [GLOBAL] mf_url, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		{
			const char *buff = get_const_string_config_value (item, &ret);
			strcpy (config->mf_url, buff);
		}

		if (ret)
		{
			loginfo_error ("get_const_string_config_value [GLOBAL] mf_url, error %i (%s)", ret, parsing_error_str (ret));
			goto error;
		}

		loginfo_debug ("mf_url\t=\"%s\"",config->mf_url);
        


		if(nPowerComponents	> 0 )
		{
			if(config->components == (struct components_t*)0)
			{
				config->components = (struct components_t*) malloc(sizeof(struct components_t)*nPowerComponents);
			}
			for (component=0; (component<nPowerComponents); component++)
		    {
			  sprintf (section, "COMPONENT_%i", component);

              /* New component */
              loginfo_debug ("[COMPONENT_%i]", component);
              sprintf (name, "id");

			  ret = get_config_item (section, name, ini_config, &item);
			  if (ret)
			  {
			  	loginfo_error ("get_config_item [COMPONENT_%i] id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
              ui32val = get_uint32_config_value (item, 0, 0, &ret);
			  if (ret)
			  {
				loginfo_error ("get_uint32_config_value [COMPONENT_%i] id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
			  config->components[component].id= ui32val;
			  loginfo_debug ("id\t= %i",config->components[component].id);
			  /* Read other values only if the channel is to be used */
			  if (ui32val<0)
				continue;

              sprintf (name, "measure_interval_micro_sec");

			  ret = get_config_item (section, name, ini_config, &item);
			  if (ret)
			  {
			  	loginfo_error ("get_config_item [COMPONENT_%i] measure_interval_micro_sec, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
              ui32val = get_uint32_config_value (item, 0, 0, &ret);
			  if (ret)
			  {
				loginfo_error ("get_uint32_config_value [COMPONENT_%i] measure_interval_micro_sec, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
			  config->components[component].measure_interval_micro_sec= ui32val;
			  loginfo_debug ("measure_interval_micro_sec\t= %i",config->components[component].measure_interval_micro_sec);



                sprintf (name, "label");

				ret = get_config_item (section, name, ini_config, &item);
				if (ret)
				{
					loginfo_error ("get_config_item [COMPONENT_%i] label, error %i (%s)", component ,ret, parsing_error_str (ret));
					goto error;
				}
				{
					const char *buff = get_const_string_config_value (item, &ret);
					strcpy (config->components[component].label, buff);
				}

				if (ret)
				{
					loginfo_error ("get_config_item [COMPONENT_%i] label, error %i (%s)", component ,ret, parsing_error_str (ret));
					goto error;
				}

				loginfo_debug ("label\t= \"%s\"", config->components[component].label);
 
 			  sprintf (name, "current_channel_id");

			  ret = get_config_item (section, name, ini_config, &item);
			  if (ret)
			  {
			  	loginfo_error ("get_config_item [COMPONENT_%i] current_channel_id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
              ui32val = get_uint32_config_value (item, 0, 0, &ret);
			  if (ret)
			  {
				loginfo_error ("get_uint32_config_value [COMPONENT_%i] current_channel_id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
			  config->components[component].current_channel_id= ui32val;
			  loginfo_debug ("current_channel_id\t= %i",config->components[component].current_channel_id);

			  
 			  sprintf (name, "current_board_id");

			  ret = get_config_item (section, name, ini_config, &item);
			  if (ret)
			  {
			  	loginfo_error ("get_config_item [COMPONENT_%i] current_board_id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
              ui32val = get_uint32_config_value (item, 0, 0, &ret);
			  if (ret)
			  {
				loginfo_error ("get_uint32_config_value [COMPONENT_%i] current_board_id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
			  config->components[component].current_board_id= ui32val;
			  loginfo_debug ("current_board_id\t= %i",config->components[component].current_board_id);

			  sprintf (name, "voltage_channel_id");

			  ret = get_config_item (section, name, ini_config, &item);
			  if (ret)
			  {
			  	loginfo_error ("get_config_item [COMPONENT_%i] voltage_channel_id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
              ui32val = get_uint32_config_value (item, 0, 0, &ret);
			  if (ret)
			  {
				loginfo_error ("get_uint32_config_value [COMPONENT_%i] voltage_channel_id, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			  }
			  config->components[component].voltage_channel_id= ui32val;
			  loginfo_debug ("voltage_channel_id\t= %i",config->components[component].voltage_channel_id);
   
             
             sprintf (name, "voltage_board_id");

			 ret = get_config_item (section, name, ini_config, &item);
			 if (ret)
			 {
			 	loginfo_error ("get_config_item [COMPONENT_%i] voltage_board_id, error %i (%s)", component, ret, parsing_error_str (ret));
			    goto error;
			 }
             ui32val = get_uint32_config_value (item, 0, 0, &ret);
			 if (ret)
			 {
			   loginfo_error ("get_uint32_config_value [COMPONENT_%i] voltage_board_id, error %i (%s)", component, ret, parsing_error_str (ret));
			   goto error;
			 }
			 config->components[component].voltage_board_id= ui32val;
			 loginfo_debug ("voltage_board_id\t= %i",config->components[component].voltage_board_id);
			 
			 sprintf (name, "coeff1");

			 ret = get_config_item (section, name, ini_config, &item);
			 if (ret)
			 {
			 	loginfo_error ("get_config_item [COMPONENT_%i] coeff1, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			 }

			 doubleval = get_double_config_value (item, 0, 0, &ret);
			 if (ret)
			 {
			 	loginfo_error ("get_double_config_value [COMPONENT_%i] coeff1, error %i (%s)", component, ret, parsing_error_str (ret));
			 	goto error;
			 }
             config->components[component].coeff1 = doubleval;
             loginfo_debug ("coeff1\t= %lf", doubleval);
             
             sprintf (name, "coeff2");

			 ret = get_config_item (section, name, ini_config, &item);
			 if (ret)
			 {
			 	loginfo_error ("get_config_item [COMPONENT_%i] coeff2, error %i (%s)", component, ret, parsing_error_str (ret));
				goto error;
			 }

			 doubleval = get_double_config_value (item, 0, 0, &ret);
			 if (ret)
			 {
			 	loginfo_error ("get_double_config_value [COMPONENT_%i] coeff2, error %i (%s)", component, ret, parsing_error_str (ret));
			 	goto error;
			 }
             config->components[component].coeff2 = doubleval;
             loginfo_debug ("coeff2\t= %lf", doubleval);

			}
		   }
         }

	error:

	if (ini_config)
		free_ini_config (ini_config);

	if (error_list)
		free_ini_config_errors (error_list);

	return ret;
}
