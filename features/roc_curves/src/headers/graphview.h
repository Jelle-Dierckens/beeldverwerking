#ifndef  GRAPHVIEW_H
#define GRAPHVIEW_H

#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <vector>



using namespace cv;

class graphview {
public:
    static void createGraph(Mat &outputImage, const std::vector<std::pair<double, double> > &datapoints, const char* name_x_axis, const char* name_y_axis);
private:

};

void graphview::createGraph(Mat &outputImage, const std::vector<std::pair<double, double> > &datapoints, const char *name_x_axis, const char *name_y_axis) {
    Point startPoint(0,0);
    Point nextPoint;
    for (int i=0; i<datapoints.size(); i++) {
        nextPoint=Point(datapoints[i].first*outputImage.cols,outputImage.rows- datapoints[i].second*outputImage.rows);
        line(outputImage, startPoint, nextPoint, Scalar(125,125,125));
        startPoint=nextPoint;
        circle(outputImage, startPoint,2, Scalar(125,125,125), -1);
    }
}


#endif
