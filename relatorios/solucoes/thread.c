#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <math.h>

typedef struct candidato
{
    int nCandidato;
    int qtdVotos;
} Candidato;

//Definicoes Gerais
typedef int item;
#define key(A) (A)
#define less(A, B) (key(A) > key(B))
#define exch(A, B)    \
    {                 \
        item t = A;   \
        A = B, B = t; \
    }
#define cmpexch(A, B)   \
    {                   \
        if (less(B, A)) \
            exch(A, B); \
    }

//Funcoes
void ordena(Candidato *v, int l, int r);
void merge(Candidato *v, int l, int r1, int r2);

int main(int argc, char *argv[])
{
    FILE *inputFile;

    int nSenadores, nDepFederal, nDepEstadual;
    int votoInvalido = 0, votoValido = 0, votoP = 0;
    int voto;
    double metade;

    Candidato presidente[100] = {0, 0};
    Candidato senador[1000] = {0, 0};
    Candidato depFederal[10000] = {0, 0};
    Candidato depEstadual[100000] = {0, 0};

    int num_threads = omp_get_max_threads();

    FILE *file = fopen(argv[1], "r");

    int start_position;

    fscanf(file, "%d %d %d%n", &nSenadores, &nDepFederal, &nDepEstadual, &start_position);

    start_position += 1;

    fseek(file, 0L, SEEK_END);
    long int end_position = ftell(file) + 1;
    fseek(file, start_position, SEEK_SET);


    int *thread_starts = calloc(num_threads + 1, sizeof(long int));
    thread_starts[0] = start_position;
    thread_starts[num_threads] = end_position;

    long int size = end_position - start_position;
    int chunk_size = (int)floor(size / num_threads);
    
    for (int i = 1; i < num_threads; i++)
    {
        fseek(file, thread_starts[i - 1] + chunk_size, SEEK_SET);
        char line[10];
        fgets(line, 10, file);
        thread_starts[i] = thread_starts[i - 1] + chunk_size + strlen(line);
    }

    for(int i = 0; i < num_threads; i++)
        printf("%d - %d\n", i, thread_starts[i]);
    printf("\n");

    fclose(file);

#pragma omp parallel reduction(+: votoInvalido, votoValido, votoP)
    {
        FILE *file = fopen(argv[1], "r");
        long int start, end;

        start = thread_starts[omp_get_thread_num()];
        end = thread_starts[omp_get_thread_num()+1];

        fseek(file, start, SEEK_SET);

        int size, vote;

        long int counter = start;
        
        while (fscanf(file, "%d%n", &voto, &size) != EOF)
        {
            counter += size;
            if(counter >= end){
                break;
            }

            if (voto >= 10)
            {
                votoValido++;
                if (voto < 100)
                {
                    #pragma omp critical
                    {
                    votoP++;
                    presidente[voto].nCandidato = voto;
                    presidente[voto].qtdVotos++;
                    }
                }
                else if (voto < 1000)
                {
                    #pragma omp critical
                    {
                    senador[voto].qtdVotos++;
                    senador[voto].nCandidato = voto;
                    }
                }
                else if (voto < 10000)
                {
                    #pragma omp critical
                    {
                    depFederal[voto].qtdVotos++;
                    depFederal[voto].nCandidato = voto;
                    }
                }
                else
                {
                    #pragma omp critical
                    {
                    depEstadual[voto].qtdVotos++;
                    depEstadual[voto].nCandidato = voto;
                    }
                }
            } else votoInvalido++;
        }
    }

    // Ordenar em ordem decrescente de acordo com qtdVotos.
    ordena(presidente, 0, 99);
    ordena(senador, 0, 999);
    ordena(depFederal, 0, 9999);
    ordena(depEstadual, 0, 99999);

    printf("%d %d\n", votoValido, votoInvalido);

    metade = votoP * 0.51;

    if (presidente[0].qtdVotos > metade)
        printf("%d\n", presidente[0].nCandidato);
    else
        printf("Segundo turno\n");

    printf("%d", senador[0].nCandidato);
    for (int i = 1; i < nSenadores; i++)
        printf(" %d", senador[i].nCandidato);
    printf("\n");

    printf("%d", depFederal[0].nCandidato);
    for (int i = 1; i < nDepFederal; i++)
        printf(" %d", depFederal[i].nCandidato);
    printf("\n");

    printf("%d", depEstadual[0].nCandidato);
    for (int i = 1; i < nDepEstadual; i++)
        printf(" %d", depEstadual[i].nCandidato);
    printf("\n");

    return 0;
}

void ordena(Candidato *v, int l, int r)
{
    if (l >= r)
        return;
    int meio = (r + l) / 2;
    ordena(v, l, meio);
    ordena(v, meio + 1, r);
    merge(v, l, meio, r);
}

void merge(Candidato *v, int l, int r1, int r2)
{
    Candidato v2[100000];
    int k = 0;
    int i = l;
    int j = r1 + 1;

    while (i <= r1 && j <= r2)
    {
        if (less(v[i].qtdVotos, v[j].qtdVotos))
            v2[k++] = v[i++];
        else
            v2[k++] = v[j++];
    }

    while (i <= r1)
        v2[k++] = v[i++];
    while (j <= r2)
        v2[k++] = v[j++];

    k = 0;
    for (int i = l; i <= r2; i++)
    {
        v[i] = v2[k++];
    }

    return;
}