#include <math.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "funcoes.h"
#define MAX 80
#define PORT 8081
#define SA struct sockaddr

int main() {
  int sockfd, connfd, bytes_recv;
  int i, j, k, iInicial, iFinal, vizinhoCima, vizinhoBaixo, iteracaoLocal,
      iteracaoGlobal, id;
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
  if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
    printf("connection with the server failed...\n");
    exit(0);
  } else {
    printf("connected to the server..\n");
  }

  // Enfia IP e Porta pro servidor...
  char buffer[MAX];
  char resposta[MAX];

  printf("Digite o seu IP e porta (XXX.XXX.XXX.XXX:XXXX: ");
  strcpy(buffer, "");
  scanf("%s", buffer);
  write(sockfd, buffer, sizeof(buffer));

  bzero(buffer, MAX);
  do {
    bytes_recv = recv(sockfd, buffer, 1024, 0);
  } while (bytes_recv <= 0);

  char iInicialString[3], iFinalString[3], vizinhoString[3], idString[3];
  bzero(iInicialString, sizeof(iInicialString));
  bzero(iFinalString, sizeof(iFinalString));
  bzero(vizinhoString, sizeof(vizinhoString));
  bzero(idString, sizeof(idString));

  j = 0;

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

  printf(
      "Cliente recebeu mensagem do servidor\niInicial: %d\tiFinal: "
      "%d\tVizinho de Cima: %d\tVizinho de Baixo: %d\tID: %d\n\n",
      iInicial, iFinal, vizinhoCima, vizinhoBaixo, id);

  iteracaoLocal = 1;
  iteracaoGlobal = 1;
  while (1) {
    // Deve receber qual a iteração global do servidor e comparar com a iteração
    // global
    strcpy(buffer, "");
    do {
      bytes_recv = recv(sockfd, buffer, 1024, 0);
    } while (strcmp(buffer,"") == 0 || strcmp(buffer, " ") == 0);
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

      if (vizinhoBaixo != 0) {
        // só vai pegar a linha final se não for o último nó
        bytes_recv =
            recv(sockfd, linhaFinalRecebida, sizeof(linhaFinalRecebida), 0);
      }
      for (i = 0; i < 402; i++) {
        // printf("[%d][%d] = %.4f\n", iInicial - 1, i,
        // linhaInicialRecebida[i]);
        matrizBlack[iInicial - 1][i] = linhaInicialRecebida[i];
        if (vizinhoBaixo != 0) {
          // quando não for o último nó, recebe a linha final e coloca na matriz
          matrizBlack[iFinal + 1][i] = linhaFinalRecebida[i];
        }
      }

      // faz os cálculos da matriz

      float erroAtual, erroAbsoluto;
      float matrizAnterior;
      erroAbsoluto = 0;
      printf("Iniciando os cálculos da iteração\n");
      for (i = iInicial; i <= iFinal; i++) {
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
      for (i = iInicial; i <= iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          matrizBlack[i][j] = matrizRed[i][j];
        }
      }

      printf("Iteração %d -> Erro: %.3f\n\n", iteracaoLocal, erroAbsoluto);

      // Deve enviar para o servidor a última linha da matriz e o erro
      printf("Enviando linha %d e %d para o servidor.\n", iInicial, iFinal);
      send(sockfd, matrizBlack[iFinal], sizeof(matrizBlack[iFinal]), 0);
      send(sockfd, matrizBlack[iInicial], sizeof(matrizBlack[iInicial]), 0);

      strcpy(buffer, "");
      snprintf(buffer, sizeof(buffer), "%.3f", erroAbsoluto);
      send(sockfd, buffer, sizeof(buffer), 0);

      printf("--- FIM DA ITERAÇÃO %d -> Erro: %.3f ---\n\n", iteracaoLocal,
             erroAbsoluto);

      iteracaoLocal++;
    } else if (iteracaoGlobal == 0) {
      escreveMatrizArquivo(matrizBlack, iInicial, iFinal, id);
      break;  // termina o while
    }
  }
  // Escreve a matriz em um arquivo
  printf("\nFIM DO CLIENTE %d\n", id);
  // close(sockfd);
}
