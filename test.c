#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"
#include "network.h"

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
	file = fopen("./test/error2.txt", "w");
	if(file == NULL)
		printf("ERROR : Can not open/create error_evolution.txt\n");
	srand(time(NULL));

	Network network = build_neural_network();


	printf("\n---------------------------------------------------------------\n");
	printf("                   Training ....");
	printf("\n");
	
	int i, j;
	for(i=0; i<10000; i++){
		Image img;
		read_input_number(i, &img);
		double summe = 0;

		double e = train_network(&network, &img);

		for(j=0; j<10; j++){
			int nb = rand()%(10000-5000) +5000;
			Image img2;
			read_input_number_test(nb, &img2);
			double error = test_for_data(&network, img2);

			summe+=error;
		}
		fprintf(file, "%d %f\n", i, (summe/10)*(summe/10));
		
	}
	printf(" Done !\n");
	printf("\n---------------------------------------------------------------\n");

	fclose(file);
	close_source_files();

	/* code */
	return 0;
}