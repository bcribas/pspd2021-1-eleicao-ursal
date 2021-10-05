ifeq (run,$(firstword $(MAKECMDGOALS)))
  # use the rest as arguments for "run"
  THREADS_NUM := $(word 2, $(MAKECMDGOALS))
  FILE_NAME := $(word 3, $(MAKECMDGOALS))
  
  # ...and turn them into do-nothing targets
  $(eval $(THREADS_NUM):;@true)
  $(eval $(FILE_NAME):;@true)
endif
all:
	gcc src/single.c -o bin/single
	gcc src/main.c -fopenmp -o bin/multi

.PHONY:run
run: 
	export OMP_NUM_THREADS=$(THREADS_NUM)
	./bin/multi $(FILE_NAME)
run_single:
	./bin/single

	