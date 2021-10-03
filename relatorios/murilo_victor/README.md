# Relatório - Trabalho 01
## Programação para Sistemas Paralelos e Distribuídos

| Estudante | Matrícula |
| :-: | :-: |
| Murilo Loiola Dantas | 17/0163571 |
| Victor Amaral Cerqueira | 17/0164411 |

## 1. Introdução
O objetivo do trabalho é o estudo e análise das regiões críticas do código para a implementação de paralelização utilizando a ferramenta OpenMP. Para a realização do estudo foi dado um problema simples de lógica de programação, o sistema CHOCOCINO (para os testes com mais threads) e a ferramenta OpenMP para a construção das threads.

## 2. Regiões críticas de paralelização
Com o código base sequencial produzido, foi estudado no código onde poderíamos adicionar as threads para otimização do código. As regiões mais relevantes para paralelização foram:
- Ordenação (merge sort);
- Cálculo final da matriz;
- Leitura dos dados do arquivo.

### 2.1. Ordenação
Como foi discutido e implementado em sala, o ganho de tempo de processamento quando feita a paralelização da ordenação não é tão relevante. Ainda assim, realizamos os testes de tempo e verificamos que realmente não era necessário.

Tempo gasto para execução: **Colocar tempo gasto no chococino**

### 2.2. Cálculo final da matriz
A matriz utilizada para evitar concorrência de acesso na leitura do arquivo precisa ter seus valores consolidados após o término da leitura. Como a matriz possui 16 linhas e 100.000 colunas, foi utilizado um laço dentro de laço com 1.600.000 ciclos. Apesar disso, o corpo de execução do laço é composto por somente uma atribuição de valor e uma operação de aritmética simples. Verificando o tempo gasto para a execução dessa parte, notou-se que o ganho de paralelização não seria tão relevante.

Tempo gasto para execução: **Colocar tempo gasto no chococino**

### 2.3. Leitura dos dados do arquivo
A leitura dos dados do arquivo é o trecho de código que mais demanda tempo de processamento, principalmente para grandes entradas. Tendo isso em envidência, ela se tornou a principal região do código que necessitava da implementação das threads. 

## 3. Experimentos e tentativas
### 3.1. Sequencial
Como o problema de lógica para o estudo foi simples, a solução sequencial não teve nenhum problema em ser realizada.

### 3.2. Threads
Com a necessidade de paralelização do código e por nunca termos utilizado a ferramenta que era requisitada no estudo, acabamos tendo alguns problemas na hora de paralelizar o código base.

A primeira solução que aplicamos foi criar um vetor de struct para cada tipo de candidato (presidente, senador e deputados), cada um contendo o numero e quantidade de votos dos candidatos. A paralelização ocorria na leitura das entradas do arquivo. Contudo, com essa solução, as threads acabavam entrando em conflitos de acesso, quando duas ou mais threads tentavam acessar o mesmo índice de um vetor simultaneamente.

A segunda solução foi criar uma matriz para os candidatos, onde cada thread operaria em uma única linha da matriz e os candidatos seriam identificados pelas colunas. Com a separação do processo entre threads e elas não podendo acessar um mesmo vetor, acabamos com os problemas de conflito e ganhamos tempo de processamento. A única desvantagem é o cálculo extra necessário após a leitura do arquivo, na consolidação dos resultados. O tempo gasto nessa etapa, no entanto, é rapidamente recompensado com o tempo ganho da paralelização em entradas relativamente maiores.

Para evitar outros conflitos de acesso, a variável 'voto' foi tornada privada de cada thread e foi aplicado um reduction nas variáveis de soma de votos.

## 4. Teste de desempenho
### 4.1. Gráfico
### 4.2. Observações