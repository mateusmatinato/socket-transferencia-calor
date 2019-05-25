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

void escreveMatrizArquivo(float matriz[402][402], int iInicial, int iFinal, int id) {
  FILE *pArq;
  int i,j;
  int nLin, nCol;
  char nomeArquivo[10];
  snprintf(nomeArquivo, sizeof(nomeArquivo), "Matriz-%d",id);
  if ((pArq = fopen(nomeArquivo, "w")) == 0x0) {
    printf("erro.");
    exit(1);
  }

  for (i = 0; i < iInicial; i++) {
    for (j = 0; j < iFinal; j++)
      fprintf(pArq, "%.2f\t", matriz[i][j]);

    fprintf(pArq, "\n");
  }

  if (fclose(pArq)) {
    printf("erro.");
    exit(1);
  }
}

int main() {
  int sockfd, connfd, bytes_recv;
  int i, j, k, iInicial, iFinal, vizinho, iteracaoLocal, iteracaoGlobal;
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
  iteracaoLocal = 1;
  while (1) {
    bzero(buffer, MAX);
    bytes_recv = recv(sockfd, buffer, 1024, 0);
    char iInicialString[3], iFinalString[3], vizinhoString[1];
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
    // Percore até o fim para pegar qual o vizinho
    j++;
    i = 0;
    while (buffer[j] != '\o') {
      vizinhoString[i] = buffer[j];
      j++;
      i++;
    }
    vizinho = atoi(vizinhoString);

    printf("Cliente recebeu mensagem do servidor\niInicial: %d\tiFinal: "
           "%d\tVizinho: %d\n",
           iInicial, iFinal, vizinho);

    // Deve receber qual a iteração global do servidor e comparar com a iteração
    // global

    // Se iteração local == iteração global pode enviar a linha
    // (matriz[iFinal][]) para o vizinho

    // Depois de enviar a linha para o vizinho deve fazer os cálculos da matriz
    // Lembrar de, ao fim de cada iteração COLOCAR NOVAMENTE os pontos
    // CONSTANTES!!

    float erro = 0;
    float erroAnterior;
    float matrizAnterior;
    k = 0;
    do {
      for (i = iInicial; i < iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          // percorre somente o intervalo de linhas que pertence
          erroAnterior = erro;
          matrizAnterior = matriz[i][j];
          matriz[i][j] = (matriz[i][j] + matriz[i - 1][j] + matriz[i][j - 1] +
                          matriz[i + 1][j] + matriz[i][j + 1]) /
                         5;
          erro = matriz[i][j] - matrizAnterior;
          if (erroAnterior > erro)
            erro = erroAnterior; // isso garante que vai pegar o maior erro
        }
      }
      matriz[75][75] = -10;
      matriz[75][175] = 25;
      matriz[75][275] = 0;
      matriz[190][75] = 20;
      matriz[190][175] = -20;
      matriz[190][275] = 10;
      matriz[305][75] = 10;
      matriz[305][175] = 30;
      matriz[305][275] = 40;
      k++;
    } while (erro > 0.01 && k < 100);

    escreveMatrizArquivo(matriz, iInicial, iFinal,vizinho);
    printf("Erro: %.2f\n", erro);
  }

  close(sockfd);
}