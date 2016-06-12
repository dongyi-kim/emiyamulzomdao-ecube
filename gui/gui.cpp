//
// Created by parallels on 6/12/16.
//

#include "gui.h"
#include "touch.h"
#include "display.h"
#include<vector>
#include<string>
using namespace std;
vector<vector<unsigned long> > pages[4];

int selected_page = -1;
namespace gui{

    void on_touch(touch::touch_event e)
    {
        
    }

    void draw_page(int page)
    {
        if(page == selected_page)
        {
            return;
        }
        switch(page){
            case 0:
                display::draw_bmp(pages[0]);
                break;
            case 1:
                display::draw_bmp(pages[1]);
                break;

            case 2:
                display::draw_bmp(pages[2]);
                break;

            case 3:
                display::draw_bmp(pages[3]);
                break;
            default:
                page = 0;
                draw_page(page);
                break;
        }
        selected_page = page;
    }
    
    void setup()
    {
        touch::add_callback(&on_touch);
        display::read_bmp(pages[0], "img/menu_flower.bmp");
        display::read_bmp(pages[1], "img/menu_flower.bmp");
        display::read_bmp(pages[2], "img/menu_flower.bmp");
        display::read_bmp(pages[3], "img/menu_flower.bmp");
        draw_page(0);
    }

}