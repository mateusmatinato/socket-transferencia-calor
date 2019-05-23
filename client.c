// Write CPP code here
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 4040
#define SA struct sockaddr

void imprimeMatriz(float matriz[402][402], int iInicial, int iFinal) {
  int i, j;
  for (i = iInicial; i < iFinal; i++) {
    printf("Linha %d\n", i);
    for (j = 0; j < 402; j++) {
      printf("%.2f ", matriz[i][j]);
    }
    printf("\n\n");
  }
}

int main() {
  int sockfd, connfd, bytes_recv;
  int i, j, k, iInicial, iFinal;
  struct sockaddr_in servaddr, cli;

  // Zera a matriz
  float matriz[402][402];
  for (i = 0; i < 402; i++) {
    for (j = 0; j < 402; j++) {
      matriz[i][j] = 0;
    }
  }

  // Insere os valores na matriz
  matriz[75][75] = -10;
  matriz[75][175] = 25;
  matriz[75][275] = 0;
  matriz[190][75] = 20;
  matriz[190][175] = -20;
  matriz[190][275] = 10;
  matriz[305][75] = 10;
  matriz[305][175] = 30;
  matriz[305][275] = 40;

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
  while (1) {
    bzero(buffer, MAX);
    bytes_recv = recv(sockfd, buffer, 1024, 0);
    char iInicialString[3], iFinalString[3];
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
    j++; // pula pro próximo, que é o iFinal até o fim
    i = 0;
    while (buffer[j] != '\0') {
      iFinalString[i] = buffer[j];
      j++;
      i++;
    }
    iFinal = atoi(iFinalString);

    float erro = 0;
    float erroAnterior;
    float matrizAnterior;
    k = 0;
    do{
      for (i = iInicial; i < iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          // percorre somente o intervalo de linhas que pertence
          erroAnterior = erro;
          matrizAnterior = matriz[i][j];
          matriz[i][j] = (matriz[i][j]+matriz[i-1][j]+matriz[i][j-1]+matriz[i+1][j]+matriz[i][j+1])/5;
          erro = matriz[i][j] - matrizAnterior;
          if(erroAnterior > erro) erro = erroAnterior; // isso garante que vai pegar o maior erro
        }
      }
      k++;
    }
    while(erro > 0.01 && k < 100);

    imprimeMatriz(matriz,iInicial,iFinal);
    printf("Erro: %.2f\n",erro);
    break;
  }

  // function for chat
  //  func(sockfd);

  // close the socket
  close(sockfd);
}