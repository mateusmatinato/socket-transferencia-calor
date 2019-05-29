#include <arpa/inet.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include "funcoes.h"
#define MAX 80
#define PORT 4040
#define SA struct sockaddr
#define NUM_NODES 3

// Cada nó possuirá um id, um ip, uma porta e um rank atribuído pelo servidor.
struct clients {
  char ip[12];
  int port;
  int iInicial;
  int iFinal;
  int vizinhoCima;
  int vizinhoBaixo;
  int id;
};

int main() {
  int nodes[NUM_NODES];
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;
  struct clients clients[NUM_NODES];
  char buffer[MAX];
  int i, j, k, bytes_recv;

  // Zera a matriz
  float matrizRed[402][402], matrizBlack[402][402];
  for (i = 0; i < 402; i++) {
    for (j = 0; j < 402; j++) {
      matrizRed[i][j] = 0;
      matrizBlack[i][j] = 0;
    }
  }

  // Insere os valores na matriz
  matrizBlack[75][75] = -10;
  matrizBlack[75][175] = 25;
  matrizBlack[75][275] = 0;
  matrizBlack[190][75] = 20;
  matrizBlack[190][175] = -20;
  matrizBlack[190][275] = 10;
  matrizBlack[305][75] = 10;
  matrizBlack[305][175] = 30;
  matrizBlack[305][275] = 40;

  // socket create and verification
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("socket creation failed...\n");
    exit(0);
  } else
    printf("Socket successfully created..\n");

  bzero(&servaddr, sizeof(servaddr));

  // assign IP, PORT
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binding newly created socket to given IP and verification
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("socket bind failed...\n");
    exit(0);
  } else
    printf("Socket successfully binded..\n");

  // Now server is ready to listen and verification
  if ((listen(sockfd, 5)) != 0) {
    printf("Listen failed...\n");
    exit(0);
  } else
    printf("Server listening..\n");

  char recv_data[1024], text[1024];

  int iInicial =
      400 / (NUM_NODES + 1) + 1;  // 400/4 + 1 = 101 ou 400/8 + 1 = 51
  int iFinal = iInicial + 400 / (NUM_NODES + 1) -
               1;  // 101 + 100 - 1 = 200 ou 51 + 50 - 1 = 100
  for (i = 0; i < NUM_NODES; i++) {
    len = sizeof(cli);
    nodes[i] = accept(sockfd, (SA*)&cli, &len);
    if (nodes[i] < 0) {
      printf("Conexão com o nó %d falhou!\n", i + 1);
    } else {
      printf("Conexão com o nó %d estabelecida com sucesso!\n", i + 1);
    }

    // Quando o cliente se conectar, ele deve informar o IP para o servidor no
    // seguinte formato: ip:porta
    bytes_recv = recv(nodes[i], recv_data, 1024, 0);
    recv_data[bytes_recv] = '\0';
    j = 0;
    k = j;
    while (recv_data[j] != ':') {
      text[k] = recv_data[j];
      k++;
      j++;
    }
    text[k] = '\0';
    j++;
    k = 0;
    strcpy(clients[i].ip, text);
    char porta[5];
    while (recv_data[j] != '\0') {
      porta[k] = recv_data[j];
      k++;
      j++;
    }
    clients[i].port = (int)strtol(porta, (char**)NULL, 10);
    clients[i].iInicial = iInicial;  // delega onde ele deve iniciar
    clients[i].iFinal = iFinal;

    iInicial += 400 / (NUM_NODES + 1);
    iFinal += 400 / (NUM_NODES + 1);

    printf("\nCliente %d conectado\nEndereço: %s:%d\n", i + 1, clients[i].ip,
           clients[i].port);
    fflush(stdout);
  }

  // o vizinho do servidor é o primeiro nó
  int vizinhoServidor = nodes[0];

  // O iInicial do servidor sempre é 1 (ignora a borda), e o final é
  // 400/(NUM_NODES+1)
  iInicial = 1;
  iFinal = 400 / (NUM_NODES + 1);

  for (i = 0; i < NUM_NODES; i++) {
    // Distribui os vizinhos entre os clientes
    clients[i].id = nodes[i];
    if (i == 0) {
      // o vizinho de cima do primeiro nó é o servidor
      clients[i].vizinhoCima = sockfd;
      clients[i].vizinhoBaixo = nodes[i + 1];
    } else if (i == NUM_NODES - 1) {
      // o vizinho de baixo do ultimo client é 0
      clients[i].vizinhoBaixo = 0;
      clients[i].vizinhoCima = nodes[i - 1];
    } else {
      clients[i].vizinhoBaixo = nodes[i + 1];
      clients[i].vizinhoCima = nodes[i - 1];
    }

    bzero(buffer, sizeof(buffer));
    snprintf(buffer, sizeof(buffer),
             "iInicial=%d; iFinal=%d;vizinhoCima=%d;vizinhoBaixo=%d;id=%d",
             clients[i].iInicial, clients[i].iFinal, clients[i].vizinhoCima,
             clients[i].vizinhoBaixo, clients[i].id);
    send(nodes[i], buffer, sizeof(buffer), 0);
    printf(
        "Enviando mensagem para o nó %d\niInicial: %d\tiFinal: "
        "%d\tVizinho de Cima: %d\tVizinho de Baixo: %d\tID:%d\n",
        nodes[i], clients[i].iInicial, clients[i].iFinal,
        clients[i].vizinhoCima, clients[i].vizinhoBaixo, clients[i].id);
  }

  int iteracaoLocal = 1;
  int iteracaoGlobal = 1;

  float maiorErro = 0;
  float erroAtual;
  k = 1;
  while (1 && iteracaoGlobal <= 2) {
    // loop para troca de mensagens

    // envia a iteração atual para todos os clientes
    bzero(buffer, sizeof(buffer));
    snprintf(buffer, sizeof(buffer), "iteracaoGlobal=%d", iteracaoGlobal);
    for (i = 0; i < NUM_NODES; i++) {
      send(nodes[i], buffer, sizeof(buffer), 0);
    }

    if (iteracaoLocal == iteracaoGlobal) {
      // envia as linhas de intersecção para todos os clientes
      matrizBlack[100][10] = 100;  // debug
      matrizBlack[200][10] = 200;  // debug
      matrizBlack[300][10] = 300;  // debug
      for (i = 0; i < NUM_NODES; i++) {
        send(nodes[i], matrizBlack[clients[i].iInicial - 1],
             sizeof(matrizBlack[clients[i].iInicial - 1]), 0);
      }

      float erroAbsoluto = 0;
      // Calcula os novos pontos e o erro em cada ponto, deve pegar o maior
      // erro.
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

      printf("Iteração %d -> Erro: %.2f\n", k, erroAbsoluto);

      iteracaoLocal++;
      k++;

      // recebe as linhas de intersecção e coloca na matrizBlack
      float linhaRecebida[402];
      float erro;
      char erroString[5];
      bzero(buffer, sizeof(buffer));
      for (i = 0; i < NUM_NODES; i++) {
        bzero(linhaRecebida, sizeof(linhaRecebida));
        bytes_recv = recv(nodes[i], linhaRecebida, sizeof(linhaRecebida), 0);
        bytes_recv = recv(nodes[i], buffer, sizeof(buffer), 0);
        buffer[bytes_recv] = '\0';
        j = 0;
        k = 0;
        while (buffer[j] != '\0') {
          erroString[k] = buffer[j];
          j++;
          k++;
        }
        erro = atof(erroString);
        if (erro > maiorErro)
          maiorErro = erro;

        printf("\nRecebendo informações do nó %d:\n", nodes[i]);
        printf("Recebendo linha %d da matriz.\n", clients[i].iFinal);
        printf("Erro recebido: %.2f\n", erro);
        for (j = 0; j < 402; j++) {
          matrizBlack[clients[i].iFinal][j] = linhaRecebida[j];
        }
      }

      if (maiorErro < 0.01) {
        // aqui deve juntar as matrizes em um arquivo e sair do while de troca
        // de mensagens
      } else {
        // incrementa iteracaoGlobal e continua no loop de troca de mensagens
        iteracaoGlobal++;
      }
    }

    escreveMatrizArquivo(matrizBlack, iInicial, iFinal, 0);

    // break;
  }
  close(sockfd);  // termina o socket
}