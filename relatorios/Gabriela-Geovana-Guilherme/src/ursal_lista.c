// INSIRA A ENTRADA COMPLETA NO ARQUIVO input.txt
// RODE COM Gcc ursal.c -o ursal && ./ursal < input.txt

#include <stdio.h>
#include <stdlib.h>

struct Candidato {
    int codigo;
    int votos;
    struct Candidato* prox;
};

int conta_digitos(int n) {
    int digitos = 0;

    while (n != 0) {
        n = n / 10;
        digitos++;
    }

    return digitos;
}

void insere_candidato_ordenado(struct Candidato** inicio, struct Candidato* novo) {
    // INSERE JÁ EM ORDEM DECRESCENTE DE VOTOS E CODIGO
    if ((*inicio) == NULL || novo->votos > (*inicio)->votos || (novo->votos == (*inicio)->votos && novo->codigo > (*inicio)->codigo)) {
        // JA ESTA NO LOCAL CERTO, PASSA A SER HEAD
        novo->prox = (*inicio);
        (*inicio) = novo;
    } else {
        // ITERAR ATE O LOCAL CERTO
        struct Candidato* temp = (*inicio);
        while (temp->prox != NULL && novo->votos < temp->prox->votos && (novo->votos == temp->prox->votos && novo->codigo < temp->prox->codigo)) {
            temp = temp->prox;
        }
        // COLOCAR NOVO CANDIDATO DEPOIS DO TEMP ATUAL (5, TEMP=4, NOVO=3, TEMP-PROX=2 ....)
        novo->prox = temp->prox;
        temp->prox = novo;
    }
}

struct Candidato* cria_candidato(int voto, int votos) {
    struct Candidato* novo = (struct Candidato*)malloc(sizeof(struct Candidato));

    novo->codigo = voto;
    novo->votos = votos;
    novo->prox = NULL;

    return novo;
}

int verifica_e_soma_existente(int voto, struct Candidato** inicio) {
    // SE JA EXISTE O CANDIDATO, SOMA AOS VOTOS, DELETA INSTANCIA, CRIA UMA NOVA, E INSERE ORDENADO ...
    // ...(O CERTO SERIA SÓ REMOVER E COLOCAR NO NOVO LOCAL)
    // SE NAO EXISTE RETORNA FALSO (=0)
    struct Candidato *candidato = *inicio;
    struct Candidato* anterior = NULL;
    struct Candidato* novo;
    int votos;
    
    while (candidato != NULL) {
        
        if (candidato->codigo == voto) {
            candidato->votos += 1;
            votos = candidato->votos;
    
            if (anterior == NULL) {
                // SE FOR O PRIMEIRO ITEM DA LISTA, NÃO TEM QUE ORDENAR POIS NAO TEM COMO SUBIR A LISTA
                return 1;
            } else {
                // REMOVE O ITEM
                anterior->prox = candidato->prox;
                free(candidato);
                // CRIA O NOVO COMO COPIA E INSERE ORDENADO
                novo = cria_candidato(voto, votos);
                insere_candidato_ordenado(inicio, novo);
                return 1;
            }
        }
        anterior = candidato;
        candidato = candidato->prox;
    }
    return 0;
}

void elege_presidente(int total_votos, struct Candidato* presidentes) {
    float minimo_para_vitoria = total_votos * 0.51;

    while (presidentes != NULL) {
        // PRECISA TER 51% DOS VOTOS OU MAIS PRA GANHAR
        if (presidentes->votos >= minimo_para_vitoria) {
            printf("%d\n", presidentes->codigo);
            return;
        }
        presidentes = presidentes->prox;
    }
    // SE NENHUM TEM 51% OU MAIS, ENTÃO É SEGUNDO TURNO
    printf("Segundo Turno\n");
}

void elege_candidato(int max_candidatos, struct Candidato* candidatos) {
    int quant = 0;

    // PRINTA ATÉ O PENULTIMO CANDIDATO A SER ELEITO
    // LEMBRANDO QUE A LISTA ESTEVE SEMPRE ORDENADA ENTAO OS QUE TEM MAIS VOTOS ...
    // ...ESTAO NO COMECO E ORDENADOS POR CODIGO EM CASO DE EMPATE
    while (candidatos != NULL && quant < max_candidatos - 1) {
        printf("%d ", candidatos->codigo);
        candidatos = candidatos->prox;
        quant += 1;
    }
    // PRINTA O ULTIMO CANDIDATO COM A QUEBRA DE LINHA
    printf("%d\n", candidatos->codigo);

}

void printar(struct Candidato* candidato, int tipo) {
    // FUNÇÃO SÓ PARA DEBBUGAR
    printf("Mostrando %d\n", tipo);
    while (candidato != NULL) {
        printf("%d=%d ", candidato->codigo, candidato->votos);
        candidato = candidato->prox;
    }
    printf("\n");
}

int main() {
    int max_senadores, max_federais, max_estaduais; // NUMERO MAXIMO DE CANDIDATOS QUE PODEM SER ELEITOS
    int total_presidente = 0, total_geral = 0, total_invalidos = 0;
    int voto, digitos;
    struct Candidato* presidentes = NULL; // LISTA DE PRESIDENTES QUE FORAM INSERIDOS NA ENTRADA
    struct Candidato* senadores = NULL;   // LISTA DE SENADORES QUE FORAM INSERIDOS NA ENTRADA
    struct Candidato* federais = NULL;    // LISTA DE FEDERAIS QUE FORAM INSERIDOS NA ENTRADA
    struct Candidato* estaduais = NULL;   // LISTA DE ESTADUAIS QUE FORAM INSERIDOS NA ENTRADA
    struct Candidato* novo;               // VARIAVEL TEMPORARIA PARA NOVOS CANDIDATOS

    scanf("%d %d %d", &max_senadores, &max_federais, &max_estaduais);

    while (scanf("%d", &voto) != EOF) {
        if (voto < 0) {
            total_invalidos += 1;
            continue;
        } else {
            total_geral += 1;
        }

        digitos = conta_digitos(voto);

        if (digitos == 2) {
            total_presidente += 1;
            if (!verifica_e_soma_existente(voto, &presidentes)){
                novo = cria_candidato(voto, 1);
                insere_candidato_ordenado(&presidentes, novo);
            }

        } else if (digitos == 3) {
            if (!verifica_e_soma_existente(voto, &senadores)){
                novo = cria_candidato(voto, 1);
                insere_candidato_ordenado(&senadores, novo);
            }

        } else if (digitos == 4) {
            if (!verifica_e_soma_existente(voto, &federais)){
                novo = cria_candidato(voto, 1);
                insere_candidato_ordenado(&federais, novo);
            }

        } else if (digitos == 5) {
            if (!verifica_e_soma_existente(voto, &estaduais)){
                novo = cria_candidato(voto, 1);
                insere_candidato_ordenado(&estaduais, novo);
            }
        }
    }

    // PARA DEBUGAR
    // printar(presidentes, 2);
    // printar(senadores, 3);
    // printar(federais, 4);
    // printar(estaduais, 5);

    printf("%d %d\n", total_geral, total_invalidos);

    // PRESIDENTE TEM REGRA ESPECIFICA
    elege_presidente(total_presidente, presidentes);
    // SENADORES FEDERAIS E ESTADUAIS SEGUEM A MESMA REGRA
    elege_candidato(max_senadores, senadores);
    elege_candidato(max_federais, federais);
    elege_candidato(max_estaduais, estaduais);

    return 0;
}