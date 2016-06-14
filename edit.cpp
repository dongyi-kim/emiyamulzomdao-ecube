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

namespace edit {//Edit standard plant's value.
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
            if( s->mode != EDIT_MODE ) {//if not edit mode, initialize
                if(init_flag == true) {//do not reapeat initialize.
                    init_flag = false;
                    init(s);
                }
            }
            else {
                fd = open(DRIVER_KEY, O_RDWR);

                cout<<"Editing"<<endl;
                int mode = -1, rdata, arr[4], origin;
                int* segValue = &s->segValue;

                while(1)
                {
                    if( s->mode != EDIT_MODE ) {
                        break;//exit edit mode.
                    }
                    pthread_mutex_lock(thread_manager::get_key());//waiting print picture complete in oled
                    read(fd,&rdata,4);
                    pthread_mutex_unlock(thread_manager::get_key());
                    if(rdata == 0)
                    {
                        continue;
                    }
                    else if(rdata%4 == 1) { //select menu
                        dip_buzzer(buz[0], buz_len[0]);//sound effect for keypad

                        mode = rdata;

                        arr[0] = s->data.illumination;//arr[0] has standard illumination value.
                        arr[1] = s->data.temperature;//arr[1] has standard temperature value.
                        arr[2] = s->data.humidity;//arr[2] has standard humidity value.
                        arr[3] = s->data.soil_humidity;//arr[3] has standard soil humidity value.

                        (*segValue) = arr[rdata/4];//7 segment value by selecting menu
                        origin = arr[rdata/4];

                        s->state.state[0] = (mode/4)+1;
                        s->state.len = 1;
                    }
                    else if(rdata <= 12 || rdata == 15) {//push number keypad(1-2, 2-3, 3-4),(4-6, 5-7, 6-8), (7-10, 8-11, 9-12), 0-15
                        if( mode != -1 ) {
                            dip_buzzer(buz[1], buz_len[1]);
                            int digit = keyMap[rdata];

                            if( (*segValue) < 100000 ) {
                                (*segValue) *= 10;
                                (*segValue) += digit;

                                cout<<(*segValue)<<endl;
                            }
                        }
                    }
                    else if(rdata == KEYPAD_SAVE) {//push save keypad 16
                        if( mode != -1 ) {
                            dip_buzzer(buz[2], buz_len[2]);

                            arr[mode/4] = (*segValue);

                            s->data.illumination = arr[0];
                            s->data.temperature = arr[1];
                            s->data.humidity = arr[2];
                            s->data.soil_humidity = arr[3];

                            int http_code = sendConfigToServer(s->data, s->id);

                            if(http_code != 200) {
                                cout<<"Error!!!"<<endl;
                                break;
                            }

                            (*segValue) = -1;
                            s->state.len = 0;
                            mode = -1;
                        }
                    }

                    else if(rdata == KEYPAD_DELETE) {//push delete keypad 14
                        dip_buzzer(buz[3], buz_len[3]);
                        if( mode != -1 ) {
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