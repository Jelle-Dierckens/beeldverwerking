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
using namespace cv;

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
int main(int argc, char **argv){
    if(argc<2) {
        printf("geef road_bestand niet_road_bestand als argument");
        exit(255);
    }
    float kernelSize=256/100.0;
    std::ifstream inputstream(argv[1]); // road
    int rows=256/kernelSize;
    int cols=256/kernelSize;
    int planes=256/kernelSize;
    int dims[3]={rows, cols, planes};
    float pRoad;
    int linesRoad=0;
    int linesNotRoad=0;
    Mat p_hls_na_road=Mat(3, dims, CV_32F);
    Mat p_hls_na_notroad=Mat(3, dims, CV_32F);
    Mat p_road_na_hls=Mat(3, dims, CV_32F);
    char delim=';';
    std::string inputline;
    std::string h,l,s;
    int hi, li, si;
    std::getline(inputstream, inputline);
    while(!inputstream.eof()) {
        std::stringstream stringstr(inputline);
        std::getline(stringstr, h, delim);
        std::getline(stringstr, l, delim);
        std::getline(stringstr, s, delim);
        hi=atoi(h.c_str());
        li=atoi(l.c_str());
        si=atoi(s.c_str());
        //std::cout<<"h: "<<hi/kernelSize<<"\tl: "<<li/kernelSize<<"\ts: "<<si/kernelSize<<"\n";
        p_hls_na_road.at<float>(hi/kernelSize,li/kernelSize,si/kernelSize)+=1.0;
        //std::cout<<p_hls_na_road.at<float>(hi/kernelSize,li/kernelSize,si/kernelSize)<<"; ";
        linesRoad++;
        std::getline(inputstream, inputline);
    }

    inputstream.close();
    inputstream.open(argv[2]); //not road
    std::getline(inputstream, inputline);
    while(!inputstream.eof()) {
        std::stringstream stringstr(inputline);
        std::getline(stringstr, h, delim);
        std::getline(stringstr, l, delim);
        std::getline(stringstr, s, delim);
        hi=atoi(h.c_str());
        li=atoi(l.c_str());
        si=atoi(s.c_str());
        //std::cout<<"h: "<<h<<"\tl: "<<l<<"\ts: "<<s<<"\n";
        p_hls_na_notroad.at<float>(hi/kernelSize,li/kernelSize,si/kernelSize)+=1.0;
        linesNotRoad++;
        std::getline(inputstream, inputline);
    }
    p_hls_na_road/=linesRoad;
    p_hls_na_notroad/=linesNotRoad;
    pRoad=linesRoad*1.0/(linesRoad+linesNotRoad);
    //std::cout<<"pRoad: "<<pRoad<<"\n";
    for (int x=0;x<rows;x++) {
        for(int y=0; y<cols; y++) {
            for(int z=0; z<planes; z++) { //https://en.wikipedia.org/wiki/Bayes%27_theorem#Alternative_form
                float numerator=(p_hls_na_road.at<float>(x,y,z)*pRoad);
                float denominator=(p_hls_na_road.at<float>(x,y,z)*pRoad+(p_hls_na_notroad.at<float>(x,y,z)*(1.0-pRoad)));
                if(denominator!=0.0) {
                    p_road_na_hls.at<float>(x,y,z)=numerator/denominator;
                }
                else {
                    p_road_na_hls.at<float>(x,y,z)=0;
                }
                //std::cout<<p_hls_na_notroad.at<float>(x,y,z)<<"; ";
                std::cout<<x<<"; "<<y<<"; "<<z<<"; "<<p_road_na_hls.at<float>(x,y,z)<<"; \n";
            }
            //std::cout<<"\n";
        }
        //std::cout<<"\n\n";
    }

    return 0;
}



