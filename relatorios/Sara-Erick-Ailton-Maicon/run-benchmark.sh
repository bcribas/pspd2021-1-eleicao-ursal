#!/bin/bash

if [ $# = 0 ]
then
   echo "Informe o caminho do .c com -c"; echo
else
   echo "Comecando..."

   while getopts c: FLAG;
   do
      case "${FLAG}" in
         c) PROG=${OPTARG};;
         \?) echo "Informe o caminho do .c com -c"; echo
             exit 0;;
      esac
   done

   BINARY="prog.out"
   gcc-10 $PROG -O3 -fopenmp -o $BINARY

   for i in 1 2 4 8 12 16
   do
      export OMP_NUM_THREADS=$i
      echo OMP_NUM_THREADS=$OMP_NUM_THREADS
      echo
      rm -rf benchmark/
      make benchmark BENCHMARKBINARY=./$BINARY
      echo
   done

fi