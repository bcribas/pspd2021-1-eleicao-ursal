# include <stdio.h>
# include <stdlib.h>
// ================================  Main ========================================== 
int main( int argc, char *argv[] )
{
    if(argc < 2)
    {
        fprintf(stderr, "[Erro]: Parametro <caminho/nome_do_arquivo> nao informado!\n");
        exit(EXIT_FAILURE);
    }

// =================================================================================
    int i, num_linhas, fsize;
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    printf("\n\n====================================================\n");
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp) - 1;
    printf("Tanhanho do arquivo = %d B = %d MB\n", fsize, fsize/1048576);

    rewind(fp);

    while ((fscanf(fp, "%*d"))!= EOF )
        num_linhas++;

    printf("====================================================\n");
    printf("Numero de linhas do arquivo = %d\n", num_linhas - 3);
    printf("====================================================\n");
    printf("Media Bytes por linha = %d\n", fsize / num_linhas);
    printf("====================================================\n\n\n");


    return (EXIT_SUCCESS);
}