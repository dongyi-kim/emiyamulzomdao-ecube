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

#define DRIVER_KEY		"/dev/cnkey"

#define KEYPAD_SAVE 16
#define KEYPAD_DELETE 14

#define KEYPAD_I 1 // illumination
#define KEYPAD_T 5 // temperature
#define KEYPAD_H 9 // humidity
#define KEYPAD_S 13 // soil_humidity

namespace edit {
    void init(Shared *s) {
        s->segValue = -1;
        s->state.len = 0;
        cout<<"edit::Init!!"<<endl;
    }

    void* edit(void* shared) {
        Shared *s = (Shared*)shared;

        bool init_flag = true;

        int cnt = 0;
        int fd = open(DRIVER_KEY, O_RDWR);

        while(1) {
            if( s->mode != EDIT_MODE ) {
                if(init_flag == true) {
                    init_flag = false;
                    init(s);
                }
            }
            else {
                cout<<"Editing"<<endl;
                int mode = -1, rdata, arr[4], origin;
                int* segValue = &s->segValue;

                while(1)
                {
                    if( s->mode != EDIT_MODE ) {
                        break;
                    }
                    pthread_mutex_lock(thread_manager::get_a());
                    read(fd,&rdata,4);
                    pthread_mutex_unlock(thread_manager::get_a());
                    if(rdata == 0)
                    {
                        continue;
                    }
                    else if(rdata%4 == 1) { //start config
                        dip_buzzer(buz[0], buz_len[0]);

                        mode = rdata;

                        arr[0] = s->data.illumination;
                        arr[1] = s->data.temperature;
                        arr[2] = s->data.humidity;
                        arr[3] = s->data.soil_humidity;

                        (*segValue) = arr[rdata/4];
                        origin = arr[rdata/4];

                        s->state.state[0] = (mode/4)+1;
                        s->state.len = 1;
                    }
                    else if(rdata <= 12 || rdata == 15) {
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
                    else if(rdata == KEYPAD_SAVE) {
                        if( mode != -1 ) {
                            dip_buzzer(buz[2], buz_len[2]);

                            arr[mode/4] = (*segValue);

                            s->data.illumination = arr[0];
                            s->data.temperature = arr[1];
                            s->data.humidity = arr[2];
                            s->data.soil_humidity = arr[3];

                            int http_code = sendConfigToServer(s->data);

                            if(http_code != 200) {
                                cout<<"Error!!!"<<endl;
                                break;
                            }

                            (*segValue) = -1;
                            s->state.len = 0;
                            mode = -1;
                        }
                    }

                    else if(rdata == KEYPAD_DELETE) {
                        dip_buzzer(buz[3], buz_len[3]);
                        if( mode != -1 ) {
                            (*segValue) /= 10;
                        }
                    }
                    usleep(200000);
                }
                init_flag = true;
            }
            usleep(100000);
        }
    }
}