CC = gcc
CFLAGS = -Wall -ggdb
LD = gcc
LDFLAGS = 

all: main.o tga.o util.o obj_loader.o vector.o
	$(CC) $(CFLAGS) -o prog main.o tga.o util.o obj_loader.o vector.o -lm

main.o: main.c
	$(CC) $(CFLAGS) -c -o main.o main.c

tga.o: tga.c
	$(CC) $(CFLAGS) -c -o tga.o tga.c

util.o: util.c
	$(CC) $(CFLAGS) -c -o util.o util.c 

vector.o: vector.c
	$(CC) $(CFLAGS) -c -o vector.o vector.c

obj_loader.o: obj_loader.c
	$(CC) $(CFLAGS) -c -o obj_loader.o obj_loader.c

clean:
	@echo Cleaning things... a faint sound of the objects files cry can be heard
	@rm *.o
