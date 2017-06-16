#include <stdio.h>
#include <stdlib.h>

#include "load_mnist.h"

#define ETA 1

#define NB_LAYOR 5

#define NB_NEURON_INPUT 784
#define NB_NEURON_HIDDEN 25
#define NB_NEURON_OUTPUT 10

typedef struct neuron
{
	double potential;
	double value;
	double error_average;
	double error_signal;

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
void print_network_weight(Network network);
void print_weight_matrix(Network network, int num_matrix);
double train_network(Network* network, Image* img);
void put_img_in_input(Network* network, Image* img);
void compute_output(Network* network);
double calcul_error(Network* network, Image img);
double* convert_label(Image img);