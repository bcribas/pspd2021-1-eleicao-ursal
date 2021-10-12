#ifndef URSAL_H
#define URSAL_H

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

#define MAX 100000
#define p_offset 100              //[0     - 99   ]   -> 100 
#define s_offset 1000             //[100   - 999  ]   -> 900
#define f_offset 10000            //[1000  - 9999 ]   -> 9k
#define e_offset 100000           //[10000 - 99999]   -> 90k    
// =============================================================================
typedef struct{
    int id;
    int cont_votos;
}Candidato;

int *votos_presidente;
int *votos_validos;
int *votos_invalidos;

int num_th;
int **matriz_votos_global;
Candidato *votos_unidos;

int compara_votos (const void *x, const void *y);
void ler_arquivo(int file_size, char *arquivo);
void unificar_resultado();
void ordenar_printar(int *sfe);

#endif