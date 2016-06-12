//
// Created by parallels on 6/12/16.
//

#ifndef EMIYAMULZOMDAO_ECUBE_DISPLAY_H
#define EMIYAMULZOMDAO_ECUBE_DISPLAY_H
#include<vector>
#include<string>
using namespace std;

namespace display{
    int init();

    void draw_bmp(const vector<vector<unsigned long> > &bitmap, int x = 0, int y = 0);
    void read_bmp(vector<vector<unsigned long> > &bitmap, const string& filename);
}
#endif //EMIYAMULZOMDAO_ECUBE_DISPLAY_H
