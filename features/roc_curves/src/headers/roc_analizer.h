#ifndef ROC_ANALIZER_H
#define ROC_ANALIZER_H

#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "headers/Ifeature.h"

using namespace cv;

class Roc_analizer {
public:
    Roc_analizer(Ifeature &_feature) {
        this->feature=&_feature;
    }
    void analize(Mat &_inputImage, Mat &_truthImage, float step=0.1) {
        inputImage=_inputImage;
        truthImage=_truthImage;
        featureImage=Mat::zeros(inputImage.size(), CV_32FC1);
        namedWindow("feature test");
        for (float i=0; i<1.0; i+=step) {
            std::cout<<"i: "<<i<<"\n";
             feature->applyFeature(inputImage, featureImage,i); //TODO!!!
             imshow("feature test", featureImage);
             waitKey(0);
             calcBinaryClassifiers();
             std::cout<<"tp: "<<tp<<"\n";
             std::cout<<"fp: "<<fp<<"\n";
             std::cout<<"tn: "<<tn<<"\n";
             std::cout<<"fn: "<<fn<<"\n";
             std::cout<<"sensitivity: "<<getSensitivity()<<"\t";
             std::cout<<"specificity: "<<getSpecificity()<<"\n";
        }



    }
    void analize(std::string inputdirectory, std::string truthdirectory);

private:
    Mat inputImage;
    Mat truthImage;
    Mat featureImage;
    Ifeature *feature;
    long tp, tn, fp, fn; //true positive, true negative, false positive, false negative
    float getSensitivity() {
        return 1.0* tp/(tp+fn);
    }
    float getSpecificity (){
        return 1.0*tn/(fp+tn);
    }
    float getPrecision(){
        return 1.0*tp/(tp+fp);
    }
    float getNegativePredictiveValue() {
        return 1.0*tn/(tn+fn);
    }
    float getFalsePositiveRate() {
        return 1.0-getSpecificity();
    }
    float getFalseDiscoveryRate() {
        return 1.0-getPrecision();
    }
    float getFalseNegativeRate() {
        return 1.0*fn/(fn+tp);
    }


    void calcBinaryClassifiers(float confidenceTreshold=0.5);
    void setSingleChannel(Mat &image) {
        image.convertTo(image, CV_32FC3);
        Mat channels[3];
        split(image, channels);
        image=channels[0];
        normalize(image, image, 0,1, NORM_MINMAX);
        std::cout<<"finished making image single channeled\n";
    }

};

void Roc_analizer::calcBinaryClassifiers(float confidenceTreshold) {
    /*
     * calculates tp, tn, fp, fn
     * requires inputImage, truthImage, featureImage to be defined
    */
    /*if(featureImage == nullptr || truthImage == nullptr) {
        std::cerr<<"calcBinaryClassifiers: not all images were provided\n";
        return;
    }*/
    if(featureImage.channels()!=1) {
        std::cout<<"feature image not single channeled! attempt to fix...\n";
        setSingleChannel(featureImage);
    }
    if(truthImage.channels()!=1) {
        std::cout<<"truth image not single channeled! attempt to fix...\n";
        setSingleChannel(truthImage);
    }
    if(featureImage.size!=truthImage.size) {
        std::cout<<"featureImage and truthImage differ in size, resizing\n";
        resize(truthImage, truthImage, Size(featureImage.cols, featureImage.rows));
    }/*
    namedWindow("feature applied");
    namedWindow("truth image");
    imshow("feature applied", featureImage);
    imshow("truth image", truthImage);*/
    tp=0; tn=0; fp=0; fn=0;
    std::cout<<"featureImage size: "<<featureImage.rows<<"*"<<featureImage.cols<<"\ttruthimage size: "<<truthImage.rows<<"*"<<truthImage.cols<<"\n";
    for (int x=0; x<featureImage.rows; x++) {
        for (int y=0; y<featureImage.cols; y++) {
            if(featureImage.at<float>(x,y)>=confidenceTreshold) { //positive
                if(truthImage.at<float>(x,y)==1.0) {//true positive
                    tp++;
                }
                else {//false positive
                    fp++;
                }
            }
            else { //negative
                if(truthImage.at<float>(x,y)==1.0) {//false negative
                    fn++;
                }
                else {//true negative
                    tn++;
                }
            }
        }
    }
}
#endif
