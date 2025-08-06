#ifndef MECHANICS_H 
#define MECHANICS_H

#ifndef MAX 
#define MAX 64
#endif

typedef struct {
    char nome[MAX];
    int score, trades, undoMoves;
} User;

void initGame(int n);
void freeMatrix(int **mat, int n);
int generateRandomPosition(int size);
int generateRandomNumber(int size);
void startGame(User u, int **mat, int size);
void insertNumber(int **mat, int size);
int moveUp(int **mat, int size);
int moveDown(int **mat, int size);
int moveLeft(int **mat, int size);
int moveRight(int **mat, int size);
int isGameWon(User u, int **mat, int size);
int noMovesLeft(User u, int **mat);
User initUser();

#endif