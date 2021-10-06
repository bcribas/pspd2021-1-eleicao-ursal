# Eleição Ursal - OpenMP

## Equipe
**Nome:** Flavio Vieira Leao **Matrícula:** 150125682  

**Nome:** João Pedro Soares Cirqueira **Matrícula:** 150132344 

**Nome:** Moacir Mascarenha Soares Junior **Matrícula:** 170080366

## Solução
  O objetivo deste trabalho é implementar o exercício Eleição [U.R.S.A.L](https://moj.naquadah.com.br/contests/bcr-PSPD-2021_1/eleicao-ursal) utilizando OpenMP para paralelizar o algoritimo, vizando diminuir o tempo de execução do projeto. A implementação sequencial, para solução do exercicio, foi utilizada como base para a elaboração da solução paralela com OpenMP. Após isso todas as tentativas da diminuição do tempo de execução estão disponíveis na pasta src.
  
  Inicialmente o arquivo é dividido em blocos de leitura, onde cada thread trabalha com a leitura dos dados de um determinado bloco. A partir desta etapa, várias estratégias foram utilizadas com o intuito de melhorar o tempo de leitura e contagem dos votos, sendo elas: 
  a utilização e vetores globais, utilização de vetores privados onde cada thread trabalha com o seu proprio vetor e posteriomente repassa aos vetores globais, e a utilização de uma matriz global onde cada thread trabalha em uma linha, evitando assim a concorrência entre as threads.

### Regiões críticas de paralelização
  O código possui sua area critica no momento da leitura dos dados de entrada, foi notório ao longo do desenvolvimento que a maior parte do tempo de execução é gasta na leitura. O grupo buscou utilizar a paralelização neste bloco de leitura tendo em vista a grande diferença do tempo de execução neste bloco. Utilizando uma thread, o tempo de execução é poucos segundos maior que a versão sequencial. Com o aumento das threads a leitura passa a ter melhor desempenho, diminuindo o tempo em mais da metade, comparado ao sequencial.

  
### Tabelas e Gráficos

#### Melhor Solução
   A tabela e gráfico abaixo exibem a comparação entre a melhor solução desenvolvida pelo grupo(eleicao_ursal_final.c) com a solução sequencial, desenvolvida pelo Flávio, e com as soluções disponibilizadas pelo professor(ribas-ac-ingenuo e ribas-ac-ioajustado).
##### Tebela com os tempos medidos.
![image](https://user-images.githubusercontent.com/48539765/136131611-9b8714a7-195d-4c91-9652-7a3b5e2f78e6.png)

![image](https://user-images.githubusercontent.com/48539765/136130757-2f829cb3-bf25-4d4e-bf71-e53d1105d05e.png)


#### Comparação de soluções da equipe

  A tabela e o grafico abaixo, exibem dados de algumas soluções desenvolvidas pela equipe, utilizando 16 threads.
  ##### Tebela com os tempos medidos.
  ![image](https://user-images.githubusercontent.com/48539765/136134168-a7488bdf-e7d4-40c3-b0cf-959213b4a12c.png)

  
  ![image](https://user-images.githubusercontent.com/48539765/136133911-0a33ff6f-b62d-4f6c-88b1-963060c7b9e4.png)




