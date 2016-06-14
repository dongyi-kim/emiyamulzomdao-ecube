/**
    @file       vision.h
    @author     dongyi kim
    @brief      about image processing based on opencv
*/

#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include <algorithm>
using namespace cv;
using namespace std;


//create cv::Mat instance from bitmap raw data
cv::Mat bitmap2mat(const vector<vector<unsigned long> > &bitmap){
    int rows = bitmap.size();
    int cols = bitmap[0].size();

    cv::Mat mat(cv::Size(cols, rows), CV_8UC3);

    for(int r = 0 ; r < rows ; r++){
        for(int c =0 ; c < cols; c++){


            unsigned long long r = ((unsigned long long)bitmap[r][c] >> (unsigned long long)16);
            unsigned long long g = ((unsigned long long)bitmap[r][c] >> (unsigned long long)8);
            unsigned long long b = ((unsigned long long)bitmap[r][c]);

            mat.at<cv::Vec3b>(r,c)[0] = (char)r;
            mat.at<cv::Vec3b>(r,c)[1] = (char)g;
            mat.at<cv::Vec3b>(r,c)[2] = (char)b;
        }
    }

    cout << "bye" << endl;
    return mat;
}

//get the vector of contour points from image
vector<cv::Point> get_contour(cv::Mat &img){
    int rows = img.rows;
    int cols = img.cols;
    cv::Mat hsv, green;

    cv::cvtColor(img, hsv, cv::COLOR_BGR2HSV);

    //detect green area from image
    cv::inRange(hsv, cv::Scalar(43, 38, 60), cv::Scalar(94, 252, 247), green);


    //use bluring to remote white-noise
    cv::Mat blured;
    cv::GaussianBlur(green, blured, cv::Size(11, 11), 0);

    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hi;

    //find contours point on blured image
    cv::findContours( blured, contours, hi, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, cv::Point(0,0));

    cout << "ccc : " << contours.size() << endl;
    if(contours.empty()){
        return vector<cv::Point>();
    }

    long long max_area = 0;
    int max_idx=0;

    //find garbage contours (noise)
    for(int i = 0 ; i < contours.size(); i++){
        if(contours[i].empty()){
            continue;
        }

        int l, r, u, d;
        l = r = contours[i][0].x;
        u = d = contours[i][0].y;
        for(int j = 0 ; j < contours[i].size(); j++){
            l = min(l, contours[i][j].x);
            r = max(r, contours[i][j].x);
            u = max(u, contours[i][j].y);
            d = min(d, contours[i][j].y);
        }

        long long dy = u - d;
        long long dx = r - l;
        long long area = dx * dy;
        if(area >= max_area){
            max_area = area;
            max_idx = i;
        }
    }

    //return contour
    return contours[max_idx];
}

namespace vision{
    void draw_contour(vector<vector<unsigned long> > &bitmap, string fname) {
        int rows = bitmap.size();
        if(rows == 0){
            return;
        }
        int cols = bitmap[0].size();

        //create mat
        cv::Mat org = bitmap2mat(bitmap);

        cv::Mat img;
        if(fname.length() > 4){
            cout << "no " << endl;
            img = cv::imread(fname.c_str(), CV_LOAD_IMAGE_COLOR);
        }else{
            img = org;
        }

        vector<cv::Point> contour = get_contour(img);
        cout << "size : " << contour.size() << endl;
        for(int i = 0 ; i < contour.size(); i++){
            bitmap[contour[i].y][contour[i].x] = 0xFF0000;
        }
    }

    void resize(vector<vector<unsigned long> > &bitmap, int rows, int cols)
    {
        cv::Mat mat(cv::Size(bitmap[0].size(), bitmap.size()), CV_8UC3);
        cv::Mat resizeF;
        for(int r = 0 ; r < bitmap.size(); r++){
            for(int c = 0 ;c < bitmap[0].size(); c++){
                mat.at<cv::Vec3b>(r,c) = bitmap[r][c];
            }
        }
//        cv::resize(mat, resizeF, cv::Size(cols, rows));
//        bitmap.clear();
//        bitmap.resize(rows);
//        for(int r = 0; r< rows; r++){
//            bitmap[r].resize(cols);
//            for(int c = 0; c < cols; c++){
//                cv::Vec3b v = resizeF.at<cv::Vec3b>(r,c);
//                bitmap[r][c] = (unsigned long)(v[2]<<16l) | (v[0] << 8l) | v[1];
//            }
//        }
    }
}
