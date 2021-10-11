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

void elege_candidato(int max_eleitos, int* votos, int inicio, int fim) {
    int eleito;
    for (int i = 0; i < max_eleitos - 1; i++) {
        eleito = retorna_maior(votos, inicio, fim);
        votos[eleito] = 0;
        printf("%d ", eleito);
    }

    eleito = retorna_maior(votos, inicio, fim);
    printf("%d\n", eleito);
}

int main(int argc, char* argv[]) {
    omp_set_num_threads(THREADS);

    // ABRE ARQUIVO
    FILE* arquivo;
    arquivo = fopen(argv[1], "rb");
    //arquivo = fopen("teste.txt", "rb");
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
    int max_senadores, max_federais, max_estaduais;
    int byte_atual = 0;

    max_senadores = atoi(&buffer[byte_atual]);  // lê o 123 (obs: atoi para sozinho no primeiro char que nao é número)

    while (buffer[byte_atual] != ' ') {  // incrementa o byte ate o primeiro espaço (após o 3)
        byte_atual++;
    }
    byte_atual++;                              // vai pro byte depois do espaço (número 4)
    max_federais = atoi(&buffer[byte_atual]);  // lê o 456

    while (buffer[byte_atual] != ' ') {  // chega ate o proximo espaço (após do 6)
        byte_atual++;
    }
    byte_atual++;                               // vai pro byte depois do espaço (número 7)
    max_estaduais = atoi(&buffer[byte_atual]);  // lê o 789

    while (buffer[byte_atual] != '\n') {  // chega ate a quebra de linha
        byte_atual++;
    }
    byte_atual++;  // marca o inicio dos votos

    // FAZER A LEITURA PARALELA DO ARQUIVO
    int total_geral = 0, total_invalidos = 0;
    int votos[99999] = {0};
    int voto;

    #pragma omp parallel for private(voto) shared(votos) reduction(+:total_geral) reduction(+:total_invalidos)
    for (int byte = byte_atual; byte < leitura; byte++) {
        // quando as threads iniciam no meio de um número, passa pro proximo e deixa o numero para a thread anterior
        if (buffer[byte - 1] != '\n') {
            continue;
        }

        // quando estiver no primeiro digito de um numero, o atoi irá ler até o \n
        // na proxima iteracao o while pula o numero que foi lido ate o \n para ler o proximo
        voto = atoi(&buffer[byte]);
        if (voto < 0) {
            total_invalidos++;
        } else if (voto > 0) {
            #pragma omp atomic
            votos[voto]++;

            total_geral++;
        }
        // voto == 0 é quando se faz atoi(EOF)
    }

    printf("%d %d\n", total_geral, total_invalidos);

    // PRESIDENTE TEM REGRA ESPECIFICA
    elege_presidente(votos);
    // SENADORES FEDERAIS E ESTADUAIS SEGUEM A MESMA REGRA
    elege_candidato(max_senadores, votos, 100, 999);
    elege_candidato(max_federais, votos, 1000, 9999);
    elege_candidato(max_estaduais, votos, 10000, 99999);

    fclose(arquivo);
    free(buffer);
    return 0;
}
