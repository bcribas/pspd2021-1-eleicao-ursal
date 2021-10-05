#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <string.h>
#define MAX_ARRAY_SIZE 100000
int encontrar_candidato(int *votos, long int size)
{
    unsigned int maior = 0;
    unsigned int posicao_maior = 0;
    for (int i = size; i >= size / 10; i--)
    {
        if (votos[i] > maior)
        {
            maior = votos[i];
            posicao_maior = i;
        }
    }
    votos[posicao_maior] = 0;
    return posicao_maior;
}
float porcentagem_presidente(int *votos, int total)
{
    int size = 100;
    int maior = 0;
    for (int i = size - 1; i >= size / 10; i--)
    {
        if (votos[i] > maior)
        {
            maior = votos[i];
        }
    }
    return (float)maior / total;
}
int main(int argc, char *argv[])
{
    const char *in = argv[1];
    FILE *file = fopen(in, "r");

    long int s, f, e;
    const int size_p = 100, size_s = 1000, size_f = 10000, size_e = 100000;

    fscanf(file, "%ld %ld %ld\n", &s, &f, &e);
    int **matrix = calloc(16, 1 + sizeof(int *));
    for (int i = 0; i < 16; i++)
    {
        matrix[i] = calloc(MAX_ARRAY_SIZE, sizeof(int));
    }
    int *votos = calloc(MAX_ARRAY_SIZE, sizeof(int));

    long int votes_start_position = ftell(file);
    fseek(file, 0L, SEEK_END);
    long int votes_end_position = ftell(file);
    fseek(file, votes_start_position, SEEK_SET);
    int threads = omp_get_max_threads();
    long int size = votes_end_position - votes_start_position;
    int *threads_read_positions = calloc(threads + 1, sizeof(long int));
    threads_read_positions[threads] = votes_end_position - 1;
    int chunk_size = (int)floor(size / threads);
    int thread = 0;
    while (thread < threads)
    {
        char line[1000];
        if (thread == 0)
        {
            threads_read_positions[thread] = votes_start_position;
        }
        else
        {
            fseek(file, threads_read_positions[thread - 1] + chunk_size, SEEK_SET);
            fgets(line, 10, file);
            threads_read_positions[thread] = threads_read_positions[thread - 1] + chunk_size + strlen(line);
        }
        thread++;
    }
    fclose(file);
    int quantidade_p = 0;
    long int validos = 0, invalidos = 0;
    char *chunk_file[16];
#pragma omp parallel reduction(+ \
                               : validos, invalidos, quantidade_p)
    {
        FILE *read_file = fopen(in, "r");
        fseek(read_file, threads_read_positions[omp_get_thread_num()], SEEK_SET);
        char line[1000];
        int size, voto;
        while(fscanf(read_file, "%d%n", &voto, &size) != EOF)
        {
            if (ftell(read_file) > threads_read_positions[omp_get_thread_num() + 1])
            {
                break;
            }
            if (voto >= 10)
            {
                validos++;
                //presidente
                if (((float)voto) / size_p <= 1.0)
                {
                    quantidade_p++;
                }
                matrix[omp_get_thread_num()][voto]++;
            }
            else
            {
                invalidos++;
            }
        }
        fclose(read_file);
    }
    int soma = 0;
    for (int i = 0; i < omp_get_max_threads(); i++)
    {
        for (int j = 0; j < MAX_ARRAY_SIZE; j++)
        {
            votos[j] += matrix[i][j];
            soma+=matrix[i][j];
        }
    }
    printf("%ld %ld\n", validos, invalidos);
    if (porcentagem_presidente(votos, quantidade_p) > 0.50)
    {
        printf("%d\n", encontrar_candidato(votos, size_p));
    }
    else
    {
        printf("Segundo turno\n");
    }
    //senador
    for (int i = 1; i < s; i++)
    {
        printf("%d ", encontrar_candidato(votos, size_s));
    }
    printf("%d\n", encontrar_candidato(votos, size_s));
    //dep. federal
    for (int i = 1; i < f; i++)
    {
        printf("%d ", encontrar_candidato(votos, size_f));
    }
    printf("%d\n", encontrar_candidato(votos, size_f));
    //dep. estadual
    for (int i = 1; i < e; i++)
    {
        printf("%d ", encontrar_candidato(votos, size_e));
    }
    printf("%d\n", encontrar_candidato(votos, size_e));
    free(votos);
    return 0;
}