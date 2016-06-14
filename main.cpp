/*
    @file   -   main.cpp
    @author -   
    @brief  -   make thread, and authorize.
    @reference - 
*/
#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <curl/curl.h>
#include <pthread.h>

#include "gui/gui.h"
#include "gui/touch.h"
#include "gui/display.h"

#include "common.h"
#include "fnd.h"
#include "mled.h"
#include "dipsw.h"
#include "bled.h"
#include "buzzer.h"

#include "oled.h"
#include "cled.h"
#include "tlcd.h"
#include "receiveSensor.h"
#include "gui/touch.h"
#include "curl_common.h"

using namespace std;

Shared shared;///< core variable, all thread share this variable.

const int arr[] = {1};

//touch screen in cog.
void click_config(touch::touch_event e)
{

    // filter touch event with bit mask
    if((e.event_code & touch::EVENT_TOUCH_DOWN) > 0 )
        if( 0 < e.y && e.y < 100 && 600 < e.x && e.x < 800 ) {
            dip_buzzer(arr, 1);///< when touch this screen sound effect.
            cout<<"CONFIG"<<endl;
            shared.mode = EDIT_MODE;///< move edit mode.
        }
}

//touch screen in flower.
void click_edit(touch::touch_event e)
{
    // filter touch event with bit mask
    if((e.event_code & touch::EVENT_TOUCH_DOWN) > 0 )
        if( 0 < e.y && e.y < 100 && 0 < e.x && e.x < 200 ) {
            dip_buzzer(arr, 1);///< when touch this screen sound effect
            cout<<"EDIT"<<endl;
            shared.mode = OBSERVE_MODE;///< move observe mode.
        }
}

int main() {
    /*
     * outer while: login -> sensor, edit -> logout -> ...
     */
    

    pthread_t observe_thread;
    pthread_t edit_thread;
    pthread_t fnd_thread;
    pthread_t mled_thread;
    pthread_t bled_thread;
    pthread_t oled_thread;
    pthread_t cled_thread;
    pthread_t tlcd_thread;
    pthread_t receive_thread;

    pthread_create(&observe_thread, NULL, observe::observe, (void*)&shared);///< make observe mode thread.
    pthread_create(&edit_thread, NULL, edit::edit, (void*)&shared);///< make edit mode thread.
    pthread_create(&fnd_thread, NULL, fnd, (void*)&shared);///< make 7-segment control thread.
    pthread_create(&mled_thread, NULL, mled, (void*)&shared);///< make dot matrix control thread.
    pthread_create(&bled_thread, NULL, bled, (void*)&shared);///< make bus led control thread.
    pthread_create(&oled_thread, NULL, oled, (void*)&shared);///< make OLED control trhead.
    pthread_create(&cled_thread, NULL, cled, (void*)&shared);///< make full color led control thread.
    pthread_create(&tlcd_thread, NULL, tlcd, (void*)&shared);///< make text LCD control thread.
    pthread_create(&receive_thread, NULL, receive, (void*)&shared);///< make receive value from sensor thread.

    while(1) {
        string id;///< plant number.
        /*
         * inner while: until login success
         */
        Data data;///< standard data to save in server.

        while(1) {
            string ret;

            auth::input_account(&shared);///< plant number save in shared variable.

            int http_status = authorize(shared.id, &ret);///< authorize from server.

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

        shared.mode = OBSERVE_MODE;

        //when authorized 
        touch::init();

        display::init();

        gui::setup();

        touch::add_callback(&click_config);
        touch::add_callback(&click_edit);

        bool sns_flag = false;
        while(1) {
            /**
             * Temp config for test
             */
            int cnt = shared.getCompare();
            if( cnt == 0 ) {
                shared.sns_exist = 1;
                if( sns_flag ) {
                    sendEventToServer("I'm angry!!", shared.id);
                    sns_flag = false;
                }
            }

            else {
                shared.sns_exist = 0;
                sns_flag = true;
            }

            if(shared.liq_exist)
            {
                shared.cledValue[0] = 1;
            }
            else {
                shared.cledValue[0] = 0;
            }
            if(shared.sns_exist)
            {
                shared.cledValue[1] = 1;
            }
            else {
                shared.cledValue[1] = 0;
            }
            usleep(1000000);
        }
    }

	return 0;
}