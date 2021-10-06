#include "ursal.h"

int compara_votos (const void *x, const void *y) 
{
    int a, b, c, d;
    a = ((Candidato*)x)->cont_votos;
    b = ((Candidato*)y)->cont_votos;
    c = ((Candidato*)x)->id;
    d = ((Candidato*)y)->id;

    if (a != b)
        return (a - b);
    else 
        return (c - d);
}
//===============================================================================
void ler_arquivo(int file_size, char *arquivo)
{
    int i, id_th, voto, bloco, inicio, fim, resto;
    
    int votos_invalidos_local = 0, 
        votos_validos_local = 0,
        votos_presidente_local = 0; 

    id_th = omp_get_thread_num();  
    FILE *fp = fopen(arquivo, "r");

    // definir blocos/chunks de leitura
    bloco = file_size / num_th;
    inicio = bloco * id_th;
    fim = inicio + bloco;
    
    fseek(fp, inicio, SEEK_SET);
    while(fgetc(fp) != '\n');

    while(inicio < fim)
    {
        fscanf(fp, "%d", &voto);
        if(voto > 0)
        {   
            votos_validos_local++;
            matriz_votos_global[id_th][voto]++;

            if( voto < 100)
                votos_presidente_local++;
        }
        else
            votos_invalidos_local++; 
        inicio = ftell(fp);
    }
    votos_invalidos[id_th]  = votos_invalidos_local;
    votos_presidente[id_th] = votos_presidente_local;
    votos_validos[id_th]    = votos_validos_local;

    fclose(fp);
}

//===============================================================================
void unificar_resultado()
{
    int sum, validos, i, j;
    validos = 0;

    for( j = 0; j < MAX; j++)
    {
        sum = 0;
        for( i = 0; i < num_th; i++)
            if (matriz_votos_global[i][j] != 0 ){
                sum += matriz_votos_global[i][j];
            }
        
        if(sum){
            votos_unidos[j].id = j;
            votos_unidos[j].cont_votos = sum;
            validos += sum; 
        }
    }

    for(i = 0; i < num_th; i++)
    {
        votos_invalidos[num_th + 1]  += votos_invalidos[i];
        votos_validos[num_th + 1]    += votos_validos[i];
        votos_presidente[num_th + 1] += votos_presidente[i];
    }
}


void ordenar_printar(int *sfe)
{
    int sfe_offset[3], i, j;
    float percent;

    sfe_offset[0] = s_offset;
    sfe_offset[1] = f_offset;
    sfe_offset[2] = e_offset;

    qsort(votos_unidos           ,  p_offset            , sizeof(Candidato), compara_votos);
    qsort(votos_unidos + p_offset, (s_offset - p_offset), sizeof(Candidato), compara_votos);
    qsort(votos_unidos + s_offset, (f_offset - s_offset), sizeof(Candidato), compara_votos);
    qsort(votos_unidos + f_offset, (e_offset - f_offset), sizeof(Candidato), compara_votos);

    percent = (votos_unidos[99].cont_votos * 100) / votos_presidente[num_th + 1];
    
    printf("%d %d\n",votos_validos[num_th + 1], votos_invalidos[num_th + 1]);
    if(percent > 51)
        printf("%d\n", votos_unidos[99].id);
    else 
        printf("Segundo turno\n");
    
    for(j = 0; j < 3; j++)
    {
        printf("%d", votos_unidos[sfe_offset[j] - 1].id);
        for(i = 2; i <= sfe[j]; i++)
            printf(" %d", votos_unidos[sfe_offset[j] -i].id);
        
        printf("\n");
    }
}