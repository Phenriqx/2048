#include <stdlib.h>
#include <time.h>

#include "utils.h"
#include "mechanics.h"

int main() {
   srand(time(NULL));
   deleteFileIfExists("temp.txt");
   Ranking *ranking = malloc(sizeof(Ranking));
   //   loadRanking(ranking);
   printMainMenu(ranking);

   //saveRanking(ranking);
   free(ranking);
}