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
#include "chrono.h"
#include <math.h>
#include <vector>
#include <fstream>

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
		cout << "Reading the Bayes matrix...";
		//Mat bayes = readBayes();
		cout << " done." << endl;
		cout << "Reading the image for the average ground truth...";
		//Mat gem = readAvgTruth();
		cout << " done." << endl;
		cout << "Preparing probability images:" << endl;
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			String name = ent->d_name;
			if (name.find(".png") < name.length()){
				if (name != "uu_000067.png"){
					printf("\t%s\n", ent->d_name);
					String filename = directory + ent->d_name;
					//cout << filename << endl;
					Mat image = imread(filename, IMREAD_COLOR); // Read the file
					Analyse analyse;
					//compareBayes(image, bayes,gem);
					//writeProb(image, name);
					analyse.processimage(image, name);
				}
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
Cre�ert een filter die alles wegfilter behalve de baan die recht voor de auto ligt.
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
	const int N = 2;
	Mat cpy;
	image.copyTo(cpy);
	//lh.drawNBestLines(cpy, N);
	lh.fillBestLines(cpy);
	imshow("Result", cpy);

	waitKey(0);

}

Mat Analyse::watershedImage(Mat & image, vector<Line>& lines){

	//initialize
	Mat markerMask, img0;
	Mat imgGray;


	image.copyTo(img0);
	cvtColor(image, markerMask, COLOR_BGR2GRAY);
	cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);
	markerMask = Scalar::all(0);
	//imshow("image", image);

	//add markers
	
	//vector<Line> lines =lh.findNBestLines(2);
	if (lines.size() == 2){
		double x = (lines[0].b() - lines[1].b()) / (lines[1].a() - lines[0].a());
		double y = (lines[0].a() * x) + lines[0].b();

		//cout << lines[0].pt1 <<" "<< lines[0].pt2 << endl;
		//cout << lines[1].pt1 << " " << lines[1].pt2 << endl;
		//cout << x << "," << y << endl;
		//cout << markerMask.rows << "," << markerMask.cols << endl;

	




		//x -= 20;
		/*circle(markerMask, Point(x, y), y-10, Scalar(255), 1);
		line(markerMask, Point(x, y), lines[0].pt2, Scalar(255), 1, 8, 0);*/
		//line(markerMask, Point(x, y), lines[1].pt2, Scalar(255), 1, 8, 0);
		int dikte = 20;
		int inshift = 40;

		vector<Point> points;
		points.push_back(Point(x, y+2));
		if (lines[0].pt2.x < lines[1].pt2.x){
			line(markerMask, Point(x - dikte, y), Point(lines[0].pt2.x - dikte, lines[0].pt2.y), Scalar(255), 1, 8, 0);
			line(markerMask, Point(x + dikte, y), Point(lines[1].pt2.x + dikte, lines[1].pt2.y), Scalar(255), 1, 8, 0);
			points.push_back(Point(lines[0].pt2.x + inshift, lines[0].pt2.y));
			points.push_back(Point(lines[1].pt2.x - inshift, lines[1].pt2.y));
		}
		else{
			line(markerMask, Point(x + dikte, y), Point(lines[0].pt2.x + dikte, lines[0].pt2.y), Scalar(255), 1, 8, 0);
			line(markerMask, Point(x - dikte, y), Point(lines[1].pt2.x - dikte, lines[1].pt2.y), Scalar(255), 1, 8, 0);
			points.push_back(Point(lines[0].pt2.x - inshift, lines[0].pt2.y));
			points.push_back(Point(lines[1].pt2.x + inshift, lines[1].pt2.y));
		}
		line(markerMask, Point(x - dikte, y), Point(x + dikte, y), Scalar(255), 1, 8, 0);

		fillConvexPoly(markerMask, points, Scalar(255));

		
		
		//imshow("markerMask", markerMask);


		//watershed

		int i, j, compCount = 0;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(markerMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);


		Mat markers(markerMask.size(), CV_32S);
		markers = Scalar::all(0);
		int idx = 0;
		for (; idx >= 0; idx = hierarchy[idx][0], compCount++)
			drawContours(markers, contours, idx, Scalar::all(compCount + 1), -1, 8, hierarchy, INT_MAX);


		vector<Vec3b> colorTab;
		colorTab.push_back(Vec3b(255, 0, 0));
		colorTab.push_back(Vec3b(0, 255, 0));
		for (i = 2; i < compCount; i++)
		{
			int b = theRNG().uniform(0, 255);
			int g = theRNG().uniform(0, 255);
			int r = theRNG().uniform(0, 255);

			colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
		}

		//double t = (double)getTickCount();
		//imshow("markers", markers);
		watershed(image, markers);
		//t = (double)getTickCount() - t;
		//printf("execution time = %gms\n", t*1000. / getTickFrequency());

		Mat wshed(markers.size(), CV_8UC3);

		// paint the watershed image
		for (i = 0; i < markers.rows; i++)
			for (j = 0; j < markers.cols; j++)
			{
			int index = markers.at<int>(i, j);
			if (index == -1)
				wshed.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
			else if (index <= 0 || index > compCount)
				wshed.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
			else
				wshed.at<Vec3b>(i, j) = colorTab[index - 1];
			}

		wshed = wshed*0.5 + imgGray*0.5;
		//imshow("watershed transform", wshed);
		return wshed;
	}
	

}

vector<Line> Analyse::waterFilter(Mat & image){
	//initialize
	Mat markerMask;
	Mat imgGray;
	double ldegree = 30;
	double rdegree = 30;
	Mat output(image.size(), CV_8UC1);
	std::vector<Vec2f> lines;


	int lanewidth=image.cols;
	Point picmid(image.cols / 2, 0);
	Point mid(image.cols / 2, 0);
	double lradian = -ldegree *(PI / 180);
	double rradian = -rdegree *(PI / 180);
	double la = tan(lradian);
	double ra = tan(rradian);
	double lb = mid.y - (mid.x*la);
	double rb = mid.y - (mid.x*ra);

	Point p0(0, (double)-la*mid.x + lb);
	Point p1(image.cols, (double)-ra*mid.x + rb);

	//circle(image, Point(mid.x, image.rows), 5, Scalar(255), 5);


	int opp;
	for(int g =0 ; g<2;g++){
		Mat rescont(image.size(), CV_8UC3);
		cvtColor(image, markerMask, COLOR_BGR2GRAY);
		cvtColor(markerMask, imgGray, COLOR_GRAY2BGR);
		markerMask = Scalar::all(0);


		line(markerMask, mid, p0, Scalar(255), 3);
		line(markerMask, mid, p1, Scalar(255), 3);

		circle(markerMask, Point(mid.x, image.rows),75, Scalar(255), 5);
		line(markerMask, Point(mid.x,300), Point(mid.x,180), Scalar(255), 5);
		//imshow("imagemask", markerMask);


		int i, j, compCount = 0;
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;

		findContours(markerMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);


		Mat markers(markerMask.size(), CV_32S);
		markers = Scalar::all(0);
		int idx = 0;
		for (; idx >= 0; idx = hierarchy[idx][0], compCount++)
			drawContours(markers, contours, idx, Scalar::all(compCount + 1), -1, 8, hierarchy, INT_MAX);

		watershed(image, markers);

		



		// paint the watershed image
		 opp = 0;
		for (i = 0; i < markers.rows; i++){
			for (j = 0; j < markers.cols; j++){
				if (markers.at<int>(i, j) == -1){
					rescont.at<Vec3b>(i, j) = Vec3b(255, 255, 255);
				}
				if (markers.at<int>(i, j) == 1){
					opp++;
				}
			}
		}
		
		//cout <<"opp: "<< opp << endl;


		cvtColor(rescont, output, CV_RGB2GRAY);
		inRange(output, 220, 255, output);
		//imshow("rescont", output);



		int index = 0;
		vector<Point> lanepoints;
		lanepoints.clear();
		//cout << output.rows<<"," << output.cols << endl;
		for (int i = 1; i < image.cols-2; i++){
			if (output.at<uchar>(image.rows-2,i) == 255){
				lanepoints.push_back(Point(i, image.rows - 2));
			}
		}

		lanewidth = lanepoints[1].x - lanepoints[0].x;
		Point lanemid(lanepoints[0].x + (lanewidth / 2), mid.y);
		
		//cout << "baan breedte: " << lanewidth << endl;



		double buffer = 10;
		double shift=0;
		if (abs(picmid.x - lanemid.x) > 20){
		if (picmid.x> lanemid.x){
			shift = -10;
		}
		else {
			shift =10;
		}
		}

		p0 = Point(lanepoints[0].x - buffer + shift, lanepoints[0].y);
		p1 = Point(lanepoints[1].x + buffer + shift, lanepoints[0].y);
		mid.x += shift;
		
	}

	//imshow("output", output);


		//circle(image, mid, 5, Scalar(0, 255, 0), 5);

		//circle(image, p0, 5, Scalar(0,0,255), 5);
		//circle(image, p1, 5, Scalar(0, 0, 255), 5);
		//circle(image, Point(mid.x,image.rows), 5, Scalar(0, 255, 0), 5);


		Mat ver(image.size(), CV_8UC1);
		Sobel(output, ver, CV_8UC1, 1, 0);
		//imshow("horizontal away", ver);
		

		LineFinder lf;
		lf.setLineLengthAndGap(15, 20);
		lf.setMinVote(30);
		lf.findLines(ver);
		lf.drawDetectedLines(ver, Scalar(255));
		//imshow("rescontss", ver);
		HoughLines(ver, lines, 1, CV_PI / 180, 75);
		lh.addLines(lines, image);

		return controle(image, p0, mid, p1,opp);

	
}

vector<Line> Analyse::controle(Mat & image, Point b, Point m, Point l, int opp){
	vector<Line> lines = lh.findNBestLines(2);
	Point middle(image.cols / 2, image.rows);
	int interval = 95;

	if (lines.size() == 2){
		bool error = false;
		double linewidth = abs(lines[0].pt2.x - lines[1].pt2.x);
		cout << "2lines" << endl;
		if (opp < 30000){
			cout << "error opp to little" << endl;
			error = true;
		}
		if (lines[0].pt2.x > lines[1].pt2.x){
			Line h = lines[1];
			lines[1] = lines[0];
			lines[0] = h;
		}
		
		if (abs(b.x - lines[0].pt2.x) > interval){
			cout << "error begin point " << endl;
			error = true;

		}
		if (abs(l.x - lines[1].pt2.x) > interval){
			cout << "error end point " << endl;
			error = true;
		}
		if (linewidth > 700){
			error = true;
			cout << "dubbele road detection" << endl;
			/*Point mid(lines[0].pt2.x + (lines[1].pt2.x - lines[0].pt2.x) / 2, lines[0].pt2.y);
			line(image, mid, Point(mid.x, lines[0].a()*mid.x + lines[0].b()), Scalar(210, 0, 0), 2);*/
			if (abs(b.x - lines[0].pt2.x) < interval){
				//spiegel beginline
				//line(image,l, Point(m.x, lines[0].a()*m.x + lines[0].b()), Scalar(210, 0, 0), 10);
				lines[1] = Line(Point(m.x, lines[0].a()*m.x + lines[0].b()),l);
			
			}
			else if (abs(l.x - lines[1].pt2.x) < interval){
				//spiegel endline
				//line(image, b, Point(m.x, lines[1].a()*m.x + lines[1].b()), Scalar(210, 0, 0), 10);
				lines[0] = Line( Point(m.x, lines[1].a()*m.x + lines[1].b()),b);
			}
			//waitKey(0);
		}
		if (!error){
			//waitKey(0);
		}
		
	}
	else if (lines.size() == 1){
		cout << "1line" << endl;
		
		if (abs(b.x - lines[0].pt2.x) < interval){
			//spiegel beginline
			//line(image, Point(m.x + abs(lines[0].pt2.x - m.x), image.rows), Point(m.x, lines[0].a()*m.x + lines[0].b()), Scalar(210, 0, 0), 2);
			lines.push_back(Line(Point(m.x, lines[0].a()*m.x + lines[0].b()), Point(m.x + abs(lines[0].pt2.x - m.x), image.rows)));
		}
		else if (abs(l.x - lines[0].pt2.x) < interval){
			//spiegel endline
			//line(image, Point(m.x - abs(lines[0].pt2.x - m.x), image.rows), Point(m.x, lines[0].a()*m.x + lines[0].b()), Scalar(210, 0, 0), 2);
			lines.push_back(lines[0]);
			lines[0] = Line(Point(m.x, lines[0].a()*m.x + lines[0].b()), Point(m.x - abs(lines[0].pt2.x - m.x), image.rows));
		}
		else {
			cout << "not repaireable" << endl;
		}
	}

	return lines;
}

void Analyse::findColor(Mat &image, Mat &mask){
	Point center = Point(image.size().width/2, image.size().height);
	//cout << center << endl;
	//ellipse(image, center, Size(3, 3), 0, 0, 360, Scalar(255,255,255), 3);
	int rect_width = 40, rect_height = 20;
	Point rect_point = Point(center.x - rect_width/2 / 2, center.y-rect_height*3/2);	// *3/2 is om zeker de eventuele zwarte rand onderaan niet mee te nemen 
	Rect rect(rect_point.x, rect_point.y, rect_width, rect_height);

	Mat cpy = image.clone();
	Mat out = image.clone();
	Mat roi = Mat(cpy, rect);

	Scalar m = mean(roi);
	rectangle(cpy, rect, m, CV_FILLED);
	rectangle(cpy, rect, Scalar(255, 255, 255));
	imshow("Color", cpy);

	int diff = 40;
	Scalar lowerb = Scalar(m[0] - diff, m[1] - diff, m[2] - diff, m[3]);
	Scalar upperb = Scalar(m[0] + diff, m[1] + diff, m[2] + diff, m[3]);
	inRange(out, lowerb, upperb, mask);
	cout << out.dims << endl;
	imshow("mask", mask);
	Mat filt;
	mask.copyTo(filt);
	
	int N = 20, lighter = 130;
	double thres = 0.8;
	int i = mask.rows-1;
	while (i >= 0){
		int j = mask.cols/2;
		while (j >= 0 && isWhite(mask,j,i,N,N,0.8)){
			j--;
		}
		while (j >= 0){
			mask.at<uchar>(Point(j,i)) = 0;
			out.at<uchar>(Point(j * 3, i)) += ((out.at<uchar>(Point(j * 3, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3, i))));
			out.at<uchar>(Point(j * 3 + 1, i)) += ((out.at<uchar>(Point(j * 3 + 1, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3 + 1, i))));
			out.at<uchar>(Point(j * 3 + 2, i)) += ((out.at<uchar>(Point(j * 3 + 2, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3 + 2, i))));
			j--;
		}
		j = mask.cols/2;
		while (j < mask.cols && isWhite(mask, j, i, N,N,0.8)){
			j++;
		}
		while (j < mask.cols){
			mask.at<uchar>(Point(j, i)) = 0;
			out.at<uchar>(Point(j * 3, i)) += ((out.at<uchar>(Point(j * 3, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3, i))));
			out.at<uchar>(Point(j * 3 + 1, i)) += ((out.at<uchar>(Point(j * 3 + 1, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3 + 1, i))));
			out.at<uchar>(Point(j * 3 + 2, i)) += ((out.at<uchar>(Point(j * 3 + 2, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3 + 2, i))));
			j++;
		}
		i--;
	}

	int ksize = 3;
	//medianBlur(mask, filt, ksize);
	Mat tex;
	lbp(image, tex);
	bitwise_and(filt, 255-tex, filt);

	//for (int i = 0; i < mask.rows; i++){
	//	for (int j = 0; j < mask.cols; j++){
	//		if (isWhite(mask, j, i, N, N, thres)){
	//			filt.at<uchar>(Point(j, i)) = 255;
	//		}else{
	//			//filt.at<uchar>(Point(j, i)) = 0;
	//			out.at<uchar>(Point(j * 3, i)) += ((out.at<uchar>(Point(j * 3, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3, i))));
	//			out.at<uchar>(Point(j * 3 + 1, i)) += ((out.at<uchar>(Point(j * 3 + 1, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3 + 1, i))));
	//			out.at<uchar>(Point(j * 3 + 2, i)) += ((out.at<uchar>(Point(j * 3 + 2, i)) < 255 - lighter) ? lighter : (255 - out.at<uchar>(Point(j * 3 + 2, i))));
	//		}
	//	}
	//}
	imshow("filtered mask", filt);
	imshow("Color Detection", out);
}

bool Analyse::isWhite(const Mat &mask, int x, int y, int dx, int dy, double threshold){
	int black = 0, white = 0;
	for (int i = x - dx / 2; i <= x + dx / 2; i++){
		for (int j = y - dy / 2; j <= y + dy / 2; j++){
			if (i >= 0 && i < mask.cols && j >= 0 && j < mask.rows){
				if (mask.at<uchar>(Point(i, j)) != 0){
					white++;
				}
				else{
					black++;
				}
			}
		}
	}
	return (white >= (white+black)*threshold);
}

void Analyse::lbp(const Mat & image, Mat & dst, int radius, double threshold_fraction){
	Mat grey;
	cvtColor(image, grey, COLOR_RGB2GRAY);
	grey.copyTo(dst);
	double minValue, maxValue;
	minMaxLoc(grey, &minValue, &maxValue);
	uchar thres = threshold_fraction * (maxValue-minValue);
	//dst = Mat::zeros(image.rows - 2, image.cols - 2, image.depth());
	for (int i = radius; i < grey.rows - radius; i++){
		for (int j = radius; j < grey.cols - radius; j++){
			uchar center = grey.at<uchar>(i, j);
			uchar points[8];
			points[0] = grey.at<uchar>(i - radius, j - radius) >= center + thres ? 1 : 0;
			points[1] = grey.at<uchar>(i - radius, j) >= center + thres ? 1 : 0;
			points[2] = grey.at<uchar>(i - radius, j + radius) >= center + thres ? 1 : 0;
			points[3] = grey.at<uchar>(i, j - radius) >= center + thres ? 1 : 0;
			points[4] = grey.at<uchar>(i, j + radius) >= center + thres ? 1 : 0;
			points[5] = grey.at<uchar>(i + radius, j - radius) >= center + thres ? 1 : 0;
			points[6] = grey.at<uchar>(i + radius, j) >= center + thres ? 1 : 0;
			points[7] = grey.at<uchar>(i + radius, j + radius) >= center + thres ? 1 : 0;
			uchar mult[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
			center = 0;
			for (int k = 0; k < 8; k++){
				center += points[k] * mult[k];
			}
			dst.at<uchar>(i, j) = center;
		}
	}
}

vector<float> Analyse::devideInBins(Mat patch, int bincount){
	vector<float> bins;
	bins.resize(bincount, 0);
	int binwidth = 255 / bincount;
	for (int r = 0; r < patch.rows; r++){
		for (int c = 0; c < patch.cols; c++){
			bins[(patch.at<uchar>(r, c)*bincount / 255)%bincount]++;
		}
	}
	return bins;
}

void Analyse::findTextures(Mat &image){
	Mat cpy;
	//image.copyTo(cpy);
	lbp(image, cpy,0.02);
	imshow("LBP", cpy);

}

Mat Analyse::readBayes(){
	string bayes_name = "C:\\Beeldverwerking\\beeldverwerking\\ProjectX\\ProjectX\\bayes.csv";
	ifstream bayes(bayes_name);
	int dim = 100;
	int dims[3] = { dim, dim, dim };
	Mat hls_bayes(3, dims, CV_32F);

	char delim = ';';
	std::string x, y, z, value;
	std::string inputline;
	std::getline(bayes, inputline);
	while (!bayes.eof()) {
		std::stringstream stringstr(inputline);
		//std::cout<<inputline<<"\n";
		std::getline(stringstr, x, delim);
		std::getline(stringstr, y, delim);
		std::getline(stringstr, z, delim);
		std::getline(stringstr, value, delim);
		hls_bayes.at<float>(atoi(x.c_str()), atoi(y.c_str()), atoi(z.c_str())) = std::stof(value);
		std::getline(bayes, inputline);
	}
	bayes.close();
	return hls_bayes;
}

Mat Analyse::readAvgTruth(){
	String gem_name = "C:\\Beeldverwerking\\beeldverwerking\\ProjectX\\ProjectX\\outputImage.png";
	Mat gem = imread(gem_name);
	//imshow("gem", gem);
	return gem;
}
Mat Analyse::compareBayes(Mat &image, Mat &hls_bayes, Mat &gem){

	resize(gem, gem, Size(image.cols, image.rows));
	GaussianBlur(image, image, Size(3, 3), 0);
	cvtColor(image, image, COLOR_BGR2HLS);
	int xi, yi, zi;
	for (int i = 0; i<image.rows; i++) {
		for (int j = 0; j<image.cols; j++) {
			xi = image.at<Vec3b>(i, j)[0];
			yi = image.at<Vec3b>(i, j)[1];
			zi = image.at<Vec3b>(i, j)[2];
			float a, b;
			//a=1-(std::max(std::min(0.8,gem.at<Vec3b>(i,j)[0]/256.0),0.2));
			a = 1 - gem.at<Vec3b>(i, j)[0] / 256.0;
			//b=1-2.5*hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56);
			/*if(hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56)>0.2f) {
			b=0;
			}
			else {
			b=256;
			}*/
			b = 1 - hls_bayes.at<float>(xi / 2.56, yi / 2.56, zi / 2.56);
			if (a<0.5 && b<0.8) {
				image.at<Vec3b>(i, j)[1] = 0;
			}
			else if (a >= 0.5 && b >= 0.8) {
				image.at<Vec3b>(i, j)[1] = 255;
			}
			else {
				image.at<Vec3b>(i, j)[1] = 125;
			}

			//image.at<Vec3b>(i,j)[1]=256*((a*b)/(a+b));
		}
	}
	//namedWindow("result");
	cvtColor(image, image, COLOR_HLS2BGR);
	//imshow("result", image);
	//waitKey(0);
	return image;
}

void Analyse::writeProb(Mat & image, String name){
	String dir = "C:\\Beeldverwerking\\beeldverwerking\\ProjectX\\ProjectX\\prob\\";
	dir += name;
	imwrite(dir, image);

}

/*
leest alle foto's en zoekt vakjes die zeker weg zijn volgens bayes (zie jelle voor meer info)
*/
void Analyse::lbpOnProb(){
	DIR *dir;
	struct dirent *ent;

	//const char* diropen = directory.c_str();
	char* diropen = "C:\\Beeldverwerking\\beeldverwerking\\ProjectX\\ProjectX\\prob\\";
	char* dirori = "C:\\Beeldverwerking\\data_road\\training\\image_2\\";
	String directory = diropen;
	if ((dir = opendir(diropen)) != NULL) {
		/* print all the files and directories within directory */
		while ((ent = readdir(dir)) != NULL) {
			String name = ent->d_name;
			if (name.find(".png") < name.length()){
				//if (name != "uu_000067.png"){
					printf("\t%s\n", ent->d_name);
					String filename = directory + ent->d_name;
					//cout << filename << endl;
					cout << dirori + name << endl;
					Mat image = imread(filename, IMREAD_COLOR); // Read the probability file
					Mat lbp_image = imread(dirori + name,IMREAD_COLOR); // Read the original file
					imshow("Original", lbp_image);
					Analyse analyse;
					const int PATCH_SIZE = 20, VECTOR_SIZE = 5, BIN_COUNT = 50;
					vector<Point2i> patches;
					analyse.findBlackPatches(image, PATCH_SIZE, VECTOR_SIZE, patches);
					Mat textures;
					analyse.compareHistograms(lbp_image, textures, patches, PATCH_SIZE, 0.2);
					analyse.lbp(lbp_image, lbp_image, 2, 0.02);
					imshow("LBP", lbp_image);
					 // test
					/*for each (Point2i patch in patches){
						cout << patch << endl;
						rectangle(image, patch, Point(patch.x + PATCH_SIZE, patch.y + PATCH_SIZE), Scalar(255, 255, 255));
						vector<int> bins = devideInBins(lbp_image(Rect(patch.x, patch.y, PATCH_SIZE, PATCH_SIZE)), BIN_COUNT);
						for (int i = 0; i < bins.size(); i++){
							cout << i * 255 / BIN_COUNT << "-" << (i+1) * 255 / BIN_COUNT - 1 << ": " << bins[i] << endl;
						}
						cout << endl << endl;
					}*/
					vector<float> bins_patch = devideInBins(lbp_image(Rect(patches[0].x, patches[0].y, PATCH_SIZE, PATCH_SIZE)), BIN_COUNT);
					vector<float> bins_random = devideInBins(lbp_image(Rect(0, 0, PATCH_SIZE, PATCH_SIZE)), BIN_COUNT);
					double compval = compareHist(bins_patch, bins_random, CV_COMP_KL_DIV);
					cout << "Comparation: " << compval << endl;
					imshow("Patches", image);
					imshow("CompareHist", textures);
					waitKey(0);
					analyse.processimage(image, name);
				//}
			}
		}
		closedir(dir);
	}
}

void Analyse::compareHistograms(Mat src, Mat & dst, vector<Point2i> &patches, int patchsize, double comp_threshold){
	Mat hsv_src;
	Mat hsv_patch;
	src.copyTo(dst);

	cvtColor(src, hsv_src, CV_BGR2HSV);

	Rect patch = Rect(patches[0].x, patches[0].y, patchsize, patchsize);

	hsv_patch = hsv_src(patch);

	int h_bins = 50; int s_bins = 60;
	int histSize[] = { h_bins, s_bins };

	float h_ranges[] = { 0, 180 };
	float s_ranges[] = { 0, 256 };

	const float* ranges[] = { h_ranges, s_ranges };

	int channels[] = { 0, 1 };
	int lighter = 130;

	for (int i = 0; i < src.rows - patchsize; i += patchsize){
		for (int j = 0; j < src.cols - patchsize; j += patchsize){

			Mat hsv_test = hsv_src(Rect(j, i, patchsize, patchsize));

			MatND hist_src;
			MatND hist_test;

			calcHist(&hsv_src, 1, channels, Mat(), hist_src, 2, histSize, ranges, true, false);
			normalize(hist_src, hist_src, 0, 1, NORM_MINMAX, -1, Mat());

			calcHist(&hsv_test, 1, channels, Mat(), hist_test, 2, histSize, ranges, true, false);
			normalize(hist_test, hist_test, 0, 1, NORM_MINMAX, -1, Mat());

			/*
			Mogelijke compare methodes:
			CV_COMP_CORREL
			CV_COMP_CHISQR
			CV_COMP_INTERSECT
			CV_COMP_BHATTACHARYYA
			CV_COMP_HELLINGER
			*/
			int compare_method = CV_COMP_CORREL; 
			double comp_value = compareHist(hist_src, hist_test, compare_method);
			if (comp_value < comp_threshold){
				for (int r = 0; r < patchsize; r++){
					for (int c = 0; c < patchsize; c++){

						dst.at<uchar>(Point((j + c) * 3, i + r)) += ((dst.at<uchar>(Point((j + c) * 3, i + r)) < 255 - lighter) ? lighter : (255 - dst.at<uchar>(Point((j + c) * 3, i + r))));
						dst.at<uchar>(Point((j + c) * 3 + 1, i + r)) += ((dst.at<uchar>(Point((j + c) * 3 + 1, i + r)) < 255 - lighter) ? lighter : (255 - dst.at<uchar>(Point((j + c) * 3 + 1, i + r))));
						dst.at<uchar>(Point((j + c) * 3 + 2, i + r)) += ((dst.at<uchar>(Point((j + c) * 3 + 2, i + r)) < 255 - lighter) ? lighter : (255 - dst.at<uchar>(Point((j + c) * 3 + 2, i + r))));
					}
				}

			}

		}
	}

	rectangle(dst, patch, Scalar(255, 255, 255));

}

void Analyse::findBlackPatches(Mat & image, int patchsize, int vectorsize, vector<Point2i> &patches){
	int x = image.cols;
	int y = image.rows;
	int offset = y / 5;
	// search with decreasing offset
	while (patches.size() < vectorsize && offset > 0){
		// searching lower half of image for patches
		while (patches.size() < vectorsize && x > image.cols / 2){
			while (patches.size() < vectorsize && y > patchsize){
				// checking current patch for being all black
				bool allBlack = true;
				int i = y - patchsize;
				while (allBlack && i <= y){
					int j = x - patchsize;
					while (allBlack && j <= x){
						allBlack = image.at<Vec3b>(i,j)[1] == 0;
						j++;
					}
					i++;
				}
				// add topleft point to all-black-patch-list
				if (allBlack)
					patches.push_back(Point2i(x - patchsize, y - patchsize));
				//--
				y -= offset;
			}
			y = image.rows;
			x -= offset;

		}
		offset /= 2;
		x = image.cols;
		y = image.rows;
	}
}


/*
voert alle methodes uit op de foto's
*/
void Analyse::processimage(Mat& image,string name){
	//imshow("before", image);

	Analyse analyse;
	//imshow("result1", analyse.findEdgeLines(image));
	//imwrite("./results/"+name, result2);
	
	Chrono chr;

	//chr.start();
	//analyse.findRoadMarkings(image);
	//chr.stop();
	//cout << "find roadmarkings " << chr.tijd() << endl;

	
	//chr.start();
	//analyse.findEdges(image);
	//chr.stop();
	//cout << "findedges " << chr.tijd() << endl;


	//chr.start();
	//analyse.findVerticalLines(image);
	//chr.stop();
	//cout << "findverticallines " << chr.tijd() << endl;

	//chr.start();
	//analyse.waterFilter(image);
	//chr.stop();
	//cout << "watershed " << chr.tijd() << endl;

	//chr.start();
	//analyse.findColor(image,Mat());
	//chr.stop();
	//cout << "color " << chr.tijd() << endl;

	//chr.start();
	analyse.findTextures(image);
	//chr.stop();
	//cout << "texture " << chr.tijd() << endl;

	analyse.printResult(image);
	//if (result.dims != 0){
	//	imwrite("./results/" + name, result);
	//}
	
	//waitKey(0);
}