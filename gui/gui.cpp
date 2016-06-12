//
// Created by parallels on 6/12/16.
//

#include "gui.h"
#include "touch.h"
#include "display.h"

using namespace std;

namespace gui{

    void on_touch(touch::touch_event e)
    {
        
    }
    
    void setup()
    {
        touch::add_callback(&on_touch);
    }
    

  
    
}