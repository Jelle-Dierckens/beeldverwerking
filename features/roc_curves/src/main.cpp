#include <iostream>
#include "headers/roc_analizer.h"
#include "headers/dummyFeature.h"
#include "headers/graphview.h"
#include <vector>

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{


        /*if (argc <2) {
            cout<<"args: inputimage  truthimage";
            return 255;
        }*/
        Mat truthImage;
        Mat inputImage;

        inputImage=imread(argv[1]);
        truthImage=imread(argv[2]);
    dummyFeature feature;
    Roc_analizer analizer(feature);

   // analizer.analize(inputImage, truthImage);
    Mat testImage=Mat::zeros(256, 256, CV_8U);
    vector<pair<double, double> > testvalues;
    for (int i=0; i<10; i++) {
        testvalues.push_back(make_pair(i/10.0, i*0.1));
    }
    graphview::createGraph(testImage, testvalues, "test", "test");
    namedWindow("test");
    imshow("test", testImage);
    waitKey(0);

    return 0;
}

