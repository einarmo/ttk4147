# Which compiler to use
CC = clang

# Compiler flags go here.
CFLAGS = -g -Wall -std=gnu11 -lpthread -O3#-fsanitize=thread

# Linker flags go here.
LDFLAGS = -g -lpthread #-fsanitize=thread

# list of sources
COMMON = common.c array.c

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

