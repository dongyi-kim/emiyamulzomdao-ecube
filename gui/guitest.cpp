//
// Created by parallels on 6/12/16.
//


#include <cstdio>
#include "gui.h"
#include "touch.h"
#include "display.h"

using namespace std;


int main(){
    touch::init();
    display::init();
    gui::setup();
    return 0;
}