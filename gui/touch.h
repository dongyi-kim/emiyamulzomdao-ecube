//
// Created by parallels on 6/12/16.
//

#ifndef EMIYAMULZOMDAO_ECUBE_TOUCH_H
#define EMIYAMULZOMDAO_ECUBE_TOUCH_H


using namespace std;

namespace touch
{
    const unsigned int EVENT_TOUCH = 1;
    const unsigned int EVENT_TOUCH_DOWN = 2;
    const unsigned int EVENT_TOUCH_UP   = 4;
    const unsigned int EVENT_TOUCH_PRESS = 8;
    const unsigned int EVENT_TOUCH_MOVE  = 16;
    const unsigned int MOVE_DISTANCE    = 10;


    const int EVENT_NUM = 2;


    struct touch_event{
        int x, y;
        int prev_x, prev_y;
        int event_code;
        touch_event(int x, int y, int prev_x = -1, int prev_y = -1)
                : x(x), y(y), prev_x(prev_x), prev_y(prev_y)
        {
            event_code = 0;
            if( x >= 0 && y >= 0){
                event_code |= EVENT_TOUCH;
                if(prev_x < 0 || prev_y < 0){
                    event_code |= EVENT_TOUCH_DOWN;
                }else{
                    event_code |= EVENT_TOUCH_PRESS;
                    long long dist = (long long) (x-prev_x) * (x-prev_x) + (y - prev_y) * (prev_y);
                    if(dist > 1){
                        event_code |= EVENT_TOUCH_MOVE;
                    }
                }
            }else{
                if(prev_x >= 0 && prev_y >= 0){
                    event_code |= EVENT_TOUCH_UP;
                }
            }
        }
    };

    int init();

    void add_callback( void(*)(touch_event) );
    void remove_callback( void(*)(touch_event) );
}
#endif //EMIYAMULZOMDAO_ECUBE_TOUCH_H
