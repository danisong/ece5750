#include <stdio.h>
#include <string.h>
 
// `N × N` chessboard
#define N 4
 
// Function to check if two queens threaten each other or not
int isSafe(char mat[][N], int r, int c)
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
 
    // return 0 if two queens share the same `/` diagonal
    for (int i = r, j = c; i >= 0 && j < N; i--, j++)
    {
        if (mat[i][j] == 'Q') {
            return 0;
        }
    }
 
    return 1;
}
 
void printSolution(char mat[][N])
{
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++) {
            printf("%c ", mat[i][j]);
        }
        printf("\n");
    }
    printf("\n");
}

 nQueen(mat, 0, max_profit, num_sol);
 
void nQueen(int *mat, int r, int max_profit, int num_sol)
{
    // if `N` queens are placed successfully, print the solution
    if (r == N)
    {
        printSolution(mat);
        return;
    }
 
    // place queen at every square in the current row `r`
    // and recur for each valid movement
    for (int i = 0; i < N; i++)
    {
        // if no two queens threaten each other
        if (isSafe(mat, r, i))
        {
            // place queen on the current square
            mat[r][i] = 'Q';
 
            // recur for the next row
            nQueen(mat, r + 1);
 
            // backtrack and remove the queen from the current square
            mat[r][i] = '-';
        }
    }
}
 
int main()
{
    int *mat, max_profit, num_sol;
    mat = (int *) malloc(N * sizeof(int));

    nQueen(mat, 0, max_profit, num_sol);

    return 0;
}