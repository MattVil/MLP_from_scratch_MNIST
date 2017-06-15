#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"
#include "network.h"

int main(int argc, char const *argv[])
{
	int nb_img;
	nb_img = open_source_files();
	Network network = build_neural_network();


	int i;
	for(i=0; i<1; i++){
		Image img;
		read_input_number(i, &img);
		printf("img n°%d\n", i);


		train_network(&network, img);

/*
		int j;
		for(j=0; j<NB_LAYOR; j++){
			print_layor(network, j);
		}*/


		affiche_img(&img);
	}

	close_source_files();

	/* code */
	return 0;
}