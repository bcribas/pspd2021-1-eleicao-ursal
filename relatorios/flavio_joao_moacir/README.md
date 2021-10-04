# Eleição Ursal - OpenMP

## Equipe
**Nome:** Flavio Vieira Leao **Matrícula:** 150125682  

**Nome:** João Pedro Soares Cirqueira **Matrícula:** 150132344 

**Nome:** Moacir Mascarenha Soares Junior **Matrícula:** 170080366

## Solução
  O objetivo deste trabalho é implementar o exercício Eleição [U.R.S.A.L](https://moj.naquadah.com.br/contests/bcr-PSPD-2021_1/eleicao-ursal) utilizando OpenMP para paralelizar o algoritimo, vizando diminuir o tempo de execução do projeto. A implmentação sequencial, para solução do exercicio, foi utilizada como base para a elaboração da solução paralela com OpenMP. Após isso todas as tentativas da dimuição do tempo de execução estão disponiveis na pasta src.
  
  Inicialmente o arquivo é dividido em blocos de leitura, onde cada thread trabalha com a leitura dos dados de um determinado bloco. A partir desta etapa, várias estratégias foram utilizadas com o intuito de melhorar o tempo de leitura e contagem dos votos, sendo elas: 
  a utilização e vetores globais, utilização de vetores privados onde cada thread trabalha com o seu proprio vetor e posteriomente repassa aos veotores globais, e a utilização de uma matriz global onde cada thread trabalha em uma linha, evitando assim a concorrencia entre as threads.

### Regiões críticas de paralelização
  O codigo possui sua area critica no momento da leitura dos dados de entrada, foi notório ao longo do desenvolvimento que a maior parte do tempo de execução é gasta na leitura. O grupo buscou utilizar a paralelização neste bloco de leitura tendo em vista a grande diferença do tempo de execução neste bloco. Utilizando uma thread, o tempo de execução é poucos segundos maior que a versão sequencial. Com o aumento das threads a leitura passa a ter melhor desempenho, dimunuindo o tempo em mais da metade, comparado ao sequencial.
  
### Gráficos

#### Solução 1


#### Solução 2

#### Solução 3(principal)

#### Conclusão


