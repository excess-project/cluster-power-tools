 
#ifndef HPC_MF_H_
#define HPC_MF_H_
#include "hpc_config.h"
#include <curl/curl.h>
#define MAX_HEADER_MESSAGE_LENGTH 512
#define MAX_COMPONENT_MESSAGE_LENGTH 512
#define MAX_MESSAGE_LENGTH 4096
struct mf_messages_t
{
    char*** mf_messages;
    uint32_t num_components;
    uint32_t* num_intervals;
};

int hpc_mf_calculate_power(float** fData, int nChannels, int* fData_channel_idx,uint32_t filter_step_left,uint32_t filter_step_right, struct fileHeader_t* header, struct iniPowerConfig_t* power_config, struct mf_messages_t* messages );
int hpc_mf_free_messages(struct mf_messages_t* messages);
int hpc_mf_print_messages(struct mf_messages_t* messages);
int hpc_mf_send_messages(struct curl_slist **headers_, CURL **curl_,char* mf_url, struct mf_messages_t* messages);
int hpc_mf_send_messages_all_in_one(struct curl_slist **headers_, CURL **curl_,char* mf_url, struct mf_messages_t* messages);
void hpc_mf_stop_messages(struct curl_slist **headers_, CURL **curl_);
int hpc_mf_calculate_power_ac( float** fData, int nChannels,struct fileHeader_t* header, struct iniPowerConfig_t* power_config, struct mf_messages_t* messages);
#endif /* HPC_MF_H_ */
