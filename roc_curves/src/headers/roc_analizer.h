#ifndef ROC_ANALIZER_H
#define ROC_ANALIZER_H

#include <cstdio>
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/highgui.hpp"
#include "headers/Ifeature.h"
#include <vector>
#include "headers/graphview.h"

#define steps 10

using namespace cv;
using namespace std;

class Roc_analizer {
public:
	vector<double> precision;
    vector<double> recall;
    vector<double> accuracy;
    vector<double> specificity;
    Roc_analizer(Ifeature &_feature) {
        this->feature=&_feature;
    }
    void analize(Mat &_inputImage, Mat &_truthImage) {
        inputImage=_inputImage;
        truthImage=_truthImage;
        featureImage=Mat::zeros(inputImage.size(), CV_32FC1);
        //namedWindow("feature test");
        //namedWindow("truth test");
        //imshow("truth test", truthImage);.
        feature->applyFeature(inputImage, featureImage);
        for (float i=0; i<=1.0; i+=1.0/steps) {
            //~ std::cout<<"i: "<<i<<"\n";
            //imshow("feature test", featureImage);
            //waitKey(0);
            calcBinaryClassifiers(i);
        }
        FILE *fp;
        
        namedWindow("graph");
        Mat graphImage=Mat::zeros(1024,1024,CV_8U);
        vector<pair<double, double> > values;
        recall = getRecall();
        specificity = getSpecificity();
        accuracy = getAccuracy();
        precision = getPrecision();
        specificity=getSpecificity();
        double areaUnderCurve=0;
        //~ for (int i=0; i<tp.size(); i++) {
            //~ values.push_back(make_pair((double)fp[i]/(tp[i]*1.0+fp[i]*1.0), (double)tp[i]/(tp[i]*1.0+fp[i]*1.0)));
            //~ printf("Values fp=%d and tp=%d and fn=%d and tn=%d.\n", (double)fp[i], (double)tp[i], (double) fn[i], (double) tn[i]);
            //~ values.push_back(make_pair(recall[i], 1-specificity[i]));
            //~ cout<<"recall: "<<recall[i]<<"\t specificity: "<<specificity[i]<<endl;
            //~ if(i>0) {
                //~ areaUnderCurve+=(specificity[i]-specificity[i-1])*(recall[i]+recall[i-1])/2;
            //~ }
        //~ }
        //~ cout<<"aprox. area under curve: "<<areaUnderCurve<<endl;
        //~ graphview::createGraph(graphImage, values);
        //~ imshow("graph", graphImage);
        //~ waitKey(0);
    }
    void analize(std::string inputdirectory, std::string truthdirectory);

private:
    Mat inputImage;
    Mat truthImage;
    Mat featureImage;
    Ifeature *feature;
    vector<long> tp, tn, fp, fn; //true positive, true negative, false positive, false negative
    vector<double> getRecall() { //recall or TPR
        vector<double> v;
        for(int i=0; i<tp.size(); i++) {
            v.push_back(1.0* tp[i]/(1.0*tp[i]+1.0*fn[i]));
        }
        return v;
    }
    vector<double>  getSpecificity (){ //TNR
        vector<double> v;
        for(int i=0; i<tp.size(); i++) {
				v.push_back(1.0*tn[i]/(fp[i]+tn[i]));
        }
        return v;
    }
    vector<double>  getPrecision(){ //Positive Predictive Value
        vector<double> v;
        for(int i=0; i<tp.size(); i++) {
			if((tp[i]+fp[i]) == 0){
				v.push_back(0);
			}
			else{
				v.push_back(1.0*tp[i]/(tp[i]+fp[i]));
				//~ printf("prec %f.\n",1.0*tp[i]/(tp[i]+fp[i]));
			}
        }
        return v;
    }
    vector<double>  getNegativePredictiveValue() {
        vector<double> v;
        for(int i=0; i<tp.size(); i++) {
            v.push_back(1.0*tn[i]/(tn[i]+fn[i]));
        }
        return v;
    }

    vector<double>  getFalseNegativeRate() {
        vector<double> v;
        for(int i=0; i<tp.size(); i++) {
            v.push_back(1.0*fn[i]/(fn[i]+tp[i]));
        }
        return v;
    }
    
    vector<double>  getAccuracy() {
        vector<double> v;
        for(int i=0; i<tp.size(); i++) {
            v.push_back((1.0*fn[i]+1.0*tn[i])/(fn[i]+tn[i]+tp[i]+fp[i]));
        }
        return v;
    }


    void calcBinaryClassifiers(float confidenceTreshold=1.0);
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
    //~ printf("Confidence = %f.\n", confidenceTreshold);
    long  _tp=0, _tn=0, _fp=0, _fn=0;
    //~ std::cout<<"featureImage size: "<<featureImage.rows<<"*"<<featureImage.cols<<"\ttruthimage size: "<<truthImage.rows<<"*"<<truthImage.cols<<"\n";
    for (int x=0; x<featureImage.rows; x++) {
        for (int y=0; y<featureImage.cols; y++) {
			//~ printf("value %f treshold %f.\n",featureImage.at<float>(x,y), confidenceTreshold);
            if(featureImage.at<float>(x,y)>=confidenceTreshold) { //positive
                if(truthImage.at<float>(x,y)>=0.9) {//true positive
                    _tp++;
                }
                else {//false positive
                    _fp++;
                }
            }
            else { //negative
                if(truthImage.at<float>(x,y)>=0.9) {//false negative
                    _fn++;
                }
                else {//true negative
                    _tn++;
                }
            }
        }
    }
    //~ cout<<"For treshold " << confidenceTreshold << " ==> tp: "<<_tp<<"\tfp: "<<_fp<<"\tfn: "<<_fn<<"\ttn: "<<_tn<<endl;
    tp.push_back(_tp);
    fp.push_back(_fp);
    fn.push_back(_fn);
    tn.push_back(_tn);
}
#endif
