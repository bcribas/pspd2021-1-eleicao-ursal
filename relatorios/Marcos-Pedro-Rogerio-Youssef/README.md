# Relatório

## Membros da equipe

| Nome            | Matrícula |
| --------------- | --------- |
| Marcos Nery     | 170017885 |
| Pedro Igor      | 170062635 |
| Rogério Júnior  | 170021751 |
| Youssef Muhamad | 170024334 |

## Experimentos e tentativas da equipe

A abordagem inicial da equipe foi entender o problema e resolvê-lo de forma sequencial e sem a utilização de paralelismo, essa solução pode ser encontrada em: /src/single.c, nela foram utilizados vários laços individuais para fazer a contagem dos votos.

Com base nessa tentativa inicial, a equipe observou que a leitura do arquivo de votos se provou ser a parte mais demorada, por isso a segunda abordagem foi quebrar o arquivo de votos em pedaços. Dessa forma, a leitura é paralelizada entre diversas threads e ao fim agregamos todos os resultados.

Mesmo tratando a entrada por meio de chunks a solução ainda parecia distante da solução ajustada do professor, decidimos então buscar formas de melhorar esse desempenho. Para isso removemos a parte da leitura dos três primeiros inteiros da parte sequencial e permitimos que cada thread realizasse uma leitura desses valores, para evitar que todas as threads ficassem esperando pela finalização do fscanf sequencial. Além disso, removemos a etapa onde era criado um vetor com posições que cada thread acessaria. Essa etapa foi substituida por uma função que recebe o número da thread e calcula a posição final e inicial que ela deverá ler. Removemos também a comparação com a posição final utilizando o ftell e substituímos por uma variável que é incrementada na quantidade de bytes lidos.

Com essas últimas melhorias, o tempo para solucionar o problema com as entradas maiores diminuiu consideravelmente em todos os testes com quatro threads ou mais. Essa comparação poderá ser vista ao final do relatório.

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

Esses testes foram feitos utilizando a flag de compilação -O3 na máquina Chococino. Cujas especificações são um processador Core i7-8700 de 6 núcleos e 16GB de memória RAM.

Se quiser ver mais detalhes nos gráficos ou avaliar a curva de uma thread de forma isolada, basta clicar no gráfico e interagir com a visualização que será aberta na ferramenta.
Uma vez na ferramenta, basta clicar nas legendas para isolar cada uma das curvas.

## Resultados - Versão Paralelizável 01

Esses são os resultados da primeira tentativa de paralelização do algoritmo.

| Entrada        | Número de Linhas | Sequencial | ribas-ac-ingenuo | ribas-ac-ajustado | 1 Thread | 2 Threads | 4 Threads | 8 Threads | 12 Threads | 16 Threads |
| -------------- | ---------------- | ---------- | ---------------- | ----------------- | -------- | --------- | --------- | --------- | ---------- | ---------- |
| file001-sample | 21               | 0          | 0                | 0                 | 0.02     | 0.01      | 0.01      | 0.02      | 0.01       | 0.03       |
| file002-sample | 51               | 0.01       | 0                | 0                 | 0.01     | 0.03      | 0.02      | 0.01      | 0.01       | 0.02       |
| file003        | 256              | 0.01       | 0                | 0                 | 0.02     | 0.02      | 0.02      | 0.01      | 0.02       | 0.05       |
| file004        | 1371             | 0.01       | 0                | 0                 | 0.02     | 0.02      | 0.02      | 0.01      | 0.01       | 0.02       |
| file005-sample | 10296            | 0.03       | 0                | 0                 | 0.03     | 0.04      | 0.03      | 0.03      | 0.02       | 0.04       |
| file006        | 100171           | 0.03       | 0.01             | 0.01              | 0.02     | 0.02      | 0.03      | 0.03      | 0.03       | 0.02       |
| file007-sample | 1000491          | 0.07       | 0.05             | 0.01              | 0.10     | 0.03      | 0.04      | 0.03      | 0.04       | 0.07       |
| file008        | 10000176         | 0.54       | 0.49             | 0.07              | 0.16     | 0.19      | 0.19      | 0.17      | 0.17       | 0.16       |
| file009        | 26               | 0          | 0                | 0                 | 0.01     | 0.01      | 0.01      | 0.03      | 0.02       | 0.00       |
| file010-big    | 50000001         | 2.76       | 2.34             | 0.32              | 0.89     | 0.81      | 0.8       | 0.83      | 0.8        | 0.86       |
| file-011-big   | 200000001        | 10.45      | 9.34             | 1.24              | 3.20     | 3.43      | 3.51      | 3.31      | 2.99       | 3.45       |

<div>
    <a href="https://plotly.com/~MarcosNBJ/5/?share_key=YB7J4nNBxmnxAN1oWNpLXX" target="_blank" title="Plot 5" style="display: block; text-align: center;"><img src="https://plotly.com/~MarcosNBJ/5.png?share_key=YB7J4nNBxmnxAN1oWNpLXX" alt="Plot 5" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plotly.com/404.png';" /></a>

</div>

## Resultados - Versão Paralelizável 02

Esses são os resultados da segunda tentativa de paralelização do algoritmo, com as melhorias descritas no inicio do relatório aplicadas a solução anterior. É possivel ver que agora o tempo com os maiores números de threads diminuiu bastante, embora ainda não tenha alcançado o do algoritmo ajustado.

| Entrada        | Número de Linhas | Sequencial | ribas-ac-ingenuo | ribas-ac-ajustado | 1 Thread | 2 Threads | 4 Threads | 8 Threads | 12 Threads | 16 Threads |
| -------------- | ---------------- | ---------- | ---------------- | ----------------- | -------- | --------- | --------- | --------- | ---------- | ---------- |
| file001-sample | 21               | 0          | 0                | 0                 | 0,00     | 0.00      | 0.00      | 0.01      | 0.03       | 0.04       |
| file002-sample | 51               | 0.01       | 0                | 0                 | 0,00     | 0.00      | 0.01      | 0.01      | 0.02       | 0.02       |
| file003        | 256              | 0.01       | 0                | 0                 | 0,00     | 0.00      | 0.01      | 0.06      | 0.03       | 0.02       |
| file004        | 1371             | 0.01       | 0                | 0                 | 0,00     | 0.00      | 0.00      | 0.01      | 0.02       | 0.02       |
| file005-sample | 10296            | 0.03       | 0                | 0                 | 0.02     | 0.01      | 0.01      | 0.06      | 0.13       | 0.03       |
| file006        | 100171           | 0.03       | 0.01             | 0.01              | 0.02     | 0.01      | 0.01      | 0.02      | 0.03       | 0.07       |
| file007-sample | 1000491          | 0.07       | 0.05             | 0.01              | 0.07     | 0.04      | 0.02      | 0.11      | 0.04       | 0.07       |
| file008        | 10000176         | 0.54       | 0.49             | 0.07              | 0.6      | 0.33      | 0.2       | 0.22      | 0.14       | 0.17       |
| file009        | 26               | 0          | 0                | 0                 | 0,00     | 0,00      | 0         | 0.01      | 0.02       | 0.01       |
| file010-big    | 50000001         | 2.76       | 2.34             | 0.32              | 03.03    | 1.67      | 0.92      | 0.89      | 0.71       | 0.71       |
| file-011-big   | 200000001        | 10.45      | 9.34             | 1.24              | 11.74    | 6.32      | 3.32      | 2.67      | 2.44       | 2.69       |

<div>
    <a href="https://plotly.com/~MarcosNBJ/7/?share_key=JGzApkUO65MkKhjAR8dshS" target="_blank" title="pspd-new" style="display: block; text-align: center;"><img src="https://plotly.com/~MarcosNBJ/7.png?share_key=JGzApkUO65MkKhjAR8dshS" alt="pspd-new" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plotly.com/404.png';" /></a>
</div>

## Resultados - Versão Paralelizável 03

Dado a oportunidade de melhoria do trabalho(`/src/new.c`), a equipe buscou por formas alternativas de fazer com a leitura fosse mais rápida. Com isso o teste com a função `fread_unlocked()`([unlocked_stdio(3)](https://man7.org/linux/man-pages/man3/unlocked_stdio.3.html)) foi bastante efetivo. Já que o controle do paralelismo está sendo feito por meio das diretivas OpenMP, decidimos por seguir com o `fread_unlocked()`, já que ele não tem o controle dos _locks_, sendo thread-unsafe.

| Entrada        | Número de Linhas | Sequencial | ribas-ac-ingenuo | ribas-ac-ajustado | 1 Thread | 2 Threads | 4 Threads | 8 Threads | 12 Threads | 16 Threads |
| -------------- | ---------------- | ---------- | ---------------- | ----------------- | -------- | --------- | --------- | --------- | ---------- | ---------- |
| file001-sample | 21               | 0          | 0                | 0                 | 0        | 0         | 0         | 0.01      | 0.03       | 0.02       |
| file002-sample | 51               | 0.01       | 0.01             | 0.01              | 0.01     | 0.01      | 0.01      | 0.02      | 0.03       | 0.05       |
| file003        | 256              | 0.01       | 0                | 0                 | 0        | 0.01      | 0.01      | 0.01      | 0.02       | 0.02       |
| file004        | 1371             | 0.01       | 0                | 0                 | 0        | 0.01      | 0.02      | 0.01      | 0.03       | 0.02       |
| file005-sample | 10296            | 0.03       | 0                | 0                 | 0.02     | 0.02      | 0.02      | 0.02      | 0.08       | 0.04       |
| file006        | 100171           | 0.03       | 0.01             | 0.01              | 0.02     | 0.01      | 0.03      | 0.02      | 0.05       | 0.03       |
| file007-sample | 1000491          | 0.07       | 0.05             | 0.03              | 0.07     | 0.02      | 0.02      | 0.02      | 0.03       | 0.07       |
| file008        | 10000176         | 0.54       | 0.5              | 0                 | 0.12     | 0.1       | 0.04      | 0.05      | 0.04       | 0.05       |
| file009        | 26               | 0          | 0                | 0.01              | 0.01     | 0         | 0.01      | 0.01      | 0.03       | 0.02       |
| file010-big    | 50000001         | 2.76       | 2.41             | 0.36              | 0.58     | 0.33      | 0.22      | 0.21      | 0.18       | 0.21       |
| file-011-big   | 200000001        | 10.45      | 9.43             | 1.26              | 2.09     | 1.1       | 0.61      | 0.52      | 0.45       | 0.49       |

<div>
    <a href="https://plotly.com/~rogerioo/9/?share_key=t2Yr6LWc3jkEHQwnym3STO" target="_blank" title="Benchmarking_3" style="display: block; text-align: center;"><img src="https://plotly.com/~rogerioo/9.png?share_key=t2Yr6LWc3jkEHQwnym3STO" alt="Benchmarking_3" style="max-width: 100%;width: 600px;"  width="600" onerror="this.onerror=null;this.src='https://plotly.com/404.png';" /></a>
</div>
