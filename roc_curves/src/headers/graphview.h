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
    static void createGraph(Mat &outputImage, const std::vector<std::pair<double, double> > &datapoints);
private:

};

void graphview::createGraph(Mat &outputImage, const std::vector<std::pair<double, double> > &datapoints) {
    Point startPoint(outputImage.rows- datapoints[1].first*outputImage.rows,datapoints[1].second*outputImage.cols);
    //~ printf("Startpoint \(%d,%d\)\n",, datapoints[i]);
    Point nextPoint;
    for (int i=0; i<datapoints.size(); i++) {
		//~ printf("Drawing point \(%d,%d\)\n",datapoints[i].first, datapoints[i].second);
        nextPoint=Point(outputImage.rows- datapoints[i].first*outputImage.rows,datapoints[i].second*outputImage.cols);
        line(outputImage, startPoint, nextPoint, Scalar(256,125,125));
        startPoint=nextPoint;
        circle(outputImage, startPoint,3, Scalar(125,256,125), -1);
    }
    line(outputImage, Point(outputImage.rows,0), Point(0,outputImage.cols), Scalar(100,100,100));
}


#endif
