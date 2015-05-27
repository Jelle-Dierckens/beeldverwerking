#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <string>
#include <fstream>
#include <istream>
#include <iostream>
#include <iterator>
#include <vector>
#include <algorithm>
#include <queue>
using namespace cv;

void calc_histogram(Mat &image_hls, Mat &image_rgb, int r, int c, int size, std::vector<float> &histogram) {
    std::cout<<"calculating histogram...\n";
    histogram=std::vector<float>(4); //4 waarden: R, G, B, O(geen baan volgens bayes-hls)
    int r_=0, g_=0, b_=0, o_=0;
    for(int i=-size/2; i<size/2; i++) {
        for(int j=-size/2; j<size/2; j++) {
            //std::cout<<"size: ("<<image_hls.rows<<","<< image_hls.cols<<")\tr: "<<r+i<<"\tc: "<<c+j<<"\n";
            if(r+i>=0 && r+i<image_hls.rows && c+j>=0 && c+j<image_hls.cols) {
                if(image_hls.at<Vec3b>(r+i,c+j)[1]==0) {
                    // std::cout<<"o "<<o<<"\n";
                    o_++;
                }
                else if(image_rgb.at<Vec3b>(r+i,c+j)[0]>5) {
                    // std::cout<<"r "<<r<<"\n";
                    r_++;
                }
                else if(image_rgb.at<Vec3b>(r+i,c+j)[1]>5){
                    // std::cout<<"g "<<g<<"\n";
                    g_++;
                }
                else if(image_rgb.at<Vec3b>(r+i,c+j)[2]>5) {
                    // std::cout<<"b "<<b<<"\n";
                    b_++;
                }
            }
        }
    }
    histogram[0]=1.0*r_/(r_+g_+b_+o_);
    histogram[1]=1.0*g_/(r_+g_+b_+o_);
    histogram[2]=1.0*b_/(r_+g_+b_+o_);
    histogram[3]=1.0*o_/(r_+g_+b_+o_);
    std::cout<<"finished calculating histogram!\n";
}

float comp_histogram(std::vector<float> &histogram_1, std::vector<float> &histogram_2, float abs_treshold, float rel_treshold) {
    //std::cout<<"comparing histograms...\n";
    int vote=0;
    //speciale gevallen: veel blauw: schaduw, dus toelaten
    //                                veel geen baan: niet toelaten
    if(histogram_2[3]>0.8 || histogram_1[3]>0.8) {
        return 0;
    }
    if(histogram_2[2]>0.9 || histogram_1[2]>0.9) {
        return 0.8;
    }
    for(int i=0; i<histogram_1.size(); i++) {
        if(fabs(histogram_1[i]-histogram_2[i]) - abs_treshold - histogram_1[1]*rel_treshold<0 &&
                fabs(histogram_1[i]-histogram_2[i]) + abs_treshold + histogram_1[1]*rel_treshold>0) {
            vote++;
        }
    }
    //std::cout<<"finished comparing histograms: "<<1.0*vote/(histogram_1.size())<<"\n";
    return 1.0*vote/(histogram_1.size());
}

void flood(Mat &image_bgr, Mat &image_hls, Mat &image_output, int startr, int startc, int size=40, int step_size=25) {
    std::cout<<"start flooding\n";
    /* image_output=Mat::ones(image_bgr.size(), CV_32F);
    image_output*=-1.0;*/
    std::queue<std::pair<int, int> > process_queue;
    process_queue.push(std::make_pair(startr, startc));
    std::pair<int, int> temp_pair;
    std::vector<float> prev_hist;
    std::vector<float> current_hist;
    std::vector<float> original_hist;
    int r,c;
    //<int size=40, step_size=25;//40, 25
    float abs_treshold=0.08;//0.1
    float rel_treshold=0.08;//0.05
    float result;
    float vote_treshold=0.55;//0.6
    /*for (int i = -10; i < 10; ++i) {
        for (int j = -10; j < 10; ++j) {
            image_output.at<float>(startr+i,startc+j)=1.0;
        }
    }
    imshow("test", image_output);
    waitKey();*/
    calc_histogram(image_hls, image_bgr, startr, startc, size, prev_hist);
    original_hist=prev_hist;
    while (!process_queue.empty()) {
        temp_pair=process_queue.front();
        process_queue.pop();
        r=temp_pair.first;
        c=temp_pair.second;
        //std::cout<<"testing at "<<r<<", "<<c<<"\n";
        calc_histogram(image_hls, image_bgr, r, c, size, current_hist);
        result=max(comp_histogram(prev_hist, current_hist, abs_treshold/2.0, rel_treshold/2.0),comp_histogram(original_hist, current_hist, abs_treshold, rel_treshold));
        prev_hist=current_hist;
        //std::cout<<"r: "<<r<<" c: "<<c<<" step: "<<step_size<<"\toutputImage: "<<image_output.rows<<", "<<image_output.cols<<"\nresult: "<<result<<"\n";
        //image_output.at<float>(r,c)=result;
        for (int i=-(step_size+1)/2; i<step_size/2; i++) {
            for (int j=-(step_size+1)/2; j<step_size/2; j++) {
                image_output.at<float>(r+i, c+j)=result;
            }
        }
        //std::cout<<"------------------------------------------------------------------------------------------------------\n";
        if(result>=vote_treshold) {

            if(c+step_size<image_output.cols && image_output.at<float>(r,c+step_size)<0) {
                //std::cout<<"pushing "<<r<<", "<<c+step_size<<"right\n";
                process_queue.push(std::make_pair(r, c+step_size));
                image_output.at<float>(r,c+step_size)=0;
            }
            if(c-step_size>0 && image_output.at<float>(r, c-step_size)<0) {
                //std::cout<<"pushing "<<r<<", "<<c-step_size<<"left\n";
                process_queue.push(std::make_pair(r, c-step_size));
                image_output.at<float>(r,c-step_size)=0;
            }
            if(r+step_size<image_output.rows && image_output.at<float>(r+step_size,c)<0) {
                //std::cout<<"pushing "<<r+step_size<<", "<<c<<"down\n";
                process_queue.push(std::make_pair(r+step_size, c));
                image_output.at<float>(r+step_size,c)=0;
            }
            std::cout<<image_output.at<float>(r-step_size,c)<<"\n";
            if(r-step_size>0 && image_output.at<float>(r-step_size,c)<0) {
                //std::cout<<"pushing "<<r-step_size<<", "<<c<<"up\n";
                process_queue.push(std::make_pair(r-step_size, c));
                //step_size*=0.95;
                //size*=0.95;
                image_output.at<float>(r-step_size,c)=0;
            }
        }
    }
    //std::cout<<"finished flooding!\n";
}

void multi_flooding(Mat &image_bgr, Mat &image_hls, Mat &image_average, Mat &image_output, int startr, int startc, int size=40, int step_size=25) {
    image_output=Mat::ones(image_bgr.size(), CV_32F);
    image_output*=-1.0;
    for (int i=0; i<image_bgr.rows; i+=step_size) {
        for (int j = 0; j < image_bgr.cols; j+=step_size) {
            if(image_hls.at<Vec3b>(i,j)[1]<100 && image_average.at<Vec3b>(i,j)[0]>100) {
                flood(image_bgr, image_hls, image_output, i,j, size, step_size);
            }
        }
    }
    //flood(image_bgr, image_hls, image_output, startr, startc);
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
        Mat image_bgr=imread(argv[arg]);
        Mat original=image_bgr.clone();
        resize(gem, gem, Size(image_bgr.cols, image_bgr.rows));
        //GaussianBlur(image_bgr,image_bgr, Size(3,3),0);
        Mat image;
        cvtColor(image_bgr, image, COLOR_BGR2HLS);
        int h,l,s;
        for(int i=0; i<image.rows; i++) {
            for (int j=0; j<image.cols; j++) {
                h=image.at<Vec3b>(i,j)[0];
                l=image.at<Vec3b>(i,j)[1];
                s=image.at<Vec3b>(i,j)[2];
                float b;
                b=1-hls_bayes.at<float>(h/2.56, l/2.56, s/2.56);
                image.at<Vec3b>(i,j)[2]=255;
                if(b<0.75) {
                    image.at<Vec3b>(i,j)[1]=125;
                }
                else if(b>=0.75) {
                    image.at<Vec3b>(i,j)[1]=0;
                }
                else {
                    image.at<Vec3b>(i,j)[1]=125;
                }

            }
        }
        namedWindow("result");
        namedWindow("original");
        namedWindow("flood");
        cvtColor(image, image, COLOR_HLS2BGR);
        Mat double_output;
        image_bgr.copyTo(double_output);
        Mat output;
        multi_flooding(image_bgr, image, gem, output, original.rows-10, original.cols/2-10, 11,7);
       Mat kernel = getStructuringElement(MORPH_ELLIPSE,Size(20,20));
        for (int i=0; i<5; i++) {
            morphologyEx(output, output, MORPH_OPEN,kernel);
        }

        cvtColor(double_output,double_output, COLOR_BGR2HSV);
        for(int i=0; i<double_output.rows; i++) {
            for(int j=0; j<double_output.cols; j++) {
                double_output.at<Vec3b>(i,j)[1]=output.at<float>(i,j)>0?255:0;
            }
        }
        cvtColor(double_output, double_output, COLOR_HSV2BGR);
        imshow("result", image);
        imshow("original", original);
        imshow("flood", double_output);
        waitKey(0);
    }
    return 0;
}



