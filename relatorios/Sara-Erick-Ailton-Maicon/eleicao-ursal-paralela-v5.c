#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

typedef struct voto {
   int codigo_candidato;
   int qtd_votos;
} Voto;

typedef struct offset {
   int byteInicial;
} Offset;

int  compara(const void *a, const void *b);
void elege_politico(Voto *votos, int inicio, int qnt);
void elege_presidente(Voto *votos);

int main(int argc, char *argv[]){

   Voto *votos = (Voto *) calloc(0b11000011010100000, sizeof(Voto));   // 0-99999

   for(int i=10; i<0b11000011010100000; i++)
     votos[i].codigo_candidato = i;

   int qnt_invalidos = 0;
   int qnt_validos   = 0;
   
   char *filename = argv[1];
   FILE *arquivo  = fopen(filename, "r");
   
   int s, f, e;   // qnt senadores, dep.fed, dep.est para a eleicao
   fscanf(arquivo, "%d %d %d", &s, &f, &e);
   
   int ultimo_primeira_linha  = ftell(arquivo);

   fseek(arquivo, 0, SEEK_END);
   int total_bytes = ftell(arquivo) - ultimo_primeira_linha;

   int numeroDeThreads  = omp_get_num_threads();
   Offset *threads      = (Offset *) malloc(numeroDeThreads * sizeof(Offset));

   threads[0].byteInicial = ultimo_primeira_linha;
   int qnt_bytes_thread = total_bytes / numeroDeThreads;

   for(int i=1; i < numeroDeThreads; i++){
      threads[i].byteInicial = i * qnt_bytes_thread;
   }

   fclose(arquivo);

   #pragma omp parallel num_threads(numeroDeThreads)
   {
      FILE *arquivo = fopen(filename, "r");
      fseek(arquivo, threads[omp_get_thread_num()].byteInicial, SEEK_SET);

      int byteFinal = (numeroDeThreads - 1) == omp_get_thread_num()
         ? total_bytes + ultimo_primeira_linha
         : threads[omp_get_thread_num() + 1].byteInicial;

      char lixo;
      do {
         fscanf(arquivo, "%c", &lixo);
      } while (lixo != '\n');

      char *valor    = (char *) malloc(6 * sizeof(char)); // 6 -> max eh 99999\0
      size_t intt    = sizeof(int);
      int voto       = 0;

      for(int i = ftell(arquivo); i < byteFinal;){
         i     += getline(&valor, &intt, arquivo);
         voto   = atoi(valor);

         if(voto < 0){
            qnt_invalidos++;
         }
         else {
            qnt_validos++;
            votos[voto].qtd_votos++;
         }
      }

      free(valor);
      fclose(arquivo);
   } // end parallel execution

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