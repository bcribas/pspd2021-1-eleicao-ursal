#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#define true 1
#define false 0
#define MIN_P 10
#define MAX_P 100
#define MAX_S 1000
#define MAX_CM 10000
#define MAX_CP 100000

void printElected(int *politicianArray, int maxElected, int minSize, int maxSize, int totalVoters) {
  while (maxElected) {
    int elected = 0, secondRound = false;
    for (int i = minSize; i < maxSize; i++) {
      if (politicianArray[i] > politicianArray[elected]) {
        elected = i;
        secondRound = false;
      } else if (politicianArray[i] == politicianArray[elected]) {
        if (minSize != MIN_P) {
          elected = i > elected ? i : elected;
        }
        secondRound = true;
      }
    }

    if (minSize == MIN_P) {
      maxElected--;
      int lessThanHalf = politicianArray[elected] <= (totalVoters / 2);
      if (secondRound || lessThanHalf) {
        printf("Segundo turno");
      } else {
        printf("%d", elected);
      }
    } else {
      for (int i = minSize; i < maxSize; i++) {
        if (politicianArray[i] == politicianArray[elected]) {
          elected = i > elected ? i : elected;
          secondRound = true;
        }
      }
      printf("%d", elected);

      politicianArray[elected] = -1;

      maxElected--;
      if (maxElected) {
        printf(" ");
      }
    }
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  struct stat statFile;
  int totalSenators, totalCongressman, totalCongressperson, vote, totalVotesPresident = 0, invalidVote = 0, validVote = 0;
  int *president = calloc(MAX_P, sizeof(int));
  int *senator = calloc(MAX_S, sizeof(int));
  int *congressman = calloc(MAX_CM, sizeof(int));
  int *congressperson = calloc(MAX_CP, sizeof(int));
  FILE *file;

  clock_t begin = clock();

  // FILE *file = fopen(argv[1], "r");
  stat(argv[1], &statFile);
  int fileSize = statFile.st_size;

  // fscanf(file, "%d", &totalSenators);
  // fscanf(file, "%d", &totalCongressman);
  // fscanf(file, "%d", &totalCongressperson);

#pragma omp parallel private(file) num_threads(2)
  {
    file = fopen(argv[1], "r");
    fseek(file, omp_get_thread_num() * fileSize / 2, SEEK_SET);
    if (omp_get_thread_num() == 0) {
      fscanf(file, "%d", &totalSenators);
      fscanf(file, "%d", &totalCongressman);
      fscanf(file, "%d", &totalCongressperson);
    }
    while (fscanf(file, "%d", &vote) != EOF) {
      if (vote >= MIN_P) {
        validVote++;
        if (vote < MAX_P) {
          president[vote]++;
          totalVotesPresident++;
        } else if (vote < MAX_S) {
          senator[vote]++;
        } else if (vote < MAX_CM) {
          congressman[vote]++;
        } else {
          congressperson[vote]++;
        }
      } else
        invalidVote++;
    }
  }
  printf("%d %d\n", validVote, invalidVote);

  printElected(president, 1, MIN_P, MAX_P, totalVotesPresident);
  printElected(senator, totalSenators, MAX_P, MAX_S, false);
  printElected(congressman, totalCongressman, MAX_S, MAX_CM, false);
  printElected(congressperson, totalCongressperson, MAX_CM, MAX_CP, false);
  clock_t end = clock();
  double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
  printf("Tempo: %lf\n", time_spent);

  return 0;
}