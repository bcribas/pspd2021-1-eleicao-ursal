all:
	mkdir -p bin
	rm bin/*
	gcc src/single.c -o bin/sequencial
	gcc src/main.c -DTHREADS=1 -fopenmp -o bin/paralelo_1
	gcc src/main.c -DTHREADS=2 -fopenmp -o bin/paralelo_2
	gcc src/main.c -DTHREADS=4 -fopenmp -o bin/paralelo_4
	gcc src/main.c -DTHREADS=8 -fopenmp -o bin/paralelo_8
	gcc src/main.c -DTHREADS=12 -fopenmp -o bin/paralelo_12
	gcc src/main.c -DTHREADS=16 -fopenmp -o bin/paralelo_16