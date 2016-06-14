/**    
    @file       touch.h
    @author     dongyi kim
    @brief      about touch event
*/

#ifndef EMIYAMULZOMDAO_ECUBE_TOUCH_H
#define EMIYAMULZOMDAO_ECUBE_TOUCH_H

using namespace std;

namespace touch
{
    
    //event code of each touch event 
    const unsigned int EVENT_TOUCH          = (1<<0);
    const unsigned int EVENT_TOUCH_DOWN     = (1<<1);
    const unsigned int EVENT_TOUCH_UP       = (1<<2);
    const unsigned int EVENT_TOUCH_PRESS    = (1<<3);
    const unsigned int EVENT_TOUCH_MOVE     = (1<<4);
    
    const unsigned int MOVE_DISTANCE        = 1;


    //structure contains informations of touch event
    struct touch_event
    {
        int x, y;           //coordinate of event
        int prev_x, prev_y; //coordinate of last event before it
        int event_code;     //event code

        //constructor
        touch_event(int x, int y, int prev_x = -1, int prev_y = -1)
                : x(x), y(y), prev_x(prev_x), prev_y(prev_y)
        {
            //event code contains information of events
            event_code = 0;

            //if event occured inside panel
            if( x >= 0 && y >= 0)
            {
                //touch on
                event_code |= EVENT_TOUCH;

                if(prev_x < 0 || prev_y < 0)
                {   //if last event is outside of panel
                    //this is new touch down event
                    event_code |= EVENT_TOUCH_DOWN;
                }else
                {   //if touch event already occured
                    //this is long-pressed event
                    event_code |= EVENT_TOUCH_PRESS;
                    long long dist = (long long) (x-prev_x) * (x-prev_x) + (y - prev_y) * (prev_y);

                    if(dist > MOVE_DISTANCE)
                    {   //if ther's distance from last coordinate at least MOVE_DISTANCE
                        //this is dragging event
                        event_code |= EVENT_TOUCH_MOVE;
                    }
                }
            }else
            {
                if(prev_x >= 0 && prev_y >= 0)
                {   //if lst evnet is inside of panel
                    //this is touch up event
                    event_code |= EVENT_TOUCH_UP;
                }
            }
        }
    };

    //initialize touch library
    int init();
    
    //add callback function to receive touch event 
    void add_callback( void(*)(touch_event) );
    
    //remote callback function 
    void remove_callback( void(*)(touch_event) );



}
#endif //EMIYAMULZOMDAO_ECUBE_TOUCH_H