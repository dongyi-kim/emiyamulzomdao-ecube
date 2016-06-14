/*
        @file   -       auth.cpp
        @author -       
        @brief  -       authorize plant number from the server.
        @reference -    /root/periApp/keytest/keytest.c(system programming practice 8th device driver.pptx)
*/
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
/*
L 1 2 3
T 4 5 6
H 7 8 9
S D 0 S
*/

const int buz[10][10] = {
        {1, 5, 9},///< do mi sol
        {12},///< do mi sol
        {9, 5, 1},///< sol mi do
        {2}///< re
};

const int buz_len[10] = {
        3, 1, 3, 1
};

#define DRIVER_KEY      "/dev/cnkey"///< keypad file driver path

#define KEYPAD_SAVE 16
#define KEYPAD_DELETE 14

#define KEYPAD_I 1 // illumination
#define KEYPAD_T 5 // temperature
#define KEYPAD_H 9 // humidity
#define KEYPAD_S 13 // soil_humidity


const string HOST = "http://immense-garden-82747.herokuapp.com";

namespace auth {
    void input_account(Shared* s) {

        ostringstream ss;

        bool init_flag = true;
        int rdata;
        int cnt = 0;
        int fd = open(DRIVER_KEY, O_RDWR);///< open keypad file driver.
        int* segValue = &(s->segValue);///< segment value

        (*segValue) = 0;
        //input plant number
        while(1)
        {
            pthread_mutex_lock(thread_manager::get_key());///< waiting for pringting picture of oled.
            read(fd,&rdata,4);
            pthread_mutex_unlock(thread_manager::get_key());

            //ignore input of not number
            if(rdata == 0 || rdata%4 == 1)
            {
                continue;
            }
            //when push number keypad,
            else if(rdata <= 12 || rdata == 15) {
                dip_buzzer(buz[1], buz_len[1]);///< number sound effect
                int digit = keyMap[rdata];

                if( (*segValue) < 100000 ) {///< show plant number by 7-segment
                    (*segValue) *= 10;
                    (*segValue) += digit;
                }
            }
            //when push save keypad
            else if(rdata == KEYPAD_SAVE) {
                dip_buzzer(buz[2], buz_len[2]);///< save sound effect.

                //authorize data
                ss << (*segValue);
                s->id = ss.str();
                //initialize
                (*segValue) = -1;
                s->state.len = 0;
                break;
            }
            //when push delete keypad
            else if(rdata == KEYPAD_DELETE) {
                dip_buzzer(buz[3], buz_len[3]);///< delete sound effect.
                (*segValue) /= 10;///< delete input
            }
            usleep(200000);
        
        }
        close(fd);
    }

    /**
        Get user's plant config info from server
        If it doesn't return 200, then it causes some error
    */
};
