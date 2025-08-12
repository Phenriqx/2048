#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "mechanics.h"
#include "utils.h"

void initGame(int n) {
   if (n < 4 || n > 6) {
      printf("Tamanho do tabuleiro não pode ser menor que 4 nem maior que 6.\n");
      exit(EXIT_FAILURE);
   }

   User *u = initUser();
   int **mat;
   mat = calloc(n, sizeof(int*)); // calloc inicializa a matriz com valores nulos.
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
   } while (mat[pos1][pos2] != 0); // Garante que dois números não serão inseridos na mesma posição.
   num = generateRandomNumber(size);

   mat[pos1][pos2] = num;
}

User* initUser()
{
   User *u;
   char str[MAX];
   getchar();
   printf("Digite o seu nome: ");
   fgets(str, MAX, stdin);

   str[strcspn(str, "\n")]  = '\0';
   strcpy(u->nome, str);

   u->score = 0;
   u->trades = 0;
   u->undoMoves = 0;

   return u;
}

void startGame(User *u, int **mat, int size) {
   char move;
   bool valid;

   clearTerminal();
   printBoard(mat, size);
   do {
      printf("Placar: %d\n", u->score);
      if (u->undoMoves != 0)
         printf("Você tem %d chance(s) de desfazer o seu último movimento\n", u->undoMoves);
      else if (u->trades != 0) 
         printf("Você tem %d chance(s) de trocar duas peças de posição\n", u->trades);

      printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar (V)>)\n");

      scanf(" %c", &move);
      switch (tolower(move)) {
         case 'w':
            valid = moveUp(mat, size, u);
            if (!valid)
               printf("Movimento inválido!\n");

            printBoard(mat, size);
            startGame(u, mat, size);
            break;
         case 'a':
            valid = moveLeft(mat, size, u);
            if (!valid)
               printf("Movimento inválido!\n");

            printBoard(mat, size);
            startGame(u, mat, size);
            break;
         case 's':
            valid = moveDown(mat, size, u);
            if (!valid)
               printf("Movimento inválido!\n");

            printBoard(mat, size);
            startGame(u, mat, size);
            break;
         case 'd':
            valid = moveRight(mat, size, u);
            if (!valid)
               printf("Movimento inválido!\n");

            printBoard(mat, size);
            startGame(u, mat, size);
            break;
         case 'u':
            if (u->undoMoves <= 0) {
               printf("Você tem %d movimentos de refazer jogadas. Atinja 256 pontos para conseguir 1 movimento.\n", u->undoMoves);
               break;
            }
            // undoMove();
         case 't':
            if (u->trades <= 0) {
               printf("Você tem %d movimentos de troca. Atinja 512 para conseguir 1 movimento.\n", u->trades);
               break;
            }

            tradePieces(mat, size);
            printBoard(mat, size);
            startGame(u, mat, size);
            u->trades--;

            break;
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

bool moveUp(int **mat, int size, User *u) {
   clearTerminal();
   bool merged = false;

   for (int j = 0; j < size; j++) {
      compactUp(mat, j, size);
      for (int i = 0; i < size - 1; i++) {
         if (mat[i][j] != 0 && mat[i + 1][j] == mat[i][j]) {
            mat[i][j] *= 2;
            mat[i + 1][j] = 0;
            u->score += mat[i][j];

            if (u->score == 256) 
               u->undoMoves++;
            else if (u->score == 8) 
               u->trades++;

            merged = true;
         }
      }
      compactUp(mat, j, size);
   }

   insertNumber(mat, size);
   printBoard(mat, size);

   return merged;
}

bool moveDown(int **mat, int size, User *u) {
   clearTerminal();
   bool merged = false;

   for (int j = 0; j < size; j++) {
      compactDown(mat, j, size);
      for (int i = size - 1; i > 0; i--) {
         if (mat[i][j] != 0 && mat[i - 1][j] == mat[i][j]) {
            mat[i][j] *= 2;
            mat[i - 1][j] = 0;
            u->score += mat[i][j];

            if (u->score == 256) 
               u->undoMoves++;
            else if (u->score == 8) 
               u->trades++;

            merged = true;
         }
      }
      compactDown(mat, j, size);
   }

   insertNumber(mat, size);

   return merged;
}

bool moveLeft(int **mat, int size, User *u) {
   clearTerminal();
   bool merged = false;

   int **aux;
   aux = malloc(size * sizeof(int *));
   for (int k = 0; k < size; k++) {
      aux[k] = malloc(size * sizeof(int));
      if (aux[k] == NULL)
         exit(EXIT_FAILURE);
   }

   // Rotaciona a matriz para a esquerda para poder reutilizar a função moveDown();
   for (int i = 0; i < size; i++) 
      for (int j = 0; j < size; j++) 
         aux[size - 1 - j][i] = mat[i][j]; 

   merged = moveDown(aux, size, u);

   // Rotaciona a matriz de volta para a original com os valores já somados pela função moveDown().
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++) 
         mat[j][size - 1 - i] = aux[i][j];

   freeMatrix(aux, size);
   return merged;
}

bool moveRight(int **mat, int size, User *u) {
   clearTerminal();
   bool merged = false;

   int **aux;
   aux = malloc(size * sizeof(int*));
   for (int k = 0; k < size; k++) {
      aux[k] = malloc(size * sizeof(int));
      if (aux[k] == NULL)
         exit(EXIT_FAILURE);
   }

   // Rotaciona a matriz para a direita e utiliza a função moveDown();
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++) 
         aux[i][j] = mat[size - 1 - j][i];

   merged = moveDown(aux, size, u);

   // Rotaciona de volta para o original com os valores somados pela função moveDown;
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++) 
         mat[j][i] = aux[i][size - 1 - j];

   freeMatrix(aux, size);
   return merged;
}

int isGameWon(User *u, int **mat, int size) {
   char opt;
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         if (mat[i][j] == 2048) {
            printf("Parabéns! Você ganhou.\n");
            printf("Você deseja continuar? (S / N)");
            scanf(" %c", &opt);
            switch (toupper(opt)) {
               case 'S':
                  printf("Continuando o jogo.\n");
                  clearTerminal();
                  return 1;
               case 'N':
                  printf("Saindo do jogo.\n");
                  clearTerminal();
                  break;
               default:
                  printf("Opção inválido\n");
                  break;
            }
            return 0;
         }
   return 1;
}

int noMovesLeft(User *u, int **mat) {
   return 0;
}

void compactUp(int **mat, int columnIdx, int size) {
   int k = 0;
   for (int i = 0; i < size; i++) 
      if (mat[i][columnIdx] != 0) {
         mat[k][columnIdx] = mat[i][columnIdx];
         if (i != k)
            mat[i][columnIdx] = 0;   
         k++;
      }
}

void compactDown(int **mat, int columnIdx, int size) {
   int k = size - 1;
   for (int i = size - 1; i >= 0; i--) 
      if (mat[i][columnIdx] != 0) {
         mat[k][columnIdx] = mat[i][columnIdx];
         if (i != k)
            mat[i][columnIdx] = 0;
         k--;
      }
}

void tradePieces(int **mat, int size) {
   Position p1, p2;

   printf("Informe a posição da peça 1 (x y (0 - %d)): ", size - 1);
   scanf("%d %d", &p1.x, &p1.y);
   printf("Informe a posição da peça 2 (x y (0 - %d)): ", size - 1);
   scanf("%d %d", &p2.x, &p2.y);

   int temp = mat[p1.y][p1.x];
   mat[p1.y][p1.x] = mat[p2.y][p2.x];
   mat[p2.y][p2.x] = temp;
}

// void loadGame(char *name, char *mode, int size) {
//    int **lastMove;
//    User u;
//    FILE *file = fopen(name, mode);

//    fscanf(file, "%s %d %d %d", u.nome, &u.score, &u.trades, &u.undoMoves);

//    for (int i = 0; i < size; i++) {
//       for (int j = 0; j < size; j++) {
//          fscanf("%d", lastMove[i][j]);
//       }
//    }

//    fclose(file);
// }