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
#include <math.h>
#include "linefinder.h"
#include "linehandler.h"

#define Gabor
//~ #define LineDetection
//~ #define Hough

using namespace cv;

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}

    Mat mkKernel(int ks, double sig, double th, double lm, double ps)
    {
        int hks = (ks-1)/2;
        double theta = th*CV_PI/180;
        double psi = ps*CV_PI/180;
        double del = 2.0/(ks-1);
        double lmbd = lm;
        double sigma = sig/ks;
        double x_theta;
        double y_theta;
        Mat kernel(ks,ks, CV_32F);
        for (int y=-hks; y<=hks; y++)
        {
            for (int x=-hks; x<=hks; x++)
            {
                x_theta = x*del*cos(theta)+y*del*sin(theta);
                y_theta = -x*del*sin(theta)+y*del*cos(theta);
                kernel.at<float>(hks+y,hks+x) = (float)exp(-0.5*(pow(x_theta,2)+pow(y_theta,2))/pow(sigma,2))* cos(2*CV_PI*x_theta/lmbd + psi);
            }
        }
        return kernel;
    }

    int kernel_size=21;
    int pos_sigma= 5;
    int pos_lm = 50;
    int pos_th = 0;
    int pos_psi = 90;
    cv::Mat src_f;
    cv::Mat dest;

    void Process(int , void *)
    {
        double sig = pos_sigma;
        double lm = 0.5+pos_lm/100.0;
        double th = pos_th;
        double ps = pos_psi;
        Mat kernel = mkKernel(kernel_size, sig, th, lm, ps);
        filter2D(src_f, dest, CV_32F, kernel);
        imshow("Process window", dest);
        Mat Lkernel(kernel_size*20, kernel_size*20, CV_32F);
        resize(kernel, Lkernel, Lkernel.size());
        Lkernel /= 2.;
        Lkernel += 0.5;
        imshow("Kernel", Lkernel);
        Mat mag;
        cv::pow(dest, 2.0, mag);
        imshow("Mag", mag);
    }
    
#ifdef LineDetection
	Mat findEdgeLines(Mat image){

		// Canny algorithm
		Mat contours;
		double_t thres1 = 180;
		double_t thres2 = 160;
		Canny(image, contours, thres1, thres2);

		std::vector<Vec2f> lines;
		int houghVote = 100;
		
				
		// http://answers.opencv.org/question/2966/how-do-the-rho-and-theta-values-work-in-houghlines/
		
		//rechterrand
		
		//~ while (lines.size() != 1 && houghVote < 1500){
			//~ HoughLines(contours, lines, image.rows*(3/4.0), CV_PI/4.0, houghVote,0,0); //voor theta  = Pi/3 of Pi/6
			//~ houghVote += 5;
		//~ }
		
		//linkerrand
		
		while (lines.size() != 1 && houghVote < 1500){
			HoughLines(contours, lines, 5, CV_PI/3.0, houghVote,0,0); //voor theta  = Pi/3 of Pi/6
			houghVote += 5;
		}

		
		printf("Aantal lijnen = %d.\n", lines.size());
		std::cout << houghVote << "\n";
		Mat result(contours.rows, contours.cols, CV_8U, Scalar(255));
		contours.copyTo(result);

		// Draw the limes
		std::vector<Vec2f>::const_iterator it = lines.begin();
		//Mat hough(contours.size(), CV_8U, Scalar(0));
		while (it != lines.end()) {
			float rho = (*it)[0];   // first element is distance rho
			float theta = (*it)[1]; // second element is angle theta

			Point pt1(rho / cos(theta), 0);
			// point of intersection of the line with last row
			Point pt2((rho - result.rows*sin(theta)) / cos(theta), result.rows);
			// draw a white line
			line(result, pt1, pt2, Scalar(255), 2);
			++it;
		}

		// Create LineFinder instance
		LineFinder ld;

		// Set probabilistic Hough parameters
		ld.setLineLengthAndGap(60, 10);
		ld.setMinVote(4);

		// Detect lines
		std::vector<Vec4i> li = ld.findLines(contours);
		Mat houghP(contours.size(), CV_8U, Scalar(0));
		ld.drawDetectedLines(houghP);
		//imshow("hough", houghP);
		return result;
	}
#endif    


int main(int argc, char **argv){
	
#ifdef Hough			
    if(argc<2) {
        printf("geef hlsfile afbeeldingen als argument\n");
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
    //~ Mat gem=imread(argv[2]);

    for (int arg=2; arg<argc; arg++) {
        std::cout<<"loading: "<<argv[arg]<<"\n";
        Mat image = imread(argv[arg]);
        Mat cannydest;
        Mat origineel = image.clone();
		namedWindow("origineel");
		moveWindow("origineel",0,0);
		imshow("origineel", image);
        //~ resize(gem, gem, Size(image.cols, image.rows));
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
                //~ a=1-gem.at<Vec3b>(i,j)[0]/256.0;
                b=1-2.5*hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56);
               /*if(hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56)>0.2f) {
                   b=0;
                }
                else {
                   b=256;
                }*/
                //~ b=1-hls_bayes.at<float>(xi/2.56, yi/2.56, zi/2.56);
               //~ if(a<0.5 && b<0.8) {
                   //~ image.at<Vec3b>(i,j)[1]=0;
               //~ }
               //~ else if(a>=0.5 && b>=0.8) {
                   //~ image.at<Vec3b>(i,j)[1]=255;
               //~ }
               //~ else {
                   //~ image.at<Vec3b>(i,j)[1]=125;
               //~ }
               
               if(b >= 0.8){
				   image.at<Vec3b>(i,j)[1] = 255;
				}
				else{
					image.at<Vec3b>(i,j)[1] = 0;
				}
			}
		}
               //image.at<Vec3b>(i,j)[1]=256*((a*b)/(a+b));
            //~ }
         
        waitKey();

        namedWindow("result");
        moveWindow("result",0,350);
        //~ cvtColor(image, image, COLOR_HLS2BGR);
		//~ cvtColor( dst, color_dst, COLOR_GRAY2BGR );
		
		std::vector<Vec4i> lines;
		double_t thres1 = 180;
		double_t thres2 = 120;
		Canny( origineel, cannydest, thres1, thres2 );
        HoughLines(cannydest, lines, 1, M_PI/180, 200);
        
        //verkeerde type: fixen 
        Mat result(origineel.rows, origineel.cols, CV_8U, Scalar(255));
		origineel.copyTo(result);

 		
 		printf("Lijnen : %d.\n", lines.size());       
        for( size_t i = 0; i < lines.size(); i++ )
		{

			line( origineel, Point(lines[i][0], lines[i][1]),
            Point(lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
		}	
				
        imshow("result", origineel);
        waitKey(0);
#endif			
		//Gabor filters
		//Werkt best voor theta tussen 30-60 en 120-150 om de randen van het wegdek te detecteren.
		
#ifdef Gabor		
for(int i = 1; i < argc; i++){
	    Mat image = cv::imread(argv[i]);
        imshow("Src", image);
        Mat src;
        cvtColor(image, src, COLOR_BGR2GRAY);
        src.convertTo(src_f, CV_32F, 1.0/255, 0);
        if (!kernel_size%2)
        {
            kernel_size+=1;
        }
        namedWindow("Process window", 1);
        //standaarddeviatie
        createTrackbar("Sigma", "Process window", &pos_sigma, kernel_size, Process);
        //frequentie
        createTrackbar("Lambda", "Process window", &pos_lm, 100, Process);
        //theta tussen 30-60 voor linkerrand en eventueel middellijn, theta tussen 120-150 voor de rechterrand.
        createTrackbar("Theta", "Process window", &pos_th, 180, Process);
        //intensiteit
        createTrackbar("Psi", "Process window", &pos_psi, 360, Process);
        Process(0,0);
        waitKey(0);
	}
#endif
		//FindEdgeLines
		
		//~ Mat image = cv::imread("../data/uu_000044.png",1);
		//~ Mat edgeLines = findEdgeLines(image); 
		//~ namedWindow("Origineel");
		//~ namedWindow("Lines");
		//~ imshow("Origineel", image);
		//~ imshow("Lines", edgeLines);
		//~ waitKey();
		
		//Local Binary patterns
		
    return 0;
}
