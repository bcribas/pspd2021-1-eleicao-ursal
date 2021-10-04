#include <stdio.h>
#include <stdlib.h>

int retorna_maior(int *vetor, int inicio, int fim)
{
    int maior = vetor[inicio];
    int cod = inicio;

    for (int i = inicio; i < fim; ++i)
    {
        if (vetor[i] > maior || (vetor[i] == maior && i > cod))
        {
            maior = vetor[i];
            cod = i;
        }
    }

    return cod;
}

int retorna_maior_presidente(int *vetor, int inicio, int fim)
{
    int maior = vetor[inicio];
    int cod = inicio;
    int votos_presidente = 0;

    for (int i = inicio; i < fim; i++)
    {
        if (vetor[i] > 0)
            votos_presidente += vetor[i];
        if (vetor[i] > maior)
        {
            maior = vetor[i];
            cod = i;
        };
    }
    if (maior < (votos_presidente * 0.51))
    {
        return -1;
    }

    return cod;
}

void elege_presidente(int *votos)
{

    int eleito = retorna_maior_presidente(votos, 10, 99);

    if (eleito != -1)
    {
        printf("%d\n", eleito);
    }
    else
    {
        printf("Segundo Turno\n");
    }
}

void elege_candidato(int max_eleitos, int *votos, int inicio, int fim)
{
    int eleito;
    for (int i = 0; i < max_eleitos - 1; i++)
    {
        eleito = retorna_maior(votos, inicio, fim);
        votos[eleito] = 0;
        printf("%d ", eleito);
    }

    eleito = retorna_maior(votos, inicio, fim);
    printf("%d\n", eleito);
}

int main()
{
    int max_senadores, max_federais, max_estaduais; // NUMERO MAXIMO DE CANDIDATOS QUE PODEM SER ELEITOS
    int total_presidente = 0, total_geral = 0, total_invalidos = 0;
    int voto, digitos;
    int votos[99999] = {0};

    scanf("%d %d %d", &max_senadores, &max_federais, &max_estaduais);
    while (scanf("%d", &voto) != EOF)
    {
        if (voto < 0)
        {
            total_invalidos++;
            continue;
        }
        else
        {
            total_geral++;
        }

        votos[voto]++;
    }
    printf("%d %d\n", total_geral, total_invalidos);
    // PRESIDENTE TEM REGRA ESPECIFICA
    elege_presidente(votos);
    // SENADORES FEDERAIS E ESTADUAIS SEGUEM A MESMA REGRA
    elege_candidato(max_senadores, votos, 100, 999);
    elege_candidato(max_federais, votos, 1000, 9999);
    elege_candidato(max_estaduais, votos, 10000, 99999);

    return 0;
}