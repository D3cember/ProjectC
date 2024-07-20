cmake_minimum_required(VERSION 3.10)
project(ProjectC C)

set(CMAKE_C_STANDARD 90)

# Add the executable and specify the source files
add_executable(ProjectC main.c preproc.c util.c data_struct.c errors.c input.asm output.asm)

