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
    std::ifstream inputstream(argv[1]);
    int rows=256;
    int cols=256;
    int planes=256;
    int dims[3]={rows, cols, planes};
    Mat points=Mat(3, dims, CV_32S);
    int i=0;

    std::string inputline;
    std::string h,l,s;
    int hi, li, si;
    std::getline(inputstream, inputline);
    while(!inputstream.eof()) {

        std::stringstream stringstr(inputline);
        std::getline(stringstr, h, ';');
        std::getline(stringstr, l, ';');
        std::getline(stringstr, s, ';');
        hi=atoi(h.c_str());
        li=atoi(l.c_str());
        si=atoi(s.c_str());
        //std::cout<<"h: "<<h<<"\tl: "<<l<<"\ts: "<<s<<"\n";
        points.at<int>(hi,li,si)+=1;
        i++;
        std::getline(inputstream, inputline);
    }
    inputstream.close();
    inputstream.open(argv[2]);
    std::getline(inputstream, inputline);
    while(!inputstream.eof()) {

        std::stringstream stringstr(inputline);
        std::getline(stringstr, h, ';');
        std::getline(stringstr, l, ';');
        std::getline(stringstr, s, ';');
        hi=atoi(h.c_str());
        li=atoi(l.c_str());
        si=atoi(s.c_str());
        //std::cout<<"h: "<<h<<"\tl: "<<l<<"\ts: "<<s<<"\n";
        points.at<int>(hi,li,si)-=1;
        i++;
        std::getline(inputstream, inputline);
    }
    int z=0;
    for (int i = 0; i < 256; i++) {
      for (int j = 0; j < 256; j++) {
        for (int k = 0; k < 256; k++) {
          std::cout <<points.at<int>(i,j,k) << ", ";
          z++;
        }
      }
    }
    std::cout<<"\naantal: "<<z<<"\n";
    return 0;
}



