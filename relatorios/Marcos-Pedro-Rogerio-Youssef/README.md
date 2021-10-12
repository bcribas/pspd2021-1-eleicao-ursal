# Relatório

## Membros da equipe

| Nome            | Matrícula |
| --------------- | --------- |
| Marcos Nery     | 170017885 |
| Pedro Igor      | 170062635 |
| Rogério Júnior  | 170021751 |
| Youssef Muhamad | 170024334 |

## Experimentos e tentativas da equipe

A abordagem inicial da equipe foi entender o problema e resolvê-lo de forma sequencial e sem a utilização de paralelismo, essa solução pode ser encontrada em: /src/single.c, nela foi utilizado vários laços individuais para fazer a contagem dos votos.

Com base na tentativa inicial, a equipe observou que a leitura do arquivo de votos se provou ser a parte mais demorada, por isso a segunda abordagem foi quebrar o arquivo de votos em pedaços de forma a cada thread ficar responsável por esses chunks, e fazer a tratativa de possíveis corner cases por conta da divisão do arquivo em threads.

Mesmo tratando a entrada por meio de chunks a solução ainda parecia distante da solução ajustada do professor, decidimos então buscar formas de melhorar a solução. Para isso removemos a parte da leitura dos 3 inteiros da parte sequencial e permitimos que cada thread realizasse uma leitura desses valores pra evitar que todas as threads ficassem esperando pela finalização do fscanf sequencial. Além disso removemos a etapa onde era criado um vetor com posições que cada thread acessaria, criamos uma função que recebe o número da thread e calcula a posição final e inicial de leitura de cada thread. Removemos também a comparação com a posição final utilizando o ftell e substituímos por uma variável que se incrementava na quantidade de bytes lidos.

## Informações sobre as regiões críticas de paralelização

Com base na explicação sobre as tentativas da equipe, o código paralelizado utilizou as diretivas do OpenMP: omp, parallel, reduction. Além proteger as variáveis validos, invalidos, quantidade_p, já que existiria uma race condition ao várias threads tentarem acessá-las.
```c
    #pragma omp parallel reduction(+ \
                               : validos, invalidos, quantidade_p)
    {
        
        FILE *read_file = fopen(in, "r");
        fscanf(read_file, "%ld %ld %ld\n", &s, &f, &e);
        int votes_start_position = ftell(read_file);
        fseek(read_file, 0L, SEEK_END);
        long int votes_end_position = ftell(read_file);
        long int bytes_size = votes_end_position - votes_start_position;
        threads = omp_get_num_threads();
        int chunk_size = (int)floor(bytes_size / threads);
        int voto;
        int thread_num = omp_get_thread_num();
        int start_position = get_start_position(thread_num, read_file, votes_start_position, chunk_size, threads);
        int end_position = get_end_position(thread_num, read_file, votes_start_position, chunk_size, threads);
        
        fseek(read_file, start_position, SEEK_SET);
        char *line;
        line = malloc(6);
        size_t size = sizeof(int);
        int total_read = start_position;
        while (fscanf(read_file, "%d%n", &voto, &size) != EOF)
        {
            total_read+=size;
            if (total_read > end_position && thread_num<(threads-1))
            {
                break;
            }
            if (voto >= 10)
            {
                validos++;
                //presidente
                if (((float)voto) / size_p <= 1.0)
                {
                    quantidade_p++;
                }
                #pragma omp atomic
                matrix[thread_num][voto]++;
            }
            else
            {
                invalidos++;
            }
        }
        free(line);
        fclose(read_file);
    }
```
## Teste de desempenho

### Comparar os tempos da versão:

- sequencial da equipe
- das soluções de referência fornecidas pelo professor
- paralela com 1,2,4,8,12,16 threads

- Gráficos com os tempos das diversas soluções
- Use todas as entradas disponíveis neste repositório para fazer os testes para descobrir se a versão paralela é prejudicada nas entradas pequenas


## Resultados - V1

|Entrada       |Número de Linhas|Sequencial|ribas-ac-ingenuo|ribas-ac-ajustado|1 Thread|2 Threads|4 Threads|8 Threads|12 Threads|16 Threads|
|--------------|----------------|----------|----------------|-----------------|--------|---------|---------|---------|----------|----------|
|file001-sample|21              |0         |0               |0                |0.02    |0.01     |0.01     |0.02     |0.01      |0.03      |
|file002-sample|51              |0.01      |0               |0                |0.01    |0.03     |0.02     |0.01     |0.01      |0.02      |
|file003       |256             |0.01      |0               |0                |0.02    |0.02     |0.02     |0.01     |0.02      |0.05      |
|file004       |1371            |0.01      |0               |0                |0.02    |0.02     |0.02     |0.01     |0.01      |0.02      |
|file005-sample|10296           |0.03      |0               |0                |0.03    |0.04     |0.03     |0.03     |0.02      |0.04      |
|file006       |100171          |0.03      |0.01            |0.01             |0.02    |0.02     |0.03     |0.03     |0.03      |0.02      |
|file007-sample|1000491         |0.07      |0.05            |0.01             |0.10    |0.03     |0.04     |0.03     |0.04      |0.07      |
|file008       |10000176        |0.54      |0.49            |0.07             |0.16    |0.19     |0.19     |0.17     |0.17      |0.16      |
|file009       |26              |0         |0               |0                |0.01    |0.01     |0.01     |0.03     |0.02      |0.00      |
|file010-big   |50000001        |2.76      |2.34            |0.32             |0.89    |0.81     |0.8      |0.83     |0.8       |0.86      |
|file-011-big  |200000001       |10.45     |9.34            |1.24             |3.20    |3.43     |3.51     |3.31     |2.99      |3.45      |

<div>
    <a href="https://plotly.com/~MarcosNBJ/5/?share_key=YB7J4nNBxmnxAN1oWNpLXX" target="_blank" title="Plot 5" style="display: block; text-align: center;"><img src="https://plotly.com/~MarcosNBJ/5.png?share_key=YB7J4nNBxmnxAN1oWNpLXX" alt="Plot 5" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plotly.com/404.png';" /></a>
    <script data-plotly="MarcosNBJ:5" sharekey-plotly="YB7J4nNBxmnxAN1oWNpLXX" src="https://plotly.com/embed.js" async></script>
</div>

## Resultados - V2

|Entrada       |Número de Linhas|Sequencial|ribas-ac-ingenuo|ribas-ac-ajustado|1 Thread|2 Threads|4 Threads|8 Threads|12 Threads|16 Threads|
|--------------|----------------|----------|----------------|-----------------|--------|---------|---------|---------|----------|----------|
|file001-sample|21              |0         |0               |0                |0,00    |0.00     |0.00     |0.01     |0.03      |0.04      |
|file002-sample|51              |0.01      |0               |0                |0,00    |0.00     |0.01     |0.01     |0.02      |0.02      |
|file003       |256             |0.01      |0               |0                |0,00    |0.00     |0.01     |0.06     |0.03      |0.02      |
|file004       |1371            |0.01      |0               |0                |0,00    |0.00     |0.00     |0.01     |0.02      |0.02      |
|file005-sample|10296           |0.03      |0               |0                |0.02    |0.01     |0.01     |0.06     |0.13      |0.03      |
|file006       |100171          |0.03      |0.01            |0.01             |0.02    |0.01     |0.01     |0.02     |0.03      |0.07      |
|file007-sample|1000491         |0.07      |0.05            |0.01             |0.07    |0.04     |0.02     |0.11     |0.04      |0.07      |
|file008       |10000176        |0.54      |0.49            |0.07             |0.6     |0.33     |0.2      |0.22     |0.14      |0.17      |
|file009       |26              |0         |0               |0                |0,00    |0,00     |0        |0.01     |0.02      |0.01      |
|file010-big   |50000001        |2.76      |2.34            |0.32             |03.03   |1.67     |0.92     |0.89     |0.71      |0.71      |
|file-011-big  |200000001       |10.45     |9.34            |1.24             |11.74   |6.32     |3.32     |2.67     |2.44      |2.69      |

<div>
    <a href="https://plotly.com/~MarcosNBJ/7/?share_key=JGzApkUO65MkKhjAR8dshS" target="_blank" title="pspd-new" style="display: block; text-align: center;"><img src="https://plotly.com/~MarcosNBJ/7.png?share_key=JGzApkUO65MkKhjAR8dshS" alt="pspd-new" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plotly.com/404.png';" /></a>
    <script data-plotly="MarcosNBJ:7" sharekey-plotly="JGzApkUO65MkKhjAR8dshS" src="https://plotly.com/embed.js" async></script>
</div>
