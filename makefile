all: projectC

projectC: data_struct.o errors.o preproc.o util.o main.o
	gcc -Wall -Wextra -pedantic -std=c90 -g -o projectC data_struct.o errors.o preproc.o util.o main.o

data_struct.o: data_struct.c
	gcc -Wall -Wextra -pedantic -std=c90 -g -c -o data_struct.o data_struct.c

errors.o: errors.c
	gcc -Wall -Wextra -pedantic -std=c90 -g -c -o errors.o errors.c

preproc.o: preproc.c
	gcc -Wall -Wextra -pedantic -std=c90 -g -c -o preproc.o preproc.c

util.o: util.c
	gcc -Wall -Wextra -pedantic -std=c90 -g -c -o util.o util.c

main.o: main.c
	gcc -Wall -Wextra -pedantic -std=c90 -g -c -o main.o main.c

clean:
	rm -f *.o projectC
