all:
	rm -f bin/ursal_
	gcc src/ursal_hash.c -o bin/hash_seq
	gcc src/ursal_hash_paralelo.c -DTHREADS=1 -fopenmp -o bin/hash_paralelo_1_thread
	gcc src/ursal_hash_paralelo.c -DTHREADS=2 -fopenmp -o bin/hash_paralelo_2_thread
	gcc src/ursal_hash_paralelo.c -DTHREADS=4 -fopenmp -o bin/hash_paralelo_4_thread
	gcc src/ursal_hash_paralelo.c -DTHREADS=8 -fopenmp -o bin/hash_paralelo_8_thread
	gcc src/ursal_hash_paralelo.c -DTHREADS=12 -fopenmp -o bin/hash_paralelo_12_thread
	gcc src/ursal_hash_paralelo.c -DTHREADS=16 -fopenmp -o bin/hash_paralelo_16_thread
