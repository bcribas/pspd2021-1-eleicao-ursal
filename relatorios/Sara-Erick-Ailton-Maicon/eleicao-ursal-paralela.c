#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void elege_politico(int *votos, int inicio, int fim, int qnt);
void elege_presidente(int *votos);

int main(int argc, char *argv[]){

    int *votos       = (int *) calloc(0b11000011010100000, sizeof(int));   // 0-99999

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
                    votos[voto]++;
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
    elege_politico(votos, 1000, 9999, s);  // elege deputados federais
    elege_politico(votos, 10000, 99999, s);  // elege deputados estaduais

    return 0;
} 

void elege_presidente(int *votos){
    int pos_eleito = 0;
    char segundo_turno = 0b00000000; // boolean falso
    int qnt_votos_presidente = 0;

    for(int i=1; i<100; i++){
        if(votos[i] > 0){
            qnt_votos_presidente++;
        }

        if(votos[i] > votos[pos_eleito]){
            pos_eleito = i;
            segundo_turno = 0b00000000;
        }
        else if(votos[i] == votos[pos_eleito]){
            segundo_turno = 0b00000001;   // true
        }
    }

    if(segundo_turno || (votos[pos_eleito] <= (qnt_votos_presidente/2))){
        printf("Segundo turno\n");
    }
    else{
        printf("%d\n", pos_eleito);
    }
}
   

void elege_politico(int *votos, int inicio, int fim, int qnt){
   int pos_eleito;
   char segundo_turno; // boolean

   while(qnt){
       pos_eleito = 0;

       for(int i=inicio; i<fim; i++){
           if(votos[i] > votos[pos_eleito]){
               pos_eleito = i;
           }
           else if(votos[i] == votos[pos_eleito]){
               i > pos_eleito ? pos_eleito = i : pos_eleito;
           }
       }

       printf("%d", pos_eleito);
       qnt--;

       if(qnt){
           printf(" ");   // tem mais 1 candidato para elegermos
       }

       votos[pos_eleito] = 0;
   }
   printf("\n");

   return;
}
