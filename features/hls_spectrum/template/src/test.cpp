#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <fstream>
#include <istream>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
using namespace cv;

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
int main(int argc, char **argv){
    if(argc<3) {
        printf("geef hlsfile gemiddelde afbeeldingen als argument");
        exit(255);
    }
    std::ifstream inputstream(argv[1]);
    int dim=100;
    int dims[3]={dim,dim,dim};
    Mat hls_bayes(3,dims, CV_32F);

    char delim=';';
    std::string x,y,z, value;
    std::string inputline;
    std::getline(inputstream, inputline);
    while(!inputstream.eof()) {
        std::stringstream stringstr(inputline);
        //std::cout<<inputline<<"\n";
        std::getline(stringstr, x, delim);
        std::getline(stringstr, y, delim);
        std::getline(stringstr, z, delim);
        std::getline(stringstr, value, delim);
        hls_bayes.at<float>(atoi(x.c_str()),atoi(y.c_str()),atoi(z.c_str()))=std::stof(value);
        std::getline(inputstream, inputline);
    }
    inputstream.close();
    Mat gem=imread(argv[2]);

    for (int arg=3; arg<argc; arg++) {
        std::cout<<"loading: "<<argv[arg]<<"\n";
        Mat image=imread(argv[arg]);
        resize(gem, gem, Size(image.cols, image.rows));
        GaussianBlur(image,image, Size(3,3),0);
        cvtColor(image, image, COLOR_BGR2HLS);
        int xi,yi,zi;
        for(int i=0; i<image.rows; i++) {
            for (int j=0; j<image.cols; j++) {
                xi=image.at<Vec3b>(i,j)[0];
                yi=image.at<Vec3b>(i,j)[1];
                zi=image.at<Vec3b>(i,j)[2];
                float a,b;
                //a=1-(std::max(std::min(0.8,gem.at<Vec3b>(i,j)[0]/256.0),0.2));
                //a=1-gem.at<Vec3b>(i,j)[0]/256.0;
                b=1-hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56);
                //image.at<Vec3b>(i,j)[1]=256*b;//;((a*b)/(a+b));
               if(hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56)>0.15f) {
                    //std::cout<<image.at<Vec3b>(i,j)[1]<<" ";
                    image.at<Vec3b>(i,j)[1]=0;
                }
                else {
                    image.at<Vec3b>(i,j)[1]=255;
                }
            }
        }
        namedWindow("result");
        cvtColor(image, image, COLOR_HLS2BGR);
        imshow("result", image);
        waitKey(0);
    }
    return 0;
}



