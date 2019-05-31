# Socket - Transferência de Calor
Trabalho da disciplina do 7º semestre do curso de Bacharelado em Ciência da Computação da UNESP - São José do Rio Preto.

**Data de entrega**:  07/06, até 23h59<br/>
**Apresentação**: Marcar na semana seguinte a entrega

### Enunciado do trabalho:
* Escrever um programa para o cálculo de transferência de calor em uma chapa metálica, usando uma estrutura do tipo peer-computing com sockets em C.  
* O programa deve ser disparado em uma máquina (o servidor) e, em seguida, ser disparado em outras máquinas, que informarão o servidor de seus endereços. 
* O servidor estabelecerá então um ranking entre os "clientes", colocando-se como primeiro da lista e devolvendo aos demais os endereços de seus vizinhos.  
* A partir disso os "clientes" e servidor devem fazer a computação das temperaturas até a diferença entre os valores de uma iteração e outra ficar abaixo de 0.01  
* O programa deve ser executado com 1, 3 e 7 clientes, medindo o tempo gasto em cada execução (repetir a medida pelo menos 10 vezes).
* Deve ser considerada uma chapa de 400x400 pontos, com bordas a 0 C e pontos de calor dados na tabela abaixo. 

<center>

| Linha 	| Coluna 	| Temperatura 	|
|------:	|--------	|-------------	|
| 75    	| 75     	| -10         	|
| 75    	| 175    	| 25          	|
| 75    	| 275    	| 0           	|
| 190   	| 75     	| 20          	|
| 190   	| 175    	| -20         	|
| 190   	| 275    	| 10          	|
| 305   	| 75     	| 10          	|
| 305   	| 175    	| 30          	|
| 305   	| 275    	| 40          	|
</center>

### Todo List
* Não está calculando o tempo direitinho (eu acho).

* As vezes simplesmente não inicia a iteração em algum dos clientes. (?)

### Anotaçoes
* TA QUASE VEI
