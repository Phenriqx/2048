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
int generateRandomPosition(int size);
int generateRandomNumber(int size);
void startGame(User u, int **mat, int size);
void insertNumber(int **mat, int size);
bool moveUp(int **mat, int size, User u);
bool moveDown(int **mat, int size, User u);
bool moveLeft(int **mat, int size, User u);
bool moveRight(int **mat, int size, User u);
void compactUp(int **mat, int columnIdx, int size);
void compactDown(int **mat, int columnIdx, int size);
int isGameWon(User u, int **mat, int size);
int noMovesLeft(User u, int **mat);
void loadGame(char *name, char *mode, int size);
User initUser();

#endif