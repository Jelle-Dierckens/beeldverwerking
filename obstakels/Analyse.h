#if !defined ANALYSE
#define ANALYSE
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
//using namespace cv;
class Analyse{
public:
	void readallpics(cv::String directory);
	void processimage(cv::Mat& image, string name);
	
};



#endif
