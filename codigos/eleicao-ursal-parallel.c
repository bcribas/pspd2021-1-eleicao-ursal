/*
   * Erick Giffoni (170141161), Sara Silva (160144752)
   * Universidade de Brasilia - UnB - PSPD 2021/1
   * 08/2021
   * Eleicao U.R.S.A.L - Versao paralela com OpenMP
*/

/* .Voto valido         = numero inteiro positivo
   .Voto invalido/nulo  =   ||   inteiro negativo
*/

/* .Presidente = 2 digitos & vitoria = 51% dos votos
   .Senador    = 3   ||    & cada eleitor escolhe 2
   .Dep.Fed    = 4   ||
   .Dep.Est    = 5   ||

   .Vitoria = maioria simples (exceto o presidente) dos votos validos

   .Partido = identificado pelos 2 primeiros dígitos do codigo de um candidato.
*/

/* COMPILE WITH gcc-10 -fopenmp */

#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void eleger_politico(int qtd_p, int *politico, int iteracoes);
void computar_voto(int voto, int *qtd_votos_presidente, int *qtd_invalidos,
                   int *qtd_validos, int *presidente, int *senador, int *depFed,
                   int *depEst);

int main(int argc, char *argv[]){

    /*int *presidente   = (int *) calloc(0b1100100, sizeof(int));             // 0-99
    int *senador      = (int *) calloc(0b1111101000, sizeof(int));          // 0-999
    int *depFed       = (int *) calloc(0b10011100010000, sizeof(int));      // 0-9999
    int *depEst       = (int *) calloc(0b11000011010100000, sizeof(int));   // 0-99999*/

    int lines = omp_get_num_threads() + 1;
    int cols = 99999;

    int **all_votes = (int **) calloc(lines, sizeof(int));

    if (all_votes == NULL) {
       printf("Somethin went wrong during allocation\n");
       exit(EXIT_FAILURE);
    }

    for (int i = 0; i < cols; i++) {
       all_votes[i] = (int *) calloc(cols, sizeof(int));
       if (all_votes[i] == NULL) {
         printf("Somethin went wrong during allocation\n");
         exit(EXIT_FAILURE);
      }
    }

    int voto;
    int qtd_votos_presidente = 0;
    int qtd_invalidos        = 0;
    int qtd_validos          = 0;
    
    char *filename = argv[1];
    FILE *arquivo = fopen(filename, "r");
    
    int s, f, e;   // qtd senadores, dep.fed, dep.est para a eleicao
    fscanf(arquivo, "%d %d %d", &s, &f, &e);
    
    int ultimo_primeira_linha = ftell(arquivo); // ultimo byte primeira linha, ela precisa ser ignorada nas threads

    int total_bytes = 0;
    fseek(arquivo, 0, SEEK_END); // posiciona no final do arquivo para pegar o total de bytes
    total_bytes = ftell(arquivo) - ultimo_primeira_linha;
    // printf("Quantidade de bytes no arquivo: %d\n", total_bytes);

    #pragma omp parallel
    {
        // quantidade de bytes pelo número total de threas em execução
        int qnt_bytes_thread = total_bytes / omp_get_num_threads();
        // printf("Quantidade de bytes por thread: %d\n", qnt_bytes_thread);

        FILE *arquivo = fopen(filename, "r");
        fseek(arquivo, ultimo_primeira_linha+(qnt_bytes_thread*omp_get_thread_num()), SEEK_SET);

        int lido = 0;
        int count = 0;
        int lixo = 0;
        int nova_posicao = 0;
        int posicao_atual = 0;
        for (count = 0; count < qnt_bytes_thread; count+=nova_posicao-posicao_atual){
            posicao_atual = ftell(arquivo);
            fseek(arquivo, posicao_atual-1, SEEK_SET);
            if(fgetc(arquivo) == '\n'){
                fscanf(arquivo, "%d ", &lido);
                // todo: substituir chamada a funcao computa_voto por calculo na matriz
                #pragma omp parallel for
                for (int i = 0; i < lines; i++) {
                   for (int j = 0; j < cols; j++) {
                      if (lido > 0) {
                        all_votes[omp_get_thread_num()][lido] += 1;
                        qtd_validos += 1;
                      } else 
                        qtd_invalidos += 1;
                   }
                }
                // printf("Thread %d leu %d\n", omp_get_thread_num(), lido);
            }
            else{
                fseek(arquivo, posicao_atual-1, SEEK_SET);
                fscanf(arquivo, "%d ", &lixo);
            }
            nova_posicao = ftell(arquivo);
            //count += nova_posicao - posicao_atual; // incrementa qnt de bytes lido no ultimo scanf
        }

        fclose(arquivo);
    }

   printf("%d %d\n", qtd_validos, qtd_invalidos);

   // logica para ver quem ganhou, se houve empate etc

   int pos_eleito = 0;
   char segundo_turno = 0b00000000; // boolean falso

// Adaptar para cada thread computar presidente, sen., dep. fed e dep. est. e adicionar 
// o maior do seu intervalo para cada um deles na ultima linha
#pragma omp parallel sections private(pos_eleito, segundo_turno)
   {
#pragma omp section
       {   
           for(int i=1; i<100; i++){
               if(presidente[i] > presidente[pos_eleito]){
                   pos_eleito = i;
                   segundo_turno = 0b00000000;
               }
               else if(presidente[i] == presidente[pos_eleito]){
                   segundo_turno = 0b00000001;   // true
               }
           } // end for elege presidente

           if(segundo_turno || (presidente[pos_eleito] <= (qtd_votos_presidente/2))){
               printf("Segundo turno\n");
           }
           else{
               printf("%d\n", pos_eleito);
           }
       } // end parallel section presidente

       // eleger senador
#pragma omp section
       {
           eleger_politico(s, senador, 1000);
       } // end parallel section senador

       // eleger dep.Fed
#pragma omp section
       {
           eleger_politico(f, depFed, 10000);
       } // end parallel section dep.Fed

       // eleger dep.Est
#pragma omp section
       {   
           eleger_politico(e, depEst, 100000);
       } // end parallel section dep.Est
   } // end omp parallel sections

    return 0;
} 

void eleger_politico(int qtd_p, int *politico, int iteracoes){
   int pos_eleito;
   char segundo_turno; // boolean

   while(qtd_p){
       pos_eleito = 0;
       segundo_turno = 0b00000000; // empate = false
       for(int i=1; i<iteracoes; i++){
           if(politico[i] > politico[pos_eleito]){
               pos_eleito = i;
               segundo_turno = 0b0000000;
           }
           else if(politico[i] == politico[pos_eleito]){
               i > pos_eleito ? pos_eleito = i : pos_eleito;
               segundo_turno = 0b00000001;
           }
       }

       printf("%d", pos_eleito);
       qtd_p--;

       if(qtd_p){
           printf(" ");   // tem mais 1 candidato para elegermos
       }

       politico[pos_eleito] = 0;
   }
   printf("\n");

   return;
} // end eleger_politico

void computar_voto(int voto, int *qtd_votos_presidente, int *qtd_invalidos,
                   int *qtd_validos, int *presidente, int *senador, int *depFed,
                   int *depEst){
    // voto valido ?
       if(voto < 0){
           *qtd_invalidos += 1;
       }
       else{
           *qtd_validos += 1;

           // para quem e o voto ? verificar qtd de digitos do voto
           if((voto / 10000) > 0){
               // voto depEst
               depEst[voto] += 1;
           } // end if voto depEst
           else if((voto / 1000) > 0){
               // voto depFed
               depFed[voto] += 1;
           } // end else if voto depFed
           else if((voto / 100) > 0){
               // voto senador
               senador[voto] += 1;
           } // end else if voto senador
           else {
               // voto presidente
               presidente[voto] += 1;
               *qtd_votos_presidente += 1;
           } // end else voto presidente

       } // end else voto valido
} //end computar_voto