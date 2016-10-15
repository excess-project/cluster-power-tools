#include <stdlib.h>
#include <unistd.h>

#include <signal.h>
#include <curl/curl.h>
#include <string.h>

void  hpc_curl_init(struct curl_slist **headers_, CURL **curl_) {
    
	if (*curl_ != NULL ) {
		return;
	}

	curl_global_init(CURL_GLOBAL_ALL);
	*curl_ = curl_easy_init();

    if (*headers_ != NULL ) {
		return;
	}
	*headers_ = curl_slist_append(*headers_, "Accept: application/json");
	*headers_ = curl_slist_append(*headers_, "Content-Type: application/json");
	*headers_ = curl_slist_append(*headers_, "charsets: utf-8");
    
}

void  hpc_curl_finish(struct curl_slist **headers_, CURL **curl_) {
    
	if (*curl_ == NULL ) {
		return;
	}
    curl_easy_cleanup(*curl_);
    curl_slist_free_all(*headers_);
    *curl_ = NULL;
    *headers_ = NULL;
}


int hpc_curl_send_monitoring_data(struct curl_slist **headers_, CURL **curl_, char *URL,char *data) {
	CURLcode res;
	int result = 0;
    CURLcode  ret_code;

	/* perform some error checking */
	if (URL == NULL || strlen(URL) == 0) {
		fprintf(stderr,
				"send_monitoring_data(): Error - the given url is empty.\n");
		return 1;
	}

	if (*curl_ == NULL ) {
		hpc_curl_init(headers_,curl_);
	}

	//printf("curl -X POST %s -- len: %d\n", URL, (int) strlen(URL));
	//printf("Msg = %s -- len: %d\n", data, (int) strlen(data));

    ret_code = curl_easy_setopt(*curl_, CURLOPT_VERBOSE, 		0);
    if(ret_code != CURLE_OK)
        fprintf(stderr,"curl_easy_setopt(curl_, CURLOPT_VERBOSE %s error: %d\n", URL, (int) ret_code);
	ret_code = curl_easy_setopt(*curl_, CURLOPT_URL, URL);
    if(ret_code != CURLE_OK)
        fprintf(stderr,"curl_easy_setopt(curl_, CURLOPT_UR %s error: %d\n", URL, (int) ret_code);
	ret_code = curl_easy_setopt(*curl_, CURLOPT_HTTPHEADER, *headers_);
    if(ret_code != CURLE_OK)
       fprintf(stderr,"curl_easy_setopt(curl_, CURLOPT_HTTPHEADER %s error: %d\n", URL, (int) ret_code);
	ret_code = curl_easy_setopt(*curl_, CURLOPT_POSTFIELDS, data);
    if(ret_code != CURLE_OK)
        fprintf(stderr,"curl_easy_setopt(curl_, CURLOPT_POSTFIELDS %s error: %d\n", URL, (int) ret_code);
	ret_code = curl_easy_setopt(*curl_, CURLOPT_POSTFIELDSIZE, (long ) strlen(data));
    if(ret_code != CURLE_OK)
        fprintf(stderr,"curl_easy_setopt(curl_, CURLOPT_POSTFIELDSIZE %s error: %d\n", URL, (int) ret_code);

	res = curl_easy_perform(*curl_);

	/* Check for errors */
	if (res != CURLE_OK) {
		result = -1;
		fprintf(stderr, "send_monitoring_data() failed: %s\n",curl_easy_strerror(res));

	}

	curl_easy_reset(*curl_);
    
	return result;
}
