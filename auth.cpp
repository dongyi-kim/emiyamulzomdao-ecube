#include <iostream>
#include <pthread.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sstream>
#include "common.h"
#include "buzzer.h"
#include "curl_common.h"
#include "thread_manager.h"

using namespace std;

const int keyMap[17] = {-1,-1,1,2,3,-1,4,5,6,-1,7,8,9,-1,-1,0,-1};

const int buz[10][10] = {
        {1, 5, 9},
        {12},
        {9, 5, 1},
        {2}
};

const int buz_len[10] = {
        3, 1, 3, 1
};

#define DRIVER_KEY      "/dev/cnkey"

#define KEYPAD_SAVE 16
#define KEYPAD_DELETE 14

#define KEYPAD_I 1 // illumination
#define KEYPAD_T 5 // temperature
#define KEYPAD_H 9 // humidity
#define KEYPAD_S 13 // soil_humidity


const string HOST = "http://immense-garden-82747.herokuapp.com";

namespace auth {
    void input_account(Shared* s) {
        /* input id by touch screen
         */

        ostringstream ss;

        bool init_flag = true;
        int rdata;
        int cnt = 0;
        int fd = open(DRIVER_KEY, O_RDWR);
        int* segValue = &(s->segValue);

        (*segValue) = 0;

        while(1)
        {
            pthread_mutex_lock(thread_manager::get_key());
            read(fd,&rdata,4);
            pthread_mutex_unlock(thread_manager::get_key());

            if(rdata == 0)
            {
                continue;
            }
            else if(rdata%4 == 1) { //start config
                
            }
            else if(rdata <= 12 || rdata == 15) {
                dip_buzzer(buz[1], buz_len[1]);
                int digit = keyMap[rdata];

                if( (*segValue) < 100000 ) {
                    (*segValue) *= 10;
                    (*segValue) += digit;
                }
            }
            else if(rdata == KEYPAD_SAVE) {
                dip_buzzer(buz[2], buz_len[2]);

                ss << (*segValue);
                s->id = ss.str();
                (*segValue) = -1;
                s->state.len = 0;
                break;
            }

            else if(rdata == KEYPAD_DELETE) {
                dip_buzzer(buz[3], buz_len[3]);
                (*segValue) /= 10;
            }
            usleep(200000);
        
        }
    }

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
};