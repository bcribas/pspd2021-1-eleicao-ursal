#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int resultadoUrna ( int Candidato[], int tipoCandidato) {

	int tamanhoMax = 0;
	int quantidadeVotos = 0;
	int numeroCandidato = 0;

	if(tipoCandidato == 1) tamanhoMax = 1000;
	else {
		if(tipoCandidato == 2) tamanhoMax = 10000;
		else tamanhoMax = 100000;
	}

	for(int i = tamanhoMax-1; i>=0; i--) {
		if ( quantidadeVotos < Candidato[i] ) {
			quantidadeVotos = Candidato[i];
			numeroCandidato = i;
		}
	}

	Candidato[numeroCandidato] = 0;

	return numeroCandidato;


}

int main() {

	int Presidentes[100];
	memset(Presidentes, 0, sizeof(int));
	int Senadores[1000];
	memset(Senadores, 0, sizeof(int));
	int Federal[100000];
	memset(Federal, 0, sizeof(int));
	int Estadual[1000000];
	memset(Estadual, 0, sizeof(int));

	int numeroSenadores, numeroFederal, numeroEstadual;
	int numeroVoto = 0;
	int votosValidos = 0;
	int votosInvalidos = 0;
	int votosPresidente = 0;
	float ganhouPrimeiroTurno = 0.51f;

	int numeroCandidato = 0;


	scanf("%d %d %d", &numeroSenadores, &numeroFederal, &numeroEstadual);

	while( scanf("%d", &numeroVoto) != EOF ) {
		if( numeroVoto > 0) {
			votosValidos++;

			if( (numeroVoto / 10000) > 0 ) {

				(Estadual[numeroVoto]) += 1 ;

			} else {
				if( ( numeroVoto / 1000 ) > 0 ) {

					Federal[numeroVoto] += 1;

				} else {
					if ( (numeroVoto / 100 ) > 0 ) {

						Senadores[numeroVoto] += 1;

					} else {
						votosPresidente++;

						Presidentes[numeroVoto] += 1;
					}
				}
			}

		} else votosInvalidos++;
	}

	printf("%d %d\n", votosValidos, votosInvalidos);

	numeroVoto = 0;

	for(int i = 0; i<100; i++) {
		if ( numeroVoto < Presidentes[i]){
			 numeroVoto = Presidentes[i];
			 numeroCandidato = i;
		}
	}

		if( ( (float)Presidentes[numeroCandidato]/(float)votosPresidente ) >= ganhouPrimeiroTurno ) {
			printf("%d\n", numeroCandidato);

		}else  printf("Segundo turno\n");


	while( numeroSenadores > 0 ) {

		if (numeroSenadores > 1) printf("%d ",resultadoUrna( Senadores, 1 ));
		else printf("%d\n",resultadoUrna( Senadores, 1 ));
		numeroSenadores--;

	}

	while( numeroFederal > 0 ) {

		if (numeroFederal > 1) printf("%d ",resultadoUrna( Federal, 2 ));
		else printf("%d\n",resultadoUrna( Federal, 2 ));
		numeroFederal--;
	}

	while( numeroEstadual > 0 ) {

		if (numeroEstadual > 1) printf("%d ",resultadoUrna( Estadual, 3 ));
		else printf("%d\n",resultadoUrna( Estadual, 3 ));
		numeroEstadual--;
	}


	return 0;
}
