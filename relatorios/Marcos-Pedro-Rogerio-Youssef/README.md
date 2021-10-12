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

|Entrada       |Número de Linhas|Sequencial|ribas-ac-ingenuo|ribas-ac-ajustado|1 Thread|2 Threads|4 Threads|8 Threads|12 Threads|16 Threads|
|--------------|----------------|----------|----------------|-----------------|--------|---------|---------|---------|----------|----------|
|file001-sample|21              |0         |0               |0                |0,00    |0.04     |0.01     |0.03     |0.03      |0.04      |
|file002-sample|51              |0         |0               |0                |0,00    |0.02     |0.02     |0.04     |0.01      |0.02      |
|file003       |256             |0.02      |0               |0                |0,00    |0.04     |0.02     |0.04     |0.06      |0.04      |
|file004       |1371            |0.02      |0               |0                |0,00    |0.04     |0.03     |0.02     |0.03      |0.04      |
|file005-sample|10296           |0.08      |0               |0                |0,00    |0.05     |0.07     |0.05     |0.08      |0.08      |
|file006       |100171          |0.04      |0.01            |0.01             |0.02    |0.05     |0.07     |0.08     |0.07      |0.08      |
|file007-sample|1000491         |0.14      |0.08            |0.03             |0.06    |0.23     |0.22     |0.23     |0.23      |0.26      |
|file008       |10000176        |0.91      |0.72            |0.11             |0.62    |1.84     |1.78     |1.84     |1.85      |1.84      |
|file009       |26              |0         |0               |0                |0,00    |0,00     |0.01     |0.04     |0.01      |0.01      |
|file010-big   |50000001        |4.49      |3.41            |0.44             |03.01   |8.84     |8.62     |8.67     |8.93      |8.92      |
|file-011-big  |200000001       |16.99     |12.27           |1.77             |12.22   |32.65    |34.03    |33.45    |35.54     |35.83     |

<div>
    <a href="https://plotly.com/~rogerioo/7/?share_key=quggXYU785ruiP6OVN3yHc" target="_blank" title="Benchmarking" style="display: block; text-align: center;"><img src="https://plotly.com/~rogerioo/7.png?share_key=quggXYU785ruiP6OVN3yHc" alt="Benchmarking" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plotly.com/404.png';" /></a>
    <script data-plotly="rogerioo:7" sharekey-plotly="quggXYU785ruiP6OVN3yHc" src="https://plotly.com/embed.js" async></script>
</div>