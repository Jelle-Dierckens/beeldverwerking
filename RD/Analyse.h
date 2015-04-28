#if !defined ANALYSE
#define ANALYSE
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <cv.h>
#include "linehandler.h"
using namespace std;
using namespace cv;
class Analyse{
public:
	void readallpics(String directory);
	void processimage(Mat& image);
	Mat findEdgeLines(Mat image);
	void findEdges(Mat& image);
	void findRoadMarkings(Mat& image);
	Mat findLines(Mat contours,Mat image);
	Mat noiseFilter(Mat image,int noise);
	void findVerticalLines(Mat& image);
	void printResult(Mat & image);
private:
	LineHandler lh;
	
};



#endif