void escreveMatrizArquivo(float matriz[402][402], int iInicial, int iFinal,
                          int id) {
  FILE *pArq;
  int i, j;
  int nLin, nCol;
  char nomeArquivo[10];
  snprintf(nomeArquivo, sizeof(nomeArquivo), "Matriz-%d", id);
  if ((pArq = fopen(nomeArquivo, "w")) == 0x0) {
    printf("erro.");
    exit(1);
  }

  for (i = iInicial; i <= iFinal; i++) {
    fprintf(pArq, "LINHA %d->\t", i);
    for (j = 0; j <= 400; j++)
      fprintf(pArq, "%.2f\t", matriz[i][j]);

    fprintf(pArq, "\n");
  }

  if (fclose(pArq)) {
    printf("erro.");
    exit(1);
  }
}