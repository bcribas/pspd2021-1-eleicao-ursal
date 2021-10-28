#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct voto {
   int codigo_candidato;
   int qtd_votos;
} Voto;

int  compara(const void *a, const void *b);
void elege_politico(Voto *votos, int inicio, int qnt);
void elege_presidente(Voto *votos);

int main(int argc, char *argv[]){

   Voto *votos       = (Voto *) calloc(0b11000011010100000, sizeof(Voto));   // 0-99999
   for(int i=10; i<0b11000011010100000; i++)
     votos[i].codigo_candidato = i;
   int voto;
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
   elege_politico(votos, 100, s);  // elege senadores
   elege_politico(votos, 1000, f);  // elege deputados federais
   elege_politico(votos, 10000, e);  // elege deputados estaduais

   free(votos);

   return 0;
} // end main

void elege_presidente(Voto *votos){
   qsort(votos, 99, sizeof(Voto), compara);
   
   int qtdVotosPres = 0;
   for(int i=0; i<100; i++){
      qtdVotosPres += votos[i].qtd_votos;
   }

   if(votos[0].qtd_votos > (qtdVotosPres / 2))
        printf("%d\n",votos[0].codigo_candidato);
   else
        printf("Segundo turno\n");

   return;
} // end elege_presidente

void elege_politico(Voto *votos, int inicio, int qnt){
   qsort(votos + inicio, inicio * 9, sizeof(Voto), compara);

   for(int i=0; i<qnt; i++){
      printf("%d", votos[i+inicio].codigo_candidato);

      if(i != (qnt-1)){
         printf(" ");
      }
   }
   printf("\n");

   return;
} // end elege_politico

int compara(const void *a, const void *b){
   Voto *candA =  (Voto *) a;
   Voto *candB =  (Voto *) b;

   if((candB->qtd_votos - candA->qtd_votos) == 0)
      return candB->codigo_candidato - candA->codigo_candidato;
   else
      return candB->qtd_votos - candA->qtd_votos;
} // end 