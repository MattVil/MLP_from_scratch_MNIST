#ifndef __LOAD_MNIST_H
#define __LOAD_MNIST_H

/* lecture de la base mnist : http://yann.lecun.com/exdb/mnist/ */

#define __NB_IMAGES 1
#define __SIZE_IMAGE 28
#define __SIZE_INT 4

typedef struct Image{
    unsigned char imgbuf[__SIZE_IMAGE*__SIZE_IMAGE];
    unsigned char label;
}Image;

void endianness();
void close_source_files();
int open_source_files();
void read_input_number(int pos,Image *ret); /*read_one image, at position pos in the input file*/
void affiche_img(Image *rt);


#endif /* __LOAD_MNIST_H*/
