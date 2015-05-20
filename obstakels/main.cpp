#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <dirent.h>
//~ #include "chrono.h"
#include <math.h>
#include <vector>

using namespace cv;
using namespace std;

static void help()
{
    cout
    << "\nThis program illustrates the use of findContours and drawContours\n"
    << "The original image is put up along with the image of drawn contours\n"
    << "Usage:\n"
    << "./contours2\n"
    << "\nA trackbar is put up which controls the contour level from -3 to 3\n"
    << endl;
}

const int w = 500;
int levels = 3;
vector<vector<Point> > contours;
vector<Vec4i> hierarchy;

//~ static void on_trackbar(int, void*)
//~ 
//~ {
    //~ Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
    //~ int _levels = levels - 3;
    //~ drawContours( cnt_img, contours, _levels <= 0 ? 3 : -1, Scalar(128,255,255),
                  //~ 3, LINE_AA, hierarchy, std::abs(_levels) );
    //~ imshow("contours", cnt_img);
//~ }
int getContours(Mat* image, String name)
{
    //~ Mat img = Mat::zeros(w, w, CV_8UC1);
    
    //~ namedWindow( "image", 1 );
    //~ imshow( "image", 1 );
    Mat zwartwit;
    cvtColor(*image, zwartwit, COLOR_BGR2GRAY);
    
    //~ imshow("zwartwit", zwartwit);
    vector<vector<Point> > contours0;
    
    findContours(zwartwit.clone(), contours0, RETR_TREE , CHAIN_APPROX_NONE );	
    
    imshow("test", zwartwit);
    waitKey();
    
    contours.resize(contours0.size());
    for( size_t k = 0; k < contours0.size(); k++ ){
        approxPolyDP(Mat(contours0[k]), contours[k], 3, true);
	}
    
    Mat cnt_img = Mat::zeros(w, w, CV_8UC3);
    drawContours( cnt_img, contours, -1, Scalar(128,255,255),
                  3, LINE_AA, hierarchy, std::abs(3) );
    imshow("contours", cnt_img);

    waitKey();
    exit(0);
    //Extract the contours so that
    

    //~ findContours( img, contours0, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    //~ contours.resize(contours0.size());
    //~ for( size_t k = 0; k < contours0.size(); k++ )
        //~ approxPolyDP(Mat(contours0[k]), contours[k], 3, true);
    //~ namedWindow( "contours", 1 );
    //~ createTrackbar( "levels+3", "contours", &levels, 7, on_trackbar );
    //~ on_trackbar(0,0);
    //~ waitKey();
    return 0;
}

/*
leest alle foto's
*/
void readallpics(String directory){
	DIR *dir;
	struct dirent *ent;

	const char* diropen = directory.c_str();
	//char* diropen = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\data_road\\training\\image_2\\";
	if ((dir = opendir(diropen)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			String name = ent->d_name;
			if (name.find(".png") < name.length()){
				if (name != "uu_000067.png"){
					printf("%s\n", ent->d_name);
					String filename = directory + ent->d_name;
					//cout << filename << endl;
					Mat image = imread(filename, IMREAD_COLOR); // Read the file
					getContours(&image, filename);
					exit(0);
				}
            }
		}
		closedir(dir);
	}
}

int main(int argc, char** argv)
{	
	//url http://www.transistor.io/revisiting-lane-detection-using-opencv.html
	//String filename = "original.bmp";
	
	//String filename = "um_000025.PNG";

	//Mat image;
	//image = imread(filename, IMREAD_COLOR); // Read the file
	//imshow("picture", image);
	//String directory = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\results";
	
	// directory Frederick:
	//cv::String directory = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\data_road\\training\\image_2\\";
	//directory Jens:
	//~ String directory = "C:\\Beeldverwerking\\data_road\\training\\image_2\\";
	//~ String bev_directory = "C:\\Beeldverwerking\\data_road\\training\\image_bev\\";
	String directory = "../../data_road/training/image_2/";
	
	readallpics(directory);
	
	printf("exit\n");

	return 0;
}

