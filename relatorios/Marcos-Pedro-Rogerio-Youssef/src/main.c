#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <math.h>
#include <string.h>
#define MAX_ARRAY_SIZE 100000
#ifndef THREADS
    #define THREADS 1
#endif
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
int get_start_position(int thread, FILE * file, int votes_start_position, int chunk_size, int threads){
    long int position = votes_start_position;
    for(int i = 0; i < thread; i++){
        position+=chunk_size;
    }
    if(thread>0 && thread < threads){
        char line[7];
        fseek(file, position, SEEK_SET);
        fgets(line, 10, file);
        position+=strlen(line);
    }
    return position;
}
int get_end_position(int thread, FILE * file, int votes_start_position, int chunk_size, int threads){
    return get_start_position(thread+1, file, votes_start_position, chunk_size, threads);
}
int main(int argc, char *argv[])
{
    omp_set_num_threads(THREADS);
    const char *in = argv[1];
    const int size_p = 100, size_s = 1000, size_f = 10000, size_e = 100000;

    int **matrix = calloc(16, 1 + sizeof(int *));
    for (int i = 0; i < 16; i++)
    {
        matrix[i] = calloc(MAX_ARRAY_SIZE, sizeof(int));
    }
    int *votos = calloc(MAX_ARRAY_SIZE, sizeof(int));

    int threads;
    int quantidade_p = 0;
    long int validos = 0, invalidos = 0;
    char *chunk_file[16];
    long int s, f, e;
#pragma omp parallel reduction(+ \
                               : validos, invalidos, quantidade_p)
    {
        
        FILE *read_file = fopen(in, "r");
        fscanf(read_file, "%ld %ld %ld\n", &s, &f, &e);
        int votes_start_position = ftell(read_file);
        fseek(read_file, 0L, SEEK_END);
        long int votes_end_position = ftell(read_file);
        long int bytes_size = votes_end_position - votes_start_position;
        threads = omp_get_num_threads();
        int chunk_size = (int)floor(bytes_size / threads);
        int voto;
        int thread_num = omp_get_thread_num();
        int start_position = get_start_position(thread_num, read_file, votes_start_position, chunk_size, threads);
        int end_position = get_end_position(thread_num, read_file, votes_start_position, chunk_size, threads);
        
        fseek(read_file, start_position, SEEK_SET);
        char *line;
        line = malloc(6);
        size_t size = sizeof(int);
        int total_read = start_position;
        while (fscanf(read_file, "%d%n", &voto, &size) != EOF)
        {
            total_read+=size;
            if (total_read > end_position && thread_num<(threads-1))
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
                #pragma omp atomic
                matrix[thread_num][voto]++;
            }
            else
            {
                invalidos++;
            }
        }
        free(line);
        fclose(read_file);
    }
    int soma = 0;
    for (int i = 0; i < threads; i++)
    {
        for (int j = 0; j < MAX_ARRAY_SIZE; j++)
        {
            votos[j] += matrix[i][j];
            soma += matrix[i][j];
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
    free(matrix);
    free(votos);
    return 0;
}