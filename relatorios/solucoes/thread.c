#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct candidato
{
    int nCandidato;
    int qtdVotos;
} Candidato;

// Definicoes gerais
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

Candidato results[17][100000] = {0, 0};

// Funcoes
void ordena(Candidato *v, int l, int r);
void merge(Candidato *v, int l, int r1, int r2);

int main(int argc, char **argv)
{
    FILE *inputFile;

    int nSenadores, nDepFederal, nDepEstadual;
    int votoP = 0, votoValido = 0, votoInvalido = 0;
    int voto = 0;
    double metade;
    int nThreads = omp_get_max_threads();
    int thread_starting_points[nThreads];

    Candidato presidente[100] = {0, 0};
    Candidato senador[1000] = {0, 0};
    Candidato depFederal[10000] = {0, 0};
    Candidato depEstadual[100000] = {0, 0};

    inputFile = fopen(argv[1], "r");

    fseek(inputFile, 0, SEEK_END);
    long int end_position = ftell(inputFile);
    rewind(inputFile);

    fscanf(inputFile, "%d %d %d\n", &nSenadores, &nDepFederal, &nDepEstadual);

    int starting_position = ftell(inputFile);

    long int fileSize = end_position - starting_position;

    int chunkSize = (int)(fileSize / nThreads);

    char temp;
    thread_starting_points[0] = starting_position;

    for (int i = 1; i < nThreads; i++)
    {
        fseek(inputFile, thread_starting_points[i - 1] + chunkSize, SEEK_SET);

        fscanf(inputFile, "%c", &temp);
        while (temp != '\n' && ftell(inputFile) < end_position)
        {
            fseek(inputFile, ftell(inputFile) - 2, SEEK_SET);
            fscanf(inputFile, "%c", &temp);
        }
        thread_starting_points[i] = ftell(inputFile);
    }
    fclose(inputFile);

#pragma omp parallel private(voto) reduction(+ \
                                             : votoInvalido, votoValido, votoP)
    {
        FILE *inputFile = fopen(argv[1], "r");

        int current_thread = omp_get_thread_num();
        int current_pointer = thread_starting_points[current_thread];
        int end_pointer = thread_starting_points[current_thread + 1];
        if (current_thread == omp_get_max_threads() - 1)
        {
            end_pointer = end_position;
        }

        fseek(inputFile, current_pointer, SEEK_SET);
        int size_read;

        while (fscanf(inputFile, "%d%n", &voto, &size_read) != EOF)
        {
            current_pointer += size_read;
            if (current_pointer >= end_pointer)
            {
                break;
            }
            if (voto < 10)
            {
                votoInvalido++;
            }
            else
            {
                results[current_thread][voto].nCandidato = voto;
                results[current_thread][voto].qtdVotos += 1;
                votoValido++;
                if (voto < 100)
                    votoP++;
            }
        }
    }

    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 100000; j++)
        {
            if (results[i][j].qtdVotos > 0)
            {
                results[16][j].nCandidato = results[i][j].nCandidato;
                results[16][j].qtdVotos += results[i][j].qtdVotos;
            }
        }
    }

    for (int i = 10; i < 100; i++)
    {
        presidente[i].nCandidato = results[16][i].nCandidato;
        presidente[i].qtdVotos = results[16][i].qtdVotos;
    }

    for (int i = 100; i < 1000; i++)
    {
        senador[i].nCandidato = results[16][i].nCandidato;
        senador[i].qtdVotos = results[16][i].qtdVotos;
    }

    for (int i = 1000; i < 10000; i++)
    {
        depFederal[i].nCandidato = results[16][i].nCandidato;
        depFederal[i].qtdVotos = results[16][i].qtdVotos;
    }

    for (int i = 10000; i < 100000; i++)
    {
        depEstadual[i].nCandidato = results[16][i].nCandidato;
        depEstadual[i].qtdVotos = results[16][i].qtdVotos;
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