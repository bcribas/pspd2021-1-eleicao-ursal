v1:
	mkdir -p build
	rm build/*
	gcc src/ursal_array.c -o build/sequencial
	gcc src/ursal_paralelo.c -DTHREADS=1 -fopenmp -o build/paralelo_1
	gcc src/ursal_paralelo.c -DTHREADS=2 -fopenmp -o build/paralelo_2
	gcc src/ursal_paralelo.c -DTHREADS=4 -fopenmp -o build/paralelo_4
	gcc src/ursal_paralelo.c -DTHREADS=8 -fopenmp -o build/paralelo_8
	gcc src/ursal_paralelo.c -DTHREADS=12 -fopenmp -o build/paralelo_12
	gcc src/ursal_paralelo.c -DTHREADS=16 -fopenmp -o build/paralelo_16

v1O3:
	mkdir -p build
	rm build/*
	gcc src/ursal_array.c -O3 -o build/sequencial
	gcc src/ursal_paralelo.c -O3 -DTHREADS=1 -fopenmp -o build/paralelo_1
	gcc src/ursal_paralelo.c -O3 -DTHREADS=2 -fopenmp -o build/paralelo_2
	gcc src/ursal_paralelo.c -O3 -DTHREADS=4 -fopenmp -o build/paralelo_4
	gcc src/ursal_paralelo.c -O3 -DTHREADS=8 -fopenmp -o build/paralelo_8
	gcc src/ursal_paralelo.c -O3 -DTHREADS=16 -fopenmp -o build/paralelo_16
	gcc src/ursal_paralelo.c -O3 -DTHREADS=12 -fopenmp -o build/paralelo_12

v2:
	mkdir -p build
	rm build/*
	gcc src/ursal_array.c -o build/sequencial
	gcc src/ursal_paralelo_v2.c -O3 -DTHREADS=1 -fopenmp -o build/paralelo_1
	gcc src/ursal_paralelo_v2.c -O3 -DTHREADS=2 -fopenmp -o build/paralelo_2
	gcc src/ursal_paralelo_v2.c -O3 -DTHREADS=4 -fopenmp -o build/paralelo_4
	gcc src/ursal_paralelo_v2.c -O3 -DTHREADS=8 -fopenmp -o build/paralelo_8
	gcc src/ursal_paralelo_v2.c -O3 -DTHREADS=12 -fopenmp -o build/paralelo_12
	gcc src/ursal_paralelo_v2.c -O3 -DTHREADS=16 -fopenmp -o build/paralelo_16
