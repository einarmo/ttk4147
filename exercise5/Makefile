# Which compiler to use
CC = gcc

# Compiler flags go here.
CFLAGS = -g -Wall -std=gnu11 -lpthread

# Linker flags go here.
LDFLAGS = -g -lpthread -lcomedi -lm

# list of sources
COMMON = io.c taskA.c

# program executable file name.
TARGET = exercise

# top-level rule, to compile everything.
all: $(TARGET)

# Define all object files.
OBJ = $(SRC:.c=.o) $(COMMON:.c=.o)

# rule to link the program
$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

# Compile: create object files from C source files.
%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@ 

# rule for cleaning re-compilable files.
clean:
	rm -f $(TARGET) $(OBJ)

rebuild:	clean all

.PHONY: all rebuild clean

