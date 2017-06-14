#include <stdio.h>
#include <stdlib.h>

#define ETA 1

#define NB_LAYOR 5

#define NB_NEURON_INPUT 5
#define NB_NEURON_HIDDEN 7
#define NB_NEURON_OUTPUT 3

typedef struct neuron
{
	double value;
	double error_average;

}Neuron;

typedef struct layor
{
	Neuron* tab_neuron;
	int nb_neuron;
}Layor;

typedef struct weight_matrix
{
	Layor* input;
	int size_in;
	Layor* output;
	int size_out;

	double** weight;
}Weight_matrix;

typedef struct network
{
	Layor* tab_layor;
	int nb_layor;
	Weight_matrix* tab_weight_matrix;
	int nb_weight_matrix;
}Network;


Network build_neural_network();