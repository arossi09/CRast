CC = gcc
CFLAGS = -Wall -glldb
LD = gcc
LDFLAGS = 

all: main.o tga.o
	$(CC) $(CFLAGS) -o prog main.o tga.o

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

tga.o: tga.c
	$(CC) $(CFLAGS) -c -o tga.o tga.c

clean:
	@echo Cleaning things...
	@rm *.o
