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
   mat = calloc(n, sizeof(int *)); // calloc inicializa a matriz com valores nulos.
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

   free(u);
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

User *initUser() {
   User *u;
   u = malloc(sizeof(User));
   if (u == NULL)
      exit(EXIT_FAILURE);

   char str[MAX];
   printf("Digite o seu nome: ");
   fgets(str, MAX, stdin);

   str[strcspn(str, "\n")] = '\0';
   strcpy(u->nome, str);

   u->score = 0;
   u->trades = 0;
   u->undoMoves = 0;

   return u;
}

void startGame(User *u, int **mat, int size) {
   char move[MAX];
   bool moveOccurred;
   bool gameContinues = true;

   while (gameContinues) {
      clearTerminal();
      printBoard(mat, size);
      printf("\n\tPlacar: %d\tTrocas: %d\tDesfazer movimento: %d\n", u->score, u->trades, u->undoMoves);
      printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar>)\n");

      do {
         printf("Movimento: ");
         fgets(move, MAX, stdin);
         toLowerString(move);
         move[strcspn(move, "\n")] = '\0'; 
         moveOccurred = false;

         if (!strcmp(move, "w")) {
            moveOccurred = moveUp(mat, size, u);
         } 
         else if (!strcmp(move, "a")) {
            moveOccurred = moveLeft(mat, size, u);
         } 
         else if (!strcmp(move, "s")) {
            moveOccurred = moveDown(mat, size, u);
         } 
         else if (!strcmp(move, "d")) {
            moveOccurred = moveRight(mat, size, u);
         } 
         else if (!strcmp(move, "u")) {
            if (u->undoMoves > 0) {
               u->undoMoves--;
               moveOccurred = true;
            } 
            else 
               printf("Você tem %d movimentos de refazer jogadas. Atinja 256 pontos para conseguir 1 movimento.\n", u->undoMoves);
            
         } 
         else if (!strcmp(move, "t")) {
            if (u->trades > 0) {
               tradePieces(mat, size);
               u->trades--;
               moveOccurred = true;
            } 
            else 
               printf("Você tem %d movimentos de troca. Atinja 512 para conseguir 1 movimento.\n", u->trades);
         } 
         else if (!strcmp(move, "voltar")) {
            printf("Voltando ao menu principal e salvando jogo.\n");
            printMainMenu();
         }
         else 
            printf("Erro! Comando inválido. Tente novamente.\n");
         
         if (!moveOccurred && strcmp(move, "voltar") != 0) {
            printf("Movimento inválido ou não possível. Tente novamente.\n");
            printf("Pressione Enter para continuar...");
            getchar();
            // Reprint the board after invalid move
            clearTerminal();
            printBoard(mat, size);
            printf("Placar: %d\n", u->score);
            printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar>)\n");
         }
      } while (!moveOccurred);
      
      if (isGameWon(u, mat, size) || noMovesLeft(u, mat)) 
         gameContinues = false; 
   }
   
   // Print final result
   clearTerminal();
   printBoard(mat, size);
   printf("Placar final: %d\n", u->score);

   if (isGameWon(u, mat, size)) {
      printf("Parabéns! Você venceu o jogo!\n");
   } else {
      printf("Fim de jogo! Não há mais movimentos.\n");
   }
}

bool moveUp(int **mat, int size, User *u) {
   bool moved = false;

   // Checa se o movimento é possível
   for (int j = 0; j < size; j++) {
      for (int i = 0; i < size - 1; i++) {
         if (mat[i][j] == 0 && mat[i + 1][j] != 0) {
            moved = true;
            break;
         }
         if (mat[i][j] != 0 && mat[i][j] == mat[i + 1][j]) {
            moved = true;
            break;
         }
      }
      if (moved) 
         break;
   }

   if (!moved) 
      return false;

   // Performa o movimento em si
   for (int j = 0; j < size; j++) {
      compactUp(mat, j, size);
      for (int i = 0; i < size - 1; i++) {
         if (mat[i][j] != 0 && mat[i + 1][j] == mat[i][j]) {
            mat[i][j] *= 2;
            mat[i + 1][j] = 0;
            u->score += mat[i][j];

            if (mat[i][j] == 256)
               u->undoMoves++;
            if (mat[i][j] == 512)
               u->trades++;
         }
      }
      compactUp(mat, j, size);
   }

   insertNumber(mat, size);
   return true;
}

bool moveDown(int **mat, int size, User *u) {
   bool moved = false;

   for (int j = 0; j < size; j++) {
      for (int i = size - 1; i > 0; i--) {
         if (mat[i][j] == 0 && mat[i - 1][j] != 0) {
            moved = true;
            break;
         }
         if (mat[i][j] != 0 && mat[i][j] == mat[i - 1][j]) {
            moved = true;
            break;
         }
      }
      if (moved) 
         break;
   }

   if (!moved) 
      return false;

   // Perform the actual movement
   for (int j = 0; j < size; j++) {
      compactDown(mat, j, size);
      for (int i = size - 1; i > 0; i--) {
         if (mat[i][j] != 0 && mat[i - 1][j] == mat[i][j]) {
            mat[i][j] *= 2;
            mat[i - 1][j] = 0;
            u->score += mat[i][j];

            if (mat[i][j] == 256)
               u->undoMoves++;
            if (mat[i][j] == 512)
               u->trades++;
         }
      }
      compactDown(mat, j, size);
   }

   insertNumber(mat, size);
   return true;
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
   aux = malloc(size * sizeof(int *));
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
            return 1;
         }
   return 0;
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