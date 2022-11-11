CC = gcc
.PHONY: build clean

build: key 

key: key.o 
	$(CC) key.o -o key

key.o: key.c
	$(CC) -c key.c -o key.o

clean:
	rm *.o
	rm key