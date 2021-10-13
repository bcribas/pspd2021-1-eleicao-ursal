// =================================================================================
//                              Trabalho 1 - PSPD
// 	Professor  : Bruno Ribas
// 	Alunos     : Flavio Vieira / Joao Cirqueira / Moacir Mascarenha
// 	Matricula  : 15/0125682    / 15/0132344 / 
// =================================================================================
#include <stdio.h>
#include "ursal.h"

// ================================  Main ====================================== 
int main( int argc, char *argv[] )
{
    int i, fsize;
    int sfe[3], sfe_offset[3];
    FILE *fp;
    
    if(argc < 2){
        fprintf(stderr, "[Erro]: Parametro <caminho/nome_do_arquivo> nao informado!\n");
        exit(EXIT_FAILURE);
    }
    if( argc == 3)
        num_th = strtol(argv[2], NULL, 10);
    else
        num_th = 1;                            //sem argv[2] -> sequencial
// =============================================================================
    
    votos_invalidos  = (int* ) calloc(num_th + 1, sizeof(int));
    votos_validos    = (int* ) calloc(num_th + 1, sizeof(int));
    votos_presidente = (int* ) calloc(num_th + 1, sizeof(int));
    
    matriz_votos_global = (int**) calloc(num_th, sizeof(int*));
    for(i = 0; i < num_th; i++)
        matriz_votos_global[i] = (int*) calloc(MAX, sizeof(int));        
    
    votos_unidos = (Candidato*) calloc(MAX, sizeof(Candidato));        

    fp = fopen(argv[1], "r");
   
    for(i = 0; i < 3; i++)
        fscanf(fp, "%d", &sfe[i]);
   
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp) - 1;
    fclose(fp);

//==============================================================================
    # pragma omp parallel num_threads(num_th)
        ler_arquivo(fsize, argv[1]);
    unificar_resultado();
    ordenar_printar(sfe);

// limpar ponteiros
    for(i = 0; i < num_th; i++)
        free(*(matriz_votos_global + i));

    free(matriz_votos_global);
    free(votos_unidos);
    free(votos_invalidos);
    free(votos_validos);
    free(votos_presidente);
 
    return (EXIT_SUCCESS);
}
