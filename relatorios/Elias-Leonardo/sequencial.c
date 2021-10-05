#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

int main(void) {
    int qtdVotosPresidente = 0, votosValidos = 0, invalidos = 0;

    int senadoresEleitos, depFederalEleitos, depEstaduaisEleitos, voto;

    scanf("%d %d %d", &senadoresEleitos, &depFederalEleitos, &depEstaduaisEleitos);

    while(scanf("%d", &voto) != EOF) {
        if (voto > 0) {
            votosValidos++;
            candidatos[voto].index = voto;
            candidatos[voto].votos++;
        }
        else {
            invalidos++;
        }
    }

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