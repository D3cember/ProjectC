# Compiler and flags
CC = gcc
CFLAGS = -ansi -Wall -pedantic -g

# Executable target name
TARGET = projectC

# Object files
OBJS = main.o util.o data_struct.o preproc.o first_pass.o second_pass.o files_handler.o errors.o

# Build the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# Individual object file compilation
main.o: source/main.c headers/globaldefine.h
	$(CC) $(CFLAGS) -c source/main.c -o main.o

util.o: source/util.c headers/util.h
	$(CC) $(CFLAGS) -c source/util.c -o util.o

data_struct.o: source/data_struct.c headers/data_struct.h
	$(CC) $(CFLAGS) -c source/data_struct.c -o data_struct.o

preproc.o: source/preproc.c headers/preproc.h
	$(CC) $(CFLAGS) -c source/preproc.c -o preproc.o

first_pass.o: source/first_pass.c headers/first_pass.h
	$(CC) $(CFLAGS) -c source/first_pass.c -o first_pass.o

second_pass.o: source/second_pass.c headers/second_pass.h
	$(CC) $(CFLAGS) -c source/second_pass.c -o second_pass.o

files_handler.o: source/files_handler.c headers/files_handler.h
	$(CC) $(CFLAGS) -c source/files_handler.c -o files_handler.o

errors.o: source/errors.c headers/errors.h
	$(CC) $(CFLAGS) -c source/errors.c -o errors.o

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET) *.am *.ob *.ent *.ext
