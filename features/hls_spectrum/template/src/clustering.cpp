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
using namespace cv;

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
int main(int argc, char **argv){
    if(argc<2) {
        printf("geef bestandsnaam als argument");
        exit(255);
    }
    std::ifstream inputstream(argv[1]);
    int line_count;
    line_count=std::count(std::istreambuf_iterator<char>(inputstream),
               std::istreambuf_iterator<char>(), '\n');
    //std::cout<<"lines: "<<line_count<<std::endl;

    Mat points(line_count,3,CV_32F);
    Mat labels;
    int i=0;
    inputstream.close();
    inputstream.open(argv[1]);
    if(inputstream.is_open()) {
        char inputline[16];
        while(!inputstream.eof()) {
            inputstream.getline(inputline, 16);
            points.at<float>(i,0)=(float)atof(inputline)/255.0;
            points.at<float>(i,1)=(float)atof(&inputline[4])/255.0;
            points.at<float>(i,2)=(float)atof(&inputline[8])/255.0;
            i++;
        }
        //std::cout<<points<<std::endl;
        inputstream.close();
        int attempts=25;
        int cluster_count=10000;
        Mat centers;
        kmeans(points, cluster_count, labels, TermCriteria(TermCriteria::MAX_ITER|TermCriteria::EPS, 1000, 0.005), attempts,  KMEANS_RANDOM_CENTERS, centers);
        centers*=255;
        centers.convertTo(centers, CV_32S);
        std::cout<<centers<<std::endl;
    }

    else {
        return 254;
    }

    return 0;
}



