#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <math.h>

#define true 1
#define false 0
#define MIN_P 10
#define MAX_P 100
#define MAX_S 1000
#define MAX_CM 10000
#define MAX_CP 100000

void printElected(int *politicianArray, int maxElected, int minSize, int maxSize, int totalVoters) {
  int *best = calloc(maxElected, sizeof(int));
  int secondRound = false;
  for (int i = 0; i < maxElected; i++) {
      int elected = 0;
      for (int j = minSize; j < maxSize; j++) {
          if (politicianArray[j] > politicianArray[elected]) {
              elected = j;
              secondRound = false;
          }
          else if (politicianArray[j] == politicianArray[elected]) {
              if (minSize != MIN_P)
                  elected = j > elected ? j : elected;
              secondRound = true;
          }
      }
      best[i] = elected;
      politicianArray[elected] = -1;
  }

  if (minSize == MIN_P) {
    int lessThanHalf = politicianArray[best[0]] <= (totalVoters / 2);

    if (secondRound || lessThanHalf) {
      printf("Segundo turno");
    } else {
      printf("%d", best[0]);
    }
  } else {
    for (int i = 0; i < maxElected; i++) {
      printf("%d", best[i]);
      if (i + 1 < maxElected)
        printf(" ");
    }
  }
  printf("\n");
}

int main(int argc, char *argv[]) {
  int totalSenators, totalCongressman, totalCongressperson, totalVotesPresident = 0, invalidVote = 0, validVote = 0;
  int *president = calloc(MAX_P, sizeof(int));
  int *senator = calloc(MAX_S, sizeof(int));
  int *congressman = calloc(MAX_CM, sizeof(int));
  int *congressperson = calloc(MAX_CP, sizeof(int));

  int num_threads = omp_get_max_threads();

  FILE *file = fopen(argv[1], "r");

  int start_position;

  fscanf(file, "%d %d %d%n", &totalSenators, &totalCongressman, &totalCongressperson, &start_position);

  start_position += 1;

  fseek(file, 0L, SEEK_END);
  long int end_position = ftell(file) + 1;
  fseek(file, start_position, SEEK_SET);

  long int size = end_position - start_position;

  int* thread_starts = calloc(num_threads + 1, sizeof(long int));
  thread_starts[0] = start_position;
  thread_starts[num_threads] = end_position;

  int chunk_size = (int)floor(size / num_threads);

  for(int i = 1; i < num_threads; i++) {
    fseek(file, thread_starts[i-1] + chunk_size, SEEK_SET);
    char line[10];
    fgets(line, 10, file);
    thread_starts[i] = thread_starts[i-1] + chunk_size + strlen(line);
  }

  fclose(file);

  #pragma omp parallel reduction(+ : validVote, invalidVote, totalVotesPresident)
  {
    FILE* file = fopen(argv[1], "r");
    long int start, end;

    start = thread_starts[omp_get_thread_num()];
    end = thread_starts[omp_get_thread_num() + 1];

    fseek(file, start, SEEK_SET);

    int size, vote;

    long int counter = start;

    while(fscanf(file, "%d%n", &vote, &size) != EOF) {
      counter += size;
      if (counter >= end) {
        break;
      }
      if (vote >= MIN_P) {
        validVote++;
        if (vote < MAX_P) {
          totalVotesPresident++;
          president[vote]++;
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

  return 0;
}