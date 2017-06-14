#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"
#include "network.h"

int main(int argc, char const *argv[])
{
	Network network = build_neural_network();

	int k, i, j;
	for(k=0; k<NB_LAYOR-1; k++){

		for(i=0; i<network.tab_weight_matrix[k].size_in; i++){
			for(j=0; j<network.tab_weight_matrix[k].size_out; j++){
				printf("%lf ", network.tab_weight_matrix[k].weight[i][j]);
			}
			printf("\n");
		}
		printf("\n---------------------------------------------------------------\n");
	}

	/*srand(time(NULL));

	endianness();

	open_source_files();

	image img;
	int i ;
	for(i = rand()%(50-1) +1; i<255; i+=100){
		read_input_number(i, &img);
		affiche_img(&img);
	}

	close_source_files();*/

	/* code */
	return 0;
}