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

   startGame(u, mat, NULL, n, ranking);

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
void startGame(User *u, int **mat, int **previousState, int size, RankingData *ranking) {
   char move[MAX], command[MAX];
   char pos1[MAX], pos2[MAX];

   bool moveOccurred;
   bool gameContinues = true;
   bool isLastMove = false;

   int **localPreviousState = NULL;
   int previousScore = u->score;
   int previousTradeCount = u->trades;
   int counter = 0;

   RankingEntry entry;

   if (localPreviousState != previousState)
      localPreviousState = previousState;

   clearTerminal();
   printf("Bom jogo %s!\n", u->nome);
   while (gameContinues) {
      printf("\n\n");
      printBoard(mat, size);
      printf(BLUE(BOLD("\n\tPlacar: %d\tTrocas: %d\tDesfazer movimento: %d\n\n")), u->score, u->trades, u->undoMoves);
      printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar>)\n");

      do {
         printf("Movimento: ");
         fgets(move, MAX, stdin);
         toLowerString(move); // Transforma toda a string para lowercase.
         move[strcspn(move, "\n")] = '\0'; // Remove o '\n' da string e coloca um \0 no lugar. 
         moveOccurred = false;

         if (!strcmp(move, "w")) {
            if (previousState != NULL)
               freeMatrix(previousState, size);
            previousState = undoMovement(mat, size);
            /*
            A variável previousState salva a matriz exata do último movimento do usuário. É usada na função undoMovement()
            */
            previousScore = u->score;
            previousTradeCount = u->trades;
            isLastMove = false;

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
            previousTradeCount = u->trades;

            moveOccurred = moveLeft(mat, size, u);
            isLastMove = false;

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
            previousTradeCount = u->trades;

            moveOccurred = moveDown(mat, size, u);
            isLastMove = false;

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
            previousTradeCount = u->trades;

            moveOccurred = moveRight(mat, size, u);
            isLastMove = false;

            if (!moveOccurred) {
               freeMatrix(previousState, size);
               previousState = NULL;
            }
         } 
         else if (!strcmp(move, "u")) {
            if (u->undoMoves > 0 && previousState != NULL && !isLastMove) {
               for (int i = 0; i < size; i++)
                  for (int j = 0; j < size; j++)
                     mat[i][j] = previousState[i][j];

               u->undoMoves--;
               u->score = previousScore;
               u->trades = previousTradeCount;
               freeMatrix(previousState, size);
               previousState = NULL;

               isLastMove = true;
               moveOccurred = true;
            } 
            else if (u->undoMoves == 0)
               printf("\nVocê tem %d movimentos de refazer jogadas. Consiga uma peça de 256 pontos para conseguir 1 movimento.\n", u->undoMoves);
            else if (isLastMove)
               printf("\nVocê não pode fazer dois movimentos de desfazer seguidos!\n");
            
         } 
         else if (sscanf(move, "%s %s %s", command, pos1, pos2) == 3) {
            if (!strcmp(command, "t")) {
               Position p1, p2;

               // Avalia se não há espaços entre os pontos da coordenada, exemplo: A 1 é inválido, mas A1 é válido.
               if (sscanf(pos1, "%c%d", &p1.row, &p1.column) != 2)
                  printf("A primeira coordenada não está no formato correto!\n");
               else if (sscanf(pos2, "%c%d", &p2.row, &p2.column) != 2)
                  printf("A segunda coordenada não está no formato correto!\n");

               else {
                  if (previousState != NULL)
                     freeMatrix(previousState, size);
                  previousState = undoMovement(mat, size);
                  previousScore = u->score;
                  previousTradeCount = u->trades;

                  if (u->trades > 0) {
                     if (tradePieces(mat, u, size, p1, p2))
                        moveOccurred = true;
                  }
                  else
                     printf("Você tem %d movimentos de troca. Consiga uma peça de 512 pontos para conseguir um movimento.\n", u->trades);
                  if (!moveOccurred) {
                     freeMatrix(previousState, size);
                     previousState = NULL;
                  }
               
               }
            }
         } 
         else if (!strcmp(move, "voltar")) {
            printf("Voltando ao menu principal e salvando jogo.\n");
            saveData(mat, previousState, size, u, "temp.txt");

            return;
         }
         
         if (!moveOccurred && strcmp(move, "voltar") != 0) {
            printf("Movimento inválido ou não possível. Tente novamente.\n");
            printf("Pressione Enter para continuar...");
            getchar();

            clearTerminal();
            printBoard(mat, size);
            printf(BLUE(BOLD("\n\tPlacar: %d\tTrocas: %d\tDesfazer movimento: %d\n\n")), u->score, u->trades, u->undoMoves);
            printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar>)\n");
         }
      }
      while (!moveOccurred);
      clearTerminal();

      if (isGameWon(u, mat, size) && counter < 1) {
         clearTerminal();
         printBoard(mat, size);
         gameContinues = askUser(u);
         counter++; // Garante que este input não aparece ao usuário mais de uma vez caso ele vença e opte por continuar jogando;
         if ((gameContinues == false)) {
            printf(GREEN(BOLD("Parabéns! Você ganhou.\n")));
            saveData(mat, previousState, size, u, "temp.txt");
            entry.score = u->score;
            strcpy(entry.name, u->nome);
            updateRanking(ranking, u, size);
            saveRanking(ranking);

            return;
         }
      }
      else if (noMovesLeft(u, mat, size)) {
         printBoard(mat, size);
         if (u->undoMoves > 0) {
            char c;
            printf("Você não tem mais movimentos válidos, mas possui %d movimento(s) de desfazer, deseja usá-lo? (S/N) ", u->undoMoves);
            scanf(" %c", &c);
            cleanBuffer();
            if (tolower(c) == 's') {
               clearTerminal();
               // printBoard(mat, size);
               // printf(BLUE(BOLD("\n\tPlacar: %d\tTrocas: %d\tDesfazer movimento: %d\n\n")), u->score, u->trades, u->undoMoves);
               // printf("Insira seu movimento: (<w>, <a>, <s>, <d>, <u>, <t pos1 pos2>, <voltar>)\n");
            }
            else {
               printf(RED(BOLD("O jogo acabou! O jodador optou por não continuar o jogo.\n")));
               printf("Pressione Enter para continuar...\n");
               getchar();

               gameContinues = false;
            } 
         }
         else {
            printf(RED(BOLD("O jogo acabou. Você não tem mais movimentos válidos ou movimentos de desfazer!\n")));
            printf("Pressione Enter para continuar...\n");
            getchar();

            gameContinues = false;
         }
      }
   }
}

bool askUser(User *u) {
   char opt;
   printf("Parabéns %s!, Você ganhou!\nVocê deseja continuar o jogo? (S/N) ", u->nome);
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
   if (size == 6)
      insertNumber(mat, size);

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
   if (size == 6)
      insertNumber(mat, size);   

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

// Procura por espaços vazios no tabuleiro.
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

// Função que realiza a troca de peças, primeiro avalia se o movimento é possível
bool tradePieces(int **mat, User *u, int size, Position p1, Position p2) {
   // Calcula a linha subtraindo o input com A, logo se pos1 = B1, a linha será B - A = 1;
   int row1 = toupper(p1.row) - 'A', row2 = toupper(p2.row) - 'A';
   int col1 = p1.column - 1, col2 = p2.column - 1;

   if (row1 < 0 || row1 >= size || col1 < 0 || col1 >= size || row2 < 0 || row2 >= size || col2 < 0 || col2 >= size) {
      printf("Posição fora dos limites do tabuleiro.\n");
      return false;
   }

   if (mat[row1][col1] == 0 || mat[row2][col2] == 0) {
      printf("Uma das posições está vazia. Não foi possível realizar o movimento.\n");
      return false;
   }

   int temp = mat[row1][col1];
   mat[row1][col1] = mat[row2][col2];
   mat[row2][col2] = temp;

   u->trades--;
   return true;
}

int** undoMovement(int **mat, int size) {
   int **copy = createMatrix(size);

   for (int i = 0; i < size; i++)
      for (int j = 0; j < size; j++)
         copy[i][j] = mat[i][j];

   return copy;
}

// Lê as informações do arquivo informado pelo usuário
GameInfo* readData(const char* filename) {
   GameInfo *g;
   g = malloc(sizeof(GameInfo));
   if (g == NULL)
      exit(EXIT_FAILURE); // Caso ocorra erro na alocação de memória

   FILE *file = fopen(filename, "r");
   if (file == NULL) {
      return NULL;
   }

   // Lê os dados iniciais do arquivo.
   fscanf(file, "%d %d %d", &g->size, &g->user.undoMoves, &g->user.trades);
   fscanf(file, "%d ", &g->user.score);
   fgets(g->user.nome, MAX, file);

   g->user.nome[strcspn(g->user.nome, "\n")] = '\0'; // Remove o \n da string e coloca um \0 no lugar.

   // Lê a matriz principal do arquivo.
   g->mat = createMatrix(g->size);
   for (int i = 0; i < g->size; i++) {
      for (int j = 0; j < g->size; j++) {
         fscanf(file, "%d", &g->mat[i][j]);
      }
   }

   // Lê a matriz que contém a última jogada.
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

   // Lê o número de entradas por ranking e salva na struct.
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

   // Percorre os arrays de ranking para cada tamanho de joogo e conta o número de entradas válidas para cada um. 
   // O total de jogadores por tamanho de tabuleiro são armazenados nas variáveis abaixo.
   int num4 = 0, num5 = 0, num6 = 0;
   for (int i = 0; i < 10; i++) {
      if (ranking->ranking4x4[i].score > 0)
         num4++;
      if (ranking->ranking5x5[i].score > 0)
         num5++;
      if (ranking->ranking6x6[i].score > 0)
         num6++;
   }

   // Atualiza o número de partidas jogadas para cada tabuleiro na struct.
   ranking->num4 = num4;
   ranking->num5 = num5;
   ranking->num6 = num6;

   // Passa os dados da struct para o arquivo binário.
   fwrite(&ranking->num4, sizeof(int), 1, file);
   fwrite(&ranking->num5, sizeof(int), 1, file);
   fwrite(&ranking->num6, sizeof(int), 1, file);

   // Estes dados são atualizados na função updateRanking(), mas são transferidos para o arquivo nesta função.
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
   if (u->score > currentRank[9].score) { // Avalia primeiro se o jogador tem pontuação suficiente para entrar no top 10.
      for (i = 0; i < 10; i++) {
         if (u->score > currentRank[i].score)
            break; // Acha a colocação correta no ranking dado a sua pontuação.
      }

      for (int k = 9; k > i; k--) {
         currentRank[k] = currentRank[k - 1]; // Move os demais participantes para baixo.
      }

      // Copia o nome e pontuação do usuário atual para a struct na sua devida colocação.
      strcpy(currentRank[i].name, u->nome);
      currentRank[i].score = u->score;
   }
}

// Imprime o ranking na tela.
void printRanking(RankingData *ranking) {
   printf("Ranking:\n\n");
   printf("%d %d %d\n\n", ranking->num4, ranking->num5, ranking->num6);

   if (ranking->num4 > 0) {
      printf("\nRanking 4x4: \n\n");
      for (int i = 0; i < ranking->num4; i++)
         printf("%s %d\n", ranking->ranking4x4[i].name, ranking->ranking4x4[i].score);
   }
   
   if (ranking->num5 > 0) {
      printf("\nRanking 5x5: \n\n");
      for (int j = 0; j < ranking->num5; j++)
         printf("%s %d\n", ranking->ranking5x5[j].name, ranking->ranking5x5[j].score);
   }

   if (ranking->num6 > 0) {
      printf("\nRanking 6x6: \n\n");
      for (int k = 0; k < ranking->num6; k++)
         printf("%s %d\n", ranking->ranking6x6[k].name, ranking->ranking6x6[k].score);
   }
}