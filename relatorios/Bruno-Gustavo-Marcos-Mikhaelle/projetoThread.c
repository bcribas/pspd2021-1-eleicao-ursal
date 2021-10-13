#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>
#include <time.h>

// clock_t t;

// matriz que irá guardar o numero de voto por candidado
#define SIZE_COLUMN 1000000
// linhas representam as threads
#define SIZE_ROWS 16  // numero máximo de threads 


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

	// vetores que armazenam a contagem de votos
	int Presidentes[100]; 
	memset(Presidentes, 0, sizeof(int));   // zera vetor
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

	// threads existentes no processo
    int numberThreads;

    long int bytesIniciais;
    long int bytesFinais;
	int numBytes;


#pragma omp parallel reduction(+:votosValidos,votosInvalidos, votosPresidente)
{

	// conta numero de threads
	numberThreads = omp_get_num_threads();

	//abre arquivo com permissão de leitura
	FILE *arquivo=fopen(argv[1], "r");

	if (arquivo==NULL) {
		printf("Error ao abrir arquivo\n");
	}

	// aponta o ponteiro de leitura para o começo do arquivo
	fseek(arquivo, 0, SEEK_CUR);	
	// le a primeira linha (numero de senadores, numero deputado federal, numero deputados estadual)
	fscanf(arquivo, "%d %d %d", &numeroSenadores, &numeroFederal, &numeroEstadual);

	// ftell retorna a posição atual do ponteiro no arquivo
	// necessario adicionar +1 para contar o \n e passar para a próxima linha
	bytesIniciais = ftell(arquivo) + 1; 	   // posição de inicio dos votos

	// envia ponteiro para o final do arquivo
	fseek(arquivo, 0, SEEK_END);
    bytesFinais = ftell(arquivo);              	// posição final dos votos
    numBytes = bytesFinais - bytesIniciais;    	// armazena o numero de bytes total do arquivo
												//cada caracter é 1 byte

	int bytesDivididos = (numBytes/omp_get_num_threads());     // numero de bytes por thread

	// coloca o ponteiro no arquivo de acordo com o numero da thread 
	// que esta executando o processo (0,1,2...)
    fseek(arquivo, bytesIniciais+(bytesDivididos*omp_get_thread_num()) , SEEK_SET );

    int valorDest = ftell(arquivo);
    valorDest+=(bytesDivididos-1); 
    int c = fgetc_unlocked(arquivo);   						 // pega o caracter
    fseek(arquivo, -1, SEEK_CUR);                            // volta uma posição

	// printf("-----------------------------Thread %d: inicio %ld\n", omp_get_thread_num(), ftell(arquivo));

	// verifica se há um "\n" antes ou se ele está em um "\n"
	// continua executando até achar o "\n" para começar a contagem dos votos
	// evita de começar no "meio" de um numero


	if (c != '\n') {
		fseek(arquivo, -1, SEEK_CUR);
		c = fgetc_unlocked(arquivo);

		if (c != '\n') 	while (c != '\n') c = fgetc_unlocked(arquivo);
	} else {
		fseek(arquivo, 1, SEEK_CUR);   
	}

		// caso seja a ultima thread ele coloca o ponteiro de parada no final
		// para que seja possível ler até o final
		int lastThread = (omp_get_num_threads())-1;
    	if( lastThread == omp_get_thread_num() ) valorDest = bytesFinais-1;   
	
		int numeroVoto = 0;
		char stringLida[10];

		// MIGUÉÉÉÉ
		if (ftell(arquivo) == valorDest )valorDest++;

		// executa o laço enquanto o ponteiro não chega ao final
		while(ftell(arquivo) < valorDest+1 ) {
			// fscanf(arquivo, "%d", &numeroVoto);
			fgets_unlocked(stringLida ,10,arquivo);
        	numeroVoto = atoi(stringLida);

			//verifica se voto é valido
			if( numeroVoto > 0) {
				votosValidos++;
				votos[omp_get_thread_num()][numeroVoto]++;  //adiciona um voto na posição da matriz
				// printf("Thread %d: voto %d Valido!\n", omp_get_thread_num(), numeroVoto);

				// verifica se é um candidato a presidente
				if( (numeroVoto / 10) > 0 && (numeroVoto / 100) < 1) votosPresidente++; 

			} else 
			{
				// printf("Thread %d: voto %d INVALIDO!\n", omp_get_thread_num(), numeroVoto);
				votosInvalidos++;
			}

		}
	}

	printf("%d %d\n", votosValidos, votosInvalidos);
	// printf("Dados Contados: %d\n", numBytes);


	// t = clock();
	// coloca o numero de votos no vetor na posição dos respectivos candidatos
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

	// pega numero do candidato presidente com maior voto
	for(int i = 0; i<100; ++i) {
		if ( numeroVoto < Presidentes[i]){
			 numeroVoto = Presidentes[i];
			 numeroCandidato = i;
		}
	}
        // verifica se votos é maior que 51% dos votos válidos
		if( ( (float)Presidentes[numeroCandidato]/(float)votosPresidente ) >= ganhouPrimeiroTurno ) {
			printf("%d\n", numeroCandidato);

		}else  printf("Segundo turno\n");
	
	
	// calcula resultado da urna para os senadores
	while( numeroSenadores > 0 ) {

		if (numeroSenadores > 1) printf("%d ",resultadoUrna( Senadores, 1 ));
		else printf("%d\n",resultadoUrna( Senadores, 1 ));
		numeroSenadores--;

	}

	// calcula resultado da urna para os deputados federais
	while( numeroFederal > 0 ) {

		if (numeroFederal > 1) printf("%d ",resultadoUrna( Federal, 2 ));
		else printf("%d\n",resultadoUrna( Federal, 2 ));
		numeroFederal--;
	}

	// calcula resultado da urna para os deputados estaduais
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
