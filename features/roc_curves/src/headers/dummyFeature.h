#ifndef  DUMMYFEATURE_H
#define DUMMYFEATURE_H

#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "headers/Ifeature.h"


using namespace cv;

class dummyFeature: public Ifeature {
public:
    virtual void applyFeature(const Mat &inputImage, Mat &outputImage, float  param) {
       if(param <=0.1) { //pass inputImage to outputImage
            outputImage=inputImage;
        }
        else if(param ==05) {//return negative image
            outputImage=Mat::zeros(inputImage.size(), CV_32FC1); //32 floating, 1 channel
        }
        else  {//return positive image
            outputImage=Mat::ones(inputImage.size(), CV_32FC1);
        }
    }

private:
};



#endif
