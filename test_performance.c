#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "load_mnist.h"
#include "network.h"

#define NB_TEST 50
#define ECHANTILLON 1000

int main(int argc, char const *argv[])
{
	//training file
	int nb_img;
	nb_img = open_training_files();

	//test file
	int nb_img_test;
	nb_img_test = open_test_files();

	//file to save the error
	FILE* file = NULL;
	file = fopen("./test/error_evolution.txt", "w");
	if(file == NULL)
		printf("ERROR : Can not open/create error_evolution.txt\n");


	//file to test the resistance of the network
	FILE* performance = NULL;
	performance = fopen("./test/performance5x50.txt", "w");
	if(performance == NULL)
		printf("ERROR : Can not open/create performance5x50.txt\n");


	double moyenne = 0, temps_moyen = 0;
	int k;
	struct timeval tvBegin, tvEnd;
	for(k=0; k<NB_TEST; k++){
		Network network = build_neural_network();


		//printf("\n---------------------------------------------------------------\n");
		printf("             %d : Training ....\n", k);
		
		gettimeofday(&tvBegin, NULL);
		int i, j;
		for(i=0; i<60000; i++){
			Image img;
			read_input_number(i, &img);

			double error = train_network(&network, &img);
			//fprintf(file, "%f\n", error);
			
		}
		gettimeofday(&tvEnd, NULL);
		double timeResult = (double) (tvEnd.tv_usec - tvBegin.tv_usec) / 1000000 + (double) (tvEnd.tv_sec - tvBegin.tv_sec);
		printf ("Total time = %f seconds\n", timeResult);
		temps_moyen += timeResult;
		/*printf(" Done !\n");
		printf("\n---------------------------------------------------------------\n");


		printf("\n---------------------------------------------------------------\n");
		printf("                  Test : \n");*/

		srand(time(NULL));

	

		int good_result = 0;

		for(j=0; j<ECHANTILLON; j++){
			int nb = rand()%(10000-1) +1;
			Image img2;
			read_input_number_test(nb, &img2);

			put_img_in_input(&network, &img2);
			compute_output(&network);

			//printf("Image n°%d : %d\t", j, img2.label);
			//affiche_img(&img2);

			int network_value = convert_result(network);

			if(network_value == img2.label)
				good_result ++;

			//printf("Network : %d\t", network_value);
			//printf("at %.2f%%\n", network.tab_layor[NB_LAYOR-1].tab_neuron[network_value].value * 100);
		}

		printf("#####  Good result : %.2f %% ######\n", ((double)good_result/ECHANTILLON)*100.0);
		//printf("\n---------------------------------------------------------------\n");
		moyenne += ((double)good_result/ECHANTILLON)*100.0;

		fprintf(performance, "%f,%f\n", ((double)good_result/ECHANTILLON)*100.0, timeResult);
	}

	moyenne = moyenne/NB_TEST;
	temps_moyen = temps_moyen/NB_TEST;
	printf("\n\nFINAL RESULT : %.2f %%\t TRAINING TIME : %f\n", moyenne, temps_moyen);
	fprintf(performance, "\n\n%f, %f\n", moyenne, temps_moyen);

	fclose(file);
	close_source_files();

	/* code */
	return 0;
}