#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

typedef struct Candidato
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

Candidato results[16][100000] = {0, 0};

// Funcoes
void ordena(Candidato *v, int l, int r);
void merge(Candidato *v, int l, int r1, int r2);
void printResult(Candidato *final, double maioriaPresidente, int nSenadores, int nDepDeferal, int nDepEsadual);

int main(int argc, char **argv)
{
    // Especificar numero de threads
    int nThreads = 8;
    if (nThreads > omp_get_max_threads())
    {
        nThreads = omp_get_max_threads();
    }

    int nSenadores, nDepFederal, nDepEstadual;
    int votoPresidente = 0, votoValido = 0, votoInvalido = 0, voto;
    double maioriaPresidente;

    int threadStartingPoints[nThreads];
    int startingPosition, chunkSize;
    long int endPosition, fileSize;

    FILE *inputFile;
    inputFile = fopen(argv[1], "r");

    // Calculando byte inicial, byte final, bytes a serem lidos e divisão do arquivo em blocos
    // de acordo com o numero de threads
    fseek(inputFile, 0, SEEK_END);
    endPosition = ftell(inputFile);
    rewind(inputFile);

    fscanf(inputFile, "%d %d %d\n", &nSenadores, &nDepFederal, &nDepEstadual);

    startingPosition = ftell(inputFile);

    fileSize = endPosition - startingPosition;

    chunkSize = (int)(fileSize / nThreads);

    char character;
    threadStartingPoints[0] = startingPosition;

    for (int i = 1; i < nThreads; i++)
    {
        fseek(inputFile, threadStartingPoints[i - 1] + chunkSize, SEEK_SET);

        // Busca o final da linha anterior
        fscanf(inputFile, "%c", &character);
        while (character != '\n' && ftell(inputFile) < endPosition)
        {
            fseek(inputFile, ftell(inputFile) - 2, SEEK_SET);
            fscanf(inputFile, "%c", &character);
        }
        threadStartingPoints[i] = ftell(inputFile);
    }
    fclose(inputFile);

    // Especificando o numero de threads
    omp_set_dynamic(0);
    omp_set_num_threads(nThreads);

#pragma omp parallel private(voto) reduction(+ \
                                             : votoInvalido, votoValido, votoPresidente)
    {
        FILE *inputFile = fopen(argv[1], "r");

        int currentThread = omp_get_thread_num();
        int currentPointer = threadStartingPoints[currentThread];
        int endPointer = threadStartingPoints[currentThread + 1];
        if (currentThread == nThreads - 1)
        {
            endPointer = endPosition;
        }

        fseek(inputFile, currentPointer, SEEK_SET);
        int size_read;

        while (fscanf(inputFile, "%d%n", &voto, &size_read) != EOF)
        {
            // Evita que uma thread invada o espaço de leitura da proxima
            currentPointer += size_read;
            if (currentPointer >= endPointer)
            {
                break;
            }

            if (voto < 10)
            {
                votoInvalido++;
            }
            else
            {
                results[currentThread][voto].nCandidato = voto;
                results[currentThread][voto].qtdVotos += 1;
                votoValido++;
                if (voto < 100)
                    votoPresidente++;
            }
        }
    }

    Candidato final[100000] = {0};

    /*Cada thread escreve os resultados lidos em uma linha da matriz results,
    evitando concorrencia de acesso. Aqui, os valores lidos sao somados e atribuidos
    ao vetor final, que passa a possuir o resultado final dos votos*/
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 100000; j++)
        {
            if (results[i][j].qtdVotos > 0)
            {
                final[j].nCandidato = results[i][j].nCandidato;
                final[j].qtdVotos += results[i][j].qtdVotos;
            }
        }
    }

    // Ordena em ordem decrescente de acordo com qtdVotos
    ordena(final, 0, 99);
    ordena(final, 100, 999);
    ordena(final, 1000, 9999);
    ordena(final, 10000, 99999);

    printf("%d %d\n", votoValido, votoInvalido);

    maioriaPresidente = votoPresidente * 0.51;

    printResult(final, maioriaPresidente, nSenadores, nDepFederal, nDepEstadual);

    return 0;
}

void printResult(Candidato *final, double maioriaPresidente, int nSenadores, int nDepFederal, int nDepEstadual)
{
    if (final[0].qtdVotos > maioriaPresidente)
        printf("%d\n", final[0].nCandidato);
    else
        printf("Segundo turno\n");

    printf("%d", final[100].nCandidato);
    for (int i = 101; i < 100 + nSenadores; i++)
        printf(" %d", final[i].nCandidato);
    printf("\n");

    printf("%d", final[1000].nCandidato);
    for (int i = 1001; i < 1000 + nDepFederal; i++)
        printf(" %d", final[i].nCandidato);
    printf("\n");

    printf("%d", final[10000].nCandidato);
    for (int i = 10001; i < 10000 + nDepEstadual; i++)
        printf(" %d", final[i].nCandidato);
    printf("\n");
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