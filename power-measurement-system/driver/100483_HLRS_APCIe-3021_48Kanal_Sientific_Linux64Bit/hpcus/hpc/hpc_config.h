/** @file hpc_config.h
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

#ifndef HPC_CONFIG_H_
#define HPC_CONFIG_H_
#define parsing_error_str(a) char("-") 
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------

/**@struct board_t
 * Board configuration informations.
 */
struct board_t
{
	uint32_t sequence[8];
	uint32_t gain[8];
	uint32_t polarity[8];
	uint32_t nChannels;
	uint32_t id[8];
	uint32_t is_on[8];
	float coeff1[8];
	float coeff2[8];
        char label[8][64];
};

//---------------------------------------------------------------------------------------

/**@struct iniConfig_t
 * - Global configuration informations.
 * - Board configuration informations.
 */
struct iniConfig_t
{
	uint32_t nChannels;
	uint32_t acquisition_time;
	uint32_t number_of_sequences_per_interrupt;
	uint32_t total_sequence_count;
	uint32_t duration_time;
	uint32_t trigger_on_off;
	char log_file_path[256];
	char raw_file_path[256];
	uint32_t ac_voltage_channel_id;
	uint32_t ac_num_periods;
	struct board_t board[4];
};



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
int hpc_read_config_file (const char * config_file, struct iniConfig_t * config, int nBoards);

//---------------------------------------------------------------------------------------

/**@struct board_t
 * power channel configuration informations.
 */
struct components_t
{
    uint32_t id;
    uint32_t measure_interval_micro_sec;
    uint32_t current_channel_id;
    uint32_t current_board_id;
    uint32_t voltage_channel_id;
    uint32_t voltage_board_id;
    float coeff1;
    float coeff2;
    float shunt_ohm;
    char label[64];
};
//---------------------------------------------------------------------------------------

/**@struct iniPowerConfig_t
 * - Global informations about the measured cluster components.
 * - Cluster components informations.
 */
struct iniPowerConfig_t
{
    uint32_t nPowerComponents;
    char mf_url[1024];
    struct components_t* components;
};
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
/** Read configuration informations about measured cluster components from the config file.
 *
 * @param [in] power_config_file: Path to the configuration file.
 * @param [in,out] iniPowerConfig_t: Configuration structure.
 * @retval 0 No error.
 * @retval != 0 Error.
 */
int hpc_read_power_config_file (const char * config_file, struct iniPowerConfig_t * config);
//---------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------
#endif /* HPC_CONFIG_H_ */
