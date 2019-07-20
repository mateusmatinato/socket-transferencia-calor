#include <mpi.h>
#include "math.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"

#define maxtime 1000
/* Simulação é executada com MINPROC processos, podendo
 * ser igual a 1 para testes. Para uma matriz maior é
 * preciso ajustar MINPROC p/ o numero real de nós
 * sendo que esse valor precisa ser trocado AQUI */
#define MINPROC 1
#define cols 400
#define totrows 400
#define rows totrows / MINPROC + 2
double black[rows + 2][cols + 2];
double red[rows + 2][cols + 2];

void storeconst(int s, int e, int row, int col, double val, int isRed) {
  if (isRed == 0) {
    // Muda na black
    if (row >= s && row <= e)
      black[row - s][col] = val;
  } else {
    // Muda na red
    if (row >= s && row <= e)
      red[row - s][col] = val;
  }
}

void matrizParcial(int id, int mylen, int s) {
  int r, c;
  char nomeArquivo[10];
  FILE* pArq;
  snprintf(nomeArquivo, sizeof(nomeArquivo), "Matriz-%d", id);
  if ((pArq = fopen(nomeArquivo, "w")) == 0x0) {
    printf("erro.");
    exit(1);
  }

  for (r = 1; r <= mylen - 1; r++) {
    fprintf(pArq, "LINHA %d->\t", r + s);
    for (c = 0; c <= cols + 1; c++) {
      fprintf(pArq, "%.2f\t\t", red[r][c]);
    }
    fprintf(pArq, "\n");
  }
  if (fclose(pArq)) {
    printf("erro.");
    exit(1);
  }
}

void matrizSaida(int id, int flag) {
  FILE* arquivoNo;
  FILE* arquivoFinal;

  char nomeArquivo[100];
  sprintf(nomeArquivo, "Matriz-%d", id);

  arquivoNo = fopen(nomeArquivo, "r");
  arquivoFinal = fopen("MatrizFinal", "a");

  int i;
  if (flag == 1) {
    fprintf(arquivoFinal, "         \t");
    for (i = 0; i < 402; i++) {
      fprintf(arquivoFinal, "COL %d\t\t", i);
    }
    fprintf(arquivoFinal, "\n");
  }
  char ch;
  while ((ch = fgetc(arquivoNo)) != EOF)
    fputc(ch, arquivoFinal);

  fclose(arquivoNo);
  fclose(arquivoFinal);

  remove(nomeArquivo);
}

int main(int argc, char** argv) {
  double erroAtual, erroAbsoluto, maiorErro;
  int s, e, mylen, r, c, tick;
  int inum, nproc, dims, coords, ndim = 1;
  int i = 0, iteracoes = 0;
  int periods, reorder;  // usadas como booleanas
  int upproc, downproc;
  MPI_Comm comm1d;  // nova definição para comunicador!!
  MPI_Status status;
  // Chamando MPI_INIT

  /* Inicia tempos */
  double relogioParede = 0, relogioCPU = 0;
  time_t begin = time(NULL);
  clock_t inicio = clock();

  MPI_Init(&argc, &argv);
  // Recebendo o número de processos em paralelo
  MPI_Comm_size(MPI_COMM_WORLD, &nproc);

  dims = nproc;
  periods = 0;  // significa um valor falso
  reorder = 1;  // significa um valor verdade
  // cria grupo cartesiano unidimensional de processos
  MPI_Cart_create(MPI_COMM_WORLD, ndim, &dims, &periods, reorder, &comm1d);
  // Pega valor de rank dentro do grupo de comunicadores
  MPI_Comm_rank(comm1d, &inum);

  // Pega a topologia do grupo COMM1D e o lugar
  // desse processo dentro dessa topologia
  MPI_Cart_get(comm1d, ndim, &dims, &periods, &coords);
  // Retorna os vizinhos de distância 1 acima e abaixo,
  // sendo que os das extremidades terão apenas um deles
  MPI_Cart_shift(comm1d, 0, 1, &upproc, &downproc);
  s = totrows / nproc * inum;
  e = totrows / nproc * (inum + 1);
  mylen = (e - s) + 1;
  if (mylen > rows) {
    printf("No space, need %d, have %d\n", mylen, rows);
    printf("%d %d %d %d %d\n", totrows, nproc, inum, s, e);
  }
  printf("%d %d %d %d %d %d %d \n", inum, nproc, coords, upproc, downproc, s,
         e);
  // Começa frio (todos os pontos em 0◦C)
  for (r = 0; r <= mylen + 1; r++)
    for (c = 0; c <= cols + 1; c++)
      black[r][c] = 0.0;

  // Inicia a execução das iterações
  int loop = 1;
  maiorErro = 999;
  while (loop == 1) {
    erroAbsoluto = 0;
    // for (tick = 0; tick < maxtime; tick++) {
    // Inicia valores das fontes constantes de calor
    storeconst(s, e, 75, 75, -10, 0);
    storeconst(s, e, 75, 175, 25, 0);
    storeconst(s, e, 75, 275, 0, 0);
    storeconst(s, e, 190, 75, 20, 0);
    storeconst(s, e, 190, 175, -20, 0);
    storeconst(s, e, 190, 275, 10, 0);
    storeconst(s, e, 305, 75, 10, 0);
    storeconst(s, e, 305, 175, 30, 0);
    storeconst(s, e, 305, 275, 40, 0);

    // Envia para vizinho de cima e recebe do vizinho abaixo
    MPI_Send(&black[1][0], cols, MPI_DOUBLE, upproc, 1, comm1d);
    MPI_Recv(&black[mylen + 1][0], cols, MPI_DOUBLE, downproc, 1, comm1d,
             &status);

    // Envia para baixo e recebe de cima num único comando
    MPI_Sendrecv(&black[mylen][0], cols, MPI_DOUBLE, downproc, 2, &black[0][0],
                 cols, MPI_DOUBLE, upproc, 2, MPI_COMM_WORLD, &status);

    /* Calcula-se as temperaturas nessa iteração */
    for (r = 1; r <= mylen; r++) {
      for (c = 1; c <= cols; c++) {
        red[r][c] = (black[r][c] + black[r][c - 1] + black[r - 1][c] +
                     black[r + 1][c] + black[r][c + 1]) /
                    5.0;

        storeconst(s, e, 75, 75, -10, 1);
        storeconst(s, e, 75, 175, 25, 1);
        storeconst(s, e, 75, 275, 0, 1);
        storeconst(s, e, 190, 75, 20, 1);
        storeconst(s, e, 190, 175, -20, 1);
        storeconst(s, e, 190, 275, 10, 1);
        storeconst(s, e, 305, 75, 10, 1);
        storeconst(s, e, 305, 175, 30, 1);
        storeconst(s, e, 305, 275, 40, 1);

        erroAtual = fabs(red[r][c] - black[r][c]);
        if (erroAtual > erroAbsoluto) {
          erroAbsoluto = erroAtual;
        }
      }
    }

    for (r = 1; r <= mylen; r++)
      for (c = 1; c <= cols; c++)
        black[r][c] = red[r][c];

    // Cada um deve enviar seu erro e verificar qual o maior erro
    MPI_Allreduce(&erroAbsoluto, &maiorErro, 1, MPI_DOUBLE, MPI_MAX,
                  MPI_COMM_WORLD);

    if (inum == 0) {
      printf("Fim da iteração %d\tMaior erro: %f\n", i, maiorErro);
    }
    i++;
    if (maiorErro < 0.01) {
      loop = 0;
      iteracoes = i - 1;
    }
  }

  // Cada processo escreve sua matriz parcial em um arquivo
  matrizParcial(inum, mylen, s);

  if (inum == 0) {
    remove("MatrizFinal");  // Remove o arquivo final caso ele existir
    i = 0;
    for (i == 0; i < dims; i++) {
      if (i == 0) {
        matrizSaida(i, 1);
      } else {
        matrizSaida(i, 0);
      }
    }
  }
  // Cálculo dos tempos
  clock_t fim = clock();
  time_t end = time(NULL);

  relogioParede = (double)(end - begin);
  relogioCPU = (double)(fim - inicio) / CLOCKS_PER_SEC;

  if (inum == 0) {
    printf("\n\n----------------- FIM -------------\n");
    printf("Número de processos: %d\n", dims);
  }
  // Mostra informações finais
  MPI_Barrier(MPI_COMM_WORLD);
  printf("Processo %d -> Tempo de CPU: %lf segundos\n", inum, relogioCPU);
  MPI_Barrier(MPI_COMM_WORLD);
  double mediaTemposCPU;
  MPI_Allreduce(&relogioCPU, &mediaTemposCPU, 1, MPI_DOUBLE, MPI_SUM,
                MPI_COMM_WORLD);
  if (inum == 0) {
    printf("Média dos tempos de CPU: %lf segundos\n", mediaTemposCPU/dims);
    printf("Tempo total de execução (wall-clock): %lf segundos\n", relogioParede);
    printf("Número total de iterações: %d\n", iteracoes);
    printf("A matriz final foi salva no arquivo MatrizFinal.txt\n");
    printf("-----------------------------------\n\n");
  }

  MPI_Finalize();
}