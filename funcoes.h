void escreveMatrizArquivo(float matriz[402][402],
                          int iInicial,
                          int iFinal,
                          int id) {
  FILE* pArq;
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
    for (j = 0; j <= 401; j++) {
      fprintf(pArq, "%.2f\t\t", matriz[i][j]);
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

  char leitor[5000];
  bzero(leitor,sizeof(leitor));

  int i;
  if (flag == 1) {
    fprintf(arquivoFinal, "         \t");
    for (i = 0; i < 402; i++) {
      fprintf(arquivoFinal, "COL %d\t\t", i);
    }
    fprintf(arquivoFinal, "\n");
  }

  while (fgets(leitor, sizeof(leitor), arquivoNo) != NULL) {
    fputs(leitor, arquivoFinal);
  }

  fclose(arquivoNo);
  fclose(arquivoFinal);

  //remove(nomeArquivo);
}
