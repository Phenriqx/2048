#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

#include "mechanics.h"
#include "utils.h"
#include "colors.h"

// Faz o check do tamanho do tabuleiro e inicializa a matriz inicial.
void initGame(int n, RankingData *ranking) {
   User *u = initUser();
   int **mat;
   mat = calloc(n, sizeof(int *)); // calloc inicializa a matriz com valores nulos.
   if (mat == NULL) {
      free(mat);
      exit(EXIT_FAILURE);
   }

   for (int i = 0; i < n; i++) {
      mat[i] = calloc(n, sizeof(int));
      if (mat[i] == NULL) {
         free(mat[i]);
         exit(EXIT_FAILURE);
      }
   }

   insertNumber(mat, n);
   insertNumber(mat, n);
   printBoard(mat, n);

   startGame(u, mat, n, ranking);

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
   if (u == NULL) {
      free(u);
      exit(EXIT_FAILURE);
   }

   char str[MAX];
   printf("Digite o seu nome: ");
   fgets(str, MAX, stdin);

   str[strcspn(str, "\n")] = '\0'; // Encontra a primeira instância de '\n' e substitui por um '\0' 
   strcpy(u->nome, str);

   u->score = 0;
   u->trades = 0;
   u->undoMoves = 0;

   return u;
}

// pega o input do movimento do usuário e chama a função correta correspondente ao input
void startGame(User *u, int **mat, int size, RankingData *ranking) {
   char move[MAX];
   bool moveOccurred;
   bool gameContinues = true;
   int **previousState = NULL;
   int previousScore = u->score;
   int counter = 0;

   RankingEntry entry;

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
            if (previousState != NULL)
               freeMatrix(previousState, size);
            previousState = undoMovement(mat, size);
            /*
            A variável previousState salva a matriz exata do último movimento do usuário. É usada na função undoMovement()
            */
            previousScore = u->score;

            moveOccurred = moveUp(mat, size, u);

            if (!moveOccurred) {
               freeMatrix(previousState, size);
               previousState = NULL;
            }
         }
         else if (!strcmp(move, "a")) {
            if (previousState != NULL)
               freeMatrix(previousState, size);
            previousState = undoMovement(mat, size);
            previousScore = u->score;

            moveOccurred = moveLeft(mat, size, u);

            if (!moveOccurred) {
               freeMatrix(previousState, size);
               previousState = NULL;
            }
         } 
         else if (!strcmp(move, "s")) {
            if (previousState != NULL)
               freeMatrix(previousState, size);
            previousState = undoMovement(mat, size);
            previousScore = u->score;

            moveOccurred = moveDown(mat, size, u);

            if (!moveOccurred) {
               freeMatrix(previousState, size);
               previousState = NULL;
            }
         } 
         else if (!strcmp(move, "d")) {
            if (previousState != NULL)
               freeMatrix(previousState, size);
            previousState = undoMovement(mat, size);
            previousScore = u->score;

            moveOccurred = moveRight(mat, size, u);

            if (!moveOccurred) {
               freeMatrix(previousState, size);
               previousState = NULL;
            }
         } 
         else if (!strcmp(move, "u")) {
            if (u->undoMoves > 0 && previousState != NULL) {
               for (int i = 0; i < size; i++)
                  for (int j = 0; j < size; j++)
                     mat[i][j] = previousState[i][j];

               u->undoMoves--;
               u->score = previousScore;
               freeMatrix(previousState, size);
               previousState = NULL;

               moveOccurred = true;
            } 
            else 
               printf("Você tem %d movimentos de refazer jogadas. Atinja 256 pontos para conseguir 1 movimento.\n", u->undoMoves);
            
         } 
         else if (!strcmp(move, "t")) {
            if (u->trades > 0) {
               tradePieces(mat, u);
               moveOccurred = true;
            } 
            else 
               printf("Você tem %d movimentos de troca. Atinja 512 para conseguir 1 movimento.\n", u->trades);
         } 
         else if (!strcmp(move, "voltar")) {
            printf("Voltando ao menu principal e salvando jogo.\n");
            saveData(mat, previousState, size, u, "temp.txt");

            return;
         }
         else 
            printf("Erro! Comando inválido. Tente novamente.\n");
         
         if (!moveOccurred && strcmp(move, "voltar") != 0) {
            printf("Movimento inválido ou não possível. Tente novamente.\n");
            printf("Pressione Enter para continuar...\n");
            getchar();

            clearTerminal();
            printBoard(mat, size);
            printf("Placar: %d\n", u->score);
            printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar>)\n");
         }
      }
      while (!moveOccurred);

      if (isGameWon(u, mat, size) && counter < 1) {
         gameContinues = askUser();
         counter++; // Garante que este input não aparece ao usuário mais de uma vez caso ele vença e opte por continuar jogando;
         if ((gameContinues == false)) {
            printf(GREEN(BOLD("Parabéns! Você ganhou.\n")));
            entry.score = u->score;
            strcpy(entry.name, u->nome);
            updateRanking(ranking, u, size);

            exit(EXIT_SUCCESS);
         }
      }
      if (noMovesLeft(u, mat, size)) {
         printBoard(mat, size);
         printf(RED(BOLD("O jogo acabou. Você não tem mais movimentos válidos!\n")));

         printf("Pressione Enter para continuar...\n");
         getchar();

         gameContinues = false;
      }
   }
   
   if (previousState != NULL)
      freeMatrix(previousState, size);
}

bool askUser() {
   char opt;
   printf("Você deseja continuar o jogo? (S/N) ");
   scanf("%c", &opt);

   cleanBuffer();
   if (tolower(opt) == 's') 
      return true;
   else
      return false;
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

   int **aux = createMatrix(size);

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

   int **aux = createMatrix(size);

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
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         if (mat[i][j] == 2048) {
            return 1;
         }
   return 0;
}

bool noMovesLeft(User *u, int **mat, int size) {
   if (checkEmptySpaces(mat, size)) // Se existir um espaço vazio no tabuleiro, então o jogo não acabou, noMovesLeft retorna falso.
      return false;

   // Procura por merges horizontais
   for (int i = 0; i < size; i++) 
      for (int j = 0; j < size - 1; j++) 
         if (mat[i][j] == mat[i][j+1]) 
            return false; 

   // Procura por merges verticais
   for (int j = 0; j < size; j++) 
      for (int i = 0; i < size - 1; i++) 
         if (mat[i][j] == mat[i+1][j]) 
            return false; 
         
   return true; // Se não há nenhum espaço e nenhum merge disponível, então o jogo acabou.
}

bool checkEmptySpaces(int **mat, int size) {
   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         if (mat[i][j] == 0)
            return true;

   return false; // Retorna falso se nenhum espaço no tabuleiro for 0;
}

/*
As funções compactUp e compactDown são essenciais no movimento das peças.
Elas são responsáveis por compactar as colunas do tabuleiro, movendo todos os valores não-nulos para o topo ou para baixo, a depender da função.
Exemplos: {
Coluna: [0, 2, 0, 2]
   compactUp(): [0, 2, 0, 2] -> [2, 2, 0, 0]
   compactDown(): [0, 2, 0, 2] -> [0, 0, 2, 2]
}

*/
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

void tradePieces(int **mat, User *u) {
   Position p1, p2;
   printf("Insira as posições a localização das peças para mudar de posição: ");
   scanf("%c%d %c%d", &p1.row, &p1.column, &p2.row, &p2.column);
   getchar();

   // Calcula a linha subtraindo o input com A, logo se pos1 = B1, a linha será B - A = 1;
   int row1 = toupper(p1.row) - 'A', row2 = toupper(p2.row) - 'A';
   int col1 = p1.column - 1, col2 = p2.column - 1;

   int temp = mat[row1][col1];
   mat[row1][col1] = mat[row2][col2];
   mat[row2][col2] = temp;

   u->trades--;
}

int** undoMovement(int **mat, int size) {
   int **copy = createMatrix(size);

   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         copy[i][j] = mat[i][j];

   return copy;
}

// Lê as informações do arquivo
GameInfo* readData(const char* filename) {
   GameInfo *g;
   g = malloc(sizeof(GameInfo));
   if (g == NULL)
      exit(EXIT_FAILURE);

   FILE *file = fopen(filename, "r");
   if (file == NULL) {
      perror("Erro ao abrir o arquivo.\n");
      exit(EXIT_FAILURE);
   }

   fscanf(file, "%d %d %d", &g->size, &g->user.undoMoves, &g->user.trades);
   fscanf(file, "%d %s", &g->user.score, g->user.nome);

   g->mat = createMatrix(g->size);
   for (int i = 0; i < g->size; i++) {
      for (int j = 0; j < g->size; j++) {
         fscanf(file, "%d", &g->mat[i][j]);
      }
   }

   g->previousState = createMatrix(g->size);
   for (int i = 0; i < g->size; i++) {
      for (int j = 0; j < g->size; j++) {
         fscanf(file, "%d", &g->previousState[i][j]);
      }
   }

   fclose(file);
   return g;
}

// Salva as informações dentro do arquivo especificado pelo usuário
void saveData(int **mat, int **previousState, int size, User *u, const char* filename) {
   FILE *file = fopen(filename, "w");
   if (file == NULL) {
      return;
   }

   fprintf(file, "%d %d %d\n", size, u->undoMoves, u->trades);
   fprintf(file, "%d %s\n", u->score, u->nome);

   for (int i = 0; i < size; i++) {
      for (int j = 0; j < size; j++)
         fprintf(file, "%d ", mat[i][j]);
      fprintf(file, "\n");
   }

   // Se não houver movimento anterior, exemplo: é o primeiro movimento do usuário. Então salva a última jogada como sendo a primeira.
   if (previousState == NULL)
      for (int i = 0; i < size; i++) {
         for (int j = 0; j < size; j++)
            if (mat[i][j] != 0)
               fprintf(file, "%d ", mat[i][j]);
            else
               fprintf(file, "0 ");
         fprintf(file, "\n");
      }
   
    else
      for (int i = 0; i < size; i++) {
         for (int j = 0; j < size; j++)
            if (previousState[i][j] != 0)
               fprintf(file, "%d ", previousState[i][j]);
            else
               fprintf(file, "0 ");
         fprintf(file, "\n");
      }

   fclose(file);
}

void loadRanking(RankingData *ranking) {
   FILE *file = fopen("ranking.dat", "rb");
   if (file == NULL) {
      memset(ranking, 0, sizeof(RankingData));
      return;
   }

   // Lê o número de entradas por ranking.
   fread(&ranking->num4, sizeof(int), 1, file);
   fread(&ranking->num5, sizeof(int), 1, file);
   fread(&ranking->num6, sizeof(int), 1, file);

   fread(&ranking->ranking4x4, sizeof(RankingEntry), ranking->num4, file);
   fread(&ranking->ranking5x5, sizeof(RankingEntry), ranking->num5, file);
   fread(&ranking->ranking6x6, sizeof(RankingEntry), ranking->num6, file);

   fclose(file);
}

void saveRanking(RankingData *ranking) {
   FILE *file = fopen("ranking.dat", "wb");
   if (file == NULL)
      exit(EXIT_FAILURE);

   int num4 = 0, num5 = 0, num6 = 0;
   for (int i = 0; i < 10; i++) {
      if (ranking->ranking4x4[i].score > 0)
         num4++;
      if (ranking->ranking5x5[i].score > 0)
         num5++;
      if (ranking->ranking6x6[i].score > 0)
         num6++;
   }

   ranking->num4 = num4;
   ranking->num5 = num5;
   ranking->num6 = num6;

   fwrite(&ranking->num4, sizeof(int), 1, file);
   fwrite(&ranking->num5, sizeof(int), 1, file);
   fwrite(&ranking->num6, sizeof(int), 1, file);

   fwrite(&ranking->ranking4x4, sizeof(RankingEntry), ranking->num4, file);
   fwrite(&ranking->ranking5x5, sizeof(RankingEntry), ranking->num5, file);
   fwrite(&ranking->ranking6x6, sizeof(RankingEntry), ranking->num6, file);

   fclose(file);
}

void updateRanking(RankingData *ranking, User *u, int size) {
   RankingEntry *currentRank;

   if (size == 4)
      currentRank = ranking->ranking4x4;
   else if (size == 5)
      currentRank = ranking->ranking5x5;
   else
      currentRank = ranking->ranking6x6;

   // Começa do maior (posição 0) e vai até o menor (posição 9)
   int i;
   if (u->score > currentRank[9].score) {
      for (i = 0; i < 10; i++) {
         if (u->score > currentRank[i].score)
            break;
      }

      for (int k = 9; k > i; k--) {
         currentRank[k] = currentRank[k - 1];
      }

      strcpy(currentRank[i].name, u->nome);
      currentRank[i].score = u->score;
   }
}