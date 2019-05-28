#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "funcoes.h"
#define MAX 80
#define PORT 4040
#define SA struct sockaddr

int main() {
  int sockfd, connfd, bytes_recv;
  int i, j, k, iInicial, iFinal, vizinhoCima, vizinhoBaixo, iteracaoLocal, iteracaoGlobal, id;
  struct sockaddr_in servaddr, cli;

  // Zera a matriz
  float matrizRed[402][402], matrizBlack[402][402];
  for (i = 0; i < 402; i++) {
    for (j = 0; j < 402; j++) {
      matrizRed[i][j] = 0;
      matrizBlack[i][j] = 0;
    }
  }

  // Insere os valores na matriz
  matrizBlack[75][175] = 25.00;
  matrizBlack[75][75] = -10.00;
  matrizBlack[75][275] = 0.00;
  matrizBlack[190][75] = 20.00;
  matrizBlack[190][175] = -20.00;
  matrizBlack[190][275] = 10.00;
  matrizBlack[305][75] = 10.00;
  matrizBlack[305][175] = 30.00;
  matrizBlack[305][275] = 40.00;

  // socket create and varification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");
  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  servaddr.sin_port = htons(PORT);

  // connect the client socket to server socket
  if (connect(sockfd, (SA *)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else {

    printf("connected to the server..\n");
  }

  // Enfia IP e Porta pro servidor...
  char buffer[MAX];

  printf("Digite o seu IP e porta (XXX.XXX.XXX.XXX:XXXX: ");
  bzero(buffer, sizeof(buffer));
  scanf("%s", &buffer);
  write(sockfd, buffer, sizeof(buffer));

  /*
  Inicializa a iteração do cliente em 1, quando receber a iteração global do
  servidor o cliente deve comparar se a iteração global é igual a iteração
  local, somente se as ite- rações forem iguais o cliente pode prosseguir, caso
  contrário ele deve esperar. Quando o cliente receber a iteração global do
  servidor e prosseguir, deve enviar para o vizinho a linha matriz[iFinal][]
  para que ele possa fazer os cálculos sem conflito. Após enviar a ultima linha,
  deve prosseguir com os cálculos e buscar o maior erro possível. O cliente deve
  informar o maior erro para o servidor ao fim da iteração.
  */

  bzero(buffer, MAX);
  bytes_recv = recv(sockfd, buffer, 1024, 0);
  char iInicialString[3], iFinalString[3], vizinhoString[1], idString[2];
  j = 0;

  // Deve percorrer a string para pegar o iInicial e o iFinal do cliente
  buffer[bytes_recv] = '\0';
  while (buffer[j] != '=') {
    // percorre até achar o =
    j++;
  }
  j++; // pula pro próximo, que é o iInicial até o ;
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
  j++; // pula pro próximo, que é o iFinal até o ;
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

  //percorre até achar o ;
  while(buffer[j] != '='){
    j++;
  }
  j++;
  i = 0;
  bzero(vizinhoString,sizeof(vizinhoString));
  while(buffer[j] != ';'){
    //percorre até 0 ; para pegar o vizinho de baixo
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
  }
  id = atoi(idString);

  printf("Cliente recebeu mensagem do servidor\niInicial: %d\tiFinal: "
         "%d\tVizinho de Cima: %d\tVizinho de Baixo: %d\tID: %d\n",
         iInicial, iFinal, vizinhoCima, vizinhoBaixo,id);

  iteracaoLocal = 1;
  while (1) {

    // Deve receber qual a iteração global do servidor e comparar com a iteração
    // global
    bzero(buffer, MAX);
    bytes_recv = recv(sockfd, buffer, 1024, 0);
    buffer[bytes_recv] = '\0';
    j = 0;
    i = 0;
    while (buffer[j] != '=')
      j++;
    j++;
    char iteracaoString[3];
    while (buffer[j] != '\0') {
      iteracaoString[i] = buffer[j];
      j++;
      i++;
    }
    iteracaoGlobal = atoi(iteracaoString);

    float linhaRecebida[402];
    bzero(linhaRecebida, sizeof(linhaRecebida));
    if (iteracaoLocal == iteracaoGlobal) {
      // está na iteração certa, aguarda a linha do vizinho
      printf("Aguardando linha do vizinho...%d\n", iteracaoLocal);
        bytes_recv = recv(vizinhoCima, linhaRecebida, 402, 0); //NÃO TA RECEBENDO A LINHA DO VIZINHO DE CIMA

      for (i = 0; i < 402; i++) {
        matrizBlack[iInicial - 1][i] = linhaRecebida[i];
      }

      // Envia a linha pro vizinho de baixo
      if (vizinhoBaixo != 0) {
        // o ultimo cliente não precisa mandar
        matrizBlack[iFinal][10] = 1042;
        send(vizinhoBaixo, matrizBlack[iFinal], sizeof(matrizBlack[iFinal]), 0);
      }

      // faz os cálculos da matriz

      float erroAtual, erroAbsoluto;
      float matrizAnterior;
      erroAbsoluto = 0;

      for (i = iInicial; i < iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          // percorre somente o intervalo de linhas que pertence
          matrizRed[i][j] = (matrizBlack[i][j] + matrizBlack[i - 1][j] +
                             matrizBlack[i][j - 1] + matrizBlack[i + 1][j] +
                             matrizBlack[i][j + 1]) /
                            5;

          matrizRed[75][175] = 25.00;
          matrizRed[75][75] = -10.00;
          matrizRed[75][275] = 0.00;
          matrizRed[190][75] = 20.00;
          matrizRed[190][175] = -20.00;
          matrizRed[190][275] = 10.00;
          matrizRed[305][75] = 10.00;
          matrizRed[305][175] = 30.00;
          matrizRed[305][275] = 40.00;

          erroAtual = fabs(matrizRed[i][j] - matrizBlack[i][j]);

          if (erroAtual > erroAbsoluto)
            erroAbsoluto = erroAtual;
        }
      }

      // A red está certa agora, deve mandar os valores para a black
      for (i = iInicial; i < iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          matrizBlack[i][j] = matrizRed[i][j];
        }
      }

      printf("Iteração %d -> Erro: %.2f\n\n", k, erroAbsoluto);

      iteracaoLocal++;
    }

    // Escreve a matriz em um arquivo
    escreveMatrizArquivo(matrizBlack, iInicial, iFinal, id);

    // Deve enviar para o servidor o erro
    //...
    break;
  }
  close(sockfd);
}