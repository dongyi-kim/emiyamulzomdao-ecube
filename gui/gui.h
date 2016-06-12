//
// Created by parallels on 6/12/16.
//

#ifndef EMIYAMULZOMDAO_ECUBE_GUI_H
#define EMIYAMULZOMDAO_ECUBE_GUI_H
#include <algorithm>
#include<vector>
#include<string>
using namespace std;

namespace gui{

    struct area{
        int up, down, left, right;
        bool visible;
        area(int x1, int y1, int x2, int y2, bool visible = true)
            :  visible(visible)
        {
            this->image = image;
            up = min(y1, y2);
            down = max(y1, y2);
            left = min(x1, x2);
            right = max(x1, x2);
        }

//        void draw();
        bool in_area(int x,int y){
            return ( left <= x && x <= right && up <= y && y<=down);
        }
    };


    void setup();


}
#endif //EMIYAMULZOMDAO_ECUBE_GUI_H
