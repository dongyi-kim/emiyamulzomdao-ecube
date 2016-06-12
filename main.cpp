#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <curl/curl.h>
#include <pthread.h>
#include "common.h"
#include "fnd.h"
#include "mled.h"
#include "dipsw.h"
#include "bled.h"
#include "oled.h"
#include "cled.h"
#include "tlcd.h"
#include "receiveSensor.h"
#include "gui/touch.h"

using namespace std;


int main() {
    /*
     * outer while: login -> sensor, edit -> logout -> ...
     */
    Shared shared;

    pthread_t observe_thread;
    pthread_t edit_thread;
    pthread_t fnd_thread;
    pthread_t mled_thread;
    pthread_t bled_thread;
    pthread_t oled_thread;
    pthread_t cled_thread;
    pthread_t tlcd_thread;
    pthread_t receive_thread;

    pthread_create(&observe_thread, NULL, observe::observe, (void*)&shared);
    pthread_create(&edit_thread, NULL, edit::edit, (void*)&shared);
    pthread_create(&fnd_thread, NULL, fnd, (void*)&shared);
    pthread_create(&mled_thread, NULL, mled, (void*)&shared);
    pthread_create(&bled_thread, NULL, bled, (void*)&shared);
    pthread_create(&oled_thread, NULL, oled, (void*)&shared);
    pthread_create(&cled_thread, NULL, cled, (void*)&shared);
    pthread_create(&tlcd_thread, NULL, tlcd, (void*)&shared);
    pthread_create(&receive_thread, NULL, receive, (void*)&shared);

    while(1) {
        string id;
        /*
         * inner while: until login success
         */
        Data data;

        while(1) {
            string ret;
            auth::input_account(id);

            shared.id = id;

            int http_status = auth::authorize(id, &ret);

            if( http_status == 200 ) { //when if login is complete
                int illum, temp, humid, s_humid;
                sscanf(ret.c_str(), "%d %d %d %d", &illum, &temp, &humid, &s_humid);
                data = Data(illum, temp, humid, s_humid);
                
                break;
            }
            else {
                cout<<http_status<<endl;
                cout<<"Error!!!"<<endl;
            }
        }

        shared.data = data;

        shared.mode = EDIT_MODE;

        while(1) {
            /**
             * Temp config for test
             */
            int dip = dipsw();

            int isEdit = dip&(1<<7);
            int logout = dip&(1<<6);

            int cledValue = dip&(1<<5);

            shared.liq_exist = cledValue;

            if(logout) {
                shared.mode = NOT_AUTHORIZED;
                break;
            }

            if(isEdit) {
                shared.mode = EDIT_MODE;
            }
            else {
                shared.mode = OBSERVE_MODE;
            }

            usleep(1000000);
        }
    }

	return 0;
}