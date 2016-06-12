//
// Created by parallels on 6/11/16.
//

#include <iostream>
#include<cstdio>

#include "touch.h"

using namespace std;

void print_event(touch::touch_event e)
{
    printf("<%d, %d> %d\n", e.x, e.y, e.event_code);
}

int main(){

    touch::init();
    touch::add_callback(&print_event);
    while(true){
        int a =1;
        a=  2;

    }
    return 0;

}