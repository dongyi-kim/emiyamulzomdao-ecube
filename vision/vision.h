/**
    @file       vision.h
    @author     dongyi kim
    @brief      about touch event
*/
#include <iostream>
//#include <opencv2/opencv.hpp>
//#include <opencv2/core/core.hpp>
//#include <opencv2/video/background_segm.hpp>
//#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
using namespace std;

#ifndef EMIYAMULZOMDAO_ECUBE_VISION_H
#define EMIYAMULZOMDAO_ECUBE_VISION_H

namespace vision{

    //read image from file(fname) and draw the contour of plan in it on bitmap
    void draw_contour(vector<vector<unsigned long> > &bitmap, string fname="");

    //resize image to (rows, cols) size
    void resize(vector<vector<unsigned long> > &bitmap, int rows, int cols);
}
#endif //EMIYAMULZOMDAO_ECUBE_VISION_H