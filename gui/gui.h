//
// Created by parallels on 6/12/16.
//

#ifndef EMIYAMULZOMDAO_ECUBE_GUI_H
#define EMIYAMULZOMDAO_ECUBE_GUI_H
#include <algorithm>

using namespace std;

namespace gui{

    class ui_area{
    private:
        int up, down, left, right;
        bool visible;
        const vector<vector<unsigned long> > image;
    public:
        ui_area(int x1, int y1, int x2, int y2, const vector<vector<unsigned long> > &image, bool visible = true)
            : image(image), visible(visible)
        {
            this->image = image;
            up = min(y1, y2);
            down = max(y1, y2);
            left = min(x1, x2);
            right = max(x1, x2);
        }

        void draw();

        bool in_area(int x,int y);



    };

    void setup();


}
#endif //EMIYAMULZOMDAO_ECUBE_GUI_H
