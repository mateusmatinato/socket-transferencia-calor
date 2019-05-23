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
#define MAX 80
#define PORT 4040
#define SA struct sockaddr
#define NUM_NODES 3

// Cada nó possuirá um id, um ip, uma porta e um rank atribuído pelo servidor.
struct nodes {
  char ip[12];
  int port;
  int rank;
};

/* NÃO UTILIZADO POR ENQUANTO 
// Função para troca de mensagens
void func(int sockfd) {
  char buff[MAX];
  int n;
  // infinite loop for chat
  for (;;) {
    bzero(buff, MAX);

    // read the message from client and copy it in buffer
    read(sockfd, buff, sizeof(buff));
    // print buffer which contains the client contents
    printf("From client: %s\nTo client : ", buff);
    bzero(buff, MAX);
    n = 0;
    // copy server message in the buffer
    while ((buff[n++] = getchar()) != '\n')
      ;

    // and send that buffer to client
    write(sockfd, buff, sizeof(buff));

    // if msg contains "Exit" then server exit and chat ended.
    if (strncmp("exit", buff, 4) == 0) {
      printf("Server Exit...\n");
      break;
    }
  }
}
*/

// Driver function
int main() {
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;
  int nodes[NUM_NODES];
  struct nodes clients[NUM_NODES];
  char buffer[MAX];

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
  if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0) {
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

  int i, j, k, bytes_recv;
  char recv_data[1024], text[1024];
  for (i = 0; i < NUM_NODES; i++) {

    len = sizeof(cli);
    nodes[i] = accept(sockfd, (SA *)&cli, &len);
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
    clients[i].port = (int)strtol(porta, (char **)NULL, 10);
    clients[i].rank = (i + 1); // atribui o rank com base na ordem que chegou
    printf("\nCliente %d conectado\nEndereço: %s:%d\n", i + 1, clients[i].ip,
           clients[i].port);
    fflush(stdout);
  }

  // Enviar mensagem para um nó específico
  while (1) {
    // loop para troca de mensagens
    int numNode, n;
    n = 0;
    bzero(buffer,sizeof(buffer));
    printf("Para qual nó deseja enviar mensagem: ");
    while ((buffer[n++] = getchar()) != '\n');
    numNode = atoi(buffer);

    printf("Digite a mensagem: ");
    n = 0;
    bzero(buffer, sizeof(buffer));
    while ((buffer[n++] = getchar()) != '\n');
    write(nodes[numNode], buffer, sizeof(buffer)); // envia para o nó escolhido a mensagem do buffer
    printf("\n");
  }
  close(sockfd); // termina o socket
}