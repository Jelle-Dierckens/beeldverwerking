#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "watershedwrapper.h"

#include <iostream>
#include <ctype.h>

using namespace cv;
using namespace std;


int main( int argc, const char** argv )
{
    if( argc != 2)
    {
        cout <<" Usage: display_image ImageToLoadAndDisplay" << endl;
        argv[1] = "um_000000.png";
    }
    Mat image = imread(argv[1]);   // Read the file
    Mat markerMask = imread("um_000000__edit.png");   // Read the file
    cv::cvtColor(markerMask, markerMask, COLOR_RGB2GRAY);

    if(! image.data )                              // Check for invalid input
    {
        cout <<  "Could not open or find the image" << std::endl ;
        return -1;
    }

    imshow("Display window",image);
    imshow("imageEdit",markerMask);

    WatershedWrapper water(true);
    Mat output = water.runWatershed(image,markerMask);

    while(true){
        char key = (char)waitKey(30);
        if (key == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
        {
            cout << "esc key is pressed by user" << endl;
            break;
        }
    }
    return 0;
}
