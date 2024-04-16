#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define MAX 5

//gameBoard matrix - corresponding value as 0: dead and 1: alive
int gameBoard[(MAX + 2) * (MAX + 2)];   //2 for ghost rows and ghost cols

//gameBoardNext - to store the next state of the elements
int gameBoardNext[(MAX + 2) * (MAX + 2)];   //2 for ghost rows and ghost cols

/**
 * @brief
 * initialize the cell values randomly
*/
void initializeGame(){
    srand(time(NULL));
    //doesn't fill the ghost rows or cols
    for(int row=1; row< (MAX + 2) - 1; row++){
        for(int col=1;col<(MAX + 2) - 1; col++){
            gameBoard[row * (MAX + 2) + col] = rand() % 2;
        }
    }
    //setting the ghost row val
    for(int row=1; row< (MAX + 2) - 1; row++){
        //left ghost column
        gameBoard[row * (MAX + 2) + 0] = gameBoard[row * (MAX + 2) + MAX];
        //right ghost column
        gameBoard[row * (MAX + 2) + (MAX + 2) - 1] = gameBoard[row * (MAX + 2) + 1];  
    }
    //setting the ghost col val
    for(int col=1;col<(MAX + 2) - 1; col++){
        //top ghost row
        gameBoard[0 * (MAX + 2) + col] = gameBoard[MAX * (MAX + 2) + col];
        //bottom ghost row
        gameBoard[((MAX + 2) - 1) * (MAX + 2) + col] = gameBoard[1 * (MAX + 2) + col];
    }

    //initializing the corner ghost cell values
    gameBoard[0] = gameBoard[MAX * (MAX + 2) + (MAX)];   //left top ghost corner
    gameBoard[((MAX + 2) - 1) * (MAX + 2)] = gameBoard[1 * (MAX + 2) + MAX];    //left bottom ghost corner
    gameBoard[((MAX + 2) - 1) * (MAX + 2) + ((MAX + 2) - 1)] = gameBoard[1 * (MAX + 2) + 1];    //right bottom ghost corner
    gameBoard[0 * (MAX + 2) + ((MAX + 2) - 1)] = gameBoard[MAX * (MAX + 2) + 1];    //right top ghost corner
}

/**
 * @brief
 * prints the state of the cells in all grid
*/
void printGameBoard(){
    for(int row=0; row< (MAX + 2); row++){
        for(int col=0;col<(MAX + 2); col++){
            printf(" %d ", gameBoard[row * (MAX + 2) + col]);
        }
        printf("\n");
    }
}

int main(){
    initializeGame();
    printGameBoard();
    return 0;
}