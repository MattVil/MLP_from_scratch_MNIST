#include <stdio.h>
#include <stdlib.h>
#include "load_mnist.h"

int main(int argc, char const *argv[])
{
	endianness();

	open_source_files();

	image img;
	int i ;
	for(i = rand()%(50-1) +1; i<124; i+=7){
		read_input_number(i, &img);
		affiche_img(&img);
	}

	close_source_files();

	/* code */
	return 0;
}