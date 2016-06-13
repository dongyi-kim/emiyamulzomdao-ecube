#ifndef SYSPROG_CURL_COMMON_H
#define SYSPROG_CURL_COMMON_H

extern size_t writeCallback(char* buf, size_t size, size_t nmemb, void* up);
extern int sendConfigToServer(Data data, string id);
extern int sendDataToServer(Data data, string id);
extern int sendFileToServer(string path, string id);
#endif //SYSPROG_CURL_COMMON_H
