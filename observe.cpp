#include <iostream>
#include "common.h"

using namespace std;

namespace observe {
    void init() {
        cout<<"observe::Init!!"<<endl;
    }

    void* observe(void* shared) {
        Shared *s = (Shared*)shared;

        bool init_flag = true;

        int cnt = 0;
        while(1) {
            if( s->mode != OBSERVE_MODE ) {
                if(init_flag == true) {
                    init_flag = false;
                    init();
                }
                continue;
            }
            else {
                cout<<"Observing"<<endl;
                cnt++;

                if(cnt == 10) {
                    s->mode = -1;
                    cnt = 0;
                }


                init_flag = true;
            }
            usleep(100000);
        }
    }
}