/*
 *  memset-bench.c --- basic test of memory access speed
 *  Warning: it is not exact!
 *
 *  Uses instruction RDTSCP (supported on all modern x86 processors)
 *
 */

/* max size of buffers */
#define MSIZE (500*1024*1024L)

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//#define rdtsc  __builtin_ia32_rdtsc
unsigned int xxx;
#define rdtsc()  __builtin_ia32_rdtscp(&xxx)

/* buffer */
char *buffer;

void print_time(long long delta, long size)
{
    const double dt = (double) delta;   /* time in clk */
    printf("%10.3f %6.4f\n", size / 1024.0, size / dt);       /* KB, B/clk */
    fflush(stdin);
}

void MEASURE(size_t size)
{
    int k;
    unsigned long long start;
    unsigned long long delta;
    unsigned long long mintime;
    memset(buffer, 0, size);      /* cache fill */
    /* 1 */
    start = rdtsc();
    memset(buffer, 0, size);
    mintime = rdtsc() - start;          /* time */
    /* 2...N */
    for (k = 0; k < 7; k++) {
        start = rdtsc();
        memset(buffer, 0, size);
        delta = rdtsc() - start;        /* time */
        if (delta < mintime)
            mintime = delta;            /* take minimum */
    } /* for */
    print_time(mintime, size);          /* print fastest */
}

/* main test */
int main(int argc, const char *argv[])
{
    size_t n;
    size_t size;
    size_t step;
    size_t end = MSIZE;
    if (argc > 1)
        end = atoi(argv[1]) * 1024 * 1024L;
    if (end > MSIZE)
        end = MSIZE;
    buffer = malloc(MSIZE);
    if(buffer==NULL) {
        fprintf(stderr, "Error: no memory\n");
        return 1;
    }
#if 1
    /* try without this initialization -- interesting results */
    unsigned x=666;
    for (n = 0; n < MSIZE; n++) {
        buffer[n] = x = x * 13 + 111;
    }
#endif
    printf("# data for GNUPLOT\n");
    printf("# PARAMETERS: MSIZE = %zu KB\n",
           (size_t) MSIZE / 1024UL);
    printf
        ("# block size in KB      transfer rate in B/clock\n\n");

    for (size = 128; size < end; size += step) {
        MEASURE(size);
        unsigned long i;
        for (i = 1L << 8; i < size; i <<= 1)
            /* empty */ ;
        step = i >> 5;
    }
    return 0;
}
