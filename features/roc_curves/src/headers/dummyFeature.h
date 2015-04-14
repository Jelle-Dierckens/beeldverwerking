#ifndef  DUMMYFEATURE_H
#define DUMMYFEATURE_H

#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "headers/Ifeature.h"


using namespace cv;

class dummyFeature: public Ifeature {
public:
    dummyFeature(const char* path) {
        image=imread(path);
        image.convertTo(image, CV_32FC3);
        Mat channels[3];
        split(image, channels);
        image=channels[0];
        normalize(image, image, 0,1, NORM_MINMAX);
    }

    virtual void applyFeature(const Mat &inputImage, Mat &outputImage, float  param) {
        threshold(image,outputImage, param, 1.0, THRESH_BINARY);
    }

private:
    Mat image;
};



#endif
