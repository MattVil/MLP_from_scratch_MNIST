all: recognition

recognition: recognition.cpp
	g++ recognition.cpp -o recognition `pkg-config --cflags opencv` `pkg-config --libs opencv`

clean:
	rm -f recognition