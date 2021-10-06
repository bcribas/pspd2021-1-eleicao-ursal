 
//  Resultados
//  file-011-big
//      4 thread  - 121,08s user 1,27s system 305% cpu 40,089 total
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
// =================================================================================
typedef struct
{
    int id;
    int cont_votos;
}Candidato;

int comparaVotos (const void *x, const void *y);
void resultado(void);

// ================================  Main ========================================== 
int main( int argc, char *argv[] )
{

    int max_threads;
    double start, wtime;
    wtime = start = omp_get_wtime();

    if(argc < 2)
    {
        fprintf(stderr, "[Erro]: Parametro <caminho/nome_do_arquivo> nao informado!\n");
        exit(EXIT_FAILURE);
    }

    if( argc == 3)
        max_threads = strtol(argv[2], NULL, 10);
    else
        max_threads = 1;                            //sem argv[2] -> sequencial

// =================================================================================
    int i, j, n, fsize, num_reg;
    int sfe[3];
    int sfe_offset[3];
    int num_votos, votos_val, votos_pres;
    float percent;
    FILE *fp;
    Candidato reg_votos_global[MAX];

    memset(reg_votos_global, 0, sizeof(Candidato) * MAX);
    fp = fopen(argv[1], "r");

    sfe_offset[0] = s_offset;
    sfe_offset[1] = f_offset;
    sfe_offset[2] = e_offset;
    
    num_votos = votos_val = votos_pres = num_reg = 0;

    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp) - 1;

    rewind(fp);
    for(i = 0; i < 3; i++)
        fscanf(fp, "%d", &sfe[i]);

//==============================================================================
    # pragma omp parallel num_threads(max_threads)
    {
        int id_th = omp_get_thread_num();
        int num_th = omp_get_num_threads();

        Candidato reg_votos_local[MAX];
        FILE *fp = fopen(argv[1], "r");

        int i, voto;
        int bloco, inicio, fim, resto;

        // definir blocos/chunks de leitura
        bloco = fsize / num_th;
        inicio = bloco * id_th;
        fim = inicio + bloco;

        if( id_th == num_th - 1)
        {
            resto = fsize % num_th;
            fim += resto;
        }
        fseek(fp, inicio, SEEK_SET);
        while(fgetc(fp) != '\n');

        while(inicio < fim)
        {
            fscanf(fp, "%d", &voto);
            if(voto > 0)
            {
                #pragma omp critical
                {
                    reg_votos_global[voto].cont_votos++;
                    votos_val++;
                }
                reg_votos_global[voto].id = voto;
                

                if( voto < 100){
                    #pragma omp critical
                        votos_pres++;
                }

            }
            #pragma omp critical
                num_votos++;

            inicio = ftell(fp);
        }
        fclose(fp);

    }
          // ponteiro posicao inicio    tamanho
    qsort(reg_votos_global           ,  p_offset            , sizeof(Candidato), comparaVotos);
    qsort(reg_votos_global + p_offset, (s_offset - p_offset), sizeof(Candidato), comparaVotos);
    qsort(reg_votos_global + s_offset, (f_offset - s_offset), sizeof(Candidato), comparaVotos);
    qsort(reg_votos_global + f_offset, (e_offset - f_offset), sizeof(Candidato), comparaVotos);
    
//------------------------------------------------------------------------------
    
    percent = (reg_votos_global[99].cont_votos * 100) / votos_pres;

    printf("%d %d\n", votos_val, num_votos - votos_val);

    if(percent > 51)
        printf("%d\n", reg_votos_global[99].id);
    else 
        printf("Segundo turno\n");
    
      for(j = 0; j < 3; j++)
    {
        printf("%d", reg_votos_global[sfe_offset[j] - 1].id);
        for(i = 2; i <= sfe[j]; i++)
            printf(" %d", reg_votos_global[sfe_offset[j] -i].id);
        
        printf("\n");
    }

    return (EXIT_SUCCESS);
}
// ===========================  Sub Programas ================================== 

int comparaVotos (const void *x, const void *y) 
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