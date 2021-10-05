#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int conta_votos_presidente(int *);
void conta_votos_senador(int *, int *, int);
void conta_votos_dep_fed(int *, int *, int);
void conta_votos_dep_est(int *, int *, int);

int main(){
	int *votos;
	votos = calloc(999999, sizeof(int));

	int presidente_vencedor;
	int *senador_vencedor;
	int *dep_fed_vencedor;
	int *dep_est_vencedor;


	int n_senadores, n_dep_fed, n_dep_est;
	int voto;
  int teste;
	int n_voto = 0;
	int n_voto_invalido = 0;

	scanf("%d", &n_senadores);
	if(n_senadores > 999) n_senadores = 999;

	scanf("%d", &n_dep_fed);
	if(n_dep_fed > 9999) n_senadores = 9999;

	scanf("%d", &n_dep_est);
	if(n_dep_est > 99999) n_senadores = 99999;
	
	senador_vencedor = malloc(n_senadores * sizeof(int));
	dep_fed_vencedor = malloc(n_dep_fed * sizeof(int));
	dep_est_vencedor = malloc(n_dep_est * sizeof(int));

	while(scanf("%d%n", &voto, &teste) != EOF){
		if (voto > 9){
			votos[voto]++;
			n_voto++;
		}
		else
			n_voto_invalido++;
	}
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
