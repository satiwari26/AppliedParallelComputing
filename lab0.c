#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>



#define CORE 8
#define MAX 1000
#define min(a, b) ((a) < (b) ? (a) : (b))

//2D array for matrix multiplication
float AMat[MAX][MAX];
float BMat[MAX][MAX];
float CMat[MAX][MAX];

//1D array for matrix operations(operation will be performed with off-setting)
float AMat1[MAX * MAX];
float BMat1[MAX * MAX];
float CMat1[MAX * MAX];

//to keep track of the time elapsed
struct timespec begin, end;
double elapsed;

pthread_t thread[CORE];

/**
 * @brief
 * gen the matrix with random floating values
*/
void fillMatrix() {
   srand(time(NULL));

   for (int i = 0; i < MAX; i++) {
      for (int j = 0; j < MAX; j++) {
         AMat[i][j] = (float)rand() / RAND_MAX * 1000.0;
      }
   }

   for (int i = 0; i < MAX; i++) {
      for (int j = 0; j < MAX; j++) {
         BMat[i][j] = (float)rand() / RAND_MAX * 1000.0;
      }
   }

   for (int i = 0; i < MAX; i++) {
      for (int j = 0; j < MAX; j++) {
         CMat[i][j] = 0;
      }
   }

   //setting the oneD array elements:

   for(int row=0; row< MAX; row++){
      for(int col=0;col<MAX; col++){
         AMat1[row * MAX + col] = AMat[row][col];
      }
   }

   for(int row=0; row< MAX; row++){
      for(int col=0;col<MAX; col++){
         BMat1[row * MAX + col] = BMat[row][col];
      }
   }

   for(int row=0; row< MAX; row++){
      for(int col=0;col<MAX; col++){
         CMat1[row * MAX + col] = 0;
      }
   }
}

/**
 * Matrix addition method - multithreads
 * 
 * @param arg the current core index
*/
void* addMatricesParallel(void* arg) 
{
   int core = (int) arg;

   for (int i = core * MAX / CORE; i < (core + 1) * MAX / CORE; i++) 
   {
      for (int j = 0; j < MAX; j++) 
      {
         CMat[i][j] = AMat[i][j] + BMat[i][j];
      }
   }

   return NULL;
}

/**
 * @brief
 * clear the data of matrix C
*/
void clearResultMat(){
   for(int i=0;i<MAX;i++){
      for(int j=0;j<MAX;j++){
         CMat[i][j] = 0;
      }
   }

   for(int i=0;i<MAX;i++){
      for(int j=0;j<MAX;j++){
         CMat1[i * MAX + j] = 0;
      }
   }
}

/**
 * Matrix addition method - singleThread
 * 
*/
void addMatricesSequential() 
{
   clock_gettime(CLOCK_MONOTONIC, &begin);
   for (int i = 0; i < MAX; i++) 
   {
      for (int j = 0; j < MAX; j++) 
      {
         CMat[i][j] = AMat[i][j] + BMat[i][j];
      }
   }
   clock_gettime(CLOCK_MONOTONIC, &end);
   elapsed = end.tv_sec - begin.tv_sec;
   elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}

/**
 * @brief
 * performs the sequential matrix mult(2D array)
*/
void sequentialMultiplication(){
   clock_gettime(CLOCK_MONOTONIC, &begin);

   for(int i=0; i<MAX; i++){
      for(int j=0;j<MAX;j++){
         CMat[i][j] = 0;
         for(int k=0; k<MAX; k++){
            CMat[i][j] += AMat[i][k] * BMat[k][j];
         }
      }
   }

   clock_gettime(CLOCK_MONOTONIC, &end);
   elapsed = end.tv_sec - begin.tv_sec;
   elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}

/**
 * @brief
 * sequential multiplication(1D array with offsetting)
*/
void sequentialMultOneD(){
   //2D array is array of pointers that point to another allocated memory.
   //the memory allocated in 2D array is not sequential where as 1D array 
   //sequentially allocates the memory. This makes it easier for the cache to prefetch the next 
   //sequential data in 1D array and use it to give the better performance.
   clock_gettime(CLOCK_MONOTONIC, &begin);

   for(int row = 0; row < MAX; row++){
      for(int col = 0; col < MAX; col++){
         CMat1[row * MAX + col] = 0;
         for(int k=0; k < MAX; k++){
            CMat1[row * MAX + col] += AMat1[row * MAX + k] * BMat1[k * MAX + col];
         }
      }
   }

   clock_gettime(CLOCK_MONOTONIC, &end);
   elapsed = end.tv_sec - begin.tv_sec;
   elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}


/**
 * @brief
 * performs the parallel matrix mult on 2D array.
*/
void* parallelMatMult1D(void* arg) 
{
   int core = (int) arg;

   // split multiplication based on the row of 1'st matrix
   for (int row = core * MAX / CORE; row < (core + 1) * MAX / CORE; row++) 
   {
      for(int k = 0; k < MAX; k++){
         for (int col = 0; col < MAX; col++)
         {
            CMat1[row * MAX + col] += AMat1[row * MAX + k] * BMat1[k * MAX + col];
         }
      }
   }

   return NULL;
}


/**
 * @brief
 * performs the parallel matrix mult on 2D array.
*/
void* parallelMatMult(void* arg) 
{
   int core = (int) arg;

   // split multiplication based on the row of 1'st matrix
   for (int i = core * MAX / CORE; i < (core + 1) * MAX / CORE; i++) 
   {
      //changed the order of the iterators from i-j-k to i-k-j due to the spacial locality
      //B mat is accessing element more frequently from cache and next sequential data can be
      //fetched faster if loop j nested at the very end.
      for(int k = 0; k < MAX; k++)
      {
         for (int j = 0; j < MAX; j++){
            CMat[i][j] += AMat[i][k] * BMat[k][j];
         }
      }
   }

   return NULL;
}

/**
 * @brief
 * call parallelMatMul on 2D matrix and times the over all execution of the run with
 * average run of 10.
*/
void matMult(){
   clock_gettime(CLOCK_MONOTONIC, &begin);
   for (int i = 0; i < CORE; i++) {
      pthread_create(&thread[i], NULL, &parallelMatMult, (void*)i);
   }
   for (int i = 0; i < CORE; i++) {
      pthread_join(thread[i], NULL);
   }
   clock_gettime(CLOCK_MONOTONIC, &end);
   elapsed = end.tv_sec - begin.tv_sec;
   elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}

/**
 * @brief
 * call parallelMatMul1D on 1D matrix and times the over all execution of the run with
 * average run of 10.
*/
void matMult1D(){
   clock_gettime(CLOCK_MONOTONIC, &begin);
   for (int i = 0; i < CORE; i++) {
      pthread_create(&thread[i], NULL, &parallelMatMult1D, (void*)i);
   }
   for (int i = 0; i < CORE; i++) {
      pthread_join(thread[i], NULL);
   }
   clock_gettime(CLOCK_MONOTONIC, &end);
   elapsed = end.tv_sec - begin.tv_sec;
   elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}

/**
 * @brief
 * prints out the given 2D matrix in the file
*/
void printMat(float Matrix[][MAX], const char* filename) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file,"\n--------------New Matrix -----------------------\n");

    for (int i = 0; i < MAX; i++) {
        fprintf(file, "[ ");
        for (int j = 0; j < MAX; j++) {
            fprintf(file, "%f, ", Matrix[i][j]);
        }
        fprintf(file, "]\n");
    }

    fclose(file);
}

/**
 * @brief
 * prints out the given 1D matrix in the file
*/
void printMat1D(float * Matrix, const char* filename) {
    FILE *file = fopen(filename, "a");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    fprintf(file,"\n--------------New Matrix -----------------------\n");

    for (int i = 0; i < MAX; i++) {
        fprintf(file, "[ ");
        for (int j = 0; j < MAX; j++) {
            fprintf(file, "%f, ", Matrix[i * MAX + j]);
        }
        fprintf(file, "]\n");
    }

    fclose(file);
}

/**
 * @brief
 * creates n number of threads based on the cores number provided and perform addition in parallel
*/
void matAdd(){
   clock_gettime(CLOCK_MONOTONIC, &begin);
   for (int i = 0; i < CORE; i++) {
      pthread_create(&thread[i], NULL, &addMatricesParallel, (void*)i);
   }
   for (int i = 0; i < CORE; i++) {
      pthread_join(thread[i], NULL);
   }
   clock_gettime(CLOCK_MONOTONIC, &end);
   elapsed = end.tv_sec - begin.tv_sec;
   elapsed += (end.tv_nsec - begin.tv_nsec) / 1000000000.0;
}



int main() {

   printf("Number of cores running: %d\n",CORE);

   fillMatrix();
   // printf("\nAddition benchmark with 2D array(no index offsetting)\n");
   //sequential addition
   double avgElapsed = 0;
   // for(int i=0;i<11;i++){
   //    addMatricesSequential();
   //    if(i != 0){
   //       avgElapsed += elapsed;
   //    }
   // }
   double avgVal;
   // avgVal = avgElapsed/10;   //calc avg of 10 runs
   // printMat(AMat, "sequencial_Method.txt");
   // printMat(BMat, "sequencial_Method.txt");
   // printMat(CMat, "sequencial_Method.txt");
   // printf("\n Time elapsed in Sequential addition(avg run): %f \n", avgVal);

   // avgElapsed = 0;
   // //parallel addition
   // for(int i=0; i<11; i++){
   //    matAdd();
   //    if(i != 0){
   //       avgElapsed += elapsed;
   //    }
   // }

   // avgVal = avgElapsed/10; //calc avg of the 10 runs

   // // printMat(AMat, "parallel_Method.txt");
   // // printMat(BMat, "parallel_Method.txt");
   // // printMat(CMat, "parallel_Method.txt");
   // printf("\n Time elapsed in Parallel addition(avg run): %f \n", avgVal);

   avgElapsed = 0;
   clearResultMat(); //clear resulting mat


   // /---------------------/matrix mult benchmark----------------------//

   // ---------------------------------------------- with 2D array sequential--------------------//
   for(int i=0; i<11; i++){
      clearResultMat();
      sequentialMultiplication();
      if(i != 0){
         avgElapsed += elapsed;
      }
   }

   avgVal = avgElapsed/10; //calc avg of the 10 runs

   // printMat(AMat, "sequencial_Method2D.txt");
   // printMat(BMat, "sequencial_Method2D.txt");
   // printMat(CMat, "sequencial_Method2D.txt");
   printf("\n Time elapsed in Sequential multiplication 2D (avg run): %f \n", avgVal);

   
   // ---------------------------------------------- with 1D array sequential--------------------//
   avgElapsed = 0;
   //matrix mult benchmark
   for(int i=0; i<11; i++){
      clearResultMat();
      sequentialMultOneD();
      if(i != 0){
         avgElapsed += elapsed;
      }
   }

   avgVal = avgElapsed/10; //calc avg of the 10 runs

   // printMat1D(AMat1, "sequencial_Method1D.txt");
   // printMat1D(BMat1, "sequencial_Method1D.txt");
   // printMat1D(CMat1, "sequencial_Method1D.txt");
   printf("\n Time elapsed in Sequential multiplication 1D (avg run): %f \n", avgVal);


   // ---------------------------------------------- with 2D array parallel--------------------//
   avgElapsed = 0;
   for(int i=0; i<11; i++){
      clearResultMat(); //clear resulting mat
      matMult();
      if(i != 0){
         avgElapsed += elapsed;
      }
   }

   avgVal = avgElapsed/10; //calc avg of the 10 runs
   // printMat(AMat, "parallel_Method2D.txt");
   // printMat(BMat, "parallel_Method2D.txt");
   // printMat(CMat, "parallel_Method2D.txt");
   printf("\n Time elapsed in parallel multiplication 2D (avg run): %f \n", avgVal);


   // ---------------------------------------------- with 1D array parallel--------------------//
   avgElapsed = 0;
   for(int i=0; i<11; i++){
      clearResultMat(); //clear resulting mat
      matMult1D();
      if(i != 0){
         avgElapsed += elapsed;
      }
   }

   avgVal = avgElapsed/10; //calc avg of the 10 runs
   // printMat1D(AMat1, "parallel_Method1D.txt");
   // printMat1D(BMat1, "parallel_Method1D.txt");
   // printMat1D(CMat1, "parallel_Method1D.txt");
   printf("\n Time elapsed in parallel multiplication 1D (avg run): %f \n", avgVal);

   return 0;
}



