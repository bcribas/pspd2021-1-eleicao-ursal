#include <stdio.h>
#include <string.h>
#include <stdlib.h>



/**
 * -> Abrir o arquivo via input de path no main argv -> OK
 * -> Verificar o tamanho do arquivo  -> OK
 * -> De forma síncrona, ler o arquivo e os inputs, vendo se o código tá ok -> OK
 * -> Atualizar implementação pra rodar no openmp
 * -> Gerar relatório 
 * 
 */

struct candidato
{
    int index;
    int votos;
} typedef Candidato;

static Candidato candidatos[99999] = { 0,0 };

int compare(const void *a, const void *b) { 
    Candidato *votoA = (Candidato*) a;
    Candidato *votoB = (Candidato*) b;

    if ((votoB->votos - votoA->votos) == 0)
        return votoB->index - votoA->index;
    return (votoB->votos - votoA->votos);
}

int main(int argc, char* argv[]) {
    int qtdVotosPresidente = 0, votosValidos = 0, invalidos = 0;

    int senadoresEleitos, depFederalEleitos, depEstaduaisEleitos, voto;

    ssize_t read;
    char * line = NULL;
    size_t len = 0;

    if (argc < 2) {
        return 1;
    }

    FILE *fptr;

    fptr = fopen(argv[1],"r");

    if (fptr == NULL) {
        printf("ERRO\n");
        return 1;
    }

    fseek(fptr, 0L, SEEK_END);
    int sz = ftell(fptr);

    rewind(fptr);
    fscanf(fptr, "%d %d %d", &senadoresEleitos, &depFederalEleitos, &depEstaduaisEleitos);

    fseek(fptr, sizeof(int)*3, SEEK_SET);

    for (int i = sizeof(int)*3; i < sz;) 
    {
        read = getline(&line, &len, fptr);
        int voto = atoi(line);
        if (voto > 0) {
            votosValidos++;
            candidatos[voto].index = voto;
            candidatos[voto].votos++;
        }
        else {
            invalidos++;
        }
        i+=read;
    }
    

    fclose(fptr);

    // TA OK 
    for (int i = 0; i < 100; i++) {
        qtdVotosPresidente += candidatos[i].votos;
    }

    qsort(candidatos, 99, sizeof(Candidato), compare);

    printf("%d %d\n", votosValidos, invalidos);

    if (candidatos[0].votos > (qtdVotosPresidente / 2)) 
        printf("%d\n",candidatos[0].index);
    else
        printf("Segundo turno\n");
    
    qsort(candidatos + 100, 900, sizeof(Candidato), compare);

    for (int i = 0; i < senadoresEleitos; i++) {
        printf("%d", candidatos[i + 100].index);
        if (i != (senadoresEleitos - 1))
            printf(" ");
    }

    printf("\n");

    qsort(candidatos + 1000, 9000, sizeof(Candidato), compare);

    for (int i = 0; i < depFederalEleitos; i++) {
        printf("%d", candidatos[i + 1000].index);
        if (i != (depFederalEleitos - 1))
            printf(" ");
    }

    printf("\n");

    qsort(candidatos + 10000, 90000, sizeof(Candidato), compare);

    for (int i = 0; i < depEstaduaisEleitos; i++) {
        printf("%d", candidatos[i + 10000].index);
        if (i != (depEstaduaisEleitos - 1))
            printf(" ");
    }

    printf("\n");


    return 0;
}