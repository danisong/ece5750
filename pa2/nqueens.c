// gcc -pthread nqueens.c -g -lm -o nqueens
// ./nqueens n

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define BILLION 1000000000L
 
int num_solutions = 0;
int max_score = 0;
char ** best_solution;

// Function to check if two queens threaten each other or not
int isSafe(int N, char mat[][N], int r, int c)
{
    // return 0 if two queens share the same column
    for (int i = 0; i < r; i++)
    {
        if (mat[i][c] == 'Q') {
            return 0;
        }
    }
 
    // return 0 if two queens share the same `` diagonal
    for (int i = r, j = c; i >= 0 && j >= 0; i--, j--)
    {
        if (mat[i][j] == 'Q') {
            return 0;
        }
    }
 
    // return 0 if two queens share the same / diagonal
    for (int i = r, j = c; i >= 0 && j < N; i--, j++)
    {
        if (mat[i][j] == 'Q') {
            return 0;
        }
    }
 
    return 1;
}
 
void printSolution(int N, char mat[][N], char best_solution[][N])
{
    num_solutions += 1;
    int score = 0;
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++) {
            //printf("%c ", mat[i][j]);
            if(mat[i][j] == 'Q') score += abs(i-j);
        }
        //printf("\n");
    }
    //printf("\n");
    if (score>max_score) 
    {
        max_score = score;
        for (int i=0; i < N; i++)
        {
            for (int j=0; j < N; j++) 
            {
                best_solution[i][j] = mat[i][j];
            }
        }
    }
}
 
void nQueen(int N, char mat[][N], int r, char best_solution[][N])
{
    // if N queens are placed successfully, print the solution
    if (r == N)
    {
        printSolution(N,mat,best_solution);
        return;
    }
 
    // place queen at every square in the current row r
    // and recur for each valid movement
    for (int i = 0; i < N; i++)
    {
        // if no two queens threaten each other
        if (isSafe(N, mat, r, i))
        {
            // place queen on the current square
            mat[r][i] = 'Q';
 
            // recur for the next row
            nQueen(N, mat, r + 1, best_solution);
 
            // backtrack and remove the queen from the current square
            mat[r][i] = '-';
        }
    }
}
 
int main(int argc, char **argv)
{   
    struct timespec start, end;
	double time, start_time, end_time;

	if(argc != 2) {
        printf("Usage: sequential n\nAborting...\n");
        exit(0);
    }
	int N = atoi(argv[1]);

    // mat[][] keeps track of the position of queens in the current configuration
    char mat[N][N];
    char best_solution[N][N];

    // initialize mat[][] by -
    memset(mat, '-', sizeof mat);
    
    clock_gettime(CLOCK_MONOTONIC, &start);
    nQueen(N, mat, 0, best_solution);
    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("Number of solutions: %d\n", num_solutions);
	printf("Solution that maximizes profit:\n");
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++) {
            printf("%c ", best_solution[i][j]);
        }
        printf("\n");
    }
    printf("\n");

	time =
    BILLION *(end.tv_sec - start.tv_sec) +(end.tv_nsec - start.tv_nsec);
    time = time / BILLION;

	start_time = ((BILLION * start.tv_sec) + start.tv_nsec)/BILLION;
	end_time = ((BILLION * end.tv_sec) + end.tv_nsec)/BILLION;

	printf("Initialization Time: %ld nsec\nFinish Time: %ld nsec\nComputation time: %lf seconds\n", BILLION * start.tv_sec + start.tv_nsec, BILLION * end.tv_sec + end.tv_nsec, time);

    return 0;
}