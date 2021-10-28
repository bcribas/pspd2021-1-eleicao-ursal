#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct voto {
   int codigo_candidato;
   int qtd_votos;
} Voto;

void quickSortMediana(Voto *v, int esq, int dir, int qtd_a_eleger, int tamanho_sub_vetor);
int  separa(Voto *v, int esq, int dir);
void elege_politico(Voto *votos, int inicio, int fim, int qnt);
void elege_presidente(Voto *votos);

int main(int argc, char *argv[]){

    Voto *votos       = (Voto *) calloc(0b11000011010100000, sizeof(Voto));   // 0-99999
    for(int i=10; i<0b11000011010100000; i++)
      votos[i].codigo_candidato = i;

    int voto;
    int qnt_votos_presidente = 0;
    int qnt_invalidos        = 0;
    int qnt_validos          = 0;
    
    char *filename = argv[1];
    FILE *arquivo = fopen(filename, "r");
    
    int s, f, e;   // qnt senadores, dep.fed, dep.est para a eleicao
    fscanf(arquivo, "%d %d %d", &s, &f, &e);
    
    int ultimo_primeira_linha = ftell(arquivo); // ultimo byte primeira linha, ela precisa ser ignorada nas threads

    int total_bytes = 0;
    fseek(arquivo, 0, SEEK_END); // posiciona no final do arquivo para pegar o total de bytes
    total_bytes = ftell(arquivo) - ultimo_primeira_linha;

    #pragma omp parallel
    {
        // quantidade de bytes pelo número total de threas em execução
        int qnt_bytes_thread = total_bytes / omp_get_num_threads();

        FILE *arquivo = fopen(filename, "r");
        fseek(arquivo, ultimo_primeira_linha+(qnt_bytes_thread*omp_get_thread_num()), SEEK_SET);

        int voto = 0;
        int lixo = 0;
        int count = 0;
        int nova_posicao = 0;
        int posicao_atual = 0;
        
        #pragma omp parallel for private(voto) shared(votos)
        for (count = 0; count < qnt_bytes_thread; count++){
            posicao_atual = ftell(arquivo);
            fseek(arquivo, posicao_atual-1, SEEK_SET);

            if(fgetc(arquivo) == '\n'){
                fscanf(arquivo, "%d ", &voto);

                if(voto < 0){
                    qnt_invalidos++;
                }
                else{
                    qnt_validos++;
                    votos[voto].qtd_votos++;
                }
            }
            else{
                fseek(arquivo, posicao_atual-1, SEEK_SET);
                fscanf(arquivo, "%d ", &lixo);
            }
            int nova_posicao = ftell(arquivo);
            count += nova_posicao - posicao_atual; // incrementa qnt de bytes voto no ultimo scanf
            count--;
        }        

        fclose(arquivo);

    }
    printf("%d %d\n", qnt_validos, qnt_invalidos);


    elege_presidente(votos);
    elege_politico(votos, 100, 999, s);  // elege senadores
    elege_politico(votos, 1000, 9999, f);  // elege deputados federais
    elege_politico(votos, 10000, 99999, e);  // elege deputados estaduais

    return 0;
} 

void elege_presidente(Voto *votos){
    int pos_eleito = 0;
    char segundo_turno = 0b00000000; // boolean falso
    int qnt_votos_presidente = 0;

    for(int i=1; i<100; i++){
        if(votos[i].qtd_votos > 0){
            qnt_votos_presidente++;
        }

        if(votos[i].qtd_votos > votos[pos_eleito].qtd_votos){
            pos_eleito = i;
            segundo_turno = 0b00000000;
        }
        else if(votos[i].qtd_votos == votos[pos_eleito].qtd_votos){
            segundo_turno = 0b00000001;   // true
        }
    }

    if(segundo_turno || (votos[pos_eleito].qtd_votos <= (qnt_votos_presidente/2))){
        printf("Segundo turno\n");
    }
    else{
        printf("%d\n", pos_eleito);
    }
}

void elege_politico(Voto *votos, int inicio, int fim, int qnt){
   int tamanho_sub_vetor = (fim-inicio)+1;
   quickSortMediana(votos, inicio, fim, qnt, tamanho_sub_vetor);

   printf("%d", votos[fim].codigo_candidato);
   qnt --;
   for(int i=fim-1; qnt>0; i--){
      printf(" %d", votos[i].codigo_candidato);
      qnt --;
   }
   printf("\n");

   return;
}


int separa(Voto *v, int esq, int dir){
    Voto c;
    c.codigo_candidato = v[dir].codigo_candidato;
    c.qtd_votos = v[dir].qtd_votos;
    int j = esq;
    for (int k = esq; k < dir; k++)
    {
        if (v[k].qtd_votos <= c.qtd_votos)
        {
            Voto tmp;
            tmp.codigo_candidato = v[k].codigo_candidato;
            tmp.qtd_votos = v[k].qtd_votos;
            v[k].codigo_candidato = v[j].codigo_candidato;
            v[k].qtd_votos = v[j].qtd_votos;
            v[j].codigo_candidato = tmp.codigo_candidato;
            v[j].qtd_votos = tmp.qtd_votos;
            j++;
        }
    } //end for
    Voto tmp;
    tmp.codigo_candidato = v[j].codigo_candidato;
    tmp.qtd_votos = v[j].qtd_votos;
    v[j].codigo_candidato = v[dir].codigo_candidato;
    v[j].qtd_votos = v[dir].qtd_votos;
    v[dir].codigo_candidato = tmp.codigo_candidato;
    v[dir].qtd_votos = tmp.qtd_votos;
    return j;
}

void quickSortMediana(Voto *v, int esq, int dir, int qtd_a_eleger, int tamanho_sub_vetor){
   if (dir <= esq)
       return;

   if (v[dir].qtd_votos < v[(dir + esq) / 2].qtd_votos)
   {
       Voto tmp;
       tmp.codigo_candidato = v[dir].codigo_candidato;
       tmp.qtd_votos = v[dir].qtd_votos;
       v[dir].codigo_candidato = v[(dir + esq) / 2].codigo_candidato;
       v[dir].qtd_votos = v[(dir + esq) / 2].qtd_votos;
       v[(dir + esq) / 2].codigo_candidato = tmp.codigo_candidato;
       v[(dir + esq) / 2].qtd_votos = tmp.qtd_votos;
   }

   if (v[(dir + esq) / 2].qtd_votos < v[esq].qtd_votos)
   {
       Voto tmp;
       tmp.codigo_candidato = v[(dir + esq) / 2].codigo_candidato;
       tmp.qtd_votos = v[(dir + esq) / 2].qtd_votos;
       v[(dir + esq) / 2].codigo_candidato = v[esq].codigo_candidato;
       v[(dir + esq) / 2].qtd_votos = v[esq].qtd_votos;
       v[esq].codigo_candidato = tmp.codigo_candidato;
       v[esq].qtd_votos = tmp.qtd_votos;
   }

   if (v[(dir + esq) / 2].qtd_votos < v[dir].qtd_votos)
   {
       Voto tmp;
       tmp.codigo_candidato = v[(dir + esq) / 2].codigo_candidato;
       tmp.qtd_votos = v[(dir + esq) / 2].qtd_votos;
       v[(dir + esq) / 2].codigo_candidato = v[dir].codigo_candidato;
       v[(dir + esq) / 2].qtd_votos = v[dir].qtd_votos;
       v[dir].codigo_candidato = tmp.codigo_candidato;
       v[dir].qtd_votos = tmp.qtd_votos;
   }

   int j = separa(v, esq, dir);
   // #pragma omp parallel
   // {
      quickSortMediana(v, j + 1, dir, qtd_a_eleger, tamanho_sub_vetor);
   // }
   if((tamanho_sub_vetor - j) < qtd_a_eleger){
      quickSortMediana(v, esq, j - 1, qtd_a_eleger, tamanho_sub_vetor);
   }
}