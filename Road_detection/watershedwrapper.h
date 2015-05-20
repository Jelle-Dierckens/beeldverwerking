#ifndef WATERSHEDWRAPPER_H
#define WATERSHEDWRAPPER_H
//#include <opencv2/core/utility.hpp>
#include "opencv2/imgproc.hpp"
//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <vector>
#include <iostream>

using namespace cv;
using namespace std;

class WatershedWrapper
{
public:
    WatershedWrapper(bool _showOutput);
    ~WatershedWrapper();
    bool showOutput;
    Mat runWatershed(Mat image, Mat markerMask);
    bool getShowOutput() const;
    void setShowOutput(bool value);

private:
    vector<Vec3b> colorTab;
};

#endif // WATERSHEDWRAPPER_H
