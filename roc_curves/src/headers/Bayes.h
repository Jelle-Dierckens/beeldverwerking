#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iterator>
#include <vector>
#include <iomanip>
#include "headers/Ifeature.h"
using namespace cv;


class Bayes: public Ifeature {

private:
    Mat hls_bayes;//(3,dims, CV_32F);
    // Ifeature interface
public:
    Bayes(char* hlsFile){
        int dim=100;
        int dims[3]={dim,dim,dim};
        Mat hls_bayes_local(3,dims, CV_32F);
        std::ifstream inputstream(hlsFile);
        char delim=';';
        std::string x,y,z, value;
        std::string inputline;
        std::getline(inputstream, inputline);
        while(!inputstream.eof()) {
            std::stringstream stringstr(inputline);
            //~ std::cout<<inputline<<"\n";
            std::getline(stringstr, x, delim);
            std::getline(stringstr, y, delim);
            std::getline(stringstr, z, delim);
            std::getline(stringstr, value, delim);
            hls_bayes_local.at<float>(atoi(x.c_str()),atoi(y.c_str()),atoi(z.c_str()))=std::stof(value);
            //~ std::cout<<x<<", "<<y<<", "<<z<<", " << value << "\n";
			std::getline(inputstream, inputline);
        }
        //~ std::cout << hls_bayes_local.at<float>(0,0,0) << std::endl;
        hls_bayes = hls_bayes_local.clone();
        inputstream.close();

    };
    void applyFeature(const Mat &inputImage, Mat &outputImage);
};

void Bayes::applyFeature(const Mat &inputImage, Mat &outputImage) {
    //~ std::cout<<"applying feature...\n";
    GaussianBlur(inputImage,inputImage, Size(3,3),0);
    //~ std::cout<<"gaussian blur..\n";
    cvtColor(inputImage, inputImage, COLOR_BGR2HLS);
    //~ std::cout<<"color to hls\n";
    int h,l,s;
    //~ std::cout<<inputImage<<"\n";
    //~ std::cout<<"inputImage.size: "<<inputImage.rows<<"\t"<<inputImage.cols<<"\n";
    for(int i=0; i<inputImage.rows; i++) {
        for (int j=0; j<inputImage.cols; j++) {
            //~ std::cout<<"pixel: "<<i<<", "<<j<<"\n";
            h=(int)inputImage.at<Vec3b>(i,j)[0]/2.56;
            l=(int)inputImage.at<Vec3b>(i,j)[1]/2.56;
            s=(int)inputImage.at<Vec3b>(i,j)[2]/2.56;
            //~ std::cout<<"h: "<<h<<", l: "<<l<<", s: "<<s;
            //~ std::cout<<"\t, value: "<<hls_bayes.at<int>(h,l,s)<<"\n";
            outputImage.at<float>(i,j)=hls_bayes.at<float>(h,l,s);

        }
    }


}
