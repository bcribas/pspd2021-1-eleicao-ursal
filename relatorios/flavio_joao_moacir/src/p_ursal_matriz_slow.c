// =================================================================================
//                              Trabalho 1 - PSPD
// 	Professor  : Bruno Ribas
// 	Alunos     : Flavio Vieira / 
// 	Matricula  : 15/0125682    /
// =================================================================================
# include <stdio.h>
# include <stdlib.h>
# include <omp.h>
# include <string.h>

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

typedef struct {
    int *votos_presidente;
    int *votos_validos;
    int *votos_total;
}Arg_votos;

int compara_votos (const void *x, const void *y);
void ler_arquivo(int id_th, int **reg_votos, int file_size, char *arquivo, Arg_votos *arg_votos);
void unificar_resultado(int num_th, int **matriz_votos_global, Candidato *votos_unidos, Arg_votos *arg_votos);
void ordenar_printar(int num_th, Candidato *votos_unidos, Arg_votos *arg_votos, int *sfe);

// ================================  Main ====================================== 
int main( int argc, char *argv[] )
{
    int num_th, id_th, sum, i, fsize;
    int sfe[3], sfe_offset[3];
    FILE *fp;
    int **matriz_votos_global;
    Candidato *votos_unidos;
    Arg_votos arg_voto;
    
    double start, wtime;
    
    if(argc < 2){
        fprintf(stderr, "[Erro]: Parametro <caminho/nome_do_arquivo> nao informado!\n");
        exit(EXIT_FAILURE);
    }
    if( argc == 3)
        num_th = strtol(argv[2], NULL, 10);
    else
        num_th = 1;                            //sem argv[2] -> sequencial
// =============================================================================
    arg_voto.votos_total      = (int* ) calloc(num_th + 1, sizeof(int));
    arg_voto.votos_validos    = (int* ) calloc(num_th + 1, sizeof(int));
    arg_voto.votos_presidente = (int* ) calloc(num_th + 1, sizeof(int));
    
    matriz_votos_global       = (int**) calloc(num_th,     sizeof(int*));
    for(i = 0; i < num_th; i++)
        matriz_votos_global[i] = (int*) calloc(MAX, sizeof(int));        
    
    votos_unidos = (Candidato*) calloc(MAX, sizeof(Candidato));        

    fp = fopen(argv[1], "r");
   
    for(i = 0; i < 3; i++)
        fscanf(fp, "%d", &sfe[i]);
   
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp) - 1;

//==============================================================================
    # pragma omp parallel num_threads(num_th)
    {
        id_th = omp_get_thread_num();
        ler_arquivo(id_th, matriz_votos_global, fsize, argv[1], &arg_voto);
    }
    unificar_resultado(num_th, matriz_votos_global, votos_unidos, &arg_voto);
    ordenar_printar(num_th, votos_unidos , &arg_voto, sfe);

// limpar ponteiros
    for(i = 0; i < num_th; i++)
        free(*(matriz_votos_global + i));

    free(matriz_votos_global);
    free(votos_unidos);

    return (EXIT_SUCCESS);
}
// =========================== Sub Programas =================================== 
// compara_votos qsort
int compara_votos (const void *x, const void *y) 
{
    int a, b, c, d;
    a = ((Candidato*)x)->cont_votos;
    b = ((Candidato*)y)->cont_votos;
    c = ((Candidato*)x)->id;
    d = ((Candidato*)y)->id;

    if (a != b)
        return (a - b);
    else 
        return (c - d);
}
//===============================================================================
void ler_arquivo(int id_th, int **reg_votos, int file_size, char *arquivo, Arg_votos *arg_votos)
{
    int i, voto, num_th;
    int bloco, inicio, fim, resto;

    FILE *fp = fopen(arquivo, "r");
    num_th = omp_get_num_threads();

    // definir blocos/chunks de leitura
    bloco = file_size / num_th;
    inicio = bloco * id_th;
    fim = inicio + bloco;
    
    fseek(fp, inicio, SEEK_SET);
    while(fgetc(fp) != '\n');

    while(inicio < fim)
    {
        fscanf(fp, "%d", &voto);
        if(voto > 0)
        {   
            arg_votos->votos_validos[id_th]++;
            reg_votos[id_th][voto]++;

            if( voto < 100)
                arg_votos->votos_presidente[id_th]++;
            
        }
        inicio = ftell(fp);
        arg_votos->votos_total[id_th]++;

    }
}

//===============================================================================
void unificar_resultado(int num_th, int **matriz_votos_global, Candidato *votos_unidos, Arg_votos *arg_votos)
{
    int sum, i, j;
    for( j = 0; j < MAX; j++)
    {
        sum = 0;
        for( i = 0; i < num_th; i++)
            if (matriz_votos_global[i][j] != 0 ){
                sum += matriz_votos_global[i][j];
            }
        
        if(sum){
            votos_unidos[j].id = j;
            votos_unidos[j].cont_votos = sum;
        }
    }

    for(i = 0; i < num_th; i++)
    {
        arg_votos->votos_total[num_th + 1]      += arg_votos->votos_total[i];
        arg_votos->votos_validos[num_th + 1]    += arg_votos->votos_validos[i];
        arg_votos->votos_presidente[num_th + 1] += arg_votos->votos_presidente[i];
    }
}
//===============================================================================
void ordenar_printar(int num_th, Candidato *votos_unidos, Arg_votos *arg_votos, int *sfe)
{
    int sfe_offset[3], i, j;

    int votos_total      = arg_votos->votos_total[num_th + 1];
    int votos_validos    = arg_votos->votos_validos[num_th + 1];
    int votos_invalidos  = votos_total - votos_validos;
    int votos_presidente = arg_votos->votos_presidente[num_th + 1];
    float percent;

    sfe_offset[0] = s_offset;
    sfe_offset[1] = f_offset;
    sfe_offset[2] = e_offset;

    qsort(votos_unidos           ,  p_offset            , sizeof(Candidato), compara_votos);
    qsort(votos_unidos + p_offset, (s_offset - p_offset), sizeof(Candidato), compara_votos);
    qsort(votos_unidos + s_offset, (f_offset - s_offset), sizeof(Candidato), compara_votos);
    qsort(votos_unidos + f_offset, (e_offset - f_offset), sizeof(Candidato), compara_votos);

    percent = (votos_unidos[99].cont_votos * 100) / votos_presidente;
    
    printf("%d %d\n",votos_validos, votos_invalidos);

    if(percent > 51)
        printf("%d\n", votos_unidos[99].id);
    else 
        printf("Segundo turno\n");
    
    for(j = 0; j < 3; j++)
    {
        printf("%d", votos_unidos[sfe_offset[j] - 1].id);
        for(i = 2; i <= sfe[j]; i++)
            printf(" %d", votos_unidos[sfe_offset[j] -i].id);
        
        printf("\n");
    }
    
}

//===============================================================================
