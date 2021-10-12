#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef THREADS
    #define THREADS 4
#endif

int retorna_maior(int* vetor, int inicio, int fim) {
    int maior = vetor[inicio];
    int cod = inicio;

    for (int i = inicio; i < fim; ++i) {
        if (vetor[i] > maior || (vetor[i] == maior && i > cod)) {
            maior = vetor[i];
            cod = i;
        }
    }

    return cod;
}

int retorna_maior_presidente(int* vetor, int inicio, int fim) {
    int maior = vetor[inicio];
    int cod = inicio;
    int votos_presidente = 0;

    for (int i = inicio; i < fim; i++) {
        if (vetor[i] > 0)
            votos_presidente += vetor[i];
        if (vetor[i] > maior) {
            maior = vetor[i];
            cod = i;
        };
    }
    if (maior < (votos_presidente * 0.51)) {
        return -1;
    }

    return cod;
}

void elege_presidente(int* votos) {
    int eleito = retorna_maior_presidente(votos, 10, 99);

    if (eleito != -1) {
        printf("%d\n", eleito);
    } else {
        printf("Segundo turno\n");
    }
}

void elege_candidato(int* eleitos, int max_eleitos, int* votos, int inicio, int fim) {
    int eleito;

    for (int i = 0; i < max_eleitos; i++) {
        eleito = retorna_maior(votos, inicio, fim);
        votos[eleito] = 0;
        eleitos[i] = eleito;
    }
}

void print_eleitos(int* eleitos, int max_eleitos){
    for (int i = 0; i < max_eleitos - 1; i++)
        printf("%d ", eleitos[i]);
    printf("%d\n", eleitos[max_eleitos - 1]);
}

int main(int argc, char* argv[]) {
    omp_set_num_threads(THREADS);

    // ABRE ARQUIVO
    FILE* arquivo;
    arquivo = fopen(argv[1], "rb");
    if (arquivo == NULL) {
        fputs("File error", stderr);
        exit(1);
    }

    // CALCULA O TAMANHO
    long long tamanho_arquivo;
    fseek(arquivo, 0, SEEK_END);
    tamanho_arquivo = ftell(arquivo);
    rewind(arquivo);

    // ALOCA MEMORIA PARA O ARQUIVO
    char* buffer;
    buffer = (char*)malloc(sizeof(char) * tamanho_arquivo);
    if (buffer == NULL) {
        fputs("Memory error", stderr);
        exit(2);
    }

    // COPIA ARQUIVO PARA O BUFFER
    size_t leitura;
    leitura = fread(buffer, 1, tamanho_arquivo, arquivo);
    if (leitura != tamanho_arquivo) {
        fputs("Reading error", stderr);
        exit(3);
    }

    // LÊ PRIMEIRA LINHA COM QUANTIDADES MÁXIMAS ex=(123 456 789\n)
    int max_senadores = 0, max_federais = 0, max_estaduais = 0;
    int byte_atual = 0;

    while (buffer[byte_atual] != ' ') { 
        max_senadores = (max_senadores*10) + (buffer[byte_atual] - '0');
        byte_atual++;
    }
    byte_atual++;  
    while (buffer[byte_atual] != ' ') { 
        max_federais = (max_federais*10) + (buffer[byte_atual] - '0');
        byte_atual++;
    }
    byte_atual++; 
    while (buffer[byte_atual] != '\n') { 
        max_estaduais = (max_estaduais*10) + (buffer[byte_atual] - '0');
        byte_atual++;
    }
    byte_atual++; 

    // FAZER A LEITURA PARALELA DO ARQUIVO
    int total_geral = 0, total_invalidos = 0;
    int votos[99999] = {0};
    int voto;

    #pragma omp parallel for private(voto) reduction(+:total_geral) reduction(+:total_invalidos) reduction(+:votos)
    for (int byte = byte_atual; byte < leitura; byte++) {
        if (buffer[byte - 1] != '\n') { 
            continue;
        }

        voto = 0;
        while (buffer[byte] != '\n') { 
            voto = (voto*10) + (buffer[byte] - '0');
            byte++;
        }
        
        if (voto < 0) {
            total_invalidos++;
        } else if (voto > 0) {
            votos[voto]++;
            total_geral++;
        }
    }
    fclose(arquivo);
    free(buffer);

    printf("%d %d\n", total_geral, total_invalidos);

    // ELEGER CANDIDATOS
    int presidente_eleito;
    int senadores_eleitos[max_senadores], federais_eleitos[max_federais], estaduais_eleitos[max_estaduais];

    #pragma omp parallel sections num_threads(THREADS)
    {
        #pragma omp section 
        {
            elege_presidente(votos);
        }
        #pragma omp section 
        {
            elege_candidato(senadores_eleitos, max_senadores, votos, 100, 999);
        }
        #pragma omp section 
        {
            elege_candidato(federais_eleitos, max_federais, votos, 1000, 9999);
        }
        #pragma omp section 
        {
            elege_candidato(estaduais_eleitos, max_estaduais, votos, 10000, 99999);
        }
    }
    
    print_eleitos(senadores_eleitos, max_senadores);
    print_eleitos(federais_eleitos, max_federais);
    print_eleitos(estaduais_eleitos, max_estaduais);

    return 0;
}
