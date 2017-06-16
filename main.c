#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"
#include "network.h"

int main(int argc, char const *argv[])
{
	int nb_img;
	nb_img = open_trainning_files();

	int nb_img_test;
	nb_img_test = open_test_files();

	
	Network network = build_neural_network();

	FILE* file = NULL;
	file = fopen("error_evolution.txt", "w");
	if(file == NULL)
		printf("ERROR : Can not open/create error_evolution.txt\n");



	printf("\n---------------------------------------------------------------\n");
	printf("                   Trainning ....");
	printf("\n");
	
	int i;
	for(i=0; i<60000; i++){
		Image img;
		read_input_number(i, &img);

		double error = train_network(&network, &img);
		fprintf(file, "%f\n", error);
		
	}
	printf(" Done !\n");
	printf("\n---------------------------------------------------------------\n");


	printf("\n---------------------------------------------------------------\n");
	printf("                  Test : ");

	srand(time(NULL));
	int nb = rand()%(10000-1) +1;
	Image img2;
	read_input_number_test(nb, &img2);

	put_img_in_input(&network, &img2);
	compute_output(&network);

	printf("Image n°%d\n", nb);
	affiche_img(&img2);

	int network_value = convert_result(network);

	printf("\nResult proposed by the network : %d\t", network_value);
	printf("at %.2f%%\n", network.tab_layor[NB_LAYOR-1].tab_neuron[network_value].value * 100);

	fclose(file);
	close_source_files();

	/* code */
	return 0;
}