#include <cstdio>
#include <iostream>
#include <pthread.h>
#include <curl/curl.h>
#include <pthread.h>
#include "common.h"

using namespace std;

int main() {
    /*
     * outer while: login -> sensor, edit -> logout -> ...
     */
    Shared shared;

    pthread_t observe_thread;
    pthread_t edit_thread;

    pthread_create(&observe_thread, NULL, observe::observe, (void*)&shared);
    pthread_create(&edit_thread, NULL, edit::edit, (void*)&shared);

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
            if( shared.mode == NOT_AUTHORIZED ) {
                cout<<"Logout!!"<<endl;
                break;
            }

            usleep(100000);
        }
    }

	return 0;
}