#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int conta_votos_presidente(int *);
void conta_votos_senador(int *, int *, int);
void conta_votos_dep_fed(int *, int *, int);
void conta_votos_dep_est(int *, int *, int);

int main(int argc, char **argv){
  int **leitura_votos;
  int *leitura_votos_invalidos;
  int *leitura_votos_validos;
	int *votos;
  if (argc != 2){
    printf("Por favor digite somente um caminho para o arquivo logo apos a execucao do binario\n");
    return 0;
  }

  FILE *p;

  p = fopen(argv[1], "r");
  leitura_votos = malloc(omp_get_max_threads() * sizeof(int *));
  for (int i = 0; i < omp_get_max_threads(); i++)leitura_votos[i] = calloc(99999, sizeof(int));
	votos = calloc(99999, sizeof(int));
	leitura_votos_invalidos = calloc(omp_get_max_threads(), sizeof(int));
	leitura_votos_validos = calloc(omp_get_max_threads(), sizeof(int));

	int presidente_vencedor;
	int *senador_vencedor;
	int *dep_fed_vencedor;
	int *dep_est_vencedor;


  long tam_arquivo;
	int n_senadores, n_dep_fed, n_dep_est;
	int n_voto = 0;
	int n_voto_invalido = 0;

  fseek(p, 0, SEEK_END);
  tam_arquivo = ftell(p);
  rewind(p);

  fscanf(p, "%d %d %d\n", &n_senadores, &n_dep_fed, &n_dep_est);
	if(n_senadores > 999) n_senadores = 999;

	if(n_dep_fed > 9999) n_senadores = 9999;

	if(n_dep_est > 99999) n_senadores = 99999;
	
	senador_vencedor = malloc(n_senadores * sizeof(int));
	dep_fed_vencedor = malloc(n_dep_fed * sizeof(int));
	dep_est_vencedor = malloc(n_dep_est * sizeof(int));

  long *marcadores = calloc(100, sizeof(long));
  marcadores[omp_get_max_threads() - 1] = tam_arquivo;

#pragma omp parallel
  {
    FILE *f;
    f = fopen(argv[1], "r");
    int n_senadores, n_dep_fed, n_dep_est;
    long offset;
    char buff = '\0';
    int voto;
    while(buff != '\n')
      fscanf(f, "%c", &buff);
    if(omp_get_thread_num() > 0){
      fscanf(f, "%c", &buff);
      offset = (long) tam_arquivo * (omp_get_thread_num()) / omp_get_max_threads();
      fseek(f, offset, SEEK_CUR);
      while(buff != '\n')
        fscanf(f, "%c", &buff);
#pragma omp critical
      marcadores[omp_get_thread_num() - 1] = ftell(f);
    }
#pragma omp barrier
    while(fscanf(f, "%d", &voto) != EOF && ftell(f) < marcadores[omp_get_thread_num()]){
      if (voto > 9){
        leitura_votos[omp_get_thread_num()][voto]++;
        leitura_votos_validos[omp_get_thread_num()]++;
      }
      else
        leitura_votos_invalidos[omp_get_thread_num()]++;
    }
  }

  for(int i = 0; i < omp_get_max_threads(); i++){
    n_voto += leitura_votos_validos[i];
    n_voto_invalido += leitura_votos_invalidos[i];
  }
  for(int i = 0; i < 99999; i++)
    for(int j = 0; j < omp_get_max_threads(); j++)
      votos[i] += leitura_votos[j][i];

#pragma omp parallel for
	for(int i = 0; i < 4; i++){
		if(i == 0){
			presidente_vencedor = conta_votos_presidente(votos);
		}
		if(i == 1){
			conta_votos_senador(votos, senador_vencedor, n_senadores);
		}
		if(i == 2){
			conta_votos_dep_fed(votos, dep_fed_vencedor, n_dep_fed);
		}
		if(i == 3){
			conta_votos_dep_est(votos, dep_est_vencedor, n_dep_est);
		}
	}

	printf("%d %d\n", n_voto, n_voto_invalido);

	if(!presidente_vencedor)
		printf("Segundo turno\n");
	else
		printf("%d\n", presidente_vencedor);

	for(int i = 0; i < n_senadores - 1; i++)
		printf("%d ", senador_vencedor[i]);
	printf("%d\n", senador_vencedor[n_senadores - 1]);

	for(int i = 0; i < n_dep_fed - 1; i++)
		printf("%d ", dep_fed_vencedor[i]);
	printf("%d\n", dep_fed_vencedor[n_dep_fed - 1]);

	for(int i = 0; i < n_dep_est - 1; i++)
		printf("%d ", dep_est_vencedor[i]);
	printf("%d\n", dep_est_vencedor[n_dep_est - 1]);

	return 0;
}

int conta_votos_presidente(int *votos){
	int vencedor;
	int n_votos_total = 0;
	int n_votos = 0;
	for (int i = 10; i < 100; i++){
		n_votos_total += votos[i];
		if(n_votos < votos[i]){
			vencedor = i;
			n_votos = votos[i];
		}
		else if(votos[i] == n_votos)
			vencedor = 0;
	}
	double porcentagem = (double) votos[vencedor] / (double) n_votos_total;
	double porcentagem_min = 51.0/100.0;
	if (vencedor != 0 && porcentagem < porcentagem_min)
		return 0;
	return vencedor;
}

void conta_votos_senador(int *votos, int *senador_vencedor, int n_senadores){
	int n_votos = 0;
	for (int i = 0; i < n_senadores; i++){
		for(int j = 100; j < 1000; j++){
			if(n_votos < votos[j]){
				senador_vencedor[i] = j;
				n_votos = votos[j];
			}
			else if(n_votos > 0 && n_votos == votos[j]){
				senador_vencedor[i] = j;
				n_votos = votos[j];
			}
		}
		votos[senador_vencedor[i]] = 0;
		n_votos = 0;
	}
}

void conta_votos_dep_fed(int *votos, int *dep_fed_vencedor, int n_dep_fed){
	int n_votos = 0;
	for (int i = 0; i < n_dep_fed; i++){
		for(int j = 1000; j < 10000; j++)
			if(n_votos < votos[j]){
				dep_fed_vencedor[i] = j;
				n_votos = votos[j];
			}
			else if(n_votos > 0 && n_votos == votos[j]){
				dep_fed_vencedor[i] = j;
				n_votos = votos[j];
			}
		votos[dep_fed_vencedor[i]] = 0;
		n_votos = 0;
	}
}

void conta_votos_dep_est(int *votos, int *dep_est_vencedor, int n_dep_est){
	int n_votos = 0;
	for (int i = 0; i < n_dep_est; i++){
		for(int j = 10000; j < 100000; j++)
			if(n_votos < votos[j]){
				dep_est_vencedor[i] = j;
				n_votos = votos[j];
			}
			else if(n_votos > 0 && n_votos == votos[j]){
				dep_est_vencedor[i] = j;
				n_votos = votos[j];
			}
		votos[dep_est_vencedor[i]] = 0;
		n_votos = 0;
	}
}
