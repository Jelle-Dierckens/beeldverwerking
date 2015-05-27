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
using namespace cv;

bool file_exist(const char *fileName)
{
    std::ifstream infile(fileName);
    return infile.good();
}
/*
void calc_lbp(const Mat& src, Mat& dst, int radius, int neighbors) {
    neighbors = max(min(neighbors,31),1); // set bounds...
    // Note: alternatively you can switch to the new OpenCV Mat_
    // type system to define an unsigned int matrix... I am probably
    // mistaken here, but I didn't see an unsigned int representation
    // in OpenCV's classic typesystem...
    dst = Mat::zeros(src.rows-2*radius, src.cols-2*radius, CV_32SC1);
    for(int n=0; n<neighbors; n++) {
        // sample points
        float x = static_cast<float>(radius) * cos(2.0*M_PI*n/static_cast<float>(neighbors));
        float y = static_cast<float>(radius) * -sin(2.0*M_PI*n/static_cast<float>(neighbors));
        // relative indices
        int fx = static_cast<int>(floor(x));
        int fy = static_cast<int>(floor(y));
        int cx = static_cast<int>(ceil(x));
        int cy = static_cast<int>(ceil(y));
        // fractional part
        float ty = y - fy;
        float tx = x - fx;
        // set interpolation weights
        float w1 = (1 - tx) * (1 - ty);
        float w2 =      tx  * (1 - ty);
        float w3 = (1 - tx) *      ty;
        float w4 =      tx  *      ty;
        // iterate through your data
        for(int i=radius; i < src.rows-radius;i++) {
            for(int j=radius;j < src.cols-radius;j++) {
                float t = w1*src.at<uchar>(i+fy,j+fx) + w2*src.at<uchar>(i+fy,j+cx) + w3*src.at<uchar>(i+cy,j+fx) + w4*src.at<uchar>(i+cy,j+cx);
                // we are dealing with floating point precision, so add some little tolerance
                dst.at<unsigned int>(i-radius,j-radius) += ((t > src.at<uchar>(i,j)) && (abs(t-src.at<uchar>(i,j)) > std::numeric_limits<float>::epsilon())) << n;
            }
        }
    }
}
*/

void calc_lbp(const Mat & image, Mat & dst, int radius, double threshold_fraction){
    Mat grey;
    cvtColor(image, grey, COLOR_RGB2GRAY);
    grey.copyTo(dst);
    double minValue, maxValue;
    minMaxLoc(grey, &minValue, &maxValue);
    uchar thres = threshold_fraction * (maxValue-minValue);
    //dst = Mat::zeros(image.rows - 2, image.cols - 2, image.depth());
    for (int i = radius; i < grey.rows - radius; i++){
        for (int j = radius; j < grey.cols - radius; j++){
            uchar center = grey.at<uchar>(i, j);
            uchar points[8];
            points[0] = grey.at<uchar>(i - radius, j - radius) >= center + thres ? 1 : 0;
            points[1] = grey.at<uchar>(i - radius, j) >= center + thres ? 1 : 0;
            points[2] = grey.at<uchar>(i - radius, j + radius) >= center + thres ? 1 : 0;
            points[3] = grey.at<uchar>(i, j - radius) >= center + thres ? 1 : 0;
            points[4] = grey.at<uchar>(i, j + radius) >= center + thres ? 1 : 0;
            points[5] = grey.at<uchar>(i + radius, j - radius) >= center + thres ? 1 : 0;
            points[6] = grey.at<uchar>(i + radius, j) >= center + thres ? 1 : 0;
            points[7] = grey.at<uchar>(i + radius, j + radius) >= center + thres ? 1 : 0;
            uchar mult[] = { 1, 2, 4, 8, 16, 32, 64, 128 };
            center = 0;
            for (int k = 0; k < 8; k++){
                center += points[k] * mult[k];
            }
            dst.at<uchar>(i, j) = center;
        }
    }
}

/*void findTextures(Mat &image){
    Mat cpy;
    //image.copyTo(cpy);
    lbp(image, cpy, 2, 0.02);
    imshow("LBP", cpy);
}*/


/*
std::vector<float> devideInBins(Mat patch, int bincount){
    std::vector<float> bins;
    bins.resize(bincount, 0);
    //int binwidth = 255 / bincount;
    for (int r = 0; r < patch.rows; r++){
        for (int c = 0; c < patch.cols; c++){
            bins[(patch.at<uchar>(r, c)*bincount / 255)%bincount]++;
        }
    }
    return bins;
}
*/



void compareHistograms(Mat src, Mat & dst, std::vector<Point2i> &patches, int patchsize, double comp_threshold){
    Mat hsv_src;
    Mat hsv_patch;
    src.copyTo(dst);
    cvtColor(src, hsv_src, COLOR_BGR2HSV);
    Rect patch = Rect(patches[0].x, patches[0].y, patchsize, patchsize);
    hsv_patch = hsv_src(patch);
    int h_bins = 50; int s_bins = 60;
    int histSize[] = { h_bins, s_bins };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };
    int channels[] = { 0, 1 };
    int lighter = 130;
    for (int i = 0; i < src.rows - patchsize; i += patchsize){
        for (int j = 0; j < src.cols - patchsize; j += patchsize){
            Mat hsv_test = hsv_src(Rect(j, i, patchsize, patchsize));
            MatND hist_src;
            MatND hist_test;
            calcHist(&hsv_src, 1, channels, Mat(), hist_src, 2, histSize, ranges, true, false);
            normalize(hist_src, hist_src, 0, 1, NORM_MINMAX, -1, Mat());

            calcHist(&hsv_test, 1, channels, Mat(), hist_test, 2, histSize, ranges, true, false);
            normalize(hist_test, hist_test, 0, 1, NORM_MINMAX, -1, Mat());

            /*
            Mogelijke compare methodes:
            CV_COMP_CORREL
            CV_COMP_CHISQR
            CV_COMP_INTERSECT
            CV_COMP_BHATTACHARYYA
            CV_COMP_HELLINGER
            */
            int compare_method = CV_COMP_CHISQR;
            double comp_value = compareHist(hist_src, hist_test, compare_method);
            if (comp_value < comp_threshold){
                for (int r = 0; r < patchsize; r++){
                    for (int c = 0; c < patchsize; c++){
                        dst.at<uchar>(Point((j + c) * 3, i + r)) += ((dst.at<uchar>(Point((j + c) * 3, i + r)) < 255 - lighter) ? lighter : (255 - dst.at<uchar>(Point((j + c) * 3, i + r))));
                        dst.at<uchar>(Point((j + c) * 3 + 1, i + r)) += ((dst.at<uchar>(Point((j + c) * 3 + 1, i + r)) < 255 - lighter) ? lighter : (255 - dst.at<uchar>(Point((j + c) * 3 + 1, i + r))));
                        dst.at<uchar>(Point((j + c) * 3 + 2, i + r)) += ((dst.at<uchar>(Point((j + c) * 3 + 2, i + r)) < 255 - lighter) ? lighter : (255 - dst.at<uchar>(Point((j + c) * 3 + 2, i + r))));
                    }
                }
            }
        }
    }
    rectangle(dst, patch, Scalar(255, 255, 255));
}

/*
void lbp_on_prob(Mat &image, Mat &lbp_image) {
    const int PATCH_SIZE = 20, VECTOR_SIZE = 5, BIN_COUNT = 50;
    std::vector<Point2i> patches;
    findBlackPatches(image, PATCH_SIZE, VECTOR_SIZE, patches);
    Mat textures;
    compareHistograms(lbp_image, textures, patches, PATCH_SIZE, 0.2);
    lbp(lbp_image, lbp_image, 2, 0.02);
    imshow("LBP", lbp_image);
    std::vector<float> bins_patch = devideInBins(lbp_image(Rect(patches[0].x, patches[0].y, PATCH_SIZE, PATCH_SIZE)), BIN_COUNT);
    std::vector<float> bins_test = devideInBins(lbp_image(Rect(0, 0, PATCH_SIZE, PATCH_SIZE)), BIN_COUNT);
    double compval = compareHist(bins_patch, bins_test, CV_COMP_KL_DIV);
    std::cout << "Comparation: " << compval << std::endl;
    imshow("Patches", image);
    imshow("CompareHist", textures);
    waitKey(0);
}
*/

void create_voteraster(Mat &src, Mat &raster, int kSize) {

    /*     dst grootte=src/kSize
     *      ieder element in raster toont de stemmen (geschaald naar [0,1]) van de overeenkomstige pixels in src
     *      dus waarde voor pixel in src: src.x/kSize, src.y/kSize
     */
    GaussianBlur(src, src, Size(3,3), 11);
    raster=Mat::zeros(src.rows/kSize+1, src.cols/kSize+1, CV_32F);
    Mat hsv_src;
    Mat hist_src;
    cvtColor(src, hsv_src, COLOR_BGR2HSV);
    int h_bins = 50; int s_bins = 60;
    int histSize[] = { h_bins, s_bins };
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };
    int channels[] = { 0, 1 };
    Rect patchRect(hsv_src.cols/2-kSize,hsv_src.rows-kSize*2,kSize,kSize);//(hsv_src.rows-kSize*2,hsv_src.cols/2-kSize,kSize,kSize);
    Mat compare_patch=hsv_src(patchRect);
    Mat dest;
    calc_lbp(compare_patch, dest, 4,0.04);
    //Mat colorPatch=src(patchRect);
    //colorPatch/=10;
    calcHist(&compare_patch, 1, channels, Mat(), hist_src, 2, histSize, ranges, true, false);
    normalize(hist_src, hist_src, 0, 1, NORM_MINMAX, -1, Mat());
    std::vector<float> normalizer;
    for(int x=0; x<src.rows; x+=kSize) {
        for(int y=0; y<src.cols; y+=kSize) {
            int vote=0;

            for(int i=0; i<=kSize; i++) {
                for(int j=0; j<=kSize; j++) {

                    if(x+i<=src.rows && y+j<=src.cols && src.at<Vec3b>(x+i, y+j)[1]==125) {
                        vote++;
                    }
                }
            }
            //raster.at<float>(x/kSize,y/kSize)=1.0*vote/(kSize*kSize);
            //std::cout<<"rows: "<<src.rows<<" cols: "<<src.cols<<"\t("<<x+kSize<<", "<<y+kSize<<")\n";
            if(x+kSize<src.rows && y+kSize<src.cols && 1.0*vote/(kSize*kSize)>0.2) {
                //std::cout<<"begin\n";
                Mat patch(src, Rect(y,x,kSize,kSize));

                Mat dest;
                Mat hist_test;
                calc_lbp(patch,dest ,4,0.04);
                calcHist(&patch, 1, channels, Mat(), hist_test, 2, histSize, ranges, true, false);
                normalize(hist_test, hist_test, 0, 1, NORM_MINMAX, -1, Mat());
                //std::cout<<hist_test<<"\n";

                /*
                    Mogelijke compare methodes:
                    CV_COMP_CORREL
                    CV_COMP_CHISQR
                    CV_COMP_INTERSECT
                    CV_COMP_BHATTACHARYYA
                    CV_COMP_HELLINGER
                    */
                int compare_method = CV_COMP_KL_DIV;
                double compare_value=compareHist(hist_src, hist_test, compare_method);
                raster.at<float>(x/kSize,y/kSize)=compare_value;
                normalizer.push_back(compare_value);
                std::cout<<compare_value<<"\n";
            }

        }
    }
    //normalize(raster,raster);
    auto minmax=std::minmax_element(normalizer.begin(), normalizer.end());

    raster-=*minmax.first;
    raster/=*minmax.second;
    //std::cout<<raster<<"\n";
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
                if(b<0.8) {
                    image.at<Vec3b>(i,j)[1]=0;
                }
                else if(b>=0.8) {
                    image.at<Vec3b>(i,j)[1]=255;
                }
                else {
                    image.at<Vec3b>(i,j)[1]=125;
                }

            }
        }
        namedWindow("result");
        namedWindow("original");
        //namedWindow("raster");
        //namedWindow("lbp");
        //Mat raster;
        //int kSize=10;
        //create_voteraster(image, raster, kSize);
        //resize(raster, raster, Size(image.cols, image.rows),0,0, INTER_NEAREST);
        cvtColor(image, image, COLOR_HLS2BGR);
        //Mat lbp;
        //calc_lbp(image, lbp, 4,0.04);
        //GaussianBlur(image, image, Size(5,5), 7);
        imshow("result", image);
        imshow("original", original);
        //imshow("raster", raster);
        //imshow("lbp", lbp);
        waitKey(0);
    }
    return 0;
}



