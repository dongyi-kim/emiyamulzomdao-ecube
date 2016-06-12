//
// Created by root on 16. 6. 12.
//

#include "thread_manager.h"

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>


using namespace std;

pthread_mutex_t mutex_tlcd;
pthread_mutex_t mutex_oled;
pthread_mutex_t mutex_bled;
pthread_mutex_t mutex_seg;
pthread_mutex_t mutex_mled;
pthread_mutex_t mutex_buzz;
pthread_mutex_t mutex_cled;
pthread_mutex_t mutex_dips;
pthread_mutex_t mutex_key;

namespace thread_manager{

    pthread_mutex_t* get_tlcd(){
        return &mutex_tlcd;
    }
    pthread_mutex_t* get_oled(){
        return &mutex_oled;
    }
    pthread_mutex_t* get_bled(){
        return &mutex_bled;
    }
    pthread_mutex_t* get_seg(){
        return &mutex_seg;
    }
    pthread_mutex_t* get_mled(){
        return &mutex_mled;
    }
    pthread_mutex_t* get_buzz(){
        return &mutex_buzz;
    }
    pthread_mutex_t* get_cled(){
        return &mutex_cled;
    }
    pthread_mutex_t* get_dips(){
        return &mutex_dips;
    }
    pthread_mutex_t* get_key(){
        return &mutex_key;
    }
}