#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "mechanics.h"
#include "utils.h"

void initGame(int n) {
   if (n < 4 || n > 6) {
      printf("Tamanho do tabuleiro não pode ser menor que 4 nem maior que 6.\n");
      exit(EXIT_FAILURE);
   }

   int **mat;
   mat = calloc(n, sizeof(int*));
   if (mat == NULL)
      exit(EXIT_FAILURE);

   for (int i = 0; i < n; i++) {
      mat[i] = calloc(n, sizeof(int));
      if (mat[i] == NULL)
         exit(EXIT_FAILURE);
   }

   mat[generateRandomPosition(n)][generateRandomPosition(n)] = generateRandomNumber(n);

   freeMatrix(mat, n);
}

void freeMatrix(int **mat, int n) {
   for (int i = 0; i < n; i++)
      free(mat[i]);
   free(mat);
}

int generateRandomPosition(int size) {
   srand(time(NULL));
   return (int)(rand() % size);
}

int generateRandomNumber(int size) {
   srand(time(NULL));
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