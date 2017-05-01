/* 
 * trans.c - Matrix transpose B = A^T
 *
 * Each transpose function must have a prototype of the form:
 * void trans(int M, int N, int A[N][M], int B[M][N]);
 *
 * A transpose function is evaluated by counting the number of misses
 * on a 1KB direct mapped cache with a block size of 32 bytes.
 */ 
#include <stdio.h>
#include "cachelab.h"

int is_transpose(int M, int N, int A[N][M], int B[M][N]);
void trans_1(int M, int N, int A[N][M], int B[M][N]);
void trans_2(int M, int N, int A[N][M], int B[M][N]);
void trans_3(int M, int N, int A[N][M], int B[M][N]);
void trans_4(int M, int N, int A[N][M], int B[M][N]);
void trans_5(int M, int N, int A[N][M], int B[M][N]);
void trans_6(int M, int N, int A[N][M], int B[M][N]);
void trans_7(int M, int N, int A[N][M], int B[M][N]);

/* 
 * transpose_submit - This is the solution transpose function that you
 *     will be graded on for Part B of the assignment. Do not change
 *     the description string "Transpose submission", as the driver
 *     searches for that string to identify the transpose function to
 *     be graded. 
 */
char transpose_submit_desc[] = "Transpose submission";
void transpose_submit(int M, int N, int A[N][M], int B[M][N])
{	
	if ((N-64)<2 && (64-N)<2) {
		int bigi,bigj,liti,litj,diag=0,temp=0;
		int incr = 4;
		for (bigi = 0; bigi < M; bigi += incr) {
			for (bigj = 0; bigj < N; bigj += incr) {		
				for (liti = bigj; (liti < bigj + incr) && (liti < N); liti++) {
					for (litj = bigi; (litj < bigi + incr) && (litj < M); litj++) {
						if (liti != litj) B[litj][liti] = A[liti][litj];
						else {
							temp = A[liti][litj];
							diag = liti;
						}
					}
					if (bigi == bigj) B[diag][diag] = temp;
				}
	 		}
		}
	} else {
		int bigi,bigj,liti,litj,diag=0,temp=0;
		int incr = 8;
		for (bigi = 0; bigi < M; bigi += incr) {
			for (bigj = 0; bigj < N; bigj += incr) {		
				for (liti = bigj; (liti < bigj + incr) && (liti < N); liti++) {
					for (litj = bigi; (litj < bigi + incr) && (litj < M); litj++) {
						if (liti != litj) B[litj][liti] = A[liti][litj];
						else {
							temp = A[liti][litj];
							diag = liti;
						}
					}
					if (bigi == bigj) B[diag][diag] = temp;
				}
	 		}
		}
	}	
}

/* 
 * You can define additional transpose functions below. We've defined
 * a simple one below to help you get started. 
 */ 

/* 
 * trans - A simple baseline transpose function, not optimized for the cache.
 */
char trans_desc[] = "Simple row-wise scan transpose";
void trans(int M, int N, int A[N][M], int B[M][N])
{
    int i, j, tmp;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; j++) {
            tmp = A[i][j];
            B[j][i] = tmp;
        }
    }    

}

char trans_desc_X[] = "";
void trans_X(int M, int N, int A[N][M], int B[M][N]){}

/*
 * registerFunctions - This function registers your transpose
 *     functions with the driver.  At runtime, the driver will
 *     evaluate each of the registered functions and summarize their
 *     performance. This is a handy way to experiment with different
 *     transpose strategies.
 */
void registerFunctions()
{
	/* Register your solution function */
	registerTransFunction(transpose_submit, transpose_submit_desc); 

	/* Register any additional transpose functions */
}

/* 
 * is_transpose - This helper function checks if B is the transpose of
 *     A. You can check the correctness of your transpose by calling
 *     it before returning from the transpose function.
 */
int is_transpose(int M, int N, int A[N][M], int B[M][N])
{
    int i, j;

    for (i = 0; i < N; i++) {
        for (j = 0; j < M; ++j) {
            if (A[i][j] != B[j][i]) {
                return 0;
            }
        }
    }
    return 1;
}

