//
// Created by parallels on 6/12/16.
//


#include <cstdio>
#include "gui.h"
#include "touch.h"
#include "display.h"
#include <iostream>
#include<vector>
#include <algorithm>
using namespace std;


int main(){
    touch::init();

    display::init();

    gui::setup();

    vector<vector<unsigned long> > bitmap(10);
    for(int i = 0 ; i < 10;i++){
        bitmap[i].resize(30);

    }

    display::draw_bmp(bitmap, 0,300);
    while(1){
        int a =1;
        a = 2;
        continue;
    }
    return 0;
}