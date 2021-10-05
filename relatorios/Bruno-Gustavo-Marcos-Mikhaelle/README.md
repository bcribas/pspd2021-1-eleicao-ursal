# Relatório

## Equipe

- Bruno Carmo Nunes - 18/0117548
- Gustavo Duarte Moreira - 20/0038141
- Marcos Vinícius Rodrigues da Conceição - 17/0150747	
- Mikhaelle de Carvalho Bueno - 15/0018673

## Partes paralelizadas e críticas

Na versão paralela foi utilizado a estratégia de paralelismo usando threads para realizar a leitura dos dados e computá-los.
O arquivo é carregado e cada thread fica responsável por computar uma parte dos dados do arquivo. Para dividir as partes entre as threads, utilizamos a abordagem de contar quantos bytes existem no arquivo, excluindo os bytes da primeira linha que corresponde ao número de senadores, deputados federais e deputados estaduais. Esse número de bytes é então dividido pelo número de threads, resultando na quantidade de bytes que aproximadamente cada thread será responsável por computar.

Para evitar a parada ou o começo no "meio" de uma linha, a thread lê até o "\n" e a próxima continua a execução até encontrar o "\n". Isso também evita a duplicação de dados.

A quantidade de votos por candidato por thread é armazenado em uma matrix [ i ] x[ j ] em que __i__ é o 
numero de linhas que corresponde a numeração das threads e o __j__ é o número de colunas que corresponde a 
numeração do candidato. O valor armazenado na posição __[ i ][ j ]__ corresponde ao número de votos válidos computados pela __thread i__ para o __candidato j__. Antes de armazenar o voto, é checado se ele é válido (votosValidos) ou inválido (votosInvalidos) e se é um voto para presidente (votosPresidente).


Depois esse resultado é armazenado em um vetor específico ao tipo do candidato, em que a posição corresponde ao numero do candidato. Esse vetor é então utilizado para todos os calculos subsequêntes.


Todos os calculos são feitos em paralelo o que demanda a utilização do __omp parallel reduction__.

A clausulá __reduction__ utiliza de redução nas variáveis que são passadas como parâmetros. Uma cópia privada das variáveis ​​são criadas para cada thread. O processo de redução é aplicada a todas as cópias privadas da variável e o resultado final é escrito na variável global.


A parte paralelizado do código é encontrado entre as linhas 81 e 166.


### Versão sequencial da equipe

    A versão sequencial da solução do problema utiliza vetores alocados dinamicamente, gerando um resultado não satisfatório na resolução do problema. Essa versão lê as entradas do arquivo por meio do *scanf* em um laço *while* que termina quando recebe a entrada *EOF*. 


### Comparação de desempenho entre as implementações


A comparação entre as implementações foram realizadas em todos os arquivos do BENCHMARK e neste relatório vamos evidenciar os dois maiores. Os testes foram obtidos em uma máquina com UBUNTU 21.04, CPU Ryzen 7 2700X, 32GB de mémoria RAM 3200MHz, SSD 512GB SATA 3, GPU GeFORCE RTX2600 SUPER.

| file010-big | PROF-INGENUO | PROF-IAJUSTADO | CDMOJ | 1 THREAD | 2 THREADS | 4 THREADS | 8 THREADS | 16 THREADS |
| ----------- | ------------ | -------------- | ----- | -------- | --------- | --------- | --------- | ---------- |
| REAL        | 2,71         | 0,43           | 3,14  | 4,39     | 2,43      | 1,33      | 0,98      | 0,75       |
| USER        | 2,68         | 0,39           | 3,07  | 4,37     | 4,55      | 4,63      | 5,13      | 6,82       |
| SYS         | 0,02         | 0,04           | 0,06  | 0,02     | 0,03      | 0,05      | 0,04      | 0,05       |
*tempos em segundos

| file-011-big | PROF-INGENUO | PROF-IOAJUSTADO | CDMOJ | 1 THREADS | 2 THREADS | 4 THREADS | 8 THREADS | 16 THREADS |
| ------------ | ------------ | --------------- | ----- | --------- | --------- | --------- | --------- | ---------- |
| REAL         | 10,79        | 1,64            | 12,01 | 16,9      | 9,07      | 4,64      | 2,73      | 2,05       |
| USER         | 10,65        | 1,56            | 11,82 | 16,73     | 17,7      | 17,66     | 19,06     | 26,59      |
| SYS          | 0,14         | 0,07            | 0,19  | 0,15      | 0,18      | 0,17      | 0,17      | 0,21       |
*tempos em segundos

![image info](./img/grafico1.png)

![image info](./img/grafico2.png)

![image info](./img/grafico3.png)


### Conclusão

Vimos que foi possível reduzir o tempo do processo quando utilizamos mais threads, no local do bloco onde ele abre o arquivo e busca as informações do mesmo (pragma omp). No entando, não conseguimos ultrapassar o tempo definido pelo professor no PROF-IOAJUSTADO, porem chegamos bem próximo com uma diferença de 0,41 segundos. 