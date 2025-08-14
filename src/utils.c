#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>

#include "utils.h"
#include "colors.h"
#include "mechanics.h"

void clearTerminal() {
   system("clear");
}

// Imprime o menu inicial com as opções de escolha para o usuário
void printMainMenu() {
   char opt, sure;
   int size;

   printf("\n----- BEM-VINDO AO JOGO 2048 -----\n");
   printf("(R) Sair\n");
   printf("(N) Novo jogo\n");
   printf("\t(4) Jogo padrão 4x4\n");
   printf("\t(5) Jogo 5x5\n");
   printf("\t(6) Jogo 6x6\n");
   printf("(J) Continuar jogo atual\n");
   printf("(C) Carregar um jogo salvo\n");
   printf("(S) Salvar o jogo atual\n");
   printf("(M) Mostrar Ranking\n");
   printf("(A) Ajuda com as instruções de como jogar\n");

   scanf("%c", &opt);
   cleanBuffer();
   int repeatMenu = 0;

   do {
      switch (toupper(opt)) {
         case 'R':
            printf("Você deseja realmente sair? (S/N)");
            scanf("%c", &sure);
            cleanBuffer();

            if (tolower(sure) == 's') {
               printf("Saindo do jogo!\n");
               exit(EXIT_FAILURE);
            }

            printMainMenu();
            repeatMenu = 1;
            break;
         case 'N':
            printf("Vamos começar um novo jogo! Selecione o tamamho do tabuleiro (4-6): ");
            scanf("%d", &size);
            initGame(size);
            break;
         case 'J':
            break;
         case 'C':
            char str[MAX];
            printf("Digite o nome do arquivo para continuar: ");
            scanf("%s", str);
            // loadGame(str, "r", size);
            break;
         case 'S':
            break;
         case 'M':
            break;
         case 'A':
         
            clearTerminal();
            gameInstructions();
            printMainMenu();
            repeatMenu = 1;
            break;
         default:
            printf("Código inválido. Tente novamente\n");
            printMainMenu();
            repeatMenu = 1;
            break;
      }
      clearTerminal();
   } while (repeatMenu);
}

// Procura o maior dígito no tabuleiro e retorna a quantidade de algarismos presente no número
int getMaxDigits(int **mat, int size) {
   int maxVal = 0;
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         if (mat[i][j] > maxVal)
            maxVal = mat[i][j];

   if (maxVal == 0)
      return 1;

   return (int) log10(maxVal) + 1;
}

// Com o valor da função getMaxDigits(), calcula o tamanho necessário para printar a formatação certa da tabela.
void printBoard(int **mat, int size) {
   clearTerminal();
   int maxDigits = getMaxDigits(mat, size);
   int cellWidth = maxDigits + 2;

   printf("     "); 
   for (int j = 0; j < size; j++)
      printf("%*d", cellWidth + 1, j + 1); // O "%-*d" é uma sintaxe para formatação dinâmica. O * indica a largura total que o número ocupa, que nesse caso seria o cellWidth + 1;

   printf("\n");

   // Imprime a borda superior
   printf("   " TAB_TL);
   for (int j = 0; j < size; j++) {
      for (int k = 0; k < cellWidth; k++) 
         printf(TAB_HOR);
      if (j < size - 1)
         printf(TAB_TJ);
   }

   printf(TAB_TR "\n");

   // Imprime as linhas do tabuleiro com a legenda de linhas (letras)
   for (int i = 0; i < size; i++) {
      printf(" %c " TAB_VER, 'A' + i);

      for (int j = 0; j < size; j++) {
         printf(" ");
         char* color = getPieceColor(mat[i][j]);
         if (mat[i][j] == 0) 
            for (int k = 0; k < maxDigits; k++) 
               printf(" ");
         else 
            printf("%s%*d%s", color, maxDigits, mat[i][j], ANSI_RESET);
         
         printf(" " TAB_VER);
      }

      printf("\n");

      // Imprime a borda do meio ou a borda inferior
      if (i < size - 1) {
         printf("   " TAB_ML);
         for (int j = 0; j < size; j++) {
            for (int k = 0; k < cellWidth; k++) 
               printf(TAB_HOR);
            if (j < size - 1) 
               printf(TAB_MJ);
         }

         printf(TAB_MR "\n");
      }
   }

   // Borda inferior
   printf("   " TAB_BL);

   for (int j = 0; j < size; j++) {
      for (int k = 0; k < cellWidth; k++) 
         printf(TAB_HOR);
      if (j < size - 1)
         printf(TAB_BJ);
   }

   printf(TAB_BR "\n");
}

char* getPieceColor(int value) {
   switch (value) {
      case 2:
         return ANSI_BG_COLOR_WHITE ANSI_COLOR_BLACK;
      case 4:
         return ANSI_BG_COLOR_YELLOW ANSI_COLOR_BLACK;
      case 8:
         return ANSI_BG_COLOR_MAGENTA ANSI_COLOR_WHITE;
      case 16:
         return ANSI_BG_COLOR_CYAN ANSI_COLOR_WHITE;
      case 32:
         return ANSI_BG_COLOR_BLUE ANSI_COLOR_WHITE;
      case 64:
         return ANSI_BG_COLOR_GREEN ANSI_COLOR_WHITE;
      case 128:
         return ANSI_BG_COLOR_RED ANSI_COLOR_WHITE;
      case 256:
         return ANSI_BG_COLOR_MAGENTA ANSI_COLOR_WHITE;
      case 512:
         return ANSI_BG_COLOR_YELLOW ANSI_COLOR_BLACK;
      case 1024:
         return ANSI_BG_COLOR_CYAN ANSI_COLOR_WHITE;
      case 2048:
         return ANSI_BG_COLOR_RED ANSI_BOLD ANSI_COLOR_WHITE; 
      default:
         return ANSI_BG_COLOR_BLACK ANSI_COLOR_WHITE;
   }
}

void freeMatrix(int **mat, int n) {
   for (int i = 0; i < n; i++)
      free(mat[i]);
   free(mat);
}

void gameInstructions() {
   printf("\nO jogo começa com duas posições aleatórias do tabuleiro preenchidas.\n");
   printf("A cada jogada, o jogador deve escolher uma direção (para cima, para baixo, para a esquerda ou para a direita).\n");
   printf("Todas as peças se movem o máximo possível nessa direçãa, algumas se movem mais do que outras.\n"
          "Duas peças adjacentes (somente nessa direção) com números iguais se combinam em uma contendo a soma desses números.\n");
   printf("Um movimento é válido quando pelo menos uma peça pode ser movida, inclusive por combinação.\n");
   printf("Uma nova peça é gerada ao final de cada jogada em uma posição vazia escolhida aleatoriamente (se houver).\n");
   printf("Na maioria das vezes, um novo 2 deve ser adicinado, mas ocasionalmente (10%% das vezes), um 4.\n");
   printf("Para vencer, o jogador deve criar uma peça com o número 2048.\n"
          "O jogador perde se não houver movimentos válidos disponíveis\n\n");
}

void cleanBuffer() {
   int ch;
   while ((ch = getchar()) != '\n' && ch != EOF);
}