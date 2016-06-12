//
// Created by parallels on 6/11/16.
//


/* Sample code for Touch Logic
 *
 */
#include <iostream>
#include<cstdio>

// touch.h should be included
#include "touch.h"

using namespace std;

// defidn callback function that is called when touch events occur
void print_event(touch::touch_event e)
{
    // filter touch event with bit mask
    if((e.event_code & touch::EVENT_TOUCH_DOWN) > 0 )
    {   //when Touch Down event occur
        printf("<%d, %d> %d\n", e.x, e.y, e.event_code);
    }

}

int main()
{
    touch::init(); //called in main

    //add callback your function with pointer
    touch::add_callback(&print_event);
    while(true){
        int a =1;
        a=  2;

    }
    return 0;

}