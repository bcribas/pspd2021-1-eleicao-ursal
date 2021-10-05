#!/bin/bash

if [ $# = 0 ]
then
   echo "Informe o numero de threads com -n"; echo
else
   echo "Comecando..."

   BENCHMARK="benchmark-solucoes.txt"
   RESULTS="results.txt"

   while getopts n: FLAG;
   do
      case "${FLAG}" in
         n) OMP_NUM_THREADS=${OPTARG};;
         \?) echo "Informe o numero de threads com -n"; echo
             exit 0;;
      esac
   done

   export OMP_NUM_THREADS=$OMP_NUM_THREADS

   gcc-10 eleicao-ursal-parallel.c -fopenmp

   echo "Testando a solucao paralela em $(date)" >> $BENCHMARK

   echo >> $BENCHMARK

   echo "Arquivo file-001 -- OMP_NUM_THREADS=$OMP_NUM_THREADS:" >> $BENCHMARK

   { time ./a.out < file-001 1> $RESULTS; } 2>> $BENCHMARK

   echo >> $BENCHMARK

   echo "Arquivo file-002 -- OMP_NUM_THREADS=$OMP_NUM_THREADS:" >> $BENCHMARK

   { time ./a.out < file-002 1> $RESULTS; } 2>> $BENCHMARK

   echo >> $BENCHMARK

   echo "Pronto!"
fi