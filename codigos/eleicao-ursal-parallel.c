/*
   * Erick Giffoni (170141161)
   * Universidade de Brasilia - UnB
   * PSPD 2021/1
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

int main(int argc, char *argv[]){

   int s, f, e;   // qtd senadores, dep.fed, dep.est para a eleicao
   scanf(" %d %d %d", &s, &f, &e);

   int *presidente   = (int *) calloc(0b1100100, sizeof(int));             // 0-99
   int *senador      = (int *) calloc(0b1111101000, sizeof(int));          // 0-999
   int *depFed       = (int *) calloc(0b10011100010000, sizeof(int));      // 0-9999
   int *depEst       = (int *) calloc(0b11000011010100000, sizeof(int));   // 0-99999

   int voto;
   int qtd_votos_presidente = 0;
   int qtd_invalidos        = 0;
   int qtd_validos          = 0;
   while(scanf(" %d", &voto) != EOF){
      // printf("voto lido %d\n", voto);
      // voto valido ?
      if(voto < 0){
         qtd_invalidos += 1;
      }
      else{
         qtd_validos += 1;

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
            qtd_votos_presidente += 1;
         } // end else voto presidente

      } // end else voto valido
   } // end while

   printf("%d %d\n", qtd_validos, qtd_invalidos);

   // logica para ver quem ganhou, se houve empate etc

   int pos_eleito = 0;
   char segundo_turno = 0b00000000; // boolean falso

   #pragma omp parallel sections private(pos_eleito, segundo_turno)
   {
      #pragma omp section
      {   
         for(int i=1; i<100; i++){
            // printf("i=%d, p[i]=%d, pos_eleito=%d, p[pos_eleito]=%d\n", i, presidente[i], pos_eleito, presidente[pos_eleito]);
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
} // end main

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