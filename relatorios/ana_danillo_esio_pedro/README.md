# Relatório

## Membros da equipe

|Nome|Matrícula|
|----|---------|
|Ana Carolina Carvalho|190063441|
|Danillo Gonçalves|170139981|
|Ésio Freitas|170033066|
|Pedro Féo|170020461|

## Introdução

Para a realização desse experimento, identificamos as duas regiões principais do código, a área de leitura do input e o processamento dos dados para print na tela.
Para realizar uma exploração mais vasta em relação ao paralelismo desse projeto criamos diferentes versões da solução, com focos em regiões diferentes, além do melhoramento de regiões.

Cada uma das versões será melhor elaborada a seguir.

## Sequencial

Esse foi o ponto de partida do nosso relatório, um código sequencial simples. 
Essa solução apresenta os seguintes resultados:

![Sequencial](imagens/sequencial.png)

## Leitura 1

Nessa versão da solução, focamos em paralelizar a leitura do arquivo de input.
Nessa versão, descobrimos o tamanho do arquivo completo e o dividimos em pedaços, assim cada uma das threads do programa fica responsável por ler uma parte do arquivo e incrementar a quantidade de votoso de cada candidato que leu. Os votos dos candidatos estão armazenados em um array para cada eleição.

### Regiões Críticas

### Resultados

#### 1 Thread

#### 2 Thread

#### 4 Thread

#### 8 Thread

#### 12 Thread

#### 16 Thread

## Processamento 1

Nessa versão da solução focamos apenas na região de processamento.

Primeira versão

### Regiões Críticas

### Resultados

#### 1 Thread

#### 2 Thread

#### 4 Thread

#### 8 Thread

#### 12 Thread

#### 16 Thread

## Processamento 2

Nessa versão da solução focamos apenas na região de processamento.

Segunda versão

### Regiões Críticas

### Resultados

#### 1 Thread

#### 2 Thread

#### 4 Thread

#### 8 Thread

#### 12 Thread

#### 16 Thread

## Solução do Professor

mostrar resultados do professor

## Conclusão