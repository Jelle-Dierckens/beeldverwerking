#if !defined ANALYSE
#define ANALYSE
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
//#include <cv.h>							// Frederick
//~ #include <opencv\cv.h>						// Jens
//~ #include "linehandler.h"
using namespace std;
//using namespace cv;
class Analyse{
public:
	void readallpics(cv::String directory);
	//~ void processimage(cv::Mat& image, string name);
	//~ cv::Mat findEdgeLines(cv::Mat image);
	//~ void findEdges(cv::Mat& image);
	//~ void findRoadMarkings(cv::Mat& image);
	//~ cv::Mat findLines(cv::Mat contours, cv::Mat image);
	//~ cv::Mat noiseFilter(cv::Mat image, int noise);
	//~ void findVerticalLines(cv::Mat& image);
	//~ void printResult(cv::Mat & image);
	//~ cv::Mat watershedImage(cv::Mat & image, vector<Line>& lines);
	//~ vector<Line> waterFilter(cv::Mat & image);
	//~ vector<Line> controle(cv::Mat & image, cv::Point b, cv::Point m, cv::Point l, int opp);
	//~ void findColor(cv::Mat &image, cv::Mat &mask);
	//~ bool isWhite(const cv::Mat &mask, int x, int y, int dx, int dy, double threshold = 0.75);
//~ private:
	//~ LineHandler lh;
	
};



#endif
