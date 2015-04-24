#include "watershedwrapper.h"

WatershedWrapper::WatershedWrapper(bool _showOutput)
{
    showOutput = _showOutput;
}

WatershedWrapper::~WatershedWrapper()
{

}

bool WatershedWrapper::getShowOutput() const
{
    return showOutput;
}

void WatershedWrapper::setShowOutput(bool value)
{
    showOutput = value;
}

/*
    *Input:
    *   image (8-bit 3-channel image):
    *       De ongewijzigde startafbeelding.
    *   markerMask (32-bit single-channel image):
    *       Afbeelding met markers (zwarte afbeelding met witte markeringen).
    *       Moet single channel zijn dus eerst omzetten bv. cv::cvtColor(markerMask, markerMask, COLOR_RGB2GRAY);
    *       Moet zelfde afmetingen zijn als image.
    * Output:
    *   Op teruggegeven mat is elk gevonden gebied een volgnummer.
    *   Dus elke pixel van gebied 0 heeft waarde 0.
    *   Indien showOutput true is wordt een afbeelding getoond waar de output ingekleurd is.
*/
Mat WatershedWrapper::runWatershed(Mat image, Mat markerMask)
{
    int i, j, compCount = 0;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    findContours(markerMask, contours, hierarchy, RETR_CCOMP, CHAIN_APPROX_SIMPLE);

    if( contours.empty() )
        return Mat();
    Mat markers(markerMask.size(), CV_32S);
    markers = Scalar::all(0);
    int idx = 0;
    for( ; idx >= 0; idx = hierarchy[idx][0], compCount++ )
        drawContours(markers, contours, idx, Scalar::all(compCount+1), -1, 8, hierarchy, INT_MAX);

    if( compCount == 0 )
        return Mat();


    if((int)colorTab.size() < compCount){
        for( i = colorTab.size(); i < compCount; i++ )
        {
            int b = theRNG().uniform(0, 255);
            int g = theRNG().uniform(0, 255);
            int r = theRNG().uniform(0, 255);

            colorTab.push_back(Vec3b((uchar)b, (uchar)g, (uchar)r));
        }
    }


    double t = (double)getTickCount();
    watershed( image, markers );
    t = (double)getTickCount() - t;
    printf( "execution time = %gms\n", t*1000./getTickFrequency() );

    Mat wshed(markers.size(), CV_8UC3);

    // paint the watershed image
    for( i = 0; i < markers.rows; i++ ){
        for( j = 0; j < markers.cols; j++ )
        {
            int index = markers.at<int>(i,j);
            if( index == -1 )
                wshed.at<Vec3b>(i,j) = Vec3b(255,255,255);
            else if( index <= 0 || index > compCount )
                wshed.at<Vec3b>(i,j) = Vec3b(0,0,0);
            else
                wshed.at<Vec3b>(i,j) = colorTab[index - 1];
        }
    }

    if(!wshed.empty() && showOutput){
        imshow( "watershed transform", wshed );
    }

    return markers;
}

