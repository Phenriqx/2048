#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "utils.h"
#include "mechanics.h"

void clearTerminal() {
   system("clear");
}

// Imprime o menu inicial com as opções de escolha para o usuário
void printMainMenu() {
   clearTerminal();
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

   scanf(" %c", &opt);
   
   switch (toupper(opt)) {
      case 'R':
         printf("Você deseja realmente sair? (S/N)");
         getchar();
         scanf("%c", &sure);

         if (tolower(sure) == 's') {
            printf("Saindo do jogo!\n");
            exit(EXIT_FAILURE);
         }

         printMainMenu();
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
         break;
      default:
         printf("Código inválido. Tente novamente\n");
         break; 
   }
}

void printBoard(int **mat, int size) {
   clearTerminal();
   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++) {
         printf("[ %d ] ", mat[i][j]);
      }
      printf("\n");
   }
}

void freeMatrix(int **mat, int n) {
   for (int i = 0; i < n; i++)
      free(mat[i]);
   free(mat);
}