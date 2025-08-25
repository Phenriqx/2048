#ifndef MECHANICS_H 
#define MECHANICS_H

#ifndef MAX 
#define MAX 64
#endif

#include <stdbool.h> // usada para os tipos booleanos

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
    int num4, num5, num6;
    RankingEntry ranking4x4[10];
    RankingEntry ranking5x5[10];
    RankingEntry ranking6x6[10];
} RankingData;

// funções de jogo
void initGame(int n, RankingData *ranking);
void startGame(User *u, int **mat, int **previousState, int size, RankingData *ranking);
int generateRandomPosition(int size);
int generateRandomNumber(int size);
void insertNumber(int **mat, int size);
int isGameWon(User *u, int **mat, int size);
bool noMovesLeft(User *u, int **mat, int size);
bool checkEmptySpaces(int **mat, int size);
bool tradePieces(int **mat, User *u, int size, Position p1, Position p2);
int **undoMovement(int **mat, int size);
User *initUser();

// funções de movimento e auxiliares
bool moveUp(int **mat, int size, User *u);
bool moveDown(int **mat, int size, User *u);
bool moveLeft(int **mat, int size, User *u);
bool moveRight(int **mat, int size, User *u);
void compactUp(int **mat, int columnIdx, int size);
void compactDown(int **mat, int columnIdx, int size);


// funções de arquivos texto
void saveData(int **mat, int **previousState, int size, User *u, const char* filename);
GameInfo* readData(const char *filename);

// funções de ranking
void loadRanking(RankingData *ranking);
void saveRanking(RankingData *ranking);
void updateRanking(RankingData *ranking, User *u, int size);
void printRanking(RankingData *ranking);

#endif