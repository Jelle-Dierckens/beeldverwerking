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

void create_voteraster(Mat &src, Mat &raster, int kSize) {

    /*     dst grootte=src/kSize
     *      ieder element in raster toont de stemmen (geschaald naar [0,1]) van de overeenkomstige pixels in src
     *      dus waarde voor pixel in src: src.x/kSize, src.y/kSize
     */
    GaussianBlur(src, src, Size(3,3), 11);
    raster=Mat::zeros(src.rows/kSize+1, src.cols/kSize+1, CV_32F);
    for(int x=0; x<src.rows; x+=kSize) {
        for(int y=0; y<src.cols; y+=kSize) {
            int vote=0;

            for(int i=0; i<kSize; i++) {
                for(int j=0; j<kSize; j++) {
                    if(x+i<=src.rows && y+j<=src.cols && src.at<Vec3b>(x+i, y+j)[1]==125) {
                        vote++;
                    }
                }
                raster.at<float>(x/kSize,y/kSize)=1.0*vote/(kSize*kSize);
            }
        }
    }
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
        Mat original=image.clone();
        resize(gem, gem, Size(image.cols, image.rows));
        GaussianBlur(image,image, Size(3,3),0);
        cvtColor(image, image, COLOR_BGR2HLS);
        int h,l,s;
        for(int i=0; i<image.rows; i++) {
            for (int j=0; j<image.cols; j++) {
                h=image.at<Vec3b>(i,j)[0];
                l=image.at<Vec3b>(i,j)[1];
                s=image.at<Vec3b>(i,j)[2];
                float a,b;
                //a=1-(std::max(std::min(0.8,gem.at<Vec3b>(i,j)[0]/256.0),0.2));
                a=1-gem.at<Vec3b>(i,j)[0]/256.0;
                //b=1-2.5*hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56);
                /*if(hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56)>0.2f) {
                   b=0;
                }
                else {
                   b=256;
                }*/
                b=1-hls_bayes.at<float>(h/2.56, l/2.56, s/2.56);
                if(a<0.5 && b<0.8) {
                    image.at<Vec3b>(i,j)[1]=0;
                }
                else if(a>=0.5 && b>=0.8) {
                    image.at<Vec3b>(i,j)[1]=255;
                }
                else {
                    image.at<Vec3b>(i,j)[1]=125;
                }

                //image.at<Vec3b>(i,j)[1]=256*((a*b)/(a+b));
            }
        }
        namedWindow("result");
        namedWindow("original");
        //namedWindow("raster");
        Mat raster;
        int kSize=25;
        create_voteraster(image, raster, kSize);
        resize(raster, raster, Size(image.cols, image.rows),0,0, INTER_NEAREST);
        cvtColor(image, image, COLOR_HLS2BGR);
        //GaussianBlur(image, image, Size(5,5), 7);
        imshow("result", image);
        imshow("original", original);
        imshow("raster", raster);
        waitKey(0);
    }
    return 0;
}



