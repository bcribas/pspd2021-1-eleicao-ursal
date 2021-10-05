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

void findGenericWinner(int *votos, int n, int qtdEleitos){
  int idx_eleito;

  while (qtdEleitos)
  {
    idx_eleito = 0;
    for (int i = 1; i < n; i++)
    {
      if(votos[i] > 0){
        //printf("votos[%d] = %d\n", i, votos[i]);
      }
      if (votos[i] >= votos[idx_eleito])
      {
        idx_eleito = i;
      }
    }

    printf("%d", idx_eleito);
    votos[idx_eleito] = 0;
    qtdEleitos--;
    if (qtdEleitos)
    {
      printf(" ");
    }
  } 
  printf("\n");
  return;
} 

int findPresidenteWinner(int* votos, int n) {
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
        printf("Segundo turno\n");
    }
    else {
        printf("%d\n", idx);
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

  const int nVotosPresidente = 100, nVotosSenadores = 1000, nVotosEstadual = 10000, nVotosFederal = 100000;
  int aux = vals[4], votosValidos = 0, votosInvalidos = 0, votosPresidente[nVotosPresidente], votosSenadores[nVotosSenadores], votosEstadual[nVotosEstadual], votosFederal[nVotosFederal];
  int *votos[4] = {votosPresidente,votosSenadores,votosEstadual,votosFederal};
  memset(votosPresidente, 0, nVotosPresidente * sizeof(int));
  memset(votosSenadores,0,nVotosSenadores*sizeof(int));
  memset(votosEstadual,0,nVotosEstadual*sizeof(int));
  memset(votosFederal,0,nVotosFederal*sizeof(int));
  int voto;
#pragma omp parallel for private(voto) reduction(+:votosInvalidos) reduction(+:votosValidos) shared(votosPresidente) shared(votosSenadores) shared(votosEstadual) shared(votosFederal)
  for (int i = aux; i < bytesRead; i++)
  {
    /*Começa a partir do prox. numero completo*/
    if (buffer[i - 1] != '\n') {
      continue;
    }
    voto = atoi(&buffer[i]);
    if (voto < 0)
    {
      #pragma omp atomic
      votosInvalidos++;
    }
    else if (voto > 0)
    {
      #pragma omp atomic
        votos[numPlaces(voto) - 2][voto]++;
        votosValidos++;
    }
    while (buffer[i] != '\n')
      i++;
  }

  printf("%d %d\n", votosValidos, votosInvalidos);
  findPresidenteWinner(votosPresidente, nVotosPresidente);
  findGenericWinner(votosSenadores, nVotosSenadores, vals[0]);
  findGenericWinner(votosEstadual, nVotosEstadual, vals[1]);
  findGenericWinner(votosFederal, nVotosFederal, vals[2]);
  fclose(arquivo);
  free(buffer);

  return 0;
}