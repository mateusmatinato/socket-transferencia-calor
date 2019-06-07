/**************************************
 * Trabalho de Sistemas Distribuídos   *
 * Transferência de Calor em Chapa     *
 * Professor:  Aleardo Manacero Jr.    *
 * Alunos:     Mateus Matinato         *
 *             Jonatan Rodrigues       *
 *             João Marcos Rosa        *
 * Data de Modificação: 06/06 19:39    *
 **************************************/

#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "funcoes.h"
#define MAX 80               // Tamanho do buffer que será usado para transmitir mensagens
#define PORT 8081           // Porta de acesso ao socket
#define SA struct sockaddr  // Struct de endereço do socket

int main() {
  // Declaração de Variáveis
  int sockfd, connfd, bytes_recv;
  int i, j, k, iInicial, iFinal, vizinhoCima, vizinhoBaixo, iteracaoLocal,
      iteracaoGlobal, id;
  struct sockaddr_in servaddr, cli;

  // Zera a matriz red e black
  float matrizRed[402][402], matrizBlack[402][402];
  for (i = 0; i < 402; i++) {
    for (j = 0; j < 402; j++) {
      matrizRed[i][j] = 0;
      matrizBlack[i][j] = 0;
    }
  }

  // Insere os valores constantes na matriz black
  matrizBlack[75][175] = 25.00;
  matrizBlack[75][75] = -10.00;
  matrizBlack[75][275] = 0.00;
  matrizBlack[190][75] = 20.00;
  matrizBlack[190][175] = -20.00;
  matrizBlack[190][275] = 10.00;
  matrizBlack[305][75] = 10.00;
  matrizBlack[305][175] = 30.00;
  matrizBlack[305][275] = 40.00;

  // Criação do socket e verificação
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Falha na criação do Socket...\n");
    exit(0);
  } else
    printf("Socket criado com sucesso...\n");
  bzero(&servaddr, sizeof(servaddr));

  // Declaração de IP e Porta do Servidor
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr =
      inet_addr("127.0.0.1");  // mudar para testar em várias maquinas
  servaddr.sin_port = htons(PORT);

  // Conecta o Cliente ao Servidor
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    printf("Conexão com o servidor falhou...\n");
    exit(0);
  } else {
    printf("Conexão com o servidor efetuada...\n");
  }

  char buffer[MAX];  // Buffer que será utilizado para comunicação

  // Envia IP e Porta pro servidor
  printf("Digite o seu IP e porta (XXX.XXX.XXX.XXX:XXXX): ");
  strcpy(buffer, "");
  scanf("%s", buffer);
  write(sockfd, buffer, sizeof(buffer));

  // Aguarda receber as instruções do servidor
  bzero(buffer, MAX);
  do {
    bytes_recv = recv(sockfd, buffer, 1024, 0);
  } while (bytes_recv <= 0);

  char iInicialString[3], iFinalString[3], vizinhoString[3],
      idString[3];  // variáveis auxiliares
  bzero(iInicialString, sizeof(iInicialString));
  bzero(iFinalString, sizeof(iFinalString));
  bzero(vizinhoString, sizeof(vizinhoString));
  bzero(idString, sizeof(idString));

  j = 0;

  /* Percorre a string recebida para armazenar as informações recebidas do
  servidor Informações Recebidas: iInicial, iFinal, vizinhoCima, vizinhoBaixo,
  id iInicial: linha inicial que o cliente deve calcular iFinal: linhaFinal que
  o cliente deve calcular vizinhoCima: id do cliente que deve receber a linha
  inicial do cliente atual vizinhoBaixo: id do cliente que deve receber a linha
  final do cliente atual
  */

  // Deve percorrer a string para pegar o iInicial e o iFinal do cliente
  buffer[bytes_recv] = '\0';
  while (buffer[j] != '=') {
    // percorre até achar o =
    j++;
  }
  j++;  // pula pro próximo, que é o iInicial até o ;
  i = 0;
  while (buffer[j] != ';') {
    iInicialString[i] = buffer[j];
    j++;
    i++;
  }
  iInicial = atoi(iInicialString);

  // Percorre para achar o iFinal
  while (buffer[j] != '=') {
    // percorre até achar o =
    j++;
  }
  j++;  // pula pro próximo, que é o iFinal até o ;
  i = 0;
  while (buffer[j] != ';') {
    iFinalString[i] = buffer[j];
    j++;
    i++;
  }
  iFinal = atoi(iFinalString);

  // Percorre para achar o vizinho
  while (buffer[j] != '=') {
    // percorre até achar o =
    j++;
  }
  // Percore até o ; para pegar qual o vizinho de cima
  j++;
  i = 0;
  while (buffer[j] != ';') {
    vizinhoString[i] = buffer[j];
    j++;
    i++;
  }
  vizinhoCima = atoi(vizinhoString);

  // percorre até achar o ;
  while (buffer[j] != '=') {
    j++;
  }
  j++;
  i = 0;
  strcpy(vizinhoString, "");
  while (buffer[j] != ';') {
    // percorre até 0 ; para pegar o vizinho de baixo
    vizinhoString[i] = buffer[j];
    j++;
    i++;
  }
  vizinhoBaixo = atoi(vizinhoString);
  // percore até achar o =
  j++;
  while (buffer[j] != '=') {
    j++;
  }
  j++;
  i = 0;
  while (buffer[j] != '\0') {
    idString[i] = buffer[j];
    j++;
    i++;
  }
  id = atoi(idString);

  // Mostra todos os dados recebidos
  printf(
      "Cliente recebeu mensagem do servidor\niInicial: %d\tiFinal: "
      "%d\tVizinho de Cima: %d\tVizinho de Baixo: %d\tID: %d\n\n",
      iInicial, iFinal, vizinhoCima, vizinhoBaixo, id);

  iteracaoLocal = 1;
  iteracaoGlobal = 1;
  while (1) {
    /*
    Deve receber do servidor qual iteração global o servidor está,
    iniciará a iteração dos cálculos somente se iteracaoGlobal == iteracaoLocal.
    Desse modo, todos os clientes estarão sempre na mesma iteração pois o
    servidor só incrementa a iteração global quando todos terminarem suas
    iterações locais.
    */

    // Recebe iteração global e percorre a string para armazenar na variável
    strcpy(buffer, "");
    do {
      bytes_recv = recv(sockfd, buffer, 1024, 0);
    } while (strcmp(buffer, "") == 0 || strcmp(buffer, " ") == 0);
    buffer[bytes_recv] = '\0';

    j = 0;
    while (buffer[j] != '=')
      j++;
    j++;
    char iteracaoString[3];
    bzero(iteracaoString, sizeof(iteracaoString));
    i = 0;
    while (buffer[j] != '\0') {
      iteracaoString[i] = buffer[j];
      j++;
      i++;
    }
    iteracaoGlobal = atoi(iteracaoString);

    if (iteracaoLocal == iteracaoGlobal) {
      /*
      Caso a iteração local for igual a iteração global, recebe do servidor as
      linhas de interseccção. Desse modo é garantido que os valores das linhas
      de intersecção estarão sempre atualizados.
      */
      printf("Nó %d iniciando iteração %d\n", id, iteracaoLocal);

      // Deve receber a linha iInicial - 1 do servidor
      float linhaInicialRecebida[402], linhaFinalRecebida[402];
      for (i = 0; i < 402; i++) {
        linhaInicialRecebida[i] = 0;
        linhaFinalRecebida[i] = 0;
      }

      printf("Recebendo linhas de intersecção...\n");
      bytes_recv =
          recv(sockfd, linhaInicialRecebida, sizeof(linhaInicialRecebida), 0);

      // O último cliente não possui vizinho de baixo então ele não espera
      if (vizinhoBaixo != 0) {
        // só vai pegar a linha final se não for o último nó
        bytes_recv =
            recv(sockfd, linhaFinalRecebida, sizeof(linhaFinalRecebida), 0);
      }
      for (i = 0; i < 402; i++) {
        // Atribui na matriz black os valores recebidos
        matrizBlack[iInicial - 1][i] = linhaInicialRecebida[i];
        if (vizinhoBaixo != 0) {
          // quando não for o último nó, recebe a linha final e coloca na matriz
          matrizBlack[iFinal + 1][i] = linhaFinalRecebida[i];
        }
      }

      /*
      Lógica da iteração: Os valores do ponto da matriz red serão calculados por
      meio da soma da vizinhança-4 do ponto na matriz black, mais o valor do
      ponto na matriz black. Esse valor é dividido por 5 e obtêm-se o valor do
      ponto. É calculado o erro (diferença absoluta entre a red e a black no
      ponto atual). O maior erro encontrado na iteração deve ser armazenado.
      Após isso os valores da red são transferidos para a matriz black para a
      próxima iteração.
      Ps: É importante setar as constantes novamente depois das contas.
      */

      float erroAtual, erroAbsoluto;
      float matrizAnterior;
      erroAbsoluto = 0;
      printf("Iniciando os cálculos da iteração\n");
      // O cliente só calcula as linhas que pertencem a ele
      for (i = iInicial; i <= iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          // Cálculo do ponto atual:
          matrizRed[i][j] = (matrizBlack[i][j] + matrizBlack[i - 1][j] +
                             matrizBlack[i][j - 1] + matrizBlack[i + 1][j] +
                             matrizBlack[i][j + 1]) /
                            5;
          // Setar constantes novamente
          matrizRed[75][175] = 25.00;
          matrizRed[75][75] = -10.00;
          matrizRed[75][275] = 0.00;
          matrizRed[190][75] = 20.00;
          matrizRed[190][175] = -20.00;
          matrizRed[190][275] = 10.00;
          matrizRed[305][75] = 10.00;
          matrizRed[305][175] = 30.00;
          matrizRed[305][275] = 40.00;

          // Calcula erro
          erroAtual = fabs(matrizRed[i][j] - matrizBlack[i][j]);

          // Armazena o maior erro da iteração
          if (erroAtual > erroAbsoluto)
            erroAbsoluto = erroAtual;
        }
      }

      // Após calcular a red, transfere os valores para a black
      for (i = iInicial; i <= iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          matrizBlack[i][j] = matrizRed[i][j];
        }
      }

      printf("Iteração %d -> Erro: %.3f\n\n", iteracaoLocal, erroAbsoluto);

      // Envia as linhas de intersecção
      send(sockfd, matrizBlack[iFinal], sizeof(matrizBlack[iFinal]), 0);
      send(sockfd, matrizBlack[iInicial], sizeof(matrizBlack[iInicial]), 0);

      // Envia o MAIOR erro encontrado na iteração para o servidor
      strcpy(buffer, "");
      snprintf(buffer, sizeof(buffer), "%.3f", erroAbsoluto);
      send(sockfd, buffer, sizeof(buffer), 0);

      // Iteração terminou, deve incrementar a iteração local e aguardar
      printf("--- FIM DA ITERAÇÃO %d -> Erro: %.3f ---\n\n", iteracaoLocal,
             erroAbsoluto);

      iteracaoLocal++;
    } else if (iteracaoGlobal == 0) {
      /*
      Quando iteração global for igual a zero, significa que terminou a
      execução, então deve salvar em um arquivo a matriz para facilitar a
      visualização
      */
      escreveMatrizArquivo(matrizBlack, iInicial, iFinal, id);
      break;  // Sai do while
    }
  }
  printf("\nFIM DO CLIENTE %d\n", id);
}
