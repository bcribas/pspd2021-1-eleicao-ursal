#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<omp.h>

void encontraFimLinha(FILE *arquivo){
    int c;
    while (1){
        c = fgetc(arquivo);
        if( c == EOF || c == '\n'){
            break;
        }
    }
}

typedef struct politico{
  int codigo;
  int votos;
}Politico;

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
    
    
    int S,F,E;
    int totalVotos =0, invalidos =0, totalvotosPresidente = 0;

    Politico *presidente = (Politico *) calloc(100,sizeof(Politico));
    Politico *senador = (Politico *) calloc(900,sizeof(Politico));
    Politico *depFederal = (Politico *) calloc(9000,sizeof(Politico));
    Politico *depEstadual = (Politico *) calloc(90000,sizeof(Politico));

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
  // for(int i=0; i<4; i++){
  //   printf("posicoes: %d\n", posicoes_iniciais_linhas_arquivos[i]);
  // }
  omp_lock_t writelock;

  omp_init_lock(&writelock);
  #pragma omp parallel shared(totalVotos, invalidos, totalvotosPresidente,presidente,senador,depFederal,depEstadual)
  {   
    int numCandidato;
    Politico *presidente_private = (Politico *) calloc(100,sizeof(Politico));
    Politico *senador_private = (Politico *) calloc(900,sizeof(Politico));
    Politico *depFederal_private = (Politico *) calloc(9000,sizeof(Politico));
    Politico *depEstadual_private = (Politico *) calloc(90000,sizeof(Politico));

    // Politico presidente_private[100];
    // Politico senador_private[900];
    // Politico depFederal_private[9000];
    // Politico depEstadual_private[90000];
    // memset(presidente_private, 0, 100*sizeof(Politico));
    // memset(senador_private, 0, 900*sizeof(Politico));
    // memset(depFederal_private, 0, 9000*sizeof(Politico));
    // memset(depEstadual_private, 0, 90000*sizeof(Politico));

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
          // omp_set_lock(&writelock);
          // presidente[numCandidato].votos++;
          // presidente[numCandidato].codigo = numCandidato;
          // omp_unset_lock(&writelock);
          presidente_private[numCandidato].votos++;
          presidente_private[numCandidato].codigo = numCandidato;
          #pragma omp atomic
          totalvotosPresidente++;
      }
      else if(numCandidato >=100 && numCandidato < 1000){
        // omp_set_lock(&writelock);
        // senador[numCandidato-100].votos++;
        //   senador[numCandidato-100].codigo = numCandidato;
        // omp_unset_lock(&writelock);
          senador_private[numCandidato-100].votos++;
          senador_private[numCandidato-100].codigo = numCandidato;
        
      }
      else if(numCandidato >=1000 && numCandidato < 10000){
          depFederal_private[numCandidato-1000].votos++;
          depFederal_private[numCandidato-1000].codigo = numCandidato;
      }
      else if(numCandidato >=10000 && numCandidato < 100000){
          depEstadual_private[numCandidato-10000].votos++;
          depEstadual_private[numCandidato-10000].codigo = numCandidato;
      }
      #pragma omp atomic
      totalVotos++;
    } 

    {
      for(int i =0; i<100;i++){
          omp_set_lock(&writelock);
          if( presidente_private[i].codigo != 0){
            presidente[i].codigo = presidente_private[i].codigo;
            presidente[i].votos += presidente_private[i].votos;
          }
          
          omp_unset_lock(&writelock);
      }
      
      for(int i =0; i<900;i++){
        omp_set_lock(&writelock);
        if( senador_private[i].codigo != 0){
            senador[i].codigo = senador_private[i].codigo;
            senador[i].votos += senador_private[i].votos;
          }
          
        omp_unset_lock(&writelock);
      }
      
      for(int i =0; i<9000;i++){
        omp_set_lock(&writelock);
        if(depFederal_private[i].codigo != 0){
            depFederal[i].codigo = depFederal_private[i].codigo;
          depFederal[i].votos += depFederal_private[i].votos;
        }
          
        omp_unset_lock(&writelock);
      }
      
      for(int i =0; i<90000;i++){
        omp_set_lock(&writelock);
        if( depEstadual_private[i].codigo != 0){
            depEstadual[i].codigo = depEstadual_private[i].codigo;
          depEstadual[i].votos += depEstadual_private[i].votos;
          }
        omp_unset_lock(&writelock);
      }
    }
      

    free(presidente_private);
    free(senador_private);
    free(depFederal_private);
    free(depEstadual_private);

    fclose(arquivo);
  }
  omp_destroy_lock(&writelock);
    int validos = totalVotos-invalidos;
  
    printf("%d %d\n",validos, invalidos);

    int indexPresidenteEleito = 0;

    for(int i=0;i<100;i++){
        if(presidente[i].votos > presidente[indexPresidenteEleito].votos){
        indexPresidenteEleito = i;
        }
    }

    if(presidente[indexPresidenteEleito].votos >= (totalvotosPresidente *0.51)){
        printf("%d\n",presidente[indexPresidenteEleito].codigo);
    }else {
        printf("Segundo turno\n");
    }

    mergesort(senador,0,899);
    imprime(S,senador,899);

    mergesort(depFederal,0,8999);
    imprime(F,depFederal,8999);

    mergesort(depEstadual,0,89999);
    imprime(E,depEstadual,89999);

    free(presidente);
    free(senador);
    free(depFederal);
    free(depEstadual);
    return 0;
}