# Equipe Sara-Erick-Ailton-Maicon

Integrantes:

- Ailton Aires (180011600)
- Erick Giffoni (170141161)
- Maicon Mares (180023411)
- Sara Silva (160144752)


# O trabalho: Eleição U.R.S.A.L

Este trabalho tem como objetivo a aplicação da programação paralela para resolver<br>
o problema "Eleição U.R.S.A.L", utilizando a API "OpenMP".<br>

O contexto do trabalho envolve os estudos sobre programação em paralelo desenvol-<br>
vidos na disciplina "Programação para Sistemas Paralelos e Distribuídos", no pe-<br>
ríodo 2021/1 da Universidade de Brasília, Faculdade do Gama.<br>


# Desenvolvimento

## Primeiras versões

No começo desse trabalho, nossa equipe desenvolveu uma solução sequencial e outra<br>
paralela para o problema proposto. Isso foi antes da criação do repositório oficial<br>
para o trabalho pelo professor Bruno Ribas.<br>

Nesse sentido, utilizamos alguns scripts em `bash`, de nossa autoria, para executar<br>
nossas soluções e gerar resultados, conforme a organização abaixo. Nesse momento,<br>
os arquivos de entrada utilizados foram `file-001` e `file-002`, disponibilizados<br>
na página web oficial do professor Ribas [1].

- [`teste-sequencial.sh`](./teste-sequencial.sh): script para executar testes com a solução sequencial;<br>
- [`teste-paralelo.sh`](./teste-paralelo.sh): script para executar testes com a solução paralela;<br>
- [`benchmark-solucoes.txt`](./benchmark-solucoes.txt): resultados obtidos a partir da utilização dos nos-<br>
sos scripts.<br>

**Sobre o desempenho dessas primeiras versões:**

Especificações do computador que executou os testes nesse momento:<br>

- MacOS 2.6 GHz Quad-Core Intel Core i7, 16 GB 2133 MHz LPDDR3

Sendo assim, a figura 1 abaixo compara os tempos de execução das nossas soluções<br>
sequencial e paralela. Os valores foram obtidos por meio da execução dos scripts<br>
de teste explicados acima.<br>

Figura 1:<br>
<img src="./imagens/g1-v1-file1.png">
<br>Fonte: autores
</img><br>

Conforme evidenciado no gráfico acima, os tempos de execução ficaram em torno de<br>
6 segundos, considerando o arquivo `file-001` como entrada. Ainda, nesse caso<br>
percebe-se que aumentar a quantidade de threads não gerou mudança significativa.<br>

De modo semelhante, a figura 2 abaixo mostra os tempos de execução das nossas so-<br>
luções sequencial e paralela, porém considerando o `file-002` como entrada.<br>

Figura 2:<br>
<img src="./imagens/g2-v1-file2.png">
<br>Fonte: autores
</img><br>

No caso do gráfico acima, deve-se levar em conta que `file-002` é bem maior que<br>
`file-001`. Por causa disso os tempos de execução aumentaram em relação ao primeiro<br>
gráfico. Mais ainda, percebe-se que a solução mais rápida é aquela que utilizou 4<br>
threads, enquanto a de 8 threads teve desempenho pior que a sequencial.<br>

**Sobre os códigos dessas primeiras versões:**

Ambas versões sequencial e paralela utilizavam vetores de tamanhos pré-definidos<br>
para computar os votos direcionados a cada político. Mais ainda, esses códigos<br>
utilizavam uma estratégia parecida com o algoritmo do *selection sort* para calcu-<br>
lar, para cada político, quem havia sido eleito. Vale lembrar que, dado o momento<br>
inicial do trabalho, ambas versões recebiam as entradas via `stdin` e, no caso<br>
da paralela, ainda não realizávamos a leitura da entrada de forma paralela.<br>

A diferença entre a solução sequencial e a paralela, então, é que a segunda<br>
utilizava a diretiva ***omp parallel sections*** para dividir o trabalho de calcu-<br>
lar os resultados da eleição de cada tipo de político entre as threads. Conforme<br>
evidenciado nos gráficos acima, essa estratégia teve desempenho baixo, principal-<br>
mente com o arquivo `file-002` como entrada.<br>

## Evoluindo a solução paralela

Nesse momento do trabalho, já estávamos com o repositório oficial para o trabalho<br>
em mãos. Fizemos um *fork* dele e passamos a trabalhar com a criação de diferentes<br>
*branches* para cada objetivo que queríamos alcançar.<br>

**Sobre o código paralelo:**

A turma logo recebeu algumas orientações do professor Ribas com o intuito de que<br>
todos entendessem quais seriam os próximos desafios do trabalho. Dentre eles, co-<br>
meçamos a trabalhar para que a leitura dos votos de entrada do programa fosse feita<br>
de forma paralela.<br>

Para isso, ao invés do codigo ler a partir da entrada padrão (*stdin*), ele foi<br>
evoluído para ler um arquivo que tivesse os valores de entrada. Tal arquivo po-<br>
de conter várias linhas, sendo que cada uma pode ter um número diferente de bytes.<br>
Nossa equipe decidiu usar uma estratégia que contasse quantos bytes o arquivo tinha,<br>
no total, dividisse essa quantidade de modo igual para cada *thread* e fizesse o<br>
controle desses processos paralelos para que cada um lesse a quantidade de bytes<br>
dividida anteriormente, sem que deixar que uma *thread* "invadisse" o espaço da<br>
outra ou que algum valor de entrada fosse perdido ou repetido.<br>

Para que a estratégia da leitura paralela fosse realizada com sucesso, tivemos que<br>
utilizar algumas funções que nos ajudassem na manipulação de arquivos. Algumas delas<br>
são: `fseek`, `fscanf`, `ftell`, `fopen` e `fclose`.

Mantivemos a utilização daqueles vetores estáticos para computar cada voto, sendo<br>
que existe a possibilidade de uma ou mais *threads* acessarem uma mesma posição<br>
"ao mesmo tempo". Considerando que, antes da leitura ser iniciada, os vetores são<br>
preenchidos de zeros, e que a computação do voto apenas incrementa em 1 determi-<br>
nada posição do vetor em questão, não há problema se *threads* acessarem aquele<br>
local "ao mesmo tempo".<br>

**Observando o desempenho nesse ponto do trabalho:**


Especificações do computador que executou os testes nesse momento:<br>

- Ubuntu 20.04 LTS, 11th Gen Intel® Core™ i5-1135G7 @ 2.40GHz × 8

Realizamos os testes para essa versão paralela evoluída e utilizamos uma planilha [2]<br>
para avaliar os resultados e montar gráficos. Além disso, os resultados para essa<br>
ocasião também estão disponíveis em [`resultados.txt`](./resultados.txt).<br>

A seguir mostramos alguns gráficos comparando a quantidade de *threads* e os tem-<br>
pos de execução. Para que o relatório não ficasse muito extenso, decidimos colocar<br>
aqui apenas 3 dos 11 possíveis gráficos.<br>

Sendo assim, a figura 3 a seguir é relativa ao arquivo de entrada `file010-big`.<br>

Figura 3:<br>
<img src="./imagens/evoluindo-paralela-file010-big.png">
<br>Fonte: autores
</img><br>

É possível notar que, nesse caso, aumentar o número de *threads* diminuiu bastante<br>
o tempo de execução. Mesmo assim, utilizar 12 *threads* foi melhor que usar 16.<br>

De modo semelhante, a figura 4 abaixo é relativa ao arquivo de entrada `file-011-big`.<br>

Figura 4:<br>
<img src="./imagens/evoluindo-paralela-file011-big.png">
<br>Fonte: autores
</img><br>

Sobre esse gráfico, também percebe-se a diminuição do tempo de execução conforme<br>
o número de *threds* aumenta. Semelhantemente ao gráfico 3, utilizar 12 *threads*<br>
foi melhor que usar 16.<br>

Já a figura 5 a seguir é relativa ao arquivo de entrada `file009`.<br>

Figura 5:<br>
<img src="./imagens/evoluindo-paralela-file009.png">
<br>Fonte: autores
</img><br>

Ocorreu um fato curioso, dado que esse arquivo de entrada é menor que os utiliza-<br>
dos para gerar as figuras 3 e 4. Percebe-se que aumentar o número de *threds* piora<br>
bastante o desempenho da solução.<br>

## Histograma

O código foi evoluído usando a lógica de histograma, que consiste em uma distribuição de frequências. Foi usado um vetor *votos* e a cada voto *v* válido lido, o valor do vetor no índice *v* é incrementado, registrando assim a frenquência desse voto. Então deixaram de ser usados os vetores diferentes para cada tipo de candidato.
Nessa versão também foi utilizada a diretiva do for do OpenMp para paralelizar o laço principal de leitura dos votos.

**Observando o desempenho nesse ponto do trabalho:**

Especificações do computador que executou os testes nesse momento:<br>

- Ubuntu 20.04 LTS, 11th Gen Intel® Core™ i5-1135G7 @ 2.40GHz × 8

A tabela a seguir apresenta os resultados para melhor solução implementada considerando todos os arquivos de entrada.<br>
Tabela 1:<br>
<img src="./imagens/tabela.jpg">
<br>Fonte: autores
</img><br>

Praticamente todos os tempos de execução ficaram próximos a 0 (zero) segundo,<br>
o que é muito bom. As 2 únicas exceções são para os arquivos `file010-big`, cujo<br>
melhor tempo foi **2.98 segundos** com 8 *threads*, e `file-011-big`, cujo melhor tem-<br>
po foi 12.67 segundos com 8 *threads*.<br>

<hr>

## Referências

[1] RIBAS, Bruno. Aula 18 de Agosto. Disponível em: https://www.brunoribas.com.br/pspd/2021-1/#orga81c164

[2] Autores. Planilha de resultados diversos. Disponível em: https://docs.google.com/spreadsheets/d/1B6N221LsnoZr-mcAZc0cFOv39i3_os2w_r7ys_ToiIs/edit?usp=sharing
