new:
	rm -rf bin/*
	mkdir -p bin
	gcc src/single.c -o bin/sequencial
	gcc src/new.c -DTHREADS=1 -fopenmp -O3 -lm -o bin/paralelo_1
	gcc src/new.c -DTHREADS=2 -fopenmp -O3 -lm -o bin/paralelo_2
	gcc src/new.c -DTHREADS=4 -fopenmp -O3 -lm -o bin/paralelo_4
	gcc src/new.c -DTHREADS=8 -fopenmp -O2 -lm -o bin/paralelo_8
	gcc src/new.c -DTHREADS=12 -fopenmp -O3 -lm -o bin/paralelo_12
	gcc src/new.c -DTHREADS=16 -fopenmp -O3 -lm -o bin/paralelo_16

all:
	rm -rf bin/*
	mkdir -p bin
	gcc src/single.c -o bin/sequencial
	gcc src/main.c -DTHREADS=1 -fopenmp -O3 -lm -o bin/paralelo_1
	gcc src/main.c -DTHREADS=2 -fopenmp -O3 -lm -o bin/paralelo_2
	gcc src/main.c -DTHREADS=4 -fopenmp -O3 -lm -o bin/paralelo_4
	gcc src/main.c -DTHREADS=8 -fopenmp -O2 -lm -o bin/paralelo_8
	gcc src/main.c -DTHREADS=12 -fopenmp -O3 -lm -o bin/paralelo_12
	gcc src/main.c -DTHREADS=16 -fopenmp -O3 -lm -o bin/paralelo_16

old:
	rm -rf bin/*
	mkdir -p bin
	gcc src/single.c -o bin/sequencial
	gcc src/old.c -DTHREADS=1 -fopenmp -O3 -lm -o bin/paralelo_1
	gcc src/old.c -DTHREADS=2 -fopenmp -O3 -lm -o bin/paralelo_2
	gcc src/old.c -DTHREADS=4 -fopenmp -O3 -lm -o bin/paralelo_4
	gcc src/old.c -DTHREADS=8 -fopenmp -O2 -lm -o bin/paralelo_8
	gcc src/old.c -DTHREADS=12 -fopenmp -O3 -lm -o bin/paralelo_12
	gcc src/old.c -DTHREADS=16 -fopenmp -O3 -lm -o bin/paralelo_16