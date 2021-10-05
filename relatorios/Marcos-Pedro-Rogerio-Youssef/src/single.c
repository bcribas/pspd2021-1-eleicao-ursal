#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#define MAX_ARRAY_SIZE 100000
int encontrar_candidato(int *votos, long int size)
{
    unsigned int maior = 0;
    unsigned int posicao_maior = 0;
    {
        for (int i = size; i >= size / 10; i--)
        {
            if (votos[i] > maior)
            {
                maior = votos[i];
                posicao_maior = i;
        }
    }
    votos[posicao_maior] = 0;
    //printf("%d\n", omp_get_thread_num());
}
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
int main()
{
    long int s, f, e;
    const int size_p = 100, size_s = 1000, size_f = 10000, size_e = 100000;
    scanf("%ld %ld %ld", &s, &f, &e);
    int *votos = calloc(MAX_ARRAY_SIZE, sizeof(int));
    long int voto;

    int quantidade_p = 0;
    long int validos = 0, invalidos = 0;
    while (scanf("%ld", &voto) != EOF)
    {
        if (voto > 0)
        {
            //presidente
            if (((float)voto) / size_p <= 1.0)
            {
                quantidade_p++;
            }

            votos[voto]++;
            validos++;
        }
        else
        {
            invalidos++;
        }
    }
    //validos e invalidos
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