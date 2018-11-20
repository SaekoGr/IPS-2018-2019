
// Test locality of reference

// Multiplication of matrix NxN where N (SIZE) is compile-time constant
// Depends on GSL (GNU Scientific Library, Debian package libgsl0-dev)

// Use gcc -DSIZE=number for setting matrix size
// Compile using -O2 or -O3
//   gcc -O3 -DSIZE=1000 ips-mat-mul-demo.c -lgsl -lcblas -lm
// Needs big stack limit - please set "ulimit -s 100000"

// TODO: use malloc()

//#define PRINT         // or use gcc -DPRINT to print result

#include <stdio.h>              // fprintf()
#include <stdlib.h>             // rand(), atoi()
#include <string.h>             // memset()
#include <time.h>               // clock()
#include <gsl/gsl_blas.h>       // GSL

#ifndef SIZE
#define SIZE 5                  // default
#endif

typedef double mat_t[SIZE][SIZE];

// textbook implementation O(N^3)
void mat_mul1(mat_t m1, mat_t m2, mat_t m3)
{
    int i, j, k;
    memset(m3, 0, sizeof(mat_t));
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            for (k = 0; k < SIZE; ++k) {
                m3[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}

/*

From Wikipedia:

By switching the looping order for j and k, the speedup in large matrix
multiplications becomes dramatic, at least for languages that put contiguous
array elements in the last dimension. This will not change the mathematical
result, but it improves efficiency. In this case, "large" means, approximately,
more than 100,000 elements in each matrix, or enough addressable memory such
that the matrices will not fit in L1 and L2 caches.

for i in 0..n
  for k in 0..p
    for j in 0..m
      C[i][j] = C[i][j] + A[i][k] * B[k][j];

The reason for this speed up is that in the first case, the reads of A[i][k]
are in cache (since the k index is the contiguous, last dimension), but B[k][j]
is not, so there is a cache miss penalty on B[k][j]. C[i][j] is irrelevant,
because it can be factored out of the inner loop. In the second case, the reads
and writes of C[i][j] are both in cache, the reads of B[k][j] are in cache, and
the read of A[i][k] can be factored out of the inner loop. Thus, the second
example has no cache miss penalty in the inner loop while the first example has
a cache penalty.

On a year 2014 processor, the second case is approximately five times faster
that the first case, when written in C and compiled with gcc -O3. (A careful
examination of the disassembled code shows that in the first case, gcc uses
SIMD instructions and in the second case it does not, but the cache penalty is
much worse than the SIMD gain).^[citation needed]

*/

// slightly optimized textbook implementation O(N^3)
void mat_mul2(mat_t m1, mat_t m2, mat_t m3)
{
    int i, j, k;
    memset(m3, 0, sizeof(mat_t));
    for (i = 0; i < SIZE; ++i) {
        for (k = 0; k < SIZE; ++k) {
            for (j = 0; j < SIZE; ++j) {
                m3[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}

void mat_mul3(mat_t m1, mat_t m2, mat_t m3)
{
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            double sum = 0.0;
            int k;
            for (k = 0; k < SIZE; ++k) {
                sum += m1[i][k] * m2[k][j];
            }
            m3[i][j] = sum;
        }
    }
}

void mat_tran(mat_t m1, mat_t m2)
{
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            m2[i][j] = m1[j][i];
        }
    }
}

void mat_mul4(mat_t m1, mat_t m2, mat_t m3)
{
/*
int  gsl_blas_dgemm (CBLAS_TRANSPOSE_t TransA,
                     CBLAS_TRANSPOSE_t TransB,
                     double alpha,
                     const gsl_matrix * A,
                     const gsl_matrix * B,
                     double beta,
                     gsl_matrix * C);
*/
    gsl_matrix_view _m1 = gsl_matrix_view_array((double *) m1, SIZE, SIZE);
    gsl_matrix_view _m2 = gsl_matrix_view_array((double *) m2, SIZE, SIZE);
    gsl_matrix_view _m3 = gsl_matrix_view_array((double *) m3, SIZE, SIZE);
    gsl_blas_dgemm(CblasNoTrans, CblasNoTrans,
                   1, &_m1.matrix,
                      &_m2.matrix,
                   0, &_m3.matrix);
}


void mat_fill_random(mat_t m)
{
    int i, j;
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            m[i][j] = 10.0 * rand() / (RAND_MAX + 1.0);
        }
    }
}

void mat_print(mat_t m)
{
    int i, j;
    printf("{\n");
    for (i = 0; i < SIZE; ++i) {
        for (j = 0; j < SIZE; ++j) {
            printf("%10.6f ", m[i][j]);
        }
        printf("\n");
    }
    printf("}\n");
}


#if defined(PRINT)
#define print(m) mat_print(m)
#else
#define print(m)
#endif

int main(int argc, char *argv[])
{
    int alg = 0;
    if (argc == 2)
        alg = atoi(argv[1]);
    if (alg == 0) {
        fprintf(stderr,
                "Usage: %s N\n"
                "       where N is algorithm number\n", argv[0]);
        return 1;
    }
//    printf("mat-mul-demo SIZE=%d\n",SIZE);
    mat_t m1, m2, m3;
    mat_fill_random(m1);
    print(m1);
    mat_fill_random(m2);
    print(m2);
    clock_t start = clock();
    switch (alg) {
    case 1:
        mat_mul1(m1, m2, m3);
        break;
    case 2:
        mat_mul2(m1, m2, m3);
        break;
    case 3:
        mat_mul3(m1, m2, m3);
        break;
    case 4:
        mat_mul4(m1, m2, m3);
        break;
    default:
        fprintf(stderr, "Error: invalid argument\n");
        break;
    };
    clock_t end = clock();
    print(m3);
    printf("%.2f seconds\n", (double) (end - start) / CLOCKS_PER_SEC);
    return 0;
}
