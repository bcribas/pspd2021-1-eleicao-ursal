// ------------------------------------------------------------------------------
// 	Professor  : Bruno Ribas
// 	Aluno     : Flavio Vieira 
// 	Matricula : 15/0125682  
//
//https://moj.naquadah.com.br/contests/bcr-EDA2-2021_1-hash/eleicao-ursal-big.html
// ------------------------------------------------------------------------------
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 100000
#define p_offset 100              //[0     - 99   ]   -> 100 
#define s_offset 1000             //[100   - 999  ]   -> 900
#define f_offset 10000            //[1000  - 9999 ]   -> 9k
#define e_offset 100000           //[10000 - 99999]   -> 90k    

typedef struct
{
    int id;
    int cont_votos;
}Candidato;

int comparaVotos (const void *x, const void *y);
void teste(Candidato *t);
void resultado(void);

// ================================  Main ======================================== 
int main(void)
{
    int i, j, voto, total_votos, votos_validos, votos_presidente;
    int sfe[3], sfe_offset[3];
    float p_percentual;
    
    Candidato registo_votos[MAX];
    memset(registo_votos, 0, sizeof(Candidato) * MAX);
    
    sfe_offset[0] = s_offset;
    sfe_offset[1] = f_offset;
    sfe_offset[2] = e_offset;

    for(i = 0; i < 3; i++)
        scanf("%d", &sfe[i]);
    
    
    total_votos = votos_validos = votos_presidente = 0;

    while (scanf("%d", &voto) != EOF)
    {
        if(voto > 0)
        {
            registo_votos[voto].id = voto;
            registo_votos[voto].cont_votos++;
            votos_validos++;
            if( voto < 100)
                votos_presidente++;
        }
        total_votos++;
    }
    
    qsort(registo_votos , p_offset, sizeof(Candidato), comparaVotos);
    qsort(registo_votos + p_offset, (s_offset - p_offset), sizeof(Candidato), comparaVotos);
    qsort(registo_votos + s_offset, (f_offset - s_offset), sizeof(Candidato), comparaVotos);
    qsort(registo_votos + f_offset, (e_offset - f_offset), sizeof(Candidato), comparaVotos);
    
//------------------------------------------------------------------------------
    
    p_percentual = (registo_votos[99].cont_votos * 100) / votos_presidente;

    printf("%d %d\n", votos_validos, total_votos - votos_validos);

    if(p_percentual > 51)
        printf("%d\n", registo_votos[99].id);
    else 
        printf("Segundo turno\n");

   for(j = 0; j < 3; j++)
    {
        printf("%d", registo_votos[sfe_offset[j] - 1].id);
        for(i = 2; i <= sfe[j]; i++)
            printf(" %d", registo_votos[sfe_offset[j] -i].id);
        
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