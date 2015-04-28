#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
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
	
	String directory = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\data_road\\training\\image_2\\";
	analyse.readallpics(directory);

	return 0;
}