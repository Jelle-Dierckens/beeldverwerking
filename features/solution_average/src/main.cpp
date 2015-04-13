#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "fstream"
using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
    cout<<argv[0]<<": "<<argc-2<<" images to process"<<endl;
    if (argc <3) {
        cout<<"args: outputfilename  inputfiles\n";
        return 255;
    }
    Mat outputImage;
    Mat inputImage;
    ifstream instream(argv[1]);
    if(instream) {
        char answer;
        while (answer!='y' && answer!='n' && answer!='j') {
            cout<<answer;
            cout<<"file exists! overwrite? y/n\n";
            cin>>answer;
            if(answer=='n') {
                cout<<"aborted\n";
                return 0;
            }
        }
    }else {
        cout<<"error in outputfile\n";
        return 253;
    }
    ofstream outfilestream(argv[1]);
    if(!outfilestream) {
        cout<<"file could not be created!"<<endl;
        return 254;
    }
    inputImage=imread(argv[2]);
    Mat frame32f;
    outputImage=Mat::zeros(inputImage.rows, inputImage.cols, CV_32FC3);
    inputImage.convertTo(frame32f, CV_32FC3);
    outputImage+=(frame32f/(argc-2));
    for (int i=3; i<argc; i++) {
        inputImage=imread(argv[i]);
        inputImage.convertTo(frame32f, CV_32FC3);
        resize(frame32f, frame32f, outputImage.size());
        outputImage+=(frame32f/(argc-2));
    }
    namedWindow("result");
    Mat channels[3];
    normalize(outputImage, outputImage, 0,1, NORM_MINMAX);
    split(outputImage, channels);
    channels[0]*=(256*256);
    channels[0].convertTo(channels[0], CV_16U);
    imshow("result", channels[0]);
    imwrite(argv[1], channels[0]);

    waitKey(0);
}

