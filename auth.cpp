#include <iostream>
#include <curl/curl.h>
#include "common.h"
#include "curl_common.h"

using namespace std;

const string HOST = "http://immense-garden-82747.herokuapp.com";

namespace auth {
    void input_account(string &id) {
        /* input id by touch screen
         */
        id = "234";
        cout<<"login by "+id<<endl;
    };

    /**
        Get user's plant config info from server
        If it doesn't return 200, then it causes some error
    */
    int authorize(string id, string* result) {
//        CURL *curl;
//        CURLcode res;
//        int http_code;
//
//        char buffer[100];
//
//        curl_global_init(CURL_GLOBAL_ALL);
//
//        curl = curl_easy_init();
//
//        if(curl) {
//
//            const string USER_GET = HOST+"/api/users/"+id;
//
//            curl_easy_setopt(curl, CURLOPT_URL, USER_GET.c_str());
//            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
//            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)result);
//            res = curl_easy_perform(curl);
//
//            if( res != CURLE_OK ) {
//                return -1;
//            }
//
//            curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
//            curl_easy_cleanup(curl);
//        }
//
//        curl_global_cleanup();

        (*result) = "10 10 10 10";
        int http_code = 200;
        return http_code;
    }
}