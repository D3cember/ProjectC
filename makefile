# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c90 -g

# Source and object files
SRCS = main.c data_struct.c errors.c first_pass.c preproc.c util.c
OBJS = $(SRCS:.c=.o)

# Include directory
INCLUDES = -I headers

# Target executable
TARGET = projectC

# Default target
all: $(TARGET)

# Link the target executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

# Clean up object files and executable
clean:
	rm -f $(OBJS) $(TARGET)
