//
// Created by root on 16. 6. 12.
//

#include "thread_manager.h"

#include <pthread.h>
#include <signal.h>
#include <stdlib.h>


using namespace std;

pthread_mutex_t a;


namespace thread_manager{

    pthread_mutex_t* get_a(){
        return &a;
    }
}