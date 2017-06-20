#include <stdio.h>
#include <stdlib.h>

#include <math.h>


#define __LOAD_MNIST_H

/* lecture de la base mnist : http://yann.lecun.com/exdb/mnist/ */

#define __NB_IMAGES 1
#define __SIZE_IMAGE 28
#define __SIZE_INT 4


#define ETA 0.2

#define NB_LAYOR 4

#define NB_NEURON_INPUT 784
#define NB_NEURON_HIDDEN 25
#define NB_NEURON_OUTPUT 10

typedef struct Image{
    unsigned char imgbuf[__SIZE_IMAGE*__SIZE_IMAGE];
    unsigned char label;
}Image;

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

void endianness();
void close_source_files();
int open_training_files();
int open_test_files();
void read_input_number(int pos,Image *ret); /*read_one image, at position pos in the input file*/
void read_input_number_test(int pos,Image *ret);
void affiche_img(Image *rt);

Network build_neural_network();
void print_network_weight(Network network);
void print_weight_matrix(Network network, int num_matrix);
double train_network(Network* network, Image* img);
void put_img_in_input(Network* network, Image* img);
void put_array_in_input(Network* network, uchar* array);
void compute_output(Network* network);
double calcul_error(Network* network, Image img);
double* convert_label(Image img);
int convert_result(Network network);
double test_for_data(Network* network, Image img);

void draw_reclangle(IplImage* img);
IplImage* extract_square(IplImage* img);
uchar* image_to_array(IplImage* img);
Image* array_to_imgNetwork(uchar* tab);