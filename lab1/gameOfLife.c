#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>
#include <sys/time.h>
#include <omp.h>

#define MAX 1000
#define NUMCORES 8

//gameBoard matrix - corresponding value as 0: dead and 1: alive
int gameBoard[(MAX + 2) * (MAX + 2)];   //2 for ghost rows and ghost cols

//gameBoardNext - to store the next state of the elements
int gameBoardNext[(MAX + 2) * (MAX + 2)];   //2 for ghost rows and ghost cols

/**
 * @brief
 * initialize the cell values randomly: sequential method
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
 * initialize the cell values randomly: Parallel method
*/
void initializeGameParallel(){
    srand(time(NULL));
    //doesn't fill the ghost rows or cols

    #pragma omp parallel
    {
        #pragma omp for    //distributing the rows for parallel computation
        for(int row=1; row< (MAX + 2) - 1; row++){
            for(int col=1;col<(MAX + 2) - 1; col++){
                gameBoard[row * (MAX + 2) + col] = rand() % 2;
            }
        }

        //setting the ghost row val
        #pragma omp for nowait  //no wait since we don't want this block to be blocking the thread execution
        for(int row=1; row< (MAX + 2) - 1; row++){
            //left ghost column
            gameBoard[row * (MAX + 2) + 0] = gameBoard[row * (MAX + 2) + MAX];
            //right ghost column
            gameBoard[row * (MAX + 2) + (MAX + 2) - 1] = gameBoard[row * (MAX + 2) + 1];  
        }
        //setting the ghost col val
        #pragma omp for nowait  //no wait, no dependency issue since we are just copying the data in the memory
        for(int col=1;col<(MAX + 2) - 1; col++){
            //top ghost row
            gameBoard[0 * (MAX + 2) + col] = gameBoard[MAX * (MAX + 2) + col];
            //bottom ghost row
            gameBoard[((MAX + 2) - 1) * (MAX + 2) + col] = gameBoard[1 * (MAX + 2) + col];
        }
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
void printGameBoard(int * gameBoardPrint){
    for(int row = 0; row < MAX + 2; row++){
        for(int col = 0; col < MAX + 2; col++){
            printf(" %d ", gameBoardPrint[row * (MAX + 2) + col]);
        }
        printf("\n");
    }
    printf("\n\n");
}

/**
 * @brief
 * Game Execution: sequential
 * 
 * If a cell has fewer than 2 ALIVE neighbors, it will be DEAD in the next time step.
 * If an ALIVE cell has 2 or 3 ALIVE neighbors, it will be ALIVE in the next time step.
 * If a cell has more than 3 ALIVE neighbors, it will be DEAD in the next time step.
 * If a DEAD cell has 3 ALIVE neighbors, it will be ALIVE in the next time step.
 * 
*/
void executeGame(){
    //taking real rows into the consideration
    int neighbourCount = 0;
    for(int row = 1; row < MAX + 1; row++){
        for(int col = 1; col < MAX + 1; col++){
            neighbourCount = gameBoard[(row - 1) * (MAX + 2) + col] == 1 ? neighbourCount + 1 : neighbourCount; //top element
            neighbourCount = gameBoard[(row + 1) * (MAX + 2) + col] == 1 ? neighbourCount + 1 : neighbourCount; //lower element 
            neighbourCount = gameBoard[(row) * (MAX + 2) + (col - 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //left element
            neighbourCount = gameBoard[(row) * (MAX + 2) + (col + 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //right element

            neighbourCount = gameBoard[(row - 1) * (MAX + 2) + (col - 1)] == 1 ? neighbourCount + 1 : neighbourCount; //left-top element
            neighbourCount = gameBoard[(row - 1) * (MAX + 2) + (col + 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //right-top element
            neighbourCount = gameBoard[(row + 1) * (MAX + 2) + (col - 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //bottom-left element
            neighbourCount = gameBoard[(row + 1) * (MAX + 2) + (col + 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //bottom-right element

            if(gameBoard[row * (MAX + 2) + col] == 1){  //if alive
                if(neighbourCount < 2){
                    gameBoardNext[row * (MAX + 2) + col] = 0;  //dead in the next state
                }
                if(neighbourCount == 2 || neighbourCount == 3){ 
                    gameBoardNext[row * (MAX + 2) + col] = 1; //alive in the next state
                }
                if(neighbourCount > 3){
                    gameBoardNext[row * (MAX + 2) + col] = 0;   //dead in next state
                }
            }
            else{   //if dead
                if(neighbourCount == 3){
                    gameBoardNext[row * (MAX + 2) + col] = 1;
                }
            }
            neighbourCount = 0;
        }
    }

    //setting the ghost row val
    for(int row=1; row< (MAX + 2) - 1; row++){
        //left ghost column
        gameBoardNext[row * (MAX + 2) + 0] = gameBoardNext[row * (MAX + 2) + MAX];
        //right ghost column
        gameBoardNext[row * (MAX + 2) + (MAX + 2) - 1] = gameBoardNext[row * (MAX + 2) + 1];  
    }
    //setting the ghost col val
    for(int col=1;col<(MAX + 2) - 1; col++){
        //top ghost row
        gameBoardNext[0 * (MAX + 2) + col] = gameBoardNext[MAX * (MAX + 2) + col];
        //bottom ghost row
        gameBoardNext[((MAX + 2) - 1) * (MAX + 2) + col] = gameBoardNext[1 * (MAX + 2) + col];
    }

    //initializing the corner ghost cell values
    gameBoardNext[0] = gameBoardNext[MAX * (MAX + 2) + (MAX)];   //left top ghost corner
    gameBoardNext[((MAX + 2) - 1) * (MAX + 2)] = gameBoardNext[1 * (MAX + 2) + MAX];    //left bottom ghost corner
    gameBoardNext[((MAX + 2) - 1) * (MAX + 2) + ((MAX + 2) - 1)] = gameBoardNext[1 * (MAX + 2) + 1];    //right bottom ghost corner
    gameBoardNext[0 * (MAX + 2) + ((MAX + 2) - 1)] = gameBoardNext[MAX * (MAX + 2) + 1];    //right top ghost corner
}

/**
 * @brief
 * execute the game in parallel
*/
void executeGameParallel(){
    //taking real rows into the consideration
    #pragma omp parallel
    {
        int neighbourCount = 0; //local variable for each thread
        #pragma omp for    //distributing the rows for parallel computation
        for(int row = 1; row < MAX + 1; row++){
            for(int col = 1; col < MAX + 1; col++){
                neighbourCount = gameBoard[(row - 1) * (MAX + 2) + col] == 1 ? neighbourCount + 1 : neighbourCount; //top element
                neighbourCount = gameBoard[(row + 1) * (MAX + 2) + col] == 1 ? neighbourCount + 1 : neighbourCount; //lower element 
                neighbourCount = gameBoard[(row) * (MAX + 2) + (col - 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //left element
                neighbourCount = gameBoard[(row) * (MAX + 2) + (col + 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //right element

                neighbourCount = gameBoard[(row - 1) * (MAX + 2) + (col - 1)] == 1 ? neighbourCount + 1 : neighbourCount; //left-top element
                neighbourCount = gameBoard[(row - 1) * (MAX + 2) + (col + 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //right-top element
                neighbourCount = gameBoard[(row + 1) * (MAX + 2) + (col - 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //bottom-left element
                neighbourCount = gameBoard[(row + 1) * (MAX + 2) + (col + 1)] == 1 ? neighbourCount + 1 : neighbourCount;   //bottom-right element

                if(gameBoard[row * (MAX + 2) + col] == 1){  //if alive
                    if(neighbourCount < 2){
                        gameBoardNext[row * (MAX + 2) + col] = 0;  //dead in the next state
                    }
                    if(neighbourCount == 2 || neighbourCount == 3){ 
                        gameBoardNext[row * (MAX + 2) + col] = 1; //alive in the next state
                    }
                    if(neighbourCount > 3){
                        gameBoardNext[row * (MAX + 2) + col] = 0;   //dead in next state
                    }
                }
                else{   //if dead
                    if(neighbourCount == 3){
                        gameBoardNext[row * (MAX + 2) + col] = 1;
                    }
                }
                neighbourCount = 0;
            }
        }

        //setting the ghost row val
        #pragma omp for nowait   //distributing the rows for parallel computation, dont block the thread
        for(int row=1; row< (MAX + 2) - 1; row++){
            //left ghost column
            gameBoardNext[row * (MAX + 2) + 0] = gameBoardNext[row * (MAX + 2) + MAX];
            //right ghost column
            gameBoardNext[row * (MAX + 2) + (MAX + 2) - 1] = gameBoardNext[row * (MAX + 2) + 1];  
        }
        //setting the ghost col val
        #pragma omp for nowait   //distributing the cols for parallel computation, dont block the thread
        for(int col=1;col<(MAX + 2) - 1; col++){
            //top ghost row
            gameBoardNext[0 * (MAX + 2) + col] = gameBoardNext[MAX * (MAX + 2) + col];
            //bottom ghost row
            gameBoardNext[((MAX + 2) - 1) * (MAX + 2) + col] = gameBoardNext[1 * (MAX + 2) + col];
        }
    }

    //initializing the corner ghost cell values
    gameBoardNext[0] = gameBoardNext[MAX * (MAX + 2) + (MAX)];   //left top ghost corner
    gameBoardNext[((MAX + 2) - 1) * (MAX + 2)] = gameBoardNext[1 * (MAX + 2) + MAX];    //left bottom ghost corner
    gameBoardNext[((MAX + 2) - 1) * (MAX + 2) + ((MAX + 2) - 1)] = gameBoardNext[1 * (MAX + 2) + 1];    //right bottom ghost corner
    gameBoardNext[0 * (MAX + 2) + ((MAX + 2) - 1)] = gameBoardNext[MAX * (MAX + 2) + 1];    //right top ghost corner
}

/**
 * @brief
 * updating the current state - sequential
*/
void updateState(){
    for(int row=0; row< (MAX + 2); row++){
        for(int col=0;col<(MAX + 2); col++){
           gameBoard[row * (MAX + 2) + col] = gameBoardNext[row * (MAX + 2) + col];
        }
    }
}

/**
 * @brief
 * updating the current state - parallel
*/
void updateStateParallel(){
    #pragma omp parallel for
    for(int row=0; row< (MAX + 2); row++){
        for(int col=0;col<(MAX + 2); col++){
           gameBoard[row * (MAX + 2) + col] = gameBoardNext[row * (MAX + 2) + col];
        }
    }
}

int main(){
    printf("Num threads: %d \n\n", NUMCORES);
    omp_set_num_threads(NUMCORES); //setting the number of threads

    printf("Sequential Execution: \n");

    struct timespec begin, end;
    double elapsed;

    clock_gettime(CLOCK_MONOTONIC, &begin);
    initializeGame();
    for(int i=0; i<100; i++){
        // printGameBoard(gameBoard);
        executeGame();
        updateState();
    }
    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

    printf("Total Elapsed time - sequential: %f \n", elapsed);



    printf("Parallel Execution: \n");
    elapsed = 0;

    clock_gettime(CLOCK_MONOTONIC, &begin);
    initializeGameParallel();
    for(int i=0; i<100; i++){
        // printGameBoard(gameBoard);
        executeGameParallel();
        updateStateParallel();
    }
    // printGameBoard(gameBoard);

    clock_gettime(CLOCK_MONOTONIC, &end);
    elapsed = end.tv_sec - begin.tv_sec;
    elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;

    printf("Total Elapsed time - parallel: %f \n", elapsed);

    return 0;
}