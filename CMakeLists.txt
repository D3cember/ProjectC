cmake_minimum_required(VERSION 3.10)
project(ProjectC C)

set(CMAKE_C_STANDARD 90)

add_executable(ProjectC main.c preproc.c util.c)  # Include all necessary .c files
