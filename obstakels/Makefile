CC=g++
CFLAGS=-Wall -g
LIBS=`pkg-config --libs opencv`

all:	main

main:	main.o functions.o
	$(CC) $(CFLAGS) main.o functions.o -o $@  $(LIBS)

main.o:	main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o $@ $(LIBS) 

functions.o: functions.cpp
	$(CC) $(CFLAGS) -c functions.cpp -o $@ $(LIBS) 

clean: 	
	rm -rf *.o main 
