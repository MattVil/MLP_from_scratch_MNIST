#include "opencv/highgui.h"
#include "opencv/cv.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#include <stdint.h>

#include "recognition.h"

#define SIZE 200
 
int fd1,fd2, ft1, ft2;
 
int main() {       

        //training file
        int nb_img;
        nb_img = open_training_files();

        //building neural network
        Network network = build_neural_network();

        //training
        printf("\nTraining ....");
        printf("\n");
        cvNamedWindow("Entrainement", CV_WINDOW_AUTOSIZE);
        int i, j, note;
        for(i=0; i<10000; i++){
            Image img;
            IplImage* training = cvCreateImage(cvSize(28, 28), IPL_DEPTH_8U ,1);
            training = image_to_IplImage(img);
            cvShowImage("Entrainement", training);
            cvReleaseImage(&training);

            read_input_number(i, &img);

            double error = train_network(&network, &img);
            note = cvWaitKey(1);
        }
        printf(" Done !\n");
        cvDestroyWindow("Entrainement");

        //camera and image features
        char key;
        IplImage *image;
        CvCapture *capture;
        capture = cvCaptureFromCAM(0);
  
        if (!capture) {
            printf("Ouverture du flux vidéo impossible !\n");
           return 1;
         }
         
        image = cvQueryFrame(capture);
          
        cvNamedWindow("Camera", CV_WINDOW_AUTOSIZE);
        cvNamedWindow("Center", CV_WINDOW_AUTOSIZE);
        cvMoveWindow("Center", 820, 380);
        cvMoveWindow("Camera", 100, 200);

        //loop demo
        while(key != 'q' && key != 'Q') {        
            
            image = cvQueryFrame(capture);
                  
            draw_reclangle(image);

            IplImage* resized = binarisation(extract_square(image));

            uchar* array = image_to_array(resized);

            put_array_in_input(&network, array);
            compute_output(&network);

            cvShowImage( "Camera", image);

            int network_value = convert_result(network);

            printf("\nResult proposed by the network : %d\t", network_value);
            printf("at %.2f%%\n", network.tab_layor[NB_LAYOR-1].tab_neuron[network_value].value * 100);


            key = cvWaitKey(10);
         }

        cvReleaseImage(&image);
  
        cvReleaseCapture(&capture);
        cvDestroyWindow("Camera");
        cvDestroyWindow("Center");
  
        return 0;
}

IplImage* image_to_IplImage(Image img){
    
    int i, j, k = 0;

    IplImage* result = cvCreateImage(cvSize(28, 28), IPL_DEPTH_8U ,1);

    for(i=0; i<__SIZE_IMAGE; i++){
        for(j=0; j<__SIZE_IMAGE; j++){
            ((uchar *)(result->imageData + i*result->widthStep))[j] = img.imgbuf[k];
            k++;
        }
    }

    return result;
}

IplImage* extract_square(IplImage* img){
    IplImage* img_center = cvCloneImage(img);
    IplImage* center_gray;
    IplImage* resized = cvCreateImage(cvSize(28, 28), IPL_DEPTH_8U ,1);

    CvPoint topLeft = cvPoint((((img_center->width)/2)-(SIZE/2)), (((img_center->height)/2)-(SIZE/2)));

    cvSetImageROI(img_center, cvRect(topLeft.x, topLeft.y, SIZE, SIZE));

    center_gray = cvCreateImage(cvGetSize(img_center),IPL_DEPTH_8U,1);
    cvCvtColor(img_center, center_gray, CV_RGB2GRAY);
    //cvFlip(center_gray, NULL, 1);

    cvResize(center_gray, resized);

    //cvShowImage("Center", resized);

    cvReleaseImage(&img_center);
    cvReleaseImage(&center_gray);

    return resized;
}

IplImage* binarisation(IplImage* img){
    IplImage* dest;

    dest = cvCloneImage(img);
    cvAdaptiveThreshold(img, dest, 255);

    IplImage* resized = cvCreateImage(cvSize(100, 100), IPL_DEPTH_8U ,1);
    cvResize(dest, resized);

    cvShowImage("Center", resized);

    return dest;
}


void draw_reclangle(IplImage* img){

    int i, j;

    CvPoint topLeft = cvPoint((((img->width)/2)-(SIZE/2)-2), (((img->height)/2)-(SIZE/2)-2));
    CvPoint buttomRight = cvPoint((((img->width)/2)+(SIZE/2)+2), (((img->height)/2)+(SIZE/2)+2));

    cvRectangle(img, topLeft, buttomRight, CV_RGB(255, 0, 0), 2);

}

uchar* image_to_array(IplImage* img){
    int i, j, k=0;

    int height = img->height;
    int width = img->width;

    uchar* tab = (uchar*)malloc(height*width*sizeof(uchar));

    for(i=0; i<height; i++){
        for(j=0; j<width; j++){
            //tab[k] = CV_IMAGE_ELEM(img, int, i, j);
            CvScalar intensity = cvGet2D(img, i, j);
            tab[k] = abs(intensity.val[0] -255);
            //tab[k] = intensity.val[0];
            printf("%d ", tab[k]);
        }
        printf("\n");
    }

    printf("--------------------------------------------------------\n");
    return tab;
}

Image* array_to_imgNetwork(uchar* tab){
    Image* img;
    int i=0;
    for(i=0; i<(__SIZE_IMAGE*__SIZE_IMAGE); i++)
        img->imgbuf[i] = tab[i];
    img->label = 1;

    return img;
}


Network build_neural_network(){

    if(NB_LAYOR < 2)
        exit(0);

    printf("\n---------------------------------------------------------------\n");
    printf("                  Start building neural network ...              \n");

    //network creation
    Network network;
    network.nb_layor = NB_LAYOR;
    network.tab_layor = (Layor*)malloc(NB_LAYOR * sizeof(Layor));
    network.nb_weight_matrix = NB_LAYOR-1;
    network.tab_weight_matrix = (Weight_matrix*)malloc((NB_LAYOR-1) * sizeof(Weight_matrix));

    int k;
    //layors creation
    printf("Layor creation :\n");
    for(k=0; k<NB_LAYOR; k++){
        //creation of the inputLayor
        if(k == 0){
            Layor inputLayor;
            inputLayor.nb_neuron = NB_NEURON_INPUT;
            inputLayor.tab_neuron = (Neuron*)malloc(NB_NEURON_INPUT * sizeof(Neuron));

            int i;
            for(i=0; i<NB_NEURON_INPUT; i++){
                Neuron neuron;
                neuron.potential = 0;
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
            outputLayor.tab_neuron = (Neuron*)malloc(NB_NEURON_OUTPUT * sizeof(Neuron));

            int i;
            for(i=0; i<NB_NEURON_OUTPUT; i++){
                Neuron neuron;
                neuron.potential = 0;
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
            hiddenLayor.tab_neuron = (Neuron*)malloc(NB_NEURON_HIDDEN * sizeof(Neuron));

            int i;
            for(i=0; i<NB_NEURON_HIDDEN; i++){
                Neuron neuron;
                neuron.potential = 0;
                neuron.value = 0;
                neuron.error_average = 0;

                hiddenLayor.tab_neuron[i] = neuron;
            }
            network.tab_layor[k] = hiddenLayor;
        }
        printf("\tLayor %d created : %d\n", k, network.tab_layor[k].nb_neuron);
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

        matrix.weight = (double**)malloc(matrix.size_in*sizeof(double));
        if(matrix.weight == NULL)
            exit(1);

        int i;
        for(i=0; i<matrix.size_in; i++){
            matrix.weight[i]=(double*)malloc(matrix.size_out*sizeof(double));
        }
        int n, m;
        for(n=0; n<matrix.size_in; n++){
            for(m=0; m<matrix.size_out; m++){
                matrix.weight[n][m] = ((double)rand() /(double)RAND_MAX)*2 -1;
            }
        }

        network.tab_weight_matrix[j] = matrix;
        printf("\tWeight matrix %d created : %dx%d\n", j, network.tab_weight_matrix[j].size_in, network.tab_weight_matrix[j].size_out);
    }

    printf("\nNetwork creation DONE !\n");
    printf("---------------------------------------------------------------\n");

    return network;
}




double train_network(Network* network, Image* img){

    put_img_in_input(network, img);
    compute_output(network);
    //double error = calcul_error(network, *img);
    double error = compute_error(network, *img);
    backpropagation(network);
    return error;
}
/*
This function put a image in input of the neural network
*/
void put_img_in_input(Network* network, Image* img){
    int i;
    for(i=0; i<NB_NEURON_INPUT; i++){
        //we put each pixels normalized on the input layor 
        network->tab_layor[0].tab_neuron[i].potential = img->imgbuf[i]/255.0;
        network->tab_layor[0].tab_neuron[i].value = img->imgbuf[i]/255.0;
    }
}

/*
This function compute the values of the output layor using the weight matrix between the
diffrent layor
*/
void compute_output(Network* network){

    int k;

    //for each layor except the first
    for(k=1; k<NB_LAYOR; k++){
        int nb_neuron = network->tab_layor[k].nb_neuron;
        int i, j;
        //we calcul the value of each neuron
        for(i=0; i<nb_neuron; i++){
            double summe = 0;
            //we look at the previous layor and the weight matrix before to calcul the summe
            for(j=0; j<network->tab_layor[k-1].nb_neuron; j++){
                double value_previous_neuron = network->tab_layor[k-1].tab_neuron[j].value;
                double weight = network->tab_weight_matrix[k-1].weight[j][i];

                summe += value_previous_neuron * weight;
            }
            network->tab_layor[k].tab_neuron[i].potential = summe;

            //we apply the logistic function f(x)=1/(1+e(-x)) to compute the value
            network->tab_layor[k].tab_neuron[i].value = 1/(1+exp(-summe));
        }
    }
}


/*
This function calcul the error and backpropagate it 
*/
double calcul_error(Network* network, Image img){

    double* expected_result = convert_label(img);
    double summe_error = 0;

    int i, j, k;

    //calcul of the error of each neuron of the output layor
    for(i=0; i<NB_NEURON_OUTPUT; i++){
        //calcul of the error
        double neuron_value = network->tab_layor[NB_LAYOR-1].tab_neuron[i].value;
        double error = expected_result[i] - neuron_value;
        summe_error += error;
        network->tab_layor[NB_LAYOR-1].tab_neuron[i].error_average = error;

        //calcul of the error signal
        network->tab_layor[NB_LAYOR-1].tab_neuron[i].error_signal = neuron_value * (1 - neuron_value) * error;
    }

    //calcul the new weight matrix for last layor
    for(i=0; i<network->tab_weight_matrix[NB_LAYOR-2].size_in; i++){
        for(j=0; j<network->tab_weight_matrix[NB_LAYOR-2].size_out; j++){
            double previous_weight = network->tab_weight_matrix[NB_LAYOR-2].weight[i][j];
            double in_signal = network->tab_layor[NB_LAYOR-2].tab_neuron[i].value;
            double error_signal = network->tab_layor[NB_LAYOR-1].tab_neuron[j].error_signal;
            network->tab_weight_matrix[NB_LAYOR-2].weight[i][j] = previous_weight + (ETA * in_signal * error_signal);
        }
    }

    //calcul the other new weight matrix
    for(k=NB_LAYOR-3; k>=0; k--){

        //we calcul the error of each neuron
        for(i=0; i<NB_NEURON_HIDDEN; i++){
            double neuron_value = network->tab_layor[k+1].tab_neuron[i].value;
            double error = 0;
            //the error of the neuron is equal to the summe of error signal it receive filter by the weight 
            for(j=0; j<network->tab_layor[k+2].nb_neuron; j++){
                error += network->tab_layor[k+2].tab_neuron[j].error_signal * network->tab_weight_matrix[k+1].weight[i][j];
            }
            network->tab_layor[k+1].tab_neuron[i].error_average = error;

            //calcul of the error signal
            network->tab_layor[k+1].tab_neuron[i].error_signal = neuron_value * (1 - neuron_value) * error;
        }

        //calcul the new weight matrix
        for(i=0; i<network->tab_weight_matrix[k].size_in; i++){
            for(j=0; j<network->tab_weight_matrix[k].size_out; j++){
                double previous_weight = network->tab_weight_matrix[k].weight[i][j];
                double in_signal = network->tab_layor[k].tab_neuron[i].value;
                double error_signal = network->tab_layor[k+1].tab_neuron[j].error_signal;
                network->tab_weight_matrix[k].weight[i][j] = previous_weight + (ETA * in_signal * error_signal);
            }
        }
    }

    return summe_error*summe_error;
}


/*
## OLD NOT USED ##
Version 2 of the fonction calcul_error with a earlier propagation of the error
*/
double compute_error(Network* network, Image img){

    double* expected_result = convert_label(img);
    double neuron_value = 0;
    double error = 0;
    double summe_error = 0;

    int i, j;

    //calcul of the error of each neuron of the output layor
    for(i=0; i<NB_NEURON_OUTPUT; i++){
        //calcul of the error
        neuron_value = network->tab_layor[NB_LAYOR-1].tab_neuron[i].value;
        error = expected_result[i] - neuron_value;
        summe_error += error;
        network->tab_layor[NB_LAYOR-1].tab_neuron[i].error_average = error;

        //calcul of the error signal
        network->tab_layor[NB_LAYOR-1].tab_neuron[i].error_signal = neuron_value * (1 - neuron_value) * error;
    }

    double error_signal = 0;
    double summe = 0;

    //propagation of the error to the layor n-1
    for(i=0; i<NB_NEURON_HIDDEN; i++){
        summe = 0;
        for(j=0; j<NB_NEURON_OUTPUT; j++){
            summe += network->tab_layor[NB_LAYOR-1].tab_neuron[j].error_signal * network->tab_weight_matrix[NB_LAYOR-2].weight[i][j];
        }
        network->tab_layor[NB_LAYOR-2].tab_neuron[i].error_average = summe;
    }

    return abs(summe_error);
}

/*
This function backpropagate the error to switch the weight matrix
*/
void backpropagation(Network* network){

    int i, j, k;

    double previous_weight; 
    double in_signal; 
    double error_signal;
    double neuron_value;
    double summe = 0;

    //for each layor, beginning at the end
    for(k=NB_LAYOR-2; k>=0; k--){

        //we calcul the new weight matrix
        for(i=0; i<network->tab_weight_matrix[k].size_in; i++){
            for(j=0; j<network->tab_weight_matrix[k].size_out; j++){
                previous_weight = network->tab_weight_matrix[k].weight[i][j];
                in_signal = network->tab_layor[k].tab_neuron[i].value;
                error_signal = network->tab_layor[k+1].tab_neuron[j].error_signal;

                network->tab_weight_matrix[k].weight[i][j] = previous_weight + (ETA * in_signal * error_signal);
            }
        }

        //calcul of the error signal
        for(i=0; i<network->tab_layor[k].nb_neuron; i++){
            neuron_value = network->tab_layor[k].tab_neuron[i].value;
            network->tab_layor[k].tab_neuron[i].error_signal = neuron_value * (1 - neuron_value) * network->tab_layor[k].tab_neuron[i].error_average;
        }

        //propagation of the error
        if(k>0){ //if it's not the firdt layor
            for(i=0; i<network->tab_layor[k-1].nb_neuron; i++){
                summe = 0;
                for(j=0; j<network->tab_layor[k].nb_neuron; j++){
                    summe += network->tab_layor[k].tab_neuron[j].error_signal * network->tab_weight_matrix[k-1].weight[i][j];
                }
                network->tab_layor[k-1].tab_neuron[i].error_average = summe;
            }
        }
    }
}

/*
This function convert the label of the image on a table of expected result for the network
*/
double* convert_label(Image img){

    double* result;
    result = (double*)malloc(NB_NEURON_OUTPUT*sizeof(double));

    int i;

    for(i=0; i<NB_NEURON_OUTPUT; i++){
        if(i==img.label)
            result[i] = 1.00;
        else
            result[i] = 0.00;
    }

    return result;
}


/*
This function convert the table in output of the network in a int result
*/
int convert_result(Network network){

    int i, max = -1; 
    double max_value = -10000000; //long min 

    for(i=0; i<network.tab_layor[NB_LAYOR-1].nb_neuron; i++){
        if(network.tab_layor[NB_LAYOR-1].tab_neuron[i].value > max_value){
            max_value = network.tab_layor[NB_LAYOR-1].tab_neuron[i].value;
            max = i;
        }
    }

    return max;
}

/*

*/
double test_for_data(Network* network, Image img){
    
    int i;
    double summe_error = 0;

    put_img_in_input(network, &img);
    compute_output(network);
    
    double* expected_result = convert_label(img);

    for(i=0; i<NB_NEURON_OUTPUT; i++){
        //calcul of the error
        double neuron_value = network->tab_layor[NB_LAYOR-1].tab_neuron[i].value;
        double error = expected_result[i] - neuron_value;
        summe_error += error;
    }

    return summe_error;
}

/*
This function print the weight matrix of the neural network
*/
void print_network_weight(Network network){
    int k, i, j;
    for(k=0; k<NB_LAYOR-1; k++){
        printf("\n---------------------------------------------------------------\n");
        printf("Weight matrix n°%d : %dx%d\n", k, network.tab_weight_matrix[k].size_in, network.tab_weight_matrix[k].size_out);
        printf("---------------------------------------------------------------\n");

        for(i=0; i<network.tab_weight_matrix[k].size_in; i++){
            for(j=0; j<network.tab_weight_matrix[k].size_out; j++){
                printf("%.2f ", network.tab_weight_matrix[k].weight[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
}


/*
This function print a selected weight matrix
*/
void print_weight_matrix(Network network, int num_matrix){
    
    if(num_matrix>=0 && num_matrix<NB_LAYOR){
        int i, j;
        printf("\n---------------------------------------------------------------\n");
        printf("Weight matrix n°%d : %dx%d\n", num_matrix, network.tab_weight_matrix[num_matrix].size_in, network.tab_weight_matrix[num_matrix].size_out);
        printf("---------------------------------------------------------------\n");

        for(i=0; i<network.tab_weight_matrix[num_matrix].size_in; i++){
            for(j=0; j<network.tab_weight_matrix[num_matrix].size_out; j++){
                printf("%.2f ", network.tab_weight_matrix[num_matrix].weight[i][j]);
            }
            printf("\n");
        }
        printf("\n\n");
    }
}

/*
This function print the value of all the neurons in a layor
*/
void print_layor(Network network, int num_layor){

    if(num_layor<=NB_LAYOR){
        int i;
        printf("\nLayor n°%d\n", num_layor); 
        for(i=0; i<network.tab_layor[num_layor].nb_neuron; i++){
            printf(" - %f", network.tab_layor[num_layor].tab_neuron[i].value);
        }
        printf(" -\n");
    }
}

void print_neuron(Neuron neuron){
    printf("Potential: %f\tSignal: %f\tError:%f\tError signal:%f\n", neuron.potential, neuron.value, neuron.error_average, neuron.error_signal);
}

void put_array_in_input(Network* network, uchar* array){
     int i;
    for(i=0; i<NB_NEURON_INPUT; i++){
        //we put each pixels normalized on the input layor 
        network->tab_layor[0].tab_neuron[i].potential = array[i]/255.0;
        network->tab_layor[0].tab_neuron[i].value = array[i]/255.0;
    }
}

void endianness() {
    int x = 1;
    
    char *y = (char*)&x;
    
    if (*y+48 == '0') printf ("big endian\n");
    else printf("little endian\n");
}



//! Byte swap int
int32_t swap_int32( int32_t val )
{
    val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF );
    return (val << 16) | ((val >> 16) & 0xFFFF);
}

void close_source_files(){

    close(fd1);
    close(fd2);
    
}


/*------------handling MNIST Files-----------------------*/


int open_training_files(){
    int magic_label,magic_train; /*magic numbers*/
    int nb_items; /*label set*/
    int nb_images,nb_rows,nb_columns; /* number of images of the training set 60000*/
    int size_image; /*image size, should be 28*/
    int temp,lu;
    
    printf("\n---------------------------------------------------------------\n");
    printf("                  Training database openning ...                 \n");


    fd1=open("../database/train-images.idx3-ubyte", O_RDONLY );
    fd2=open("../database/train-labels.idx1-ubyte", O_RDONLY );
    
    int swaped;
    
    read(fd1,&temp,sizeof(int));
    magic_train = swap_int32(temp);
    read(fd1,&temp,sizeof(int));
    nb_items = swap_int32(temp);
    read(fd1,&temp,sizeof(int));
    nb_rows = swap_int32(temp);
    lu = read(fd1,&temp,sizeof(int));
    nb_columns = swap_int32(temp);
    
    /* training set */
    printf("magic number : %d\n",magic_train);
    printf("nb images : %d\n",nb_items);
    printf("nb rows : %d\n",nb_rows);
    printf("lu : %d nb columns : %d\n",lu, nb_columns);
    printf("\nDONE !\n");
    printf("---------------------------------------------------------------\n");

    return nb_items;
}

int open_test_files(){
    int magic_label,magic_train; /*magic numbers*/
    int nb_items; /*label set*/
    int nb_images,nb_rows,nb_columns; /* number of images of the training set 60000*/
    int size_image; /*image size, should be 28*/
    int temp,lu;
    
    printf("\n---------------------------------------------------------------\n");
    printf("                  Test database openning ...                 \n");


    ft1=open("../database/t10k-images.idx3-ubyte", O_RDONLY );
    ft2=open("../database/t10k-labels.idx1-ubyte", O_RDONLY );
    
    int swaped;
    
    read(ft1,&temp,sizeof(int));
    magic_train = swap_int32(temp);
    read(ft1,&temp,sizeof(int));
    nb_items = swap_int32(temp);
    read(ft1,&temp,sizeof(int));
    nb_rows = swap_int32(temp);
    lu = read(ft1,&temp,sizeof(int));
    nb_columns = swap_int32(temp);
    
    /* training set */
    printf("magic number : %d\n",magic_train);
    printf("nb images : %d\n",nb_items);
    printf("nb rows : %d\n",nb_rows);
    printf("lu : %d nb columns : %d\n",lu, nb_columns);
    printf("\nDONE !\n");
    printf("---------------------------------------------------------------\n");

    return nb_items;
}





void read_input_number(int pos,Image *ret){ /*read_one image, at position pos in the input file*/
    int i;
    long int offset;
    
    /*for the training set image file*/
    
    offset=4*4; /*the file header : 4 32bits integers*/
    
    offset+=__SIZE_IMAGE*__SIZE_IMAGE*pos;  /* one MNIST image is 28x28 bytes, we have to skip pos images before the good one */
    pread(fd1, ret->imgbuf, __SIZE_IMAGE*__SIZE_IMAGE, offset); /*read the image*/
    
    /*for the training set label file*/
    offset = 2*4;
    offset+= pos;  /* one MNIST label is one bytes, we have to skip pos bytes before the good one */
    pread(fd2, &(ret->label), 1, offset); /*read the image*/
}

void read_input_number_test(int pos,Image *ret){ /*read_one image, at position pos in the input file*/
    int i;
    long int offset;
    
    /*for the training set image file*/
    
    offset=4*4; /*the file header : 4 32bits integers*/
    
    offset+=__SIZE_IMAGE*__SIZE_IMAGE*pos;  /* one MNIST image is 28x28 bytes, we have to skip pos images before the good one */
    pread(ft1, ret->imgbuf, __SIZE_IMAGE*__SIZE_IMAGE, offset); /*read the image*/
    
    /*for the training set label file*/
    offset = 2*4;
    offset+= pos;  /* one MNIST label is one bytes, we have to skip pos bytes before the good one */
    pread(ft2, &(ret->label), 1, offset); /*read the image*/
}
    
    
void affiche_img(Image *rt){
    int i;
    int chargl = '#';
    
     for (i=0;i<__SIZE_IMAGE*__SIZE_IMAGE;i++){
         if (i%__SIZE_IMAGE == 0) printf("\n");
         
        if (rt->imgbuf[i] < 200) chargl ='*';
        if (rt->imgbuf[i] <150 )  chargl = '+';
        if (rt->imgbuf[i] <100 )  chargl = '=';
        if (rt->imgbuf[i] <50 )  chargl = '-';
        if (rt->imgbuf[i] <25 )  chargl = ':';
        if (rt->imgbuf[i] <5 )  chargl = '.';
        if (rt->imgbuf[i] == 0 )  chargl = ' ';
        printf("%c",chargl);
      
     }
    printf("\n");
    printf("\t-------------\n");
    printf("\t|     %d     |\n", rt->label);
    printf("\t-------------\n");
}