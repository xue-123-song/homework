/* 
 * --------------------------------------------------------------------------------------
 * Name: Xue Songtao
 * 
 * LoginId: ics520030910034
 * 
 * --------------------------------------------------------------------------------------
 *
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
    if (M == 32 && N == 32) {
        int i,j,x,y,tmp;
        /* Split into 8*8 blocks */
        for (i = 0;i < M;i += 8) {
            for (j = 0;j < N;j += 8) {
                for (x = 0;x < 8;x++) {
                    for (y = 0;y < 8;y++) {
                        /* Delay operations on the diagonal of each block */
                        if (x == y) {
                            continue;                            
                        }
                        B[j + y][i + x] = A[i + x][j + y];
                    }
                    tmp = A[i + x][j + x];
                    B[j + x][i + x] = tmp;
                }
            }
        }
    }
    else if (M == 64 && N == 64) {
        /*
        * Out put address of A and B
        * FILE *file;
        * file = fopen("/home/ics/Desktop/New File","w");
        * fprintf(file,"%p  %p",&A[0][0],&B[0][0]);
        * fclose(file);
        */
        int i,j,k;
        int v0,v1,v2,v3,v4,v5,v6,v7,tmp;
        /* Split into 8*8 blocks */
        for (i = 0;i < M;i += 8) {
            for (j = 0;j < N;j += 8) {
                for (k = 0; k < 4; k++) {
                    /* Read data from a line */
                    v0 = A[k + i][j];
                    v1 = A[k + i][j + 1];
                    v2 = A[k + i][j + 2];
                    v3 = A[k + i][j + 3];
                    v4 = A[k + i][j + 4];
                    v5 = A[k + i][j + 5];
                    v6 = A[k + i][j + 6];
                    v7 = A[k + i][j + 7];

                    /* Put data in B[][] may be not correct address*/
                    B[j][k + i] = v0;
                    B[j + 1][k + i] = v1;
                    B[j + 2][k + i] = v2;
                    B[j + 3][k + i] = v3;
                    B[j + 0][k + 4 + i] = v4;
                    B[j + 1][k + 4 + i] = v5;
                    B[j + 2][k + 4 + i] = v6;
                    B[j + 3][k + 4 + i] = v7;
                }
                for (k = 0; k < 4; k++) {
                    /* Read remain data in block */
                    v0 = A[i + 4][j + k];
                    v1 = A[i + 5][j + k]; 
                    v2 = A[i + 6][j + k];
                    v3 = A[i + 7][j + k];
                    v4 = A[i + 4][j + k + 4];
                    v5 = A[i + 5][j + k + 4];
                    v6 = A[i + 6][j + k + 4];
                    v7 = A[i + 7][j + k + 4];

                    /* Rearrange data and put data into right postion */
                    tmp = B[j + k][i + 4]; B[j + k][i + 4] = v0;v0 = tmp;
                    tmp = B[j + k][i + 5]; B[j + k][i + 5] = v1; v1 = tmp;
                    tmp = B[j + k][i + 6]; B[j + k][i + 6] = v2; v2 = tmp;
                    tmp = B[j + k][i + 7]; B[j + k][i + 7] = v3; v3 = tmp;

                    B[j + k + 4][i + 0] = v0;B[j + k + 4][i + 4 + 0] = v4;
                    B[j + k + 4][i + 1] = v1;B[j + k + 4][i + 4 + 1] = v5;
                    B[j + k + 4][i + 2] = v2;B[j + k + 4][i + 4 + 2] = v6;
                    B[j + k + 4][i + 3] = v3;B[j + k + 4][i + 4 + 3] = v7;
                }
            }
        }
    }
    else if (M == 61 && N ==67) {
		int i,j,v0,v1,v2,v3,v4,v5,v6,v7;
		int n = N / 8 * 8;
		int m = M / 8 * 8;
        /* Split into 8*8 blocks */
		for (j = 0; j < m; j += 8) {
			for (i = 0; i < n; ++i) {
				v0 = A[i][j];
				v1 = A[i][j + 1];
				v2 = A[i][j + 2];
				v3 = A[i][j + 3];
				v4 = A[i][j + 4];
				v5 = A[i][j + 5];
				v6 = A[i][j + 6];
				v7 = A[i][j + 7];
				
				B[j][i] = v0;
				B[j + 1][i] = v1;
				B[j + 2][i] = v2;
				B[j + 3][i] = v3;
				B[j + 4][i] = v4;
				B[j + 5][i] = v5;
				B[j + 6][i] = v6;
				B[j + 7][i] = v7;
			}
        }

        /* Deal with remaining data */
		for (i = n; i < N; ++i) {
			for (j = m; j < M; ++j) {
				v0 = A[i][j];
				B[j][i] = v0;
			}
        }

		for (i = 0; i < N; ++i) {
			for (j = m; j < M; ++j) {
				v0 = A[i][j];
				B[j][i] = v0;
			}
        }

		for (i = n; i < N; ++i) {
			for (j = 0; j < M; ++j) {
				v0 = A[i][j];
				B[j][i] = v0;
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
    registerTransFunction(trans, trans_desc); 

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

