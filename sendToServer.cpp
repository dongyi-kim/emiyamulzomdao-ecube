#include <cstdio>
#include <curl/curl.h>
#include "common.h"

#define POST_HOST "http://immense-garden-82747.herokuapp.com/api/data/"

int send_to_server(Data d, string id) {
    char query[200];
    CURL *curl;
    CURLcode res;
    int http_code;

    /* In windows, this will init the winsock stuff */
    curl_global_init(CURL_GLOBAL_ALL);

    /* get a curl handle */
    curl = curl_easy_init();
    if(curl) {
        /* First set the URL that is about to receive our POST. This URL can
           just as well be a https:// URL if that is what should receive the
           data. */
           curl_easy_setopt(curl, CURLOPT_URL, POST_HOST+id);
        /* Now specify the POST data */
           sprintf(query, "illumination=%d&humidity=%d&temperature=%d&soil_humidity=%d", d.illumination, d.humidity, d.temperature, d.soil_humidity);
           curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);

         /* Perform the request, res will get the return code */
           curl_easy_perform(curl);

           curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);

        /* Check for errors */

           curl_easy_cleanup(curl);
       }
       curl_global_cleanup();

       return http_code;
   }