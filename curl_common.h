#ifndef SYSPROG_CURL_COMMON_H
#define SYSPROG_CURL_COMMON_H

extern size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
int sendConfigToServer(Data data);

#endif //SYSPROG_CURL_COMMON_H
