#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

#include "mechanics.h"
#include "utils.h"

void initGame(int n) {
   if (n < 4 || n > 6) {
      printf("Tamanho do tabuleiro não pode ser menor que 4 nem maior que 6.\n");
      exit(EXIT_FAILURE);
   }

   User u = initUser();
   int **mat;
   mat = calloc(n, sizeof(int*)); //malloc realloc e calloc
   if (mat == NULL)
      exit(EXIT_FAILURE);

   for (int i = 0; i < n; i++) {
      mat[i] = calloc(n, sizeof(int));
      if (mat[i] == NULL)
         exit(EXIT_FAILURE);
   }

   insertNumber(mat, n);
   insertNumber(mat, n);
   printBoard(mat, n);

   startGame(u, mat, n);

   freeMatrix(mat, n);
}

void freeMatrix(int **mat, int n) {
   for (int i = 0; i < n; i++)
      free(mat[i]);
   free(mat);
}

int generateRandomPosition(int size) {
   return (rand() % size);
}

int generateRandomNumber(int size) {
   int threshold;
   if (size == 4)
      threshold = 90;
   else if (size == 5)
      threshold = 85;
   else
      threshold = 80;

   int seed = (rand() % 100) + 1;
   if (seed <= threshold)
      return 2;
   else
      return 4;
}

void insertNumber(int **mat, int size) {
   int pos1, pos2, num;
   do {
      pos1 = generateRandomPosition(size);
      pos2 = generateRandomPosition(size);
   } while (mat[pos1][pos2] != 0);
   num = generateRandomNumber(size);

   mat[pos1][pos2] = num;
}

User initUser()
{
   User u;
   char str[MAX];
   getchar();
   printf("Digite o seu nome: ");
   fgets(str, MAX, stdin);

   str[strcspn(str, "\n")]  = '\0';
   strcpy(u.nome, str);

   u.score = 0;
   u.trades = 0;
   u.undoMoves = 0;

   return u;
}

void startGame(User u, int **mat, int size) {
   char move;
   int valid;

   printf("\nBem-vindo %s! Vamos começar o jogo.\n\n", u.nome);
   printBoard(mat, size);
   do {
      printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar (V)>)\n");
      scanf(" %c", &move);
      switch (tolower(move)) {
         case 'w':
            valid = moveUp(mat, size);
            if (!valid)
               printf("Movimento inválido!\n");
               break;
            break;
         case 'a':
            valid = moveUp(mat, size);
            if (!valid)
               printf("Movimento inválido!\n");
            break;
         case 's':
            valid = moveUp(mat, size);
            if (!valid)
               printf("Movimento inválido!\n");
            break;
         case 'd':
            valid = moveUp(mat, size);
            if (!valid)
               printf("Movimento inválido!\n");
            break;
         case 'u':
            if (u.undoMoves <= 0) {
               printf("Você tem %d movimentos de refazer jogadas. Atinja 256 pontos para conseguir 1 movimento.\n", u.undoMoves);
               break;
            }
            // undoMove();
         case 't':
            if (u.trades <= 0) {
               printf("Você tem %d movimentos de troca. Atinja 512 para conseguir 1 movimento.\n", u.trades);
               break;
            }
            // tradePiece();
         case 'v':
            printf("Voltando ao menu principal e salvando jogo.\n");
            // saveCurrentGame();
            // printMainMenu();
         default:
            printf("Erro! Comando inválido.\n");
            break;
      }
   } while (!isGameWon(u, mat, size) || noMovesLeft(u, mat));
}

int moveUp(int **mat, int size) {
   int emptySpaces = 0;
   
   return 0;
}
int moveDown(int **mat, int size) {
   printf("down");
   return 0;
}
int moveLeft(int **mat, int size) {
   printf("left");
   return 0;
}
int moveRight(int **mat, int size) {
   printf("right");
   return 0;
}
int isGameWon(User u, int **mat, int size) {
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         if (mat[i][j] == 2048) {
            printf("Parabéns! Você ganhou.\n");
            return 0;
         }
   return 1;
}
int noMovesLeft(User u, int **mat) {
   return 0;
}