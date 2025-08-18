#ifndef UTILS_H

#define UTILS_H

#include "mechanics.h"

void printMainMenu(Ranking *ranking);
void printBoard(int **mat, int size);
void freeMatrix(int **mat, int n);
void clearTerminal();
void gameInstructions();
void cleanBuffer();
int getMaxDigits(int **mat, int size);
char* getPieceColor(int value);
void toLowerString(char *str);
int** createMatrix(int size);
void deleteFileIfExists(const char* filename);
void loadRanking(Ranking *ranking);
void saveRanking(Ranking *ranking);
void printRanking(Ranking *ranking);
bool askUser();

#endif
