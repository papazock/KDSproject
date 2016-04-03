HEADERS = mygenerator.h
GENERATOR_ARGS= 15000000 dataPoints
APP_ARGS= -1 -1 dataPoints -1 -1
COMPILER= mpicc
CFLAGS=-std=c++11 -w -Wall
OPENMP_ARGS= --bynode -x OMP_NUM_THREADS=24
OPENMPI_ARG= -np 16

default: all


mygenerator.o: mygenerator.c $(HEADERS)
	gcc ${CFLAGS} -c mygenerator.c -o mygenerator.o

mygenerator: mygenerator.o
	gcc ${CFLAGS} mygenerator.o -o mygenerator

generate: mygenerator
	./mygenerator ${GENERATOR_ARGS}

examine.o:examine.c
	mpicc -c ${CFLAGS} -fopenmp  examine.c -o examine.o

App: examine.o
	mpicc ${CFLAGS} -fopenmp examine.o -o App 


examine: App
	export OMP_NUM_THREADS=4
	mpirun ${OPENMPI_ARG} ${OPENMP_ARGS} App ${APP_ARGS}



clean:
	-rm -f mygenerator.o
	-rm -f mygenerator
	-rm -f examine.o
	-rm -f App

all:
	make clean
	make generate
	make examine