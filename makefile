all: main

main.o: main.c load_mnist.h
	gcc -c main.c 

load_mnist.o: load_mnist.c load_mnist.h
	gcc -c load_mnist.c

main: main.o load_mnist.o
	gcc main.o load_mnist.o -o main

clean:
	rm -f *.o main