#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <cv.h>							// Frederick
#include <opencv\cv.h>						// Jens
#include <iostream>
#include "linefinder.h"
#include "Analyse.h"

using namespace cv;
using namespace std;



int main(int argc, char** argv)
{	
	//url http://www.transistor.io/revisiting-lane-detection-using-opencv.html
	//String filename = "original.bmp";
	
	//String filename = "um_000025.PNG";

	//Mat image;
	//image = imread(filename, IMREAD_COLOR); // Read the file
	//imshow("picture", image);
	Analyse analyse;
	//String directory = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\results";
	
	// directory Frederick:
	//cv::String directory = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\data_road\\training\\image_2\\";
	//directory Jens:
	cv::String directory = "C:\\Beeldverwerking\\data_road\\training\\image_2\\";
	cv::String bev_directory = "C:\\Beeldverwerking\\data_road\\training\\image_bev\\";

	analyse.readallpics(directory);

	return 0;
}


