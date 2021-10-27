#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int getFileSize(FILE* file){
  
  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  rewind(file);
  return size;
}

int numPlaces (int n) {
    if (n < 100) return 2;
    if (n < 1000) return 3;
    if (n < 10000) return 4;
    return 5;
}

void getFirstLineInfo(char* buf, int vals[4]){
  int aux1 = 0, aux2 = 0, flag = 1;
  while(buf[aux1]!='\n'){
    if(flag){
      vals[aux2] = atoi(&buf[aux1]);
      flag = 0;
    }
    if(buf[aux1] == ' '){
      aux2++;
      flag = 1;
    }
    aux1++;
  }
  vals[4] = aux1+1;
}

void findGenericWinner(int *votos, int n, int qtdEleitos, int* winners){
  int idx_eleito, idx = 0;

  while (qtdEleitos)
  {
    idx_eleito = 0;
    for (int i = n/10; i < n; i++)
    {
      if(votos[i] > 0){
        //printf("votos[%d] = %d\n", i, votos[i]);
      }
      if (votos[i] >= votos[idx_eleito])
      {
        idx_eleito = i;
      }
    }
    winners[idx++] = idx_eleito;
    votos[idx_eleito] = 0;
    qtdEleitos--;
  }
  return;
} 

int findPresidenteWinner(int* votos, int n, int* winner) {
    int idx = 0;
    int totalVotos = 0;

    for (int i = 1; i < n; i++) {
        if (votos[i] > 0)
            totalVotos += votos[i];
        if (votos[i] > votos[idx]) {
            idx = i;
        };
    }
    if (votos[idx] < (totalVotos * 0.51)) {
      *winner = -1;
    }
    else {
      *winner = idx;
    }
}

int main(int argc ,char** argv) {
  if(argc != 2){
    printf("usage: ./a.out path/to/file\n");
    exit(-1);
  }

  char *filePath = argv[1];
  FILE* arquivo = fopen(filePath, "rb");
  if (arquivo == NULL) {
    fputs("invalid file", stderr);
    exit(-2);
  };

  int fileSize = getFileSize(arquivo);
  char *buffer = malloc(fileSize * sizeof(char));
  if (!buffer) {
      fputs("mem alloc error", stderr);
      exit(-3);
  }

  int bytesRead = fread(buffer, 1, fileSize, arquivo);

  if (bytesRead != fileSize) {
      fputs("mem copy error", stderr);
      exit(-4);
  }

  int vals[4];

  getFirstLineInfo(buffer, vals);

  const int nVotos = 100000;
  int aux = vals[4], votosValidos = 0, votosInvalidos = 0, votos[nVotos];
  int wPresidente, wSenadores[nVotos], wEstadual[nVotos], wFederal[nVotos];
  memset(votos, 0, nVotos * sizeof(int));
  memset(wSenadores,0,nVotos*sizeof(int));
  memset(wEstadual,0,nVotos*sizeof(int));
  memset(wFederal,0,nVotos*sizeof(int));
  int voto;
#pragma omp parallel for private(voto) reduction(+:votosInvalidos) reduction(+:votosValidos) reduction(+:votos)
  for (int i = aux; i < bytesRead; i++)
  {
    /*Começa a partir do prox. numero completo*/
    if (buffer[i - 1] != '\n') {
      continue;
    }
    voto = atoi(&buffer[i]);
    if (voto < 0)
    {
      votosInvalidos++;
    }
    else if (voto > 0)
    {
      votos[voto]++;
      votosValidos++;
    }
    while (buffer[i] != '\n')
      i++;
  }

  printf("%d %d\n", votosValidos, votosInvalidos);    
#pragma omp parallel sections 
{
  #pragma omp section 
  {
    findPresidenteWinner(votos, 100, &wPresidente);
  }
  #pragma omp section 
  {
    findGenericWinner(votos, 1000, vals[0], wSenadores);
  }
  #pragma omp section 
  {
    findGenericWinner(votos, 10000, vals[1], wEstadual);
  }
  #pragma omp section 
  {
    findGenericWinner(votos, 100000, vals[2], wFederal);
  }
}
  if(wPresidente == -1){
      printf("Segundo turno\n");
  }
  else {
    printf("%d\n", wPresidente);
  }
  printf("%d", wSenadores[0]);
  for (int i = 1; i < vals[0]; i++)
  {
    printf(" %d", wSenadores[i]);
  }
  printf("\n");
  printf("%d", wEstadual[0]);
  for (int i = 1; i < vals[1]; i++)
  {
    printf(" %d", wEstadual[i]);
  }
  printf("\n");
  printf("%d", wFederal[0]);
  for (int i = 1; i < vals[2]; i++)
  {
    printf(" %d", wFederal[i]);
  }
  printf("\n");
  fclose(arquivo);
  free(buffer);
  return 0;
}