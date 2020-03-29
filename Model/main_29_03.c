#define _CRTDBG_MAP_ALLOC
#define _CRT_SECURE_NO_DEPRECATE
#include <stdlib.h>
//#include <crtdbg.h>

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <string.h>

#include "wards_lib.h"
#include "globals.h"

int main(int argc, char *argv[]){
	network *net;
	
	int **inf, **playinf;
	parameters *par;
	int s,i;
	char command[100];
	
	int nseeds;
	int *to_seed;

	double *min,*max;

	double beta[N_INF_CLASSES];
	double scaling;
	
	gsl_rng *r; // random number generator stuff;
	
	
	printf("Input 1: random seed %d\n\n",atoi(argv[1]));
	
	r = gsl_rng_alloc (gsl_rng_default);
	gsl_rng_set (r, -1*(int)time(NULL) + atoi(argv[1]));
	
	printf("Input 2: parameter file name to be read %s\n",argv[2]);
	
	max=(double *)malloc(sizeof(double));
	min=(double *)malloc(sizeof(double));
	

	to_seed=(int *)calloc(sizeof(int),100);

	par=InitialiseParameters();
	
	ReadParametersFile(par,argv[2],atoi(argv[3])); // function to read parameters from file and put into par. 
	
	par->UV=atof(argv[4]);
	
	par->controlsON = 15;
	par->controlsOFF = par->controlsON+28; // three weeks
	par->controlsOFF = par->controlsON+28+21; // six weeks
	par->controlsOFF = par->controlsON+184; // until September
	par->controlScale = 1; //R0=2.8
  //par->controlScale = 0.8/2.7; 

	SetInputFileNames(4,par);
	
	strcpy(par->AdditionalSeeding,argv[5]);
  printf("%s\n",par->AdditionalSeeding);
	
	nseeds=ReadDoneFile(par->SeedName,to_seed);
	
	printf("2\n");
	
	net=BuildWardsNetworkDistance(par);
	if(net==NULL){
		printf("network not found, exiting\n");
		return 0;
	}
	
	else printf("network built\n");
		
	inf=InitialiseInfections(net);
	playinf=InitialisePlayInfections(net);
  
	GetMinMaxDistances(net,min,max);

	par->DynDistCutoff=*max+1;

	s=-1;
	par->StaticPlayAtHome=0.7;
	ResetEverything(net,par);
	RescalePlayMatrix(net,par);
	
	par->PlayToWork=0;
	par->WorkToPlay=0;

	MovePopulationFromPlayToWork(net,par,r);
	
	
	par->DailyImports=0;
	RunModel(net,par,inf,playinf,r,to_seed,s);
	
	
	
	
	FreeInfections(inf);
	FreeInfections(playinf);

	free(inf);
	free(playinf);
	
	
	free(net->nodes);
	free(net->to_links);
	free(net->play);
	free(net);

	free(to_seed);
	free(par);
	gsl_rng_free (r);
	free(min);
	free(max);
//		_CrtDumpMemoryLeaks();
	return 0;
}




