#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <curl/curl.h>

#ifndef HPC_CURL_H_
#define HPC_CURL_H_

void  hpc_curl_init(struct curl_slist **headers_, CURL **curl_);
int hpc_curl_send_monitoring_data(struct curl_slist **headers_, CURL **curl_,char *URL,char *data);
void  hpc_curl_finish(struct curl_slist **headers_, CURL **curl_);

#endif
