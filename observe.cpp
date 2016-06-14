/*
    @file   -   observe.cpp
    @author -   
    @brief  -   observing mode.
    @reference - 
*/
#include <iostream>
#include "common.h"

using namespace std;

//make observe mode namespace
namespace observe {
    void init() {
        cout<<"observe::Init!!"<<endl;
    }

    void* observe(void* shared) {
        Shared *s = (Shared*)shared;

        bool init_flag = true;

        int cnt = 0;
        while(1) {
            //if not obseve mode, initialize.
            if( s->mode != OBSERVE_MODE ) {
                if(init_flag == true) {
                    init_flag = false;
                    init();
                }
            }
            //if observe mode
            else {
                cout<<"Observing"<<endl;
                while(1) {
                    if( s->mode != OBSERVE_MODE ) {
                        break;
                    }

                    /* read data from arduino
                     * send data to server
                     * send pump event to arduino
                     */
                }

                init_flag = true;
            }
            usleep(100000);
        }
    }
}
