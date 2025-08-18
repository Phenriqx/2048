#ifndef MECHANICS_H 
#define MECHANICS_H

#ifndef MAX 
#define MAX 64
#endif

#include <stdbool.h>

typedef struct {
    char nome[MAX];
    int score, trades, undoMoves;
} User;

typedef struct {
    char row;
    int column;
} Position;

typedef struct {
    User user;
    int size;
    int **mat, **previousState;
} GameInfo;

typedef struct {
    char name[MAX];
    int score;
} RankingEntry;

typedef struct {
    RankingEntry ranking4x4[10];
    RankingEntry ranking5x5[10];
    RankingEntry ranking6x6[10];
} Ranking;

void initGame(int n, Ranking *ranking);
void startGame(User *u, int **mat, int size, Ranking *ranking);
int generateRandomPosition(int size);
int generateRandomNumber(int size);
void insertNumber(int **mat, int size);
bool moveUp(int **mat, int size, User *u);
bool moveDown(int **mat, int size, User *u);
bool moveLeft(int **mat, int size, User *u);
bool moveRight(int **mat, int size, User *u);
void compactUp(int **mat, int columnIdx, int size);
void compactDown(int **mat, int columnIdx, int size);
int isGameWon(User *u, int **mat, int size);
bool noMovesLeft(User *u, int **mat, int size);
bool checkEmptySpaces(int **mat, int size);
void tradePieces(int **mat, User *u);
int **undoMovement(int **mat, int size);
void saveData(int **mat, int **previousState, int size, User *u, const char* filename);
GameInfo* readData(const char *filename);
User *initUser();

#endif