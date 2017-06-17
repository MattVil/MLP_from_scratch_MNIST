all: main test test_performance

main.o: main.c load_mnist.h
	gcc -c main.c 

load_mnist.o: load_mnist.c load_mnist.h
	gcc -c load_mnist.c

network.o: network.c network.h
	gcc -c network.c -lm

main: load_mnist.o network.o main.o
	gcc main.o load_mnist.o network.o -o main -lm

test: load_mnist.o network.o test.o
	gcc test.o load_mnist.o network.o -o dataTest -lm

test_performance: network.o load_mnist.o test_performance.o
	gcc test_performance.o load_mnist.o network.o -o performance -lm

clean:
	rm -f *.o main