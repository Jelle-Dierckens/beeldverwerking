#ifndef  IFEATURE_H
#define IFEATURE_H

#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"

using namespace cv;

class Ifeature {
public:
        virtual void applyFeature(const Mat &inputImage, Mat &outputImage, float param)=0; //param will be iterated between 0 and 1
private:
};


#endif IFEATURE_H
