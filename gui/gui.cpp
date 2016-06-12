//
// Created by parallels on 6/12/16.
//

#include "gui.h"
#include "touch.h"
#include "display.h"
#include<dirent.h>

#include<vector>
#include<cstdio>
#include<string>
#include<pthread.h>

#define PATH_IMG_MENU_FLOWER    "img/menu_flower.bmp"
#define PATH_IMG_MENU_INFO      "img/menu_info.bmp"
#define PATH_IMG_MENU_FUNCTION  "img/menu_function.bmp"
#define PATH_IMG_MENU_SETTING   "img/menu_flower.bmp"

#define PATH_IMG_INFO_FEW       "img/few.bmp"
#define PATH_IMG_INFO_TOO       "img/too.bmp"
#define PATH_IMG_INFO_GOOD      "img/good.bmp"

typedef vector<vector<unsigned long> > bitmap_t;


#define INFO_FEW    0
#define INFO_TOO    1
#define INFO_GOOD   2

#define MENU_FLOWER     0
#define MENU_INFO       1
#define MENU_FUNCTION   2
#define MENU_SETTING    3

using namespace std;
bitmap_t img_pages[4];
bitmap_t img_info[3];


int selected_page = -1;

pthread_t display_thread = NULL;


void* init_flower_page(void* nullable);
void* init_info_page(void *nullable);
void* init_function_page(void* nullable);
void* init_setting_page(void *nullable);

gui::area area_function_water(60, 160, 60 + 300, 160 + 300);
gui::area area_function_sound(440, 160, 440 + 300, 160 + 300);

gui::area area_flower_before(30, 1110, 30 + 130, 1110 + 130);
gui::area area_flower_after(640, 1110, 640+130, 1110 + 130);
gui::area area_flower_capture(233, 1110, 233+130, 1110+130 );
gui::area area_flower_upload(437, 1110, 437+130, 1110+130);



namespace gui{

    void on_touch(touch::touch_event e)
    {
        if((e.event_code & touch::EVENT_TOUCH_DOWN) > 0)
        {   //when touch-down event

            //tab-menu side
            if( 0 <= e.y && e.y <= 79)
            {   // y : [0, 79]  - tab menu side
                for(int i = 0 ; i <4 ; i++)
                {
                    if( i * 200 <= e.x && e.x < (i+1)*200)
                    {
                        draw_page(i);
                        return;
                    }
                }
            }

            //in page
            if(selected_page == MENU_FLOWER)
            {
                if(area_flower_after.in_area(e.x, e.y))
                {

                }else if(area_flower_before.in_area(e.x, e.y))
                {

                }else if(area_flower_capture.in_area(e.x, e.y))
                {

                }else if(area_flower_upload.in_area(e.x, e.y))
                {

                }
            }
            else if(selected_page == MENU_FUNCTION)
            {
                if(area_function_sound.in_area(e.x, e.y))
                {

                }else if(area_function_water.in_area(e.x, e.y))
                {

                }
            }
        }
    }

    void draw_page(int page)
    {

        if(page == selected_page)
        {   //same page selected
            return;
        }

        if(display_thread != NULL )
        {   //if some thread already run
            pthread_cancel(display_thread);
            pthread_join(display_thread, NULL);
            display_thread = NULL;
        }
        switch(page){
            case MENU_FLOWER:
                pthread_create(&display_thread, NULL, &init_flower_page, NULL);
                break;
            case MENU_INFO:
                pthread_create(&display_thread, NULL, &init_info_page, NULL);
                break;
            case MENU_FUNCTION:
                pthread_create(&display_thread, NULL, &init_function_page, NULL);
                break;
            case MENU_SETTING:
                pthread_create(&display_thread, NULL, &init_setting_page, NULL);
                break;
            default:
                page = MENU_FLOWER;
                draw_page(page);
                break;
        }
        selected_page = page;
    }
    
    void setup()
    {
        touch::add_callback(&on_touch);
        display::read_bmp(img_pages[MENU_FLOWER], PATH_IMG_MENU_FLOWER);
        display::read_bmp(img_pages[MENU_INFO], PATH_IMG_MENU_INFO);
        display::read_bmp(img_pages[MENU_FUNCTION], PATH_IMG_MENU_FUNCTION);
        display::read_bmp(img_pages[MENU_SETTING], PATH_IMG_MENU_SETTING);

        display::read_bmp(img_info[INFO_FEW], PATH_IMG_INFO_FEW);
        display::read_bmp(img_info[INFO_GOOD], PATH_IMG_INFO_GOOD);
        display::read_bmp(img_info[INFO_TOO], PATH_IMG_INFO_TOO);

        draw_page(MENU_FLOWER);
    }

}


void* init_flower_page(void* nullable)
{
    display::draw_bmp(pages[0]);
    while(true)
    {
        sleep(10);
    }
    pthread_exit(NULL);
}
void* init_info_page(void *nullable)
{
    display::draw_bmp(pages[1]);
    while(true)
    {
        display::draw_bmp(img_info[INFO_FEW], 500, 220);
        display::draw_bmp(img_info[INFO_FEW], 500, 440);
        display::draw_bmp(img_info[INFO_FEW], 500, 660);
        display::draw_bmp(img_info[INFO_FEW], 500, 880);
        display::draw_bmp(img_info[INFO_FEW], 500, 1100);
        sleep(10);
    }
    pthread_exit(NULL);
}
void* init_function_page(void* nullable)
{
    display::draw_bmp(pages[2]);

    pthread_exit(NULL);
}
void* init_setting_page(void *nullable)
{
    display::draw_bmp(pages[3]);

    pthread_exit(NULL);
}


