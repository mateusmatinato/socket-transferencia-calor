// Write CPP code here
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#define MAX 80
#define PORT 4040
#define SA struct sockaddr

// Função para troca de mensagens
void func(int sockfd) {
  char buff[MAX];
  int n;
  for (;;) {
    bzero(buff, sizeof(buff));
    printf("Enter the string : ");
    n = 0;
    while ((buff[n++] = getchar()) != '\n')
      ;
    write(sockfd, buff, sizeof(buff));
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
    if ((strncmp(buff, "exit", 4)) == 0) {
      printf("Client Exit...\n");
      break;
    }
  }
}

int main() {
  int sockfd, connfd;
  struct sockaddr_in servaddr, cli;

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

  //Enfia IP e Porta pro servidor...
  char buffer[MAX];
  printf("Digite o seu IP e porta (XXX.XXX.XXX.XXX:XXXX: ");
  bzero(buffer, sizeof(buffer));
  scanf("%s", &buffer);
  write(sockfd, buffer, sizeof(buffer));
  while(1){
    //aqui deve aguardar o servidor delegar qual o vizinho que ele deve se comunicar
    bzero(buffer, MAX);
    read(sockfd, buffer, sizeof(buffer));
    if(strlen(buffer) != 0){
      printf("Mensagem recebida: %s\n",buffer);
    }
    
  }

  // function for chat
  //  func(sockfd);

  // close the socket
  close(sockfd);
}