#include <stdio.h>
#include <stdlib.h>

typedef struct candidato
{
    int nCandidato;
    int qtdVotos;
} Candidato;


//Definicoes Gerais
typedef int item;
#define key(A) (A)
#define less(A,B) (key(A) > key(B))
#define exch(A,B) {item t=A; A=B, B=t;}
#define cmpexch(A,B) {if(less(B,A)) exch(A,B);}

//Funcoes
void ordena(Candidato *v, int l, int r);
void merge(Candidato *v, int l, int r1, int r2);

int main()
{
    int nSenadores, nDepFederal, nDepEstadual;
    int votoP = 0, votoS = 0, votoF = 0, votoE = 0, votoInvalido = 0;
    int voto = 1;

    Candidato presidente[100] = {0,0};
    Candidato senador[1000] = {0,0};
    Candidato depFederal[10000] = {0,0};
    Candidato depEstadual[100000] = {0,0};

    scanf("%d %d %d", &nSenadores, &nDepFederal, &nDepEstadual);

    while (voto != 0)
    {
        scanf("%d", &voto);
        if (voto < 10){
            votoInvalido++;
        }
        else
        {
            if (voto < 100){
                presidente[voto].nCandidato = voto;
                presidente[voto].qtdVotos++;
                votoP++;
            }
            else if(voto < 1000){
                senador[voto].nCandidato = voto;
                senador[voto].qtdVotos++;
                votoS++;
            }
            else if(voto < 10000){
                depFederal[voto].nCandidato = voto;
                depFederal[voto].qtdVotos++;
                votoF++;
            }
            else{
                depEstadual[voto].nCandidato = voto;
                depEstadual[voto].qtdVotos++;
                votoE++;
            }
        }
    }
        // Ordenar em ordem decrescente de acordo com qtdVotos.
        ordena(presidente, 0, 99);
        ordena(senador, 0, 999);
        ordena(depFederal, 0, 9999);
        ordena(depEstadual, 0, 99999);

        printf("%d %d\n", (votoP+votoE+votoF+votoS), votoInvalido-1);

        printf("%d\n", presidente[0].nCandidato);
        
        for(int i = 0; i < nSenadores; i++)
            printf("%d ", senador[i].nCandidato);
        printf("\n");
        
        for(int i = 0; i < nDepFederal; i++)
            printf("%d ", depFederal[i].nCandidato);
        printf("\n");

        for(int i = 0; i < nDepEstadual; i++)
            printf("%d ", depEstadual[i].nCandidato);
        printf("\n");

    return 0;
}

void ordena(Candidato *v, int l, int r){
    if(l>=r) return;
    int meio = (r+l)/2;
    ordena(v, l, meio);
    ordena(v, meio+1, r);
    merge(v, l, meio, r);
}

void merge(Candidato *v, int l, int r1, int r2){
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