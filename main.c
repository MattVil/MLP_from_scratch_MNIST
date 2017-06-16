#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"
#include "network.h"

int main(int argc, char const *argv[])
{
	int nb_img;
	nb_img = open_source_files();
	Network network = build_neural_network();

	printf("Trainning ...");

	FILE* file = NULL;
	file = fopen("error_evolution.txt", "w");
	if(file == NULL)
		printf("ERROR : Can not open/create error_evolution.txt\n");






	int i;
	for(i=0; i<60000; i++){
		Image img;
		read_input_number(i, &img);
		//printf("img n°%d\n", i);


		double error = train_network(&network, &img);
		fprintf(file, "%f\n", error);
		
/*
		int j;
		for(j=0; j<NB_LAYOR; j++){
			print_layor(network, j);
		}
*/

		//affiche_img(&img);
	}
	printf(" Done !\n");

	srand(time(NULL));
	int nb = rand()%(50000-1) +1;
	Image img2;
	read_input_number(nb, &img2);

	put_img_in_input(&network, &img2);
	compute_output(&network);

	printf("Image n°%d\n", nb);
	affiche_img(&img2);

	print_layor(network, NB_LAYOR-1);

	fclose(file);
	close_source_files();

	/* code */
	return 0;
}