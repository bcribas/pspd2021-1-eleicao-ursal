#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

// clock_t t;

#define SIZE_COLUMN 1000000
#define SIZE_ROWS 16

int votos[SIZE_ROWS][SIZE_COLUMN];

int resultadoUrna ( int Candidato[], int tipoCandidato) {

	int tamanhoMax = 0;
	int tamanhoMin = 0;
	int quantidadeVotos = 0;
	int numeroCandidato = 0;

	if(tipoCandidato == 1) {
		tamanhoMax = 1000;
		tamanhoMin = 99;
	}
	else {
		if(tipoCandidato == 2) {
		 tamanhoMax = 10000;
		 tamanhoMin = 999;
		}
		else {
			tamanhoMax = 100000;
			tamanhoMin = 9999;
		}
	}

	for(int i = tamanhoMax-1; i>=tamanhoMin; --i) {
		if ( quantidadeVotos < Candidato[i] ) {
			quantidadeVotos = Candidato[i];
			numeroCandidato = i;
		}
	}

	Candidato[numeroCandidato] = 0; 

	return numeroCandidato;


}

int main(int argc, char *argv[]) {

	int Presidentes[100];
	memset(Presidentes, 0, sizeof(int));
	int Senadores[1000];
	memset(Senadores, 0, sizeof(int));
	int Federal[100000];
	memset(Federal, 0, sizeof(int));
	int Estadual[1000000];
	memset(Estadual, 0, sizeof(int));

	int numeroSenadores, numeroFederal, numeroEstadual;
	int votosValidos = 0;
	int votosInvalidos = 0;
	int votosPresidente = 0;
	float ganhouPrimeiroTurno = 0.51f;

	int numeroCandidato = 0;

    int numberThreads;

    long int bytesIniciais;
    long int bytesFinais;
	int numBytes;

#pragma omp parallel reduction(+:votosValidos,votosInvalidos, votosPresidente)
{

	numberThreads = omp_get_num_threads();

	FILE *arquivo=fopen(argv[1], "r");

	if (arquivo==NULL) {
		printf("Error ao abrir arquivo\n");
	}

	fseek(arquivo, 0, SEEK_CUR);	
	fscanf(arquivo, "%d %d %d", &numeroSenadores, &numeroFederal, &numeroEstadual);

	bytesIniciais = ftell(arquivo) + 1;

	fseek(arquivo, 0, SEEK_END);
    bytesFinais = ftell(arquivo);
    numBytes = bytesFinais - bytesIniciais;

	int bytesDivididos = (numBytes/omp_get_num_threads());

    fseek(arquivo, bytesIniciais+(bytesDivididos*omp_get_thread_num()) , SEEK_SET );

    int valorDest = ftell(arquivo);
    valorDest+=(bytesDivididos-1);
    int c = fgetc(arquivo);
    fseek(arquivo, -1, SEEK_CUR);


    if (c != '\n') {

        fseek(arquivo, -1, SEEK_CUR);
		c = fgetc(arquivo);

		if( c != '\n') {
			while(c != '\n') {
            c = fgetc(arquivo);
        	}
		}
    }

		int lastThread = (omp_get_num_threads())-1;
		
    	if( lastThread == omp_get_thread_num() ) valorDest = bytesFinais-1;   
	
		int numeroVoto = 0;

		// MIGUÉÉÉÉ
		if (ftell(arquivo) == valorDest )valorDest++;

		while(ftell(arquivo) < valorDest ) {
			fscanf(arquivo, "%d", &numeroVoto);

			if( numeroVoto > 0) {
				votosValidos++;
				votos[omp_get_thread_num()][numeroVoto]++;
				// printf("Thread %d: voto %d Valido!\n", omp_get_thread_num(), numeroVoto);

				if( (numeroVoto / 10) > 0 && (numeroVoto / 100) < 1) votosPresidente++;

			} else 
			{
				// printf("Thread %d: voto %d INVALIDO!\n", omp_get_thread_num(), numeroVoto);
				votosInvalidos++;
			}

		}
	}

	printf("%d %d\n", votosValidos, votosInvalidos);
	//printf("Dados Contados: %d\n", numBytes);

	// t = clock();

	for(int i=0; i<numberThreads; ++i ){
		for(int j=0; j<1000000; ++j) {
			if( j<100 ) Presidentes[j] += votos[i][j];
			else {
				if( j<1000 ) Senadores[j] += votos[i][j];
				else {
					if( j<10000 ) Federal[j] += votos[i][j];
					else Estadual[j] += votos[i][j];
				}
			}

		}
	}


	int numeroVoto = 0;

	for(int i = 0; i<100; ++i) {
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

	// t= clock() - t;

	// double time_taken = ((double)t)/CLOCKS_PER_SEC; // in seconds

	// printf("tempo %f\n", time_taken);

	return 0;
}
