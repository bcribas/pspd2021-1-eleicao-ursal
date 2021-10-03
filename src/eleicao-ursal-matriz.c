#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<omp.h>

#define N_CANDIDATOS 100000

typedef struct politico{
  int codigo;
  int votos;
}Politico;

Politico **candidatos;


void aloca_matriz(int linha, int coluna);
void libera_matriz(int linha);

void encontraFimLinha(FILE *arquivo){
    int c;
    while (1){
        c = fgetc(arquivo);
        if( c == EOF || c == '\n'){
            break;
        }
    }
}





int compara(Politico A, Politico B) {
  if(A.votos == B.votos){
    if(B.codigo > A.codigo){
      return 1;
    }
  }else if(A.votos < B.votos){
    return 1;
  }else {
    return 0;
  }
}

void merge(Politico *V, int l, int m, int r)
{
  Politico *R=malloc(sizeof(Politico)*(r-l+1));
  int i=l,j=m+1,k=0;

  while(i<=m && j<=r)
  {
    if(compara(V[i],V[j]))
      R[k++]=V[i++];
    else
      R[k++]=V[j++];
  }

  while(i<=m)
      R[k++]=V[i++];
  while(j<=r)
      R[k++]=V[j++];


  k=0;
  for(i=l;i<=r;i++)
    V[i]=R[k++];
  free(R);
}

void mergesort(Politico *V,int l, int r)
{
  if (l>= r) return;
  int meio=(l+r)/2;

  #pragma omp parallel sections num_threads(2)
  {
    #pragma omp section
    {
      mergesort(V,l,meio);
    }

    #pragma omp section
    {
      mergesort(V,meio+1,r);
    }  
  }
  merge(V,l,meio,r);

}

void imprime(int numPoliticos, Politico * V, int tamanho) {
  printf("%d",V[tamanho].codigo);
  for(int i=1;i<numPoliticos;i++){
    printf(" %d",V[tamanho-i].codigo);
  }
  printf("\n");
}

int main(int argc, char *argv[]){
    if(argc < 2)
    {
        fprintf(stderr, "[Erro]: Parametro <caminho/nome_do_arquivo> nao informado!\n");
        exit(EXIT_FAILURE);
    }

    if( argc == 3)
        omp_set_num_threads(strtol(argv[2], NULL, 10));
    else
       omp_set_num_threads(3); 
    
    
    aloca_matriz(omp_get_max_threads(), N_CANDIDATOS);

    int S,F,E;
    int totalVotos =0, invalidos =0, totalvotosPresidente = 0;
    // omp_set_num_threads(3);
    // Vetor irá utilizado para salvar onde cada thread começará a ler(byte) no arquivo
    int *posicoes_iniciais_linhas_arquivos  = (int *) calloc(omp_get_max_threads()+1,sizeof(int));
    // Primeira thread será definida aqui após ler os três inteiros iniciais x y z
    posicoes_iniciais_linhas_arquivos[0] = 0;
    
    FILE *arquivo = fopen(argv[1],"r");
    fscanf(arquivo,"%d",&S);
    fscanf(arquivo,"%d",&F);
    fscanf(arquivo,"%d",&E);
    // Pegando final do arquivo
    fseek(arquivo,0,SEEK_END);
    int arquivo_tamanho =  ftell(arquivo);
    // Definindo uma ultima posição que será o final do arquivo
    // Para que a ultima thread vá até o final
    posicoes_iniciais_linhas_arquivos[omp_get_max_threads()] = arquivo_tamanho;
    //printf("Tamanho do arquivo: %d bytes\n ",arquivo_tamanho);
    //printf("%d\n",omp_get_max_threads());

    // Calculando tamanho do bloco
    int tamanho_bloco = arquivo_tamanho/omp_get_max_threads();
    //printf("Bloco tamanho:%d\n", tamanho_bloco);
    fclose(arquivo);

#pragma omp parallel
{   
    int lido;
    FILE *arquivo = fopen(argv[1],"r");
    fseek(arquivo,omp_get_thread_num()*tamanho_bloco,SEEK_SET);
    
    // Salva a posição do inicio da linha para usar como final das threads anteriores
    encontraFimLinha(arquivo);
    posicoes_iniciais_linhas_arquivos[omp_get_thread_num()] = ftell(arquivo);

    //printf("Thread %d POSICOES iniciais %ld\n",omp_get_thread_num(), ftell(arquivo));
  
    fclose(arquivo);
}

  #pragma omp parallel shared(totalVotos, invalidos, totalvotosPresidente)
  {   
    int numCandidato;
    FILE *arquivo = fopen(argv[1],"r");
    // printf("Thread %d, inicio: %d, fim: %d\n",omp_get_thread_num(),posicoes_iniciais_linhas_arquivos[omp_get_thread_num()],posicoes_iniciais_linhas_arquivos[omp_get_thread_num()+1]  );

    fseek(arquivo,posicoes_iniciais_linhas_arquivos[omp_get_thread_num()],SEEK_SET);
    // if(omp_get_thread_num() != 0){
    //     encontraFimLinha(arquivo);
    //   }
    // Enquando a Thread não chegar ao inicio da outra(byte da prxima thread) 
    while (ftell(arquivo) < posicoes_iniciais_linhas_arquivos[omp_get_thread_num()+1]-1){
      
      fscanf(arquivo,"%d",&numCandidato);
      // printf("Thread %d: valor lido %d Posicao: %ld\n",omp_get_thread_num(),numCandidato,ftell(arquivo));

      if(numCandidato < 0){
        #pragma omp atomic
          invalidos++;
      }
      else if(numCandidato < 100){
          candidatos[omp_get_thread_num()][numCandidato].votos++;
          candidatos[omp_get_thread_num()][numCandidato].codigo = numCandidato;
          // presidente_private[numCandidato].votos++;
          // presidente_private[numCandidato].codigo = numCandidato;
          #pragma omp atomic
          totalvotosPresidente++;
      }
      else if(numCandidato >=100 && numCandidato < 1000){
        // omp_set_lock(&writelock);
        // senador[numCandidato-100].votos++;
        //   senador[numCandidato-100].codigo = numCandidato;
        // omp_unset_lock(&writelock);
          candidatos[omp_get_thread_num()][numCandidato].votos++;
          candidatos[omp_get_thread_num()][numCandidato].codigo = numCandidato;
        
      }
      else if(numCandidato >=1000 && numCandidato < 10000){
          candidatos[omp_get_thread_num()][numCandidato].votos++;
          candidatos[omp_get_thread_num()][numCandidato].codigo = numCandidato;
      }
      else if(numCandidato >=10000 && numCandidato < 100000){
          candidatos[omp_get_thread_num()][numCandidato].votos++;
          candidatos[omp_get_thread_num()][numCandidato].codigo = numCandidato;
      }
      #pragma omp atomic
      totalVotos++;
    } 
    fclose(arquivo);
  }

   #pragma omp parallel
   {
     #pragma omp for 
      for(int i = 0; i < N_CANDIDATOS; i++){
        for(int j = 0; j < omp_get_thread_num(); j++){
          candidatos[0][i].votos+=candidatos[j][i].votos++;
          candidatos[0][i].codigo= i;
        }
      }
   }
    int validos = totalVotos-invalidos;
  
    printf("%d %d\n",validos, invalidos);

    int indexPresidenteEleito = 0;

    for(int i=0;i<100;i++){
        if(candidatos[0][i].votos > candidatos[0][indexPresidenteEleito].votos){
        indexPresidenteEleito = i;
        }
    }

    if(candidatos[0][indexPresidenteEleito].votos >= (totalvotosPresidente *0.51)){
        printf("%d\n",candidatos[0][indexPresidenteEleito].codigo);
    }else {
        printf("Segundo turno\n");
    }

    mergesort(candidatos[0],100,999);
    imprime(S,candidatos[0],999);

    mergesort(candidatos[0],1000,9999);
    imprime(F,candidatos[0],9999);

    mergesort(candidatos[0],10000,99999);
    imprime(E,candidatos[0],99999);

    // free(presidente);
    // free(senador);
    // free(depFederal);
    // free(depEstadual);
    libera_matriz(omp_get_max_threads());
    return 0;
}

void aloca_matriz(int linha, int coluna){

  candidatos = (Politico **)malloc(linha * sizeof(Politico *));
  for(int i = 0; i < linha; i++){
    candidatos[i] = (Politico *)calloc(coluna, sizeof(Politico));
  } 

}

void libera_matriz(int linha){
// libera a memória da matriz
  for (int i=0; i < linha; i++){
    free(candidatos[i]);
  }
  free(candidatos);
}