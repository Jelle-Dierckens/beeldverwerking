#include <iostream>
#include "headers/roc_analizer.h"
#include "headers/dummyFeature.h"
#include "headers/graphview.h"
#include <vector>
#include "headers/Bayes.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{


    if (argc <2) {
        cout<<"args: inputimage  truthimage";
        return 255;
    }
    Mat truthImage;
    Mat inputImage;

    inputImage=imread(argv[1]);
    truthImage=imread(argv[2]);

    Bayes feature(argv[3]);
    Roc_analizer analizer(feature);
    //feature.applyFeature(inputImage, outputImage, 0.5);
    analizer.analize(inputImage, truthImage);
    int wait;
    cin>>wait;
    waitKey(0);

    return 0;
}

