#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "string"
#include <fstream>
using namespace cv;

Mat image, image_bewerkt, image_solution;
char mode='r';

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
int main(int argc, char **argv){
    if(argc<2) {
        printf("geef bestandsnamen als argument");
        exit(255);
    }
    for (int i=1; i<argc; i++) {
        image=imread(argv[i]);
        std::string s(argv[i]);
        s.insert(s.size()-21, "gt_");
        if(mode=='r') {
        s.insert(s.size()-10, "road_");
        }
        else if (mode =='l') {
            s.insert(s.size()-10, "lane_");
        }
        else {
            //std::cout<<"invalid choice given\n";
            return 230;
        }
        if(file_exist(s.c_str())) {
            image_solution=imread(s);
            resize(image_solution, image_solution, image.size());
            cvtColor(image, image, COLOR_BGR2HLS);


            for( int y = 0; y < image.rows; y++ ) {
                for( int x = 0; x < image.cols; x++ ) {
                    if(image_solution.at<Vec3b>(y,x)[0]>0) {
                        //image_solution.at<Vec3b>(y,x)[0]=200;
                        std::cout<<(int)image.at<Vec3b>(y,x)[0]<<"; ";
                        std::cout<<(int)image.at<Vec3b>(y,x)[1]<<"; ";
                        std::cout<<(int)image.at<Vec3b>(y,x)[2]<<"\n";
                    }
                    else {
                        std::cerr<<(int)image.at<Vec3b>(y,x)[0]<<"; ";
                        std::cerr<<(int)image.at<Vec3b>(y,x)[1]<<"; ";
                        std::cerr<<(int)image.at<Vec3b>(y,x)[2]<<"\n";
                    }

                }
            }

        }
    }
    return 0;
}



