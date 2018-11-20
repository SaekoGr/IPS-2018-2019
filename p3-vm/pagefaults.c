/*
 * pagefault.c --- simple test of memory access time
 *
 * use with GCC and NO OPTIMIZATIONS
 * (tested on Linux and FreeBSD)
 *
 * You can see:
 *   - memory access time (with overhead)
 *   - some cache effects
 *   - page faults
 *   - interrupts
 *   - process not running (sleeping/runnable)
 *
 */

#include <stdio.h>
#include <stdlib.h>

#define rdtsc __builtin_ia32_rdtsc

long MAX = 4000;		/* default memory size in words */
long *m;			/* access time array */

/* test */
int main(int argc, char *argv[])
{
    int i;
    long long a, b;
    long long start;
    long long end;

    long dt;
    if (argc > 2) {
	printf("usage: %s number_of_%dB_words \n", argv[0], sizeof(long));
	return 1;
    }
    if (argc > 1)
	MAX = atol(argv[1]);
    if (MAX < 1 || MAX > 10000000) {
	printf("Bad number of words (1..10000000 is allowed)\n");
	return 1;
    }
    m = (long *) malloc(0x40000 + MAX * sizeof(long));
    if (m == NULL) {
	printf("Not enough memory\n");
	return 1;
    }

    // measure rdtsc overhead
    unsigned long rdtsctime=0;
    unsigned n=0;
    for (i = 0; i < 10000; i++) {
	a = rdtsc();
	b = rdtsc();
	unsigned long x = b - a;
        if(x<150) {
            rdtsctime+=x;
            n++;
        }
    }
    rdtsctime/=n;

    // start benchmark
    start = rdtsc();
    for (i = 0; i < MAX; i++) {
	a = rdtsc();
	dt = m[i];		/* read memory, page fault or interrupt possible */
	b = rdtsc();
	dt = b - a - rdtsctime;
	m[i] = dt;
    }
    end = rdtsc();

    /* print access time table: */
    printf("# Memory access benchmark (only changes printed)\n");
    printf("# address   dt[cycles] \n");
    dt = 0;
    for (i = 0; i < MAX; i++) {
	if (m[i] != dt) /* changes only */
            if(m[i]>16) /* leave out normal access time */
                printf("%p %ld\n", &m[i], m[i]);
	dt = m[i];
    }
    printf("# total cycles: %Ld \n", (end - start));
    return 0;
}
