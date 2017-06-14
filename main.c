#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"
#include "network.h"

int main(int argc, char const *argv[])
{
	Network network = build_neural_network();
	print_network_weight(network);

	open_source_files();

	image img;
	
	read_input_number(100, &img);
	affiche_img(&img);

	close_source_files();

	/* code */
	return 0;
}