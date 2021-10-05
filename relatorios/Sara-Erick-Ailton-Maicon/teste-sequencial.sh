#!/bin/bash

echo "Comecando..."

BENCHMARK="benchmark-solucoes.txt"
RESULTS="results.txt"

gcc eleicao-ursal-sequencial.c

echo "Testando a solucao sequencial em $(date)" >> $BENCHMARK

echo >> $BENCHMARK

echo "Arquivo file-001:" >> $BENCHMARK

{ time ./a.out < file-001 1> $RESULTS; } 2>> $BENCHMARK

echo >> $BENCHMARK

echo "Arquivo file-002:" >> $BENCHMARK

{ time ./a.out < file-002 1> $RESULTS; } 2>> $BENCHMARK

echo >> $BENCHMARK

echo "Pronto!"