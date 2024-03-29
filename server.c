/**************************************
 * Trabalho de Sistemas Distribuídos   *
 * Transferência de Calor em Chapa     *
 * Professor:  Aleardo Manacero Jr.    *
 * Alunos:     Mateus Matinato         *
 *             Jonatan Rodrigues       *
 *             João Marcos Rosa        *
 * Data de Modificação: 06/06 19:39    *
 **************************************/

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
#define MAX 80  // Tamanho do buffer que será usado para transmitir mensagens
#define PORT 8081           // Porta de acesso ao socket
#define SA struct sockaddr  // Struct de endereço do socket
#define NUM_NODES 3  // Número de clientes que o servidor espera: 1, 3 e 7

/*
Struct que armazenará as informações de cada cliente.
IP e Porta: Para conexão em várias máquinas.
iInicial: Linha inicial que o cliente deve calcular
iFinal: Linha final que o cliente deve calcular
vizinhoCima: id do Vizinho de cima do cliente
vizinhoBaixo: id do Vizinho de baixo do cliente
id: identificador do cliente
*/
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
  int nodes[NUM_NODES];  // Vetor de clientes
  int sockfd, connfd, len;
  struct sockaddr_in servaddr, cli;   // Structs do socket
  struct clients clients[NUM_NODES];  // Vetor de Struct de clientes
  char buffer[MAX];
  int i, j, k, bytes_recv;

  // Zera as matrizes red e black
  float matrizRed[402][402], matrizBlack[402][402];
  for (i = 0; i < 402; i++) {
    for (j = 0; j < 402; j++) {
      matrizRed[i][j] = 0;
      matrizBlack[i][j] = 0;
    }
  }

  // Insere as constantes na matriz black
  matrizBlack[75][75] = -10;
  matrizBlack[75][175] = 25;
  matrizBlack[75][275] = 0;
  matrizBlack[190][75] = 20;
  matrizBlack[190][175] = -20;
  matrizBlack[190][275] = 10;
  matrizBlack[305][75] = 10;
  matrizBlack[305][175] = 30;
  matrizBlack[305][275] = 40;

  // Criação e verificação do socket
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if (sockfd == -1) {
    printf("Socket criado com sucesso...\n");
    exit(0);
  } else
    printf("Falha na criação do Socket...\n");

  bzero(&servaddr, sizeof(servaddr));

  // Define IP e Porta do Servidor
  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_port = htons(PORT);

  // Binda o socket para o IP e Porta assinalados
  if ((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {
    printf("Socket bindado com sucesso...\n");
    exit(0);
  } else
    printf("Falha no bind do socket...\n");

  // Servidor aguarda conexões...
  if ((listen(sockfd, 5)) != 0) {
    printf("Falha ao ouvir conexões...\n");
    exit(0);
  } else
    printf("Ouvindo conexões...\n");

  // Variáveis utilizadas para receber mensagens dos clientes
  char recv_data[1024], text[1024];

  // Define o iInicial do primeiro cliente
  int iInicial = 400 / (NUM_NODES + 1) + 1;
  // Define o iFinal do primeiro cliente
  int iFinal = iInicial + 400 / (NUM_NODES + 1) - 1;

  // Aguarda as conexões dos clientes...
  for (i = 0; i < NUM_NODES; i++) {
    len = sizeof(cli);
    nodes[i] = accept(sockfd, (SA*)&cli, &len);
    if (nodes[i] < 0) {
      printf("\nConexão com o cliente %d falhou!\n", i + 1);
    } else {
      printf("\nConexão com o cliente %d estabelecida com sucesso!\n", i + 1);
    }

    /*
    Para várias maquinas: quando o cliente se conectar, ele deve informar o IP
    para o servidor. Então deve percorrer a string para armazenar esses valores
    e atribuir na struct do cliente.
    Para uma máquina: Qualquer ip e porta é válido.
    */

    // Percorre a string para armazenar os valores de ip e porta
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
    clients[i].iInicial = iInicial;  // Coloca no struct o valor de iInicial
    clients[i].iFinal = iFinal;      // Coloca no struct o valor de iFinal

    // Define o iInicial do próximo cliente
    iInicial += 400 / (NUM_NODES + 1);
    // Define o iFinal do próximo cliente
    iFinal += 400 / (NUM_NODES + 1);

    printf("Cliente %d conectado\nEndereço: %s:%d\n", i + 1, clients[i].ip,
           clients[i].port);
    fflush(stdout);
  }

  // O vizinho de baixo do servidor é o primeiro cliente, sempre.
  int vizinhoServidor = nodes[0];

  // O iInicial do servidor sempre é 1 pois ignora a borda.
  iInicial = 1;
  // Define o iFinal do servidor
  iFinal = 400 / (NUM_NODES + 1);

  /*
  Atribui os vizinhos de cima e baixo para todos os clientes.
  Após isso, deve enviar as informações pertinentes a cada cliente como:
  iInicial, iFinal, vizinhoCima, vizinhoBaixo e id.
  */
  for (i = 0; i < NUM_NODES; i++) {
    // Distribui os vizinhos entre os clientes
    clients[i].id = nodes[i];
    if (NUM_NODES > 1) {
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
    } else {
      // só tem um nó, então o vizinho de cima é o servidor e não tem vizinho de
      // baixo
      clients[i].vizinhoBaixo = 0;
      clients[i].vizinhoCima = sockfd;
    }

    // Envia informações para todos os clientes
    strcpy(buffer, "");
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

  float erroAtual;
  k = 1;

  remove("MatrizFinal");  // Remove o arquivo final caso ele existir

  double relogioParede = 0, relogioCPU = 0;
  time_t begin = time(NULL);
  clock_t inicio = clock();

  /*
  Servidor inicia enviando para todos os clientes qual iteração todos devem
  estar, após isso, verifica se a iteração local é igual a iteração global. Caso
  positivo pode iniciar a iteração.
  Quando inicia a iteração as linhas de intersecção são distribuídas conforme
  os vizinhos. Após isso é feito o cálculo da iteração de modo semelhante ao
  cliente. Caso contrário, deve aguardar pois algum servidor ainda não terminou.
  */

  while (1) {
    float maiorErro = 0;

    // envia a iteração global para todos os clientes
    strcpy(buffer, "");
    snprintf(buffer, sizeof(buffer), "iteracaoGlobal=%d", iteracaoGlobal);
    for (i = 0; i < NUM_NODES; i++) {
      send(nodes[i], buffer, sizeof(buffer), 0);
    }

    if (iteracaoLocal == iteracaoGlobal) {
      // Envio das linhas de intersecção para os vizinhos
      for (i = 0; i < NUM_NODES; i++) {
        // envia a linha de cima
        send(nodes[i], matrizBlack[clients[i].iInicial - 1],
             sizeof(matrizBlack[clients[i].iInicial - 1]), 0);

        // envia a primeira linha do de baixo somente se não for o último nó
        if (i != NUM_NODES - 1) {
          send(nodes[i], matrizBlack[clients[i].iFinal + 1],
               sizeof(matrizBlack[clients[i].iFinal + 1]), 0);
        }
      }

      float erroAbsoluto = 0;
      // Calcula os novos pontos e o erro em cada ponto, deve pegar o maior
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

      // Transfere os valores da red para a black
      for (i = iInicial; i <= iFinal; i++) {
        for (j = 1; j <= 400; j++) {
          matrizBlack[i][j] = matrizRed[i][j];
        }
      }

      // Mostra informações da iteração no servidor
      printf("\nInformações da iteração do servidor:\n");
      printf("Iteração %d -> Erro: %.3f\n", iteracaoLocal, erroAbsoluto);
      k++;

      // Recebe a linha de intersecção do vizinho e atribui nos vetores
      // auxiliares
      float linhaFinalRecebida[402], linhaInicialRecebida[402];
      float erro;
      for (i = 0; i < NUM_NODES; i++) {
        for (j = 0; j < 402; j++) {
          linhaInicialRecebida[j] = 0;
          linhaFinalRecebida[j] = 0;
        }
        do {
          bytes_recv =
              recv(nodes[i], linhaFinalRecebida, sizeof(linhaFinalRecebida), 0);

        } while (bytes_recv <= 0);

        do {
          bytes_recv = recv(nodes[i], linhaInicialRecebida,
                            sizeof(linhaInicialRecebida), 0);
        } while (bytes_recv <= 0);

        // Recebe o maior erro de cada um dos clientes
        strcpy(buffer, "");
        do {
          bytes_recv = recv(nodes[i], buffer, sizeof(buffer), 0);
        } while (bytes_recv <= 0);

        char erroString[bytes_recv];
        buffer[bytes_recv] = '\0';
        j = 0;
        k = 0;
        while (buffer[j] != '\0') {
          erroString[k] = buffer[j];
          j++;
          k++;
        }
        erro = atof(erroString);

        // Armazena o maior erro de todos os clientes
        if (erro > maiorErro)
          maiorErro = erro;

        printf("\nRecebendo informações do nó %d:\n", nodes[i]);
        printf("Recebendo linha %d da matriz.\n", clients[i].iFinal);
        printf("Erro recebido: %.3f\n", erro);
        for (j = 0; j < 402; j++) {
          matrizBlack[clients[i].iFinal][j] =
              linhaFinalRecebida[j];  // Coloca na matriz o vizinho
          matrizBlack[clients[i].iInicial][j] =
              linhaInicialRecebida[j];  // Coloca na matriz o vizinho
        }
      }

      // Verifica se o maior erro de todos os clientes é maior que o erro do
      // servidor
      if (erroAbsoluto > maiorErro)
        maiorErro = erroAbsoluto;
      printf("--- FIM DA ITERAÇÃO %d -> Maior Erro: %.3f ---\n\n",
             iteracaoGlobal, maiorErro);

      iteracaoLocal++;

      /*
      Se o maiorErro dos clientes e do servidor for menor que 0.01 deve parar a
      execução e juntar as matrizes. Caso for maior, a iteracaoGlobal deve ser
      incrementada para iniciar a próxima iteração em todos os clientes e no
      servidor.
      */
      if (maiorErro < 0.01) {
        //Define iteracaoGlobal = 0 para parar a execução dos clientes.
        iteracaoGlobal = 0;
      } else {
        // incrementa iteracaoGlobal e continua no loop de troca de mensagens
        iteracaoGlobal++;
      }
    } else if (iteracaoGlobal == 0) {
      break;
    }
  }
  //Cálculo dos tempos
  clock_t fim = clock();
  time_t end = time(NULL);

  relogioParede = (double)(end - begin);
  relogioCPU = (double)(fim - inicio) / CLOCKS_PER_SEC;
  escreveMatrizArquivo(matrizBlack, iInicial, iFinal, sockfd);

  //Junta as matrizes em um arquivo para melhor visualização.
  matrizSaida(sockfd, 1);
  for (i = 0; i < NUM_NODES; i++) {
    matrizSaida(clients[i].id, 0);
  }

  //Mostra informações finais
  printf("\n\n----------------- FIM -------------\n");
  printf("Número de Nós (Clientes + Servidor): %d\n", NUM_NODES + 1);
  printf("Tempo total do relógio de parede: %lf segundos\n", relogioParede);
  printf("Tempo total do relógio da CPU: %lf segundos\n", relogioCPU);
  printf("Número total de iterações: %d\n", iteracaoLocal - 1);
  printf("A matriz final foi salva no arquivo MatrizFinal.txt\n");
  printf("-----------------------------------\n\n");

  close(sockfd);
}
