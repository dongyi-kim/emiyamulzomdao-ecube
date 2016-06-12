//
// Created by root on 16. 6. 12.
//

#ifndef SYSPROG_THREAD_MANAGER_H
#define SYSPROG_THREAD_MANAGER_H

#include <pthread.h>

namespace thread_manager{
    pthread_mutex_t* get_tlcd();
    pthread_mutex_t* get_oled();
    pthread_mutex_t* get_bled();
    pthread_mutex_t* get_seg();
    pthread_mutex_t* get_mled();
    pthread_mutex_t* get_buzz();
    pthread_mutex_t* get_cled();
    pthread_mutex_t* get_dips();
    pthread_mutex_t* get_key();

#endif //SYSPROG_THREAD_MANAGER_H
