CC      = gcc
CFLAGS  = -Wall -ggdb
LD      = gcc
LDFLAGS = -lm

# Directories
OBJDIR  = build
SRCDIR  = src

# Source files
SRCS = main.c tga.c util.c obj_loader.c vector.c matrix.c ant_gl.c
OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(SRCS))

# Target
TARGET = $(OBJDIR)/prog

# Build all
all: $(TARGET)

# Link target
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files into object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

# Ensure build directory exists
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean up
clean:
	@echo "Cleaning things... a faint sound of the object files crying can be heard"
	@rm -rf $(OBJDIR)
