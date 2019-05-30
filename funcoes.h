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
    for (j = 0; j <= 401; j++)
      fprintf(pArq, "%.2f\t", matriz[i][j]);

    fprintf(pArq, "\n");
  }

  if (fclose(pArq)) {
    printf("erro.");
    exit(1);
  }
}

int juntaMatrizesArquivo(int *nodes, int NUM_NODES) { 
   //abre todos os arquivos txt com as matrizes processadas por cada nó
   int i;
  FILE *matrizesParciais[NUM_NODES];
   for(i=0; i < NUM_NODES; i++){
      matrizesParciais[i] = fopen("Matriz-%d.txt", "r", nodes[i]); 
   }

			// verificação de erros na leitura dos arquivos
			for(i=0; i < NUM_NODES; i++){
   if (matrizesParciais[i] == NULL) //se deu problema na leitura dos arquivos
   { 
         printf("Erro ao abrir arquivo texto."); 
         return 0;
   } 
  }
  
   // cria arquivo que vai armazenar o resultado
   FILE *matrizResultado = fopen("Matriz-resultado.txt", "w"); 
			if (matrizResultado == NULL) //se deu problema na criacao do arquivo resultado
   { 
         printf("Erro ao criar arquivo Matriz-resultado."); 
         return 0;
   } 
   
		char c;
   // copia conteudo do primeiro arquivo para o Matriz-resultado 
   for(i=0; i < NUM_NODES; i++){
   while ((c = fgetc(matrizesParciais[i])) != EOF)  // enquanto o caracter que tiver sendo lido for diferente do fim do arquivo
      fputc(c, matrizResultado); // coloca ele no arquivo resultado
    }
  
		for(i=0; i < NUM_NODES; i++){
      fclose(matrizesParciais[i]);
   }
  
   return 1; 
} 
