#include <iostream>
#include "common.h"

using namespace std;

namespace edit {
    void init() {
        cout<<"edit::Init!!"<<endl;
    }

    void* edit(void* shared) {
        Shared *s = (Shared*)shared;

        bool init_flag = true;

        int cnt = 0;
        while(1) {
            if( s->mode != EDIT_MODE ) {
                if(init_flag == true) {
                    init_flag = false;
                    init();
                }
                continue;
            }
            else {
                cout<<"Editing"<<endl;
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