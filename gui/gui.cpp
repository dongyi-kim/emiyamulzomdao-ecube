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
#include<iostream>
#include<string.h>
#include<algorithm>
#include<utility>

#include<pthread.h>
#include "../cameratest.h"
#include "../vision/camera.h"
#define PATH_IMG_MENU_FLOWER    "img/menu_flower.bmp"
#define PATH_IMG_MENU_INFO      "img/menu_info.bmp"
#define PATH_IMG_MENU_FUNCTION  "img/menu_function.bmp"
#define PATH_IMG_MENU_SETTING   "img/menu_flower.bmp"

#define PATH_IMG_INFO_FEW       "img/few.bmp"
#define PATH_IMG_INFO_TOO       "img/too.bmp"
#define PATH_IMG_INFO_GOOD      "img/good.bmp"

#define PATH_DIR_CAPTURE        "img/capture/"



#define INFO_FEW    0
#define INFO_TOO    1
#define INFO_GOOD   2

#define MENU_FLOWER     0
#define MENU_INFO       1
#define MENU_FUNCTION   2
#define MENU_SETTING    3

typedef vector<vector<unsigned long> > bitmap_t;

using namespace std;
bitmap_t img_pages[4];
bitmap_t img_info[3];


int selected_page = -1;

pthread_t display_thread = NULL;


void init_flower_page   ();
void init_info_page     ();
void init_function_page ();
void init_setting_page  ();

gui::area area_menu_flower(0,0, 0 + 200, 0 + 80);
gui::area area_menu_info(200, 0, 200 + 200, 0+ 80);
gui::area area_menu_function(400, 0, 400 + 200, 0+ 80);
gui::area area_menu_setting(600, 0, 600+200, 0 + 80);

gui::area area_function_water(60, 160, 60 + 300, 160 + 300);
gui::area area_function_sound(440, 160, 440 + 300, 160 + 300);

gui::area area_flower_before(30, 1110, 30 + 130, 1110 + 130);
gui::area area_flower_after(640, 1110, 640+130, 1110 + 130);
gui::area area_flower_capture(233, 1110, 233+130, 1110+130 );
gui::area area_flower_upload(437, 1110, 437+130, 1110+130);


namespace info_page{
    void load(){
        display::draw_bmp(img_pages[MENU_INFO]);
        display::draw_bmp(img_info[INFO_FEW], 500, 220);
        display::draw_bmp(img_info[INFO_FEW], 500, 440);
        display::draw_bmp(img_info[INFO_FEW], 500, 660);
        display::draw_bmp(img_info[INFO_FEW], 500, 880);
        display::draw_bmp(img_info[INFO_FEW], 500, 1100);
    }
    void unload()
    {

    }
}

namespace function_page{
    void load(){
        display::draw_bmp(img_pages[MENU_FUNCTION]);

    }
    void unload(){

    }
}

namespace setting_page
{
    void load(){
        display::draw_bmp(img_pages[MENU_SETTING]);

    }
    void unload(){

    }
}

namespace flower_page{
    vector<string> vpath;
    vector<vector<unsigned long> > vshowed;
    int selected_picture_idx = -1;

    void show_picture(int idx){
        if(idx < 0 || (vpath.size() > 0 && idx >= vpath.size())) {
            return;
        }
        selected_page = idx;

        display::read_bmp(vshowed, vpath[idx]);
        display::draw_bmp(vshowed, 0, 80);
    }

    void load()
    {
        display::draw_bmp(img_pages[MENU_FLOWER]);
        //get the list of image file
        vpath.clear();

        DIR *dpdf;
        struct dirent *epdf;
        dpdf = opendir(PATH_DIR_CAPTURE);
        if (dpdf != NULL){
            while (epdf = readdir(dpdf))
            {
                string fname =epdf->d_name;
                if(fname.length() > 4 )
                {
                    string ext = fname.substr(fname.length()-4);
                    transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
                    if(ext == ".bmp"){
                        fname = PATH_DIR_CAPTURE + fname;
                        cout << fname << endl;
                        vpath.push_back(fname);
                    }
                }
            }
        }
        closedir(dpdf);

        if(selected_picture_idx >= vpath.size() || selected_picture_idx < 0)
        {
            selected_picture_idx = max(0, ((int)vpath.size())-1);
        }
        show_picture(selected_picture_idx);

    }

    void unload(){

    }

    void show_next(){
        if(selected_picture_idx < 0 ){
            return;
        }
        show_picture(++selected_picture_idx);
    }
    void show_before(){
        if(selected_picture_idx >= vpath.size()) {
            return;
        }
        show_picture(--selected_picture_idx);
    }
}


namespace gui{


    void draw_page(int page)
    {
        printf("[draw_page] page :%d\n",page);
        if(page < 0 || page > 3 || page ==selected_page ){
            return;
        }

        int before = selected_page;
        selected_page = page;
        //unload before page
        switch(before)
        {
            case MENU_FLOWER:
                flower_page::unload();
                break;
            case MENU_INFO:
                info_page::unload();
                break;
            case MENU_FUNCTION:
                function_page::unload();
                break;
            case MENU_SETTING:
                setting_page::unload();
                break;
        }

        //load new page
        switch(selected_page){
            case MENU_FLOWER:
                flower_page::load();
                break;
            case MENU_INFO:
                info_page::load();
                break;
            case MENU_FUNCTION:
                function_page::load();
                break;
            case MENU_SETTING:
                setting_page::load();
                break;
            default:
                return;
        }

    }



    void on_touch(touch::touch_event e)
    {
        if((e.event_code & touch::EVENT_TOUCH_DOWN) > 0)
        {   //when touch-down event

            //tab-menu side
            if( 0 <= e.y && e.y <= 79)
            {   // y : [0, 79]  - tab menu side
                if(area_menu_flower.in_area(e.x, e.y))
                {
                    draw_page(MENU_FLOWER);
                }else if(area_menu_info.in_area(e.x, e.y))
                {
                    draw_page(MENU_INFO);
                }else if(area_menu_function.in_area(e.x, e.y))
                {
                    draw_page(MENU_FUNCTION);
                }else if(area_menu_setting.in_area(e.x, e.y))
                {
                    draw_page(MENU_SETTING);
                }
                return;
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
