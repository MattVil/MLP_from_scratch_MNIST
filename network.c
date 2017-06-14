#include "network.h"

Network build_neural_network(){

	if(NB_LAYOR < 2)
		exit(0);

	printf("\nStart building ...\n");

	//network creation
	Network network;
	network.nb_layor = NB_LAYOR;
	network.tab_layor = malloc(NB_LAYOR * sizeof(Layor));
	network.nb_weight_matrix = NB_LAYOR-1;
	network.tab_weight_matrix = malloc((NB_LAYOR-1) * sizeof(Weight_matrix));

	int k;
	//layors creation
	printf("Layor creation :\n");
	for(k=0; k<NB_LAYOR; k++){
		//creation of the inputLayor
		if(k == 0){
			Layor inputLayor;
			inputLayor.nb_neuron = NB_NEURON_INPUT;
			inputLayor.tab_neuron = malloc(NB_NEURON_INPUT * sizeof(Neuron));

			int i;
			for(i=0; i<NB_NEURON_INPUT; i++){
				Neuron neuron;
				neuron.value = 0;
				neuron.error_average = 0;

				inputLayor.tab_neuron[i] = neuron;
			}
			network.tab_layor[k] = inputLayor;
		}
		//creation of the ouputLayor
		else if(k == NB_LAYOR-1){
			Layor outputLayor;
			outputLayor.nb_neuron = NB_NEURON_OUTPUT;
		    outputLayor.tab_neuron = malloc(NB_NEURON_OUTPUT * sizeof(Neuron));

			int i;
			for(i=0; i<NB_NEURON_OUTPUT; i++){
				Neuron neuron;
				neuron.value = 0;
				neuron.error_average = 0;

				outputLayor.tab_neuron[i] = neuron;
			}
			network.tab_layor[k] = outputLayor;
		}
		//creation of the hidden layor(s)
		else{
			Layor hiddenLayor;
			hiddenLayor.nb_neuron = NB_NEURON_HIDDEN;
			hiddenLayor.tab_neuron = malloc(NB_NEURON_HIDDEN * sizeof(Neuron));

			int i;
			for(i=0; i<NB_NEURON_HIDDEN; i++){
				Neuron neuron;
				neuron.value = 0;
				neuron.error_average = 0;

				hiddenLayor.tab_neuron[i] = neuron;
			}
			network.tab_layor[k] = hiddenLayor;
		}
		printf("\tLayor %d created !\n", k);
	}

	//creation of connexion matrix table
	printf("Weight matrix creation :\n");
	int j;
	srand(time(NULL));

	for(j=0; j<NB_LAYOR-1; j++){
		Weight_matrix matrix;
		matrix.input = &(network.tab_layor[j]);
		matrix.size_in = network.tab_layor[j].nb_neuron;
		matrix.output = &(network.tab_layor[j+1]);
		matrix.size_out = network.tab_layor[j+1].nb_neuron;

		matrix.weight = malloc(matrix.size_in*sizeof(double));
		if(matrix.weight == NULL)
			exit(1);

		int i;
		for(i=0; i<matrix.size_in; i++){
			matrix.weight[i]=malloc(matrix.size_out*sizeof(double));
		}
		int n, m;
		for(n=0; n<matrix.size_in; n++){
			for(m=0; m<matrix.size_out; m++){
				matrix.weight[n][m] = (double)rand() /(double)RAND_MAX;
			}
		}

		network.tab_weight_matrix[j] = matrix;
		printf("\tWeight matrix %d created !\n", j);
	}

	printf("\nNetwork creation DONE !\n\n");

	return network;
}