CC = gcc
CFLAGS = -Wall -ggdb
LD = gcc
LDFLAGS = 

# Define directories for source, object files, and the build directory
OBJDIR = build

# Target for all files
all: $(OBJDIR)/main.o $(OBJDIR)/tga.o $(OBJDIR)/util.o $(OBJDIR)/obj_loader.o $(OBJDIR)/vector.o $(OBJDIR)/matrix.o
	$(CC) $(CFLAGS) -o $(OBJDIR)/prog $(OBJDIR)/main.o $(OBJDIR)/tga.o $(OBJDIR)/util.o $(OBJDIR)/obj_loader.o $(OBJDIR)/vector.o $(OBJDIR)/matrix.o -lm

# Rule to build object files with the correct paths
$(OBJDIR)/main.o: src/main.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/main.o src/main.c

$(OBJDIR)/tga.o: src/tga.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/tga.o src/tga.c

$(OBJDIR)/util.o: src/util.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/util.o src/util.c

$(OBJDIR)/vector.o: src/vector.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/vector.o src/vector.c

$(OBJDIR)/obj_loader.o: src/obj_loader.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/obj_loader.o src/obj_loader.c

$(OBJDIR)/matrix.o: src/matrix.c
	$(CC) $(CFLAGS) -c -o $(OBJDIR)/matrix.o src/matrix.c

# Clean rule to remove object files and binaries
clean:
	@echo Cleaning things... a faint sound of the object files cry can be heard
	@rm -rf $(OBJDIR)/*.o $(OBJDIR)/prog

