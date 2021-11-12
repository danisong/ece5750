// gcc -pthread pnqueens.c -pthread -g -lm -o pnqueens
// ./pnqueens n p

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <math.h>
#include <string.h>
#define BILLION 1000000000L

int num_solutions = 0;
int max_score = 0;

typedef struct {
    int N, pid, p;
    char ** best_solution;
    pthread_mutex_t *locks; // locks[0] for num_solutions, locks[1] for best_solution, locks[2] for max_score
} GM;

// Function to check if two queens threaten each other or not
int isSafe(int N, char **mat, int r, int c)
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

void printSolution(int N, char **mat, char **best_solution)
{
    int score = 0;
    int i, j;

    for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++) {
            //printf("%c ", mat[i][j]);
            if(mat[i][j] == 'Q') score += abs(i-j);
        }
        //printf("\n");
    }
    //printf("\n");
    if (score>max_score) 
    {
        max_score = score;
        for (i = 0; i < N; i++){
            for (j = 0; j < N; j++) {
                best_solution[i][j] = mat[i][j];
            }
        }
        //memcpy(best_solution,mat,N*N);
    }
}

void nQueen(int N, char **mat, int r, pthread_mutex_t * locks, char **best_solution)
{
    // if N queens are placed successfully, print the solution
    if (r == N)
    {
        pthread_mutex_lock(&locks[0]);
        num_solutions += 1;
        pthread_mutex_unlock(&locks[0]);
        pthread_mutex_lock(&locks[1]);
        pthread_mutex_lock(&locks[2]);
        printSolution(N,mat,best_solution);
        pthread_mutex_unlock(&locks[1]);
        pthread_mutex_unlock(&locks[2]);
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
            nQueen(N, mat, r + 1, locks, best_solution);
 
            // backtrack and remove the queen from the current square
            mat[r][i] = '-';
        }
    }
}

void * thread_work(void *varg) {
    GM *arg = varg;
    int pid, N, i, p;
    pthread_mutex_t *locks;
    N = arg->N;
    pid = arg->pid;
    locks = arg->locks;
    p = arg->p;

    if (pid < N) {
        char **mat;
        mat = (char **) malloc(N * sizeof(char *));
        for(i = 0; i < N; i++) {
            mat[i] = (char *) malloc(N * sizeof(char));
        }

        char ** best_solution;
        best_solution = arg->best_solution;

        if (N > p) 
        {
            for(i = pid; i < N; i += p)
            {
                if (isSafe(N, mat, 0, i))
                {
                    // place queen on the current square
                    mat[0][i] = 'Q';

                    // recur for the next row
                    nQueen(N, mat, 1, locks, best_solution);

                    // backtrack and remove the queen from the current square
                    mat[0][i] = '-';
                }
            }
        }
        else
        {
            if (isSafe(N, mat, 0, pid))
            {
                // place queen on the current square
                mat[0][pid] = 'Q';

                // recur for the next row
                nQueen(N, mat, 1, locks, best_solution);

                // backtrack and remove the queen from the current square
                mat[0][pid] = '-';
            }
        }
    }
}

int
main(int argc, char **argv) {
    struct timespec start, end;
    int i, j, N, p;
    double time, start_time, end_time;

    if(argc != 3) {
        printf("Usage: pnqueens n p\nAborting...\n");
        exit(0);
    }

    pthread_mutex_t *locks;

    // atoi() converts string input to integer
    N = atoi(argv[1]);
    p = atoi(argv[2]);

    locks = (pthread_mutex_t *) malloc(3 * sizeof(pthread_mutex_t)); 
    pthread_mutex_init(&locks[0], NULL);
    pthread_mutex_init(&locks[1], NULL);
    pthread_mutex_init(&locks[2], NULL);

    char **best_solution;

    best_solution = (char **) malloc(N * sizeof(char *));
    for(i = 0; i < N; i++) {
        best_solution[i] = (char *) malloc(N * sizeof(char));
        for (j = 0; j < N; j++) {
            best_solution[i][j] = '-';
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &start);
    //printf("number of threads created: %d\n", p);
    pthread_t *threads = malloc(p * sizeof(threads));
    for(i = 0; i < p; i++) {
        GM *arg = malloc(sizeof(*arg));
        arg->locks = locks;
        arg->N = N;
        arg->pid = i;
        arg->p = p;
        arg->best_solution = best_solution;
        pthread_create(&threads[i], NULL, thread_work, arg);
    }

    for(i = 0; i < p; i++)
        pthread_join(threads[i], NULL);

    clock_gettime(CLOCK_MONOTONIC, &end);
    for (i = 0; i < 3; i++)
	  pthread_mutex_destroy(&locks[i]);
    
    free(threads);
    
    printf("Number of solutions: %d\n", num_solutions);
    printf("Maximum Profit: %d\n", max_score);

	printf("Solution that maximizes profit:\n");

	for (i = 0; i < N; i++)
    {
        for (j = 0; j < N; j++) {
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