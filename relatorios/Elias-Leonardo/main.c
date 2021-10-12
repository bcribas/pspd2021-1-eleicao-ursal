#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <omp.h>


/**
 * -> Atualizar implementação pra rodar no openmp
 * -> Gerar relatório 
 * 
 */
struct file_limits 
{
    int start_limit;
} typedef File_limits;

struct candidato
{
    int index;
    int votos;
} typedef Candidato;

static Candidato candidatos[99999] = { 0,0 };

int compare(const void *a, const void *b) { 
    Candidato *votoA = (Candidato*) a;
    Candidato *votoB = (Candidato*) b;

    if ((votoB->votos - votoA->votos) == 0)
        return votoB->index - votoA->index;
    return (votoB->votos - votoA->votos);
}

int main(int argc, char* argv[]) {
    int qtdVotosPresidente = 0, votosValidos = 0, invalidos = 0, num_threads = omp_get_max_threads();

    int senadoresEleitos, depFederalEleitos, depEstaduaisEleitos, voto;

    size_t read;

    if (argc < 2) {
        printf(" O caminho do arquivo deve ser especificado!!\n");
        return 1;
    }
    if (argc == 3) {
        num_threads = atoi(argv[2]);
    }

    FILE *fptr;

    fptr = fopen(argv[1],"r");

    if (fptr == NULL) {
        printf("ERRO\n");
        return 1;
    }
    
    fseek(fptr, 0L, SEEK_END);

    int total_file_size = ftell(fptr);

    rewind(fptr);
    fscanf(fptr, "%d %d %d", &senadoresEleitos, &depFederalEleitos, &depEstaduaisEleitos);

    int initial_limit = ftell(fptr);

    File_limits *limits_per_thread = malloc(sizeof(File_limits)*num_threads);

    int file_size = total_file_size - initial_limit;

    size_t len = sizeof(int);

    limits_per_thread[0].start_limit = initial_limit;

    for (int i = 1; i < num_threads; i++ )
    {
        limits_per_thread[i].start_limit = i * (file_size/num_threads);
    }

    fclose(fptr);

    for (int i = 0; i < 99999; i++){
        candidatos[i].index = i;
    }

    #pragma omp parallel private(voto) num_threads(num_threads) reduction(+:votosValidos, invalidos)
    {
        char *line = NULL;
        line = malloc(sizeof(char)*6);

        FILE *fptr;

        size_t len = sizeof(int);

        fptr = fopen(argv[1],"r");

        fseek(fptr, limits_per_thread[omp_get_thread_num()].start_limit, SEEK_SET);

        char ch;
        do {
            fscanf(fptr, "%c", &ch);
        } while(ch != '\n');

        int end_limit = (num_threads - 1) == omp_get_thread_num()
                ? total_file_size
                : limits_per_thread[omp_get_thread_num() + 1].start_limit;

        for (int i = ftell(fptr); i < end_limit;)
        {   
            i += getline(&line, &len, fptr);
            voto = atoi(line);
        
            if (voto > 0) {
                votosValidos++;

                #pragma omp atomic
                candidatos[voto].votos++;
            }
            else {
                invalidos++;
            }

        }
        free(line);
        fclose(fptr);
    }

    // TA OK
    for (int i = 0; i < 100; i++) {
        qtdVotosPresidente += candidatos[i].votos;
    }

    qsort(candidatos, 99, sizeof(Candidato), compare);

    printf("%d %d\n", votosValidos, invalidos);

    if (candidatos[0].votos > (qtdVotosPresidente / 2))
        printf("%d\n",candidatos[0].index);
    else
        printf("Segundo turno\n");

    qsort(candidatos + 100, 900, sizeof(Candidato), compare);

    for (int i = 0; i < senadoresEleitos; i++) {
        printf("%d", candidatos[i + 100].index);
        if (i != (senadoresEleitos - 1))
            printf(" ");
    }

    printf("\n");

    qsort(candidatos + 1000, 9000, sizeof(Candidato), compare);

    for (int i = 0; i < depFederalEleitos; i++) {
        printf("%d", candidatos[i + 1000].index);
        if (i != (depFederalEleitos - 1))
            printf(" ");
    }

    printf("\n");

    qsort(candidatos + 10000, 90000, sizeof(Candidato), compare);

    for (int i = 0; i < depEstaduaisEleitos; i++) {
        printf("%d", candidatos[i + 10000].index);
        if (i != (depEstaduaisEleitos - 1))
            printf(" ");
    }

    printf("\n");

    free(limits_per_thread);
    return 0;
}