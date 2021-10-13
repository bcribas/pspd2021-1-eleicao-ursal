// =================================================================================
//                              Trabalho 1 - PSPD
// 	Professor  : Bruno Ribas
// 	Alunos     : Flavio Vieira / 
// 	Matricula  : 15/0125682    /
//
// =================================================================================
# include <stdio.h>
# include <stdlib.h>
# include <string.h>



int read_strtok_r(int fsize, FILE *fp);
int read_seq_arq (int fsize, FILE *fp);
int read_seq_men (int fsize, FILE *fp);

// ================================  Main ========================================== 
int main( int argc, char *argv[] )
{
    if(argc < 2)
    {
        fprintf(stderr, "[Erro]: Parametro <caminho/nome_do_arquivo> nao informado!\n");
        exit(EXIT_FAILURE);
    }

    int i, num_linhas, fsize;
    FILE *fp;
    fp = fopen(argv[1], "r");
    
    fseek(fp, 0, SEEK_END);
    fsize = ftell(fp);
    rewind(fp);
    
    num_linhas = 0;
    // num_linhas = read_seq_arq(fsize, fp);
    // num_linhas = read_seq_men(fsize, fp);
    num_linhas = read_strtok_r(fsize, fp);


    printf("\n\n====================================================\n");
    printf("Tanhanho do arquivo = %d B = %d MB\n", fsize, fsize/1048576);
    printf("====================================================\n");
    printf("Numero de linhas do arquivo = %d\n", num_linhas);
    printf("====================================================\n");
    printf("Media Bytes por linha = %d\n", fsize / num_linhas);
    printf("====================================================\n\n\n");


    fclose(fp);
    return (EXIT_SUCCESS);
}
//? =================================================================================
int read_strtok_r(int fsize, FILE *fp)
{
    char *token = NULL; 
    char *rest = NULL;
    char *buffer;       // buffer[fsize]      //buffer tamanho total do arquivo
    int num_linhas = 0;

    buffer = (char *) malloc(sizeof(int) * fsize);
    fread(buffer, fsize, 1, fp);
    
    // token = strtok_r(buffer, "\n", &rest); 
    // num_linhas++;
    // while(token != NULL)
    // {
    //     token = strtok_r(NULL, "\n", &rest);
    //     num_linhas++;
    // }


    for (token = strtok_r(buffer, "\n", &rest); token != NULL; token = strtok_r(NULL, "\n", &rest))  
        num_linhas++;

    return num_linhas;
}
//? =================================================================================
int read_seq_arq(int fsize, FILE *fp)
{
    int num_linhas;

    while ((fscanf(fp, "%*d"))!= EOF ) 
        num_linhas++;
    
    return num_linhas;
}
//? =================================================================================

int read_seq_men(int fsize, FILE *fp)
{
    char *buffer, c;       // buffer[fsize]      //buffer tamanho total do arquivo
    int i, num_linhas = 0;

    buffer = (char *) malloc(sizeof(char) * fsize);  // malloc( tamanho do arquivo)
    fread(buffer, fsize, 1, fp);
    i = 0;
    while(i < fsize)
    {
        if(buffer[i] == '\n')
            num_linhas++;
        
        i++;
    }

    return num_linhas;
}