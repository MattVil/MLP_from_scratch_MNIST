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
	FILE* file2 = NULL;
	FILE* file3 = NULL;
	file = fopen("./test/reussite.txt", "w");
	file2 = fopen("./test/echec.txt", "w");
	file3 = fopen("./test/error.txt", "w");
	srand(time(NULL));

	Network network = build_neural_network();


	printf("\n---------------------------------------------------------------\n");
	printf("                   Training ....");
	printf("\n");
	
	double summe;
	int good_result, bad_result;

	int i, j, k;
	for(k=0; k<5; k++){
		for(i=0; i<60000; i++){
			Image img;
			read_input_number(i, &img);
			summe = 0;
			good_result = 0;
			bad_result = 0;

			double e = train_network(&network, &img);

			for(j=0; j<100; j++){
				int nb = rand()%(10000-1) +1;
				Image img2;
				read_input_number_test(nb, &img2);
				double error = test_for_data(&network, img2);

				if(convert_result(network) == img2.label)
					good_result ++;
				else
					bad_result ++;

				summe+=error;
			}
			fprintf(file, "%d\n", good_result);
			fprintf(file2, "%d\n", bad_result);
			fprintf(file3, "%f\n", abs(summe)/100.0);
			
		}
	}
	printf(" Done !\n");
	printf("\n---------------------------------------------------------------\n");

	fclose(file);
	close_source_files();

	/* code */
	return 0;
}