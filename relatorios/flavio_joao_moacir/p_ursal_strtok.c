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

int *votos_presidente;
int *votos_validos;
int *votos_invalidos;

int num_th;
int **matriz_votos_global;
Candidato *votos_unidos;

int compara_votos (const void *x, const void *y);
void ler_arquivo(int file_size, int l1, char *arquivo);
void unificar_resultado();
void ordenar_printar(int *sfe);

// ================================  Main ====================================== 
int main( int argc, char *argv[] )
{
    int sum, i, fsize, l1;
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
    l1 = ftell(fp);
   
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    fclose(fp);

//==============================================================================
    # pragma omp parallel num_threads(num_th)
        ler_arquivo(fsize, l1, argv[1]);
    
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
void ler_arquivo(int file_size, int l1, char *arquivo)
{
    int i, id_th, voto, chunk, init, fim, buf_size, buf_size_th;
    char *p_split = NULL; 
    char *p_save = NULL;
    char *buffer;       // buffer[fsize]      //buffer tamanho total do arquivo
    
    int votos_invalidos_local = 0,              
        votos_validos_local = 0,
        votos_presidente_local = 0; 

    id_th = omp_get_thread_num();  
    FILE *fp = fopen(arquivo, "r");

    // definir blocos/chunks de leitura
    chunk = (file_size - l1) / num_th;   
    init = (chunk * id_th) + l1;  
    buf_size_th = (file_size-l1) / num_th;

    fseek(fp, init, SEEK_SET);
    while(fgetc(fp) != '\n');

    buffer = (char *) malloc(sizeof(char) * buf_size_th * 2);
    fread(buffer, buf_size_th, 1, fp);

   for (p_split = strtok_r(buffer, "\n", &p_save); 
        p_split != NULL; 
        p_split = strtok_r(NULL, "\n", &p_save))
   {
        voto = atoi(p_split);
        if(voto > 0)
        {   
            votos_validos_local++;
            matriz_votos_global[id_th][voto]++;

            if( voto < 100)
                votos_presidente_local++;
        }
        else
            votos_invalidos_local++; 

   }  
    votos_invalidos[id_th]  = votos_invalidos_local;
    votos_presidente[id_th] = votos_presidente_local;
    votos_validos[id_th]    = votos_validos_local;

    fclose(fp);
}

//===============================================================================
void unificar_resultado()
{
    int sum, validos, i, j;
    validos = 0;

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
            validos += sum; 
        }
    }

    for(i = 0; i < num_th; i++)
    {
        votos_invalidos[num_th + 1]  += votos_invalidos[i];
        votos_validos[num_th + 1]    += votos_validos[i];
        votos_presidente[num_th + 1] += votos_presidente[i];
    }
}
//===============================================================================
void ordenar_printar(int *sfe)
{
    int sfe_offset[3], i, j;
    float percent;

    sfe_offset[0] = s_offset;
    sfe_offset[1] = f_offset;
    sfe_offset[2] = e_offset;

    qsort(votos_unidos           ,  p_offset            , sizeof(Candidato), compara_votos);
    qsort(votos_unidos + p_offset, (s_offset - p_offset), sizeof(Candidato), compara_votos);
    qsort(votos_unidos + s_offset, (f_offset - s_offset), sizeof(Candidato), compara_votos);
    qsort(votos_unidos + f_offset, (e_offset - f_offset), sizeof(Candidato), compara_votos);

    percent = (votos_unidos[99].cont_votos * 100) / votos_presidente[num_th + 1];
    
    printf("%d %d\n",votos_validos[num_th + 1], votos_invalidos[num_th + 1]);
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
