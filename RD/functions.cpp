#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <cv.h>							// Frederick
#include <opencv\cv.h>						// Jens
#include <iostream>
#include "linefinder.h"
#include "linehandler.h"
#include "Analyse.h"
#include "dirent.h"
#include <vector>

using namespace cv;
using namespace std;

/*
leest alle foto's
*/
void Analyse::readallpics(cv::String directory){
	DIR *dir;
	struct dirent *ent;

	const char* diropen = directory.c_str();
	//char* diropen = "C:\\Users\\Frederick\\Documents\\Visual Studio 2013\\Projects\\RoadDetection\\data_road\\data_road\\training\\image_2\\";
	if ((dir = opendir(diropen)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			String name = ent->d_name;
			if (name.find(".png") < name.length()){
				printf("%s\n", ent->d_name);
				String filename = directory + ent->d_name;
				//cout << filename << endl;
				Mat image = imread(filename, IMREAD_COLOR); // Read the file
				Analyse analyse;
				analyse.processimage(image);
			}
		}
		closedir(dir);
	}
}



/*
Methode die baanweg detecteert aan de hand van countour lijnen.
En maakt gebruik van LineFinder klasse die lijnen selecteert met een minimum lengte ...
*/
Mat Analyse::findEdgeLines(Mat image){

	// Canny algorithm
	Mat contours;
	double_t thres1 = 180;
	double_t thres2 = 120;
	Canny(image, contours, thres1, thres2);

	std::vector<Vec2f> lines;
	int houghVote = 200;
	while (lines.size() < 5 && houghVote > 0){
		HoughLines(contours, lines, 1, CV_PI / 180, houghVote);
		houghVote -= 5;
	}
	std::cout << houghVote << "\n";
	Mat result(contours.rows, contours.cols, CV_8U, Scalar(255));
	contours.copyTo(result);

	// Draw the limes
	std::vector<Vec2f>::const_iterator it = lines.begin();
	//Mat hough(contours.size(), CV_8U, Scalar(0));
	while (it != lines.end()) {

		float rho = (*it)[0];   // first element is distance rho
		float theta = (*it)[1]; // second element is angle theta

		Point pt1(rho / cos(theta), 0);
		// point of intersection of the line with last row
		Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
		// draw a white line
		line(result, pt1, pt2, Scalar(255), 2);
		++it;
	}

	// Create LineFinder instance
	LineFinder ld;

	// Set probabilistic Hough parameters
	ld.setLineLengthAndGap(60, 10);
	ld.setMinVote(4);

	// Detect lines
	std::vector<Vec4i> li = ld.findLines(contours);
	Mat houghP(contours.size(), CV_8U, Scalar(0));
	ld.drawDetectedLines(houghP);
	//imshow("hough", houghP);
	return result;
}

/*
Hulpmethode om rap een HSV filter te maken
*/
Mat makeHSVfilter(Mat src, int iLowH, int iHighH, int iLowS, int iHighS, int iLowV, int iHighV){

	Mat imgHSV;
	Mat imgThresholded;
	cvtColor(src, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV
	inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);
	return imgThresholded;
}

/*
Methode om nieuwe filters te testen en stopt door op Esc toets te drukken
*/
void tryFilter(Mat & image){
	int iLowH = 91;
	int iHighH = 140;

	int iLowS = 6;
	int iHighS = 134;

	int iLowV = 28;
	int iHighV = 109;
	Mat imgHSV;
	Mat imgThresholded;

	namedWindow("Control", /*CV_*/WINDOW_AUTOSIZE); //create a window called "Control"
	//Create trackbars in "Control" window

	// cvCreateTrackbar is bij mij createTrackbar :s
	createTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 255);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255); //Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);
	while (true){
		cvtColor(image, imgHSV, COLOR_BGR2HSV); //Convert the captured frame from BGR to HSV

		inRange(imgHSV, Scalar(iLowH, iLowS, iLowV), Scalar(iHighH, iHighS, iHighV), imgThresholded);

		//morphological opening (remove small objects from the foreground)
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));

		//morphological closing (fill small holes in the foreground)
		dilate(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		erode(imgThresholded, imgThresholded, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));
		imshow("Thresholded Image", imgThresholded); //show the thresholded image


		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

}

/*
Filtert de lucht en grote objecten uit een afbeelding
*/
Mat skyFilter(Mat & image){
	//deze waarde heeft aan vanaf welke grootte iets weg moet gefilterd worden
	int ksize = 45;
	Mat blur = makeHSVfilter(image,0,255,0,255,255,255);

	//remove bigobjects
	medianBlur(blur, blur, ksize);
	dilate(blur, blur, getStructuringElement(MORPH_ELLIPSE, Size(ksize*(3 / 2), ksize * (3 / 2))));
	return blur;
}

/*
Creëert een filter die alles wegfilter behalve de baan die recht voor de auto ligt.
*/
Mat Analyse::noiseFilter(Mat image,int noise){
	Mat blur = skyFilter(image);
	blur.rowRange(0, (image.rows) / 2) = noise;

	///** Create some points */
	Point rook_points[1][6];
	int w = image.cols;
	int h = image.rows;
	rook_points[0][0] = Point(w-(w/3),h/2);
	rook_points[0][1] = Point(w,h/2);
	rook_points[0][2] = Point(w,3*h/4);
	int npt[] = { 3 };
	const Point* ppt[1] = { rook_points[0] };
	fillPoly(blur, ppt, npt, 1, Scalar(noise), 8);

	rook_points[0][0] = Point(w / 3, h / 2);
	rook_points[0][1] = Point(0, h / 2);
	rook_points[0][2] = Point(0, 3*h/4);
	fillPoly(blur, ppt, npt, 1, Scalar(noise), 8);
	//imshow("filter", blur);

	return blur;
}

/*
Methode gebaseert op de lijnmarkeringen
*/
void Analyse::findRoadMarkings(Mat &image) {	
	//bepaalt N beste lijnen
	int iLowV = 255;

	Mat imgThresholded;
	//tryFilter(image);
	std::vector<Vec2f> lines;
	Mat FilterBigNoise = noiseFilter(image,255);
	Mat output = image.clone();
	while (lines.size() < 5 && iLowV > 0){
		//std::cout << "matches " << handler.Matches() << std::endl;

		imgThresholded = makeHSVfilter(output,0, 75, 0, 75, iLowV, 255);
		//imshow("thres", imgThresholded);
		imgThresholded = imgThresholded - FilterBigNoise;
		medianBlur(imgThresholded, imgThresholded, 1);

		//imshow("result", imgThresholded);

		HoughLines(imgThresholded, lines, 1, CV_PI / 180, 75);
		iLowV -=10;
	}

	lh.addLines(lines, output);

	//imshow("filter applied", imgThresholded);


}
 
//-------------------------------------------
/*
TESTFASE
Methode gebasseerd op het verwijderen van de schaduw
*/
Mat removeShadowCountour(Mat &src){
	//0. Source Image
	//imshow("src", src);

	Analyse analyse;
	Mat noiseFilter = analyse.noiseFilter(src,255);

	Mat road = makeHSVfilter(src, 2, 178, 0, 77, 85, 184);
	//imshow("road", road);
	Mat shadow = makeHSVfilter(src, 91, 140, 6,136, 28, 109) - noiseFilter;
	//imshow("shadow", shadow);
	//tryFilter(src);
	Mat fullroad = (road + shadow) - noiseFilter;

	//imshow("road", road); //show the thresholded image
	shadow = shadow - road;

	double_t thres1 = 180;
	double_t thres2 = 120;
	Canny(shadow,shadow, thres1, thres2);
	dilate(shadow, shadow, getStructuringElement(MORPH_ELLIPSE, Size(10, 10)));
	//imshow("shadow", shadow); 
	//imshow("better shadow", shadow - road);
	
	//imshow("both", fullroad);




	Mat contours;
	Mat imgGRAY;
	cvtColor(src, imgGRAY, CV_RGB2GRAY);
	imgGRAY = imgGRAY - noiseFilter;
	medianBlur(imgGRAY, imgGRAY, 5);
	Canny(imgGRAY, contours, thres1, thres2);
	
	//imshow("resultshad", contours - shadow);
	dilate(noiseFilter, noiseFilter, getStructuringElement(MORPH_ELLIPSE, Size(5, 5)));


	return contours - (shadow-road) - noiseFilter;
}

/*
TESTFASE
contours zoeken zonder de invloed van schaduw
*/
void Analyse::findEdges(Mat &image){
	Mat contours = removeShadowCountour(image);
	//imshow("countours", contours);


	std::vector<Vec2f> lines;
	int houghVote = 200;
	while (lines.size() < 5 && houghVote > 0){
		HoughLines(contours, lines, 1, CV_PI / 180, houghVote);
		houghVote -= 5;
	}

	lh.addLines(lines, image);

}

/*
hulpmethode om hoek te zoeken
*/
static double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0)
{
	double dx1 = pt1.x - pt0.x;
	double dy1 = pt1.y - pt0.y;
	double dx2 = pt2.x - pt0.x;
	double dy2 = pt2.y - pt0.y;
	return (dx1*dx2 + dy1*dy2) / sqrt((dx1*dx1 + dy1*dy1)*(dx2*dx2 + dy2*dy2) + 1e-10);
}

/*
testfase
zoeken van squares
*/
void findShapes(Mat& src){
	// Convert to grayscale
	cv::Mat gray;
	cv::cvtColor(src, gray, CV_BGR2GRAY);

	// Convert to binary image using Canny
	cv::Mat bw;
	cv::Canny(gray, bw, 0, 50, 5);
	medianBlur(bw, bw, 1);

	imshow("bw", bw);
	// Find contours
	std::vector<std::vector<cv::Point> > contours;
	cv::findContours(bw.clone(), contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	cout << "contours size " << contours.size() << endl;
	vector<Point> approx;
	vector<vector<Point> > squares;
	for (size_t i = 0; i < contours.size(); i++)
	{
		approxPolyDP(Mat(contours[i]), approx,
			arcLength(Mat(contours[i]), true)*0.02, true);

		// &&		fabs(contourArea(Mat(approx))) > 1000 &&			isContourConvex(Mat(approx)))
		if (approx.size() == 4 && fabs(contourArea(Mat(approx))) > 50 && fabs(contourArea(Mat(approx))) < 300 && isContourConvex(Mat(approx)))
		{
			cout << "algo" << endl;
			double maxCosine = 0;

			for (int j = 2; j < 5; j++)
			{
				double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
				maxCosine = MAX(maxCosine, cosine);
			}

			//if (maxCosine < 0.3)
				squares.push_back(approx);
		}
	}
	cout << "squares size " << squares.size() << endl;
	vector<vector<Point> >::iterator it = squares.begin();
	while (it != squares.end()){
		polylines(src, *it, true, Scalar(255, 0, 0), 2);
		Rect r = cv::boundingRect(*it);
		rectangle(src, r, Scalar(0, 0, 255), 3);
		//cout <<"points: " << *it << endl;
		it++;
	}
	
}

/*
testfase
zoeken van squares andere methode
*/
void find_squares(Mat& image, vector<vector<Point> >& squares)
{
	// blur will enhance edge detection
	Mat blurred(image);
    medianBlur(image, blurred, 9);

	Mat gray0(blurred.size(), CV_8U), gray;
	vector<vector<Point> > contours;

	// find squares in every color plane of the image
	for (int c = 0; c < 3; c++)
	{
		int ch[] = { c, 0 };
		mixChannels(&blurred, 1, &gray0, 1, ch, 1);

		// try several threshold levels
		const int threshold_level = 2;
		for (int l = 0; l < threshold_level; l++)
		{
			// Use Canny instead of zero threshold level!
			// Canny helps to catch squares with gradient shading
			if (l == 0)
			{
				Canny(gray0, gray, 10, 20, 3); // 

				// Dilate helps to remove potential holes between edge segments
				dilate(gray, gray, Mat(), Point(-1, -1));
			}
			else
			{
				gray = gray0 >= (l + 1) * 255 / threshold_level;
			}

			// Find contours and store them in a list
			findContours(gray, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);

			// Test contours
			vector<Point> approx;
			for (size_t i = 0; i < contours.size(); i++)
			{
				// approximate contour with accuracy proportional
				// to the contour perimeter
				approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true)*0.02, true);

				// Note: absolute value of an area is used because
				// area may be positive or negative - in accordance with the
				// contour orientation
				if (approx.size() == 4 &&
					fabs(contourArea(Mat(approx))) > 50 && fabs(contourArea(Mat(approx))) < 150 &&
					isContourConvex(Mat(approx)))
				{
					double maxCosine = 0;

					for (int j = 2; j < 5; j++)
					{
						double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
						maxCosine = MAX(maxCosine, cosine);
					}

					//if (maxCosine < 0.3)
						squares.push_back(approx);
				}
			}
		}
	}
}

/*
onderzoek naar alle verticale lijnen
*/
void Analyse::findVerticalLines(Mat& image){
	Mat  ver;
	Mat gray;
	cvtColor(image, gray, CV_RGB2GRAY);
	Sobel(gray, ver, CV_32F, 1, 0);
	ver = abs(ver) / 250;
	Mat res;
	ver.convertTo(res, CV_8U, 255, 0);
	//imshow("before", res);
	inRange(res, Scalar(230), Scalar(255), res);
	medianBlur(res, res, 3);
	
	std::vector<Vec2f> lines;
	Analyse analyse;
	Mat FilterBigNoise = analyse.noiseFilter(image, 255);
	res = res - FilterBigNoise;
	//imshow("ver", res);
	HoughLines(res, lines, 1, CV_PI / 180, 30);

	lh.addLines(lines, image);


	//imshow("filt", res);
	//// Draw the limes
	//std::vector<Vec2f>::const_iterator it = lines.begin();
	////Mat hough(contours.size(), CV_8U, Scalar(0));
	//while (it != lines.end()) {

	//	float rho = (*it)[0];   // first element is distance rho
	//	float theta = (*it)[1]; // second element is angle theta

	//	Point pt1(rho / cos(theta), 0);
	//	// point of intersection of the line with last row
	//	Point pt2((rho - res.rows*sin(theta)) / cos(theta), res.rows);
	//	// draw a white line
	//	line(res, pt1, pt2, Scalar(255), 2);
	//	++it;
	//}
	//imshow("re", res);
}


void Analyse::printResult(Mat & image){
	const int N = 3;
	//lh.drawNBestLines(image, N);
	lh.fillBestLines(image);
	imshow("Result", image);
}




/*
voert alle methodes uit op de foto's
*/
void Analyse::processimage(Mat& image){
	//imshow("before", image);

	Analyse analyse;
	//imshow("result1", analyse.findEdgeLines(image));
	
	analyse.findRoadMarkings(image);

	//imwrite("./results/"+name, result2);

	analyse.findEdges(image);

	

	analyse.findVerticalLines(image);

	analyse.printResult(image);
	waitKey(0);
}