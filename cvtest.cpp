//
// Created by root on 16. 6. 12.
//
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;

int main(){
    cv::VideoCapture cap("/dev/video0");
    //capture.set(CV_CAP_PROP_FRAME_WIDTH,640);
    //capture.set(CV_CAP_PROP_FRAME_HEIGHT,480);
    if ( !cap.isOpened() )  // if not success, exit program
    {
        cout << "Cannot open the web cam" << endl;
        exit(-1);
    }
    return 0;
}