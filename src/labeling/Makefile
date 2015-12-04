CC =g++
CFLAGS =-c -g -std=c++11 -Wall -pthread

CFLAGS +=`pkg-config --cflags opencv`
LDFLAGS =`pkg-config --libs opencv` 

all: labeling-color

labeling-color: labeling-color.o 
	$(CC) $(LDFLAGS) labeling-color.o -o labeling-color $(LDFLAGS)

labeling-color.o: labeling-color.cpp
	$(CC) $(CFLAGS) labeling-color.cpp


clean:
	rm -rf *o core
