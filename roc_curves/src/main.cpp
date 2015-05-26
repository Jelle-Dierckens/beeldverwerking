#include <iostream>
#include "headers/roc_analizer.h"
#include "headers/dummyFeature.h"
#include "headers/graphview.h"
#include <vector>
#include "headers/Bayes.h"

using namespace std;
using namespace cv;

int main(int argc, char** argv)
{
	unsigned int i,j;
	vector<double> recallGem(steps);
	vector<double> specGem(steps);
	vector<double> accurGem(steps);
	vector<double> precGem(steps);
	FILE* fp = fopen("roc.dat","w");
	
    if (argc <4) {
        cout<<"args: data  truth featurefile.\n";
        return 255;
    }
    
    Bayes feature(argv[argc-1]);
    Mat truthImage;
    Mat inputImage;
	
	for(i=1;i<(argc)/2;i++){
		
		inputImage=imread(argv[i]);
		truthImage=imread(argv[((argc-1)/2)+i]);
		//~ printf("Data %s and truth %s.\n", argv[i], argv[((argc-1)/2)+i]);
		
		resize(inputImage, inputImage, Size(100,100));
		resize(truthImage, truthImage, Size(100,100));
		

		Roc_analizer analizer(feature);
		//feature.applyFeature(inputImage, outputImage, 0.5);
		
		//per afbeelding per step de specificity and recall ophalen
		analizer.analize(inputImage, truthImage);
		for(j=0; j<analizer.precision.size();j++){
			//~ printf("prec %f.\n", analizer.precision[j]);
			recallGem[j] += analizer.recall[j];
			specGem[j] += analizer.specificity[j];
			accurGem[j] += analizer.accuracy[j];
			precGem[j] += analizer.precision[j];
			
		}
	}
	
	//gemiddelde over alle afbeeldingen nemen
	for(i=0;i<recallGem.size();i++){
		recallGem[i] /= (argc-2)/2;
		specGem[i] /= (argc-2)/2;
		accurGem[i] /= (argc-2)/2;
		precGem[i] /= (argc-2)/2;
		printf("\nTreshold %f:\nGemiddelde recall = %f\nGemiddelde specificity = %f.\nGemiddelde accuracy = %f\nGemiddelde precision = %f\n--------	",i*0.1,recallGem[i],specGem[i],accurGem[i], precGem[i]);
		//~ fprintf(fp, "%f ; %f\n",recallGem[i], specGem[i]);
	}


	
    //~ int wait;
    //~ cin>>wait;
    printf("Finished.\n");
    //~ waitKey(0);

    return 0;
}

