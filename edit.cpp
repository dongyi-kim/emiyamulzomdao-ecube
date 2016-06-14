/*
        @file   -       edit.cpp
        @author -       
        @brief  -       edit mode
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
        {12},///< si
        {9, 5, 1},///< sol mi do
        {2}///< re
};

const int buz_len[10] = {
        3, 1, 3, 1
};

#define DRIVER_KEY      "/dev/cnkey"///< keypad file driver path.

#define KEYPAD_SAVE 16
#define KEYPAD_DELETE 14

#define KEYPAD_I 1 // illumination
#define KEYPAD_T 5 // temperature
#define KEYPAD_H 9 // humidity
#define KEYPAD_S 13 // soil_humidity

namespace edit {
    void init(Shared *s) {
        //s->segValue = -1;
        s->state.len = 0;
        cout<<"edit::Init!!"<<endl;
    }

    void* edit(void* shared) {
        Shared *s = (Shared*)shared;

        bool init_flag = true;

        int cnt = 0;
        int fd;

        while(1) {
            if( s->mode != EDIT_MODE ) {
                if(init_flag == true) {
                    init_flag = false;
                    init(s);
                }
            }
            else {
                fd = open(DRIVER_KEY, O_RDWR); ///< keypad file driver open.

                cout<<"Editing"<<endl;
                int mode = -1, rdata;
                int arr[4], origin;///< arr 0-illumination, 1-temperature, 2-humidity, 3-soil humidity
                int* segValue = &s->segValue;///< 7-segment value

                while(1)///< start edit mode
                {
                    //when mode changed, break edit mode.
                    if( s->mode != EDIT_MODE ) {
                        break;
                    }
                    pthread_mutex_lock(thread_manager::get_key());//waiting for printing picture of oled
                    read(fd,&rdata,4);
                    pthread_mutex_unlock(thread_manager::get_key());
                    if(rdata == 0)
                    {
                        continue;
                    }
                    //when push menu keypad(1, 5, 9, 13), start configure
                    else if(rdata%4 == 1) { 
                        dip_buzzer(buz[0], buz_len[0]);///< menu sound effect.

                        mode = rdata;

                        arr[0] = s->data.illumination;
                        arr[1] = s->data.temperature;
                        arr[2] = s->data.humidity;
                        arr[3] = s->data.soil_humidity;

                        (*segValue) = arr[rdata/4];///< load standard data and show 7-segment
                        origin = arr[rdata/4];

                        s->state.state[0] = (mode/4)+1;///< show dot matrix led what selected menu
                        s->state.len = 1;
                    }
                    //when push number keypad(2, 3, 4, 6, 7, 8, 10, 11, 12, 15)
                    else if(rdata <= 12 || rdata == 15) {
                        //mode == -1 is not menu configure, so menu == -1, ignore all keypad input
                        if( mode != -1 ) {
                            dip_buzzer(buz[1], buz_len[1]);///< number sound effect
                            int digit = keyMap[rdata];

                            if( (*segValue) < 100000 ) {///< show increase value by 7-segment
                                (*segValue) *= 10;
                                (*segValue) += digit;

                            }
                        }
                    }
                    //when push save keypad(16)
                    else if(rdata == KEYPAD_SAVE) {
                        //mode == -1 is not menu configure, so menu == -1, ignore all keypad input
                        if( mode != -1 ) {
                            dip_buzzer(buz[2], buz_len[2]);///< save sound effect.

                            arr[mode/4] = (*segValue);

                            //save value in standard data.
                            s->data.illumination = arr[0];
                            s->data.temperature = arr[1];
                            s->data.humidity = arr[2];
                            s->data.soil_humidity = arr[3];
                            
                            int http_code = sendConfigToServer(s->data, s->id);///< send saved data to server.

                            if(http_code != 200) {
                                cout<<"Error!!!"<<endl;
                                break;
                            }

                            (*segValue) = -1;//initialize 7-segment value
                            s->state.len = 0;
                            mode = -1;///< end menu configure.
                        }
                    }
                    //when push delete keypad(14)
                    else if(rdata == KEYPAD_DELETE) {
                        dip_buzzer(buz[3], buz_len[3]);///< delete sound effect.
                        //mode == -1 is not menu configure, so menu == -1, ignore all keypad input
                        if( mode != -1 ) {///< delete in 7-segment.
                            (*segValue) /= 10;
                        }
                    }
                    usleep(200000);
                }
                close(fd);
                init_flag = true;
            }
            usleep(100000);
        }
    }
}
