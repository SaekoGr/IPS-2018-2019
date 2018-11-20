/* align-test.c --- test of alignment

 * use with GCC on x86 with RDTSC instruction (Pentium+,K6,...)
 * Author: P. Peringer, 1999-2016, Public domain
 *
 * use optimization: gcc -O2
 *
 * Sample output on Athlon64/2GHz:
 * ==============================
 * 32bit
 * Timing of (un)aligned data access
 * copy 100 items cache->cache
 * item size = sizeof(long) = 4
 *
 * aligned/unaligned time in CPU clocks
 *    416 / 416  (offset=0)
 *    416 / 616  (offset=1)
 *    420 / 626  (offset=2)
 *    415 / 615  (offset=3)
 *
 *
 * Sample output on Core2 Duo E8300/2.83GHz:
 * ========================================
 * 64bit
 * Timing of (un)aligned data access
 * copy 32 items cache->cache
 * item size = sizeof(long) = 8
 *
 * aligned/unaligned time in CPU clocks
 *    93 / 93  (offset=0)
 *    93 / 178  (offset=1)
 *    93 / 178  (offset=2)
 *    93 / 178  (offset=3)
 *    93 / 178  (offset=4)
 *    93 / 178  (offset=5)
 *    93 / 178  (offset=6)
 *    93 / 178  (offset=7)
 *
 * Sample output on AMD A6-5400K APU 3.6GHz:
 * ========================================
 * 64bit
 * Timing of (un)aligned data access
 * copy 100 items cache->cache
 * item size = sizeof(double) = 8
 *
 * aligned/unaligned time in CPU clocks
 *    345 / 345  (offset=0)
 *    346 / 362  (offset=1)
 *    345 / 361  (offset=2)
 *    345 / 361  (offset=3)
 *    345 / 360  (offset=4)
 *    348 / 359  (offset=5)
 *    345 / 359  (offset=6)
 *    345 / 421  (offset=7)
 *
 */

#define DATATYPE(TT) typedef  TT  T; const char Tstring[] = #TT
DATATYPE(double);

#include <stdio.h>
#include <stdlib.h>

//#define rdtsc __builtin_ia32_rdtsc
unsigned int xxx;
#define rdtsc() __builtin_ia32_rdtscp(&xxx)

#define MAX      100		/* array length (should be small) */

long long test(T *restrict t, T *restrict x)
{
    long long start;
    long long end;
    long long dt = 1000000;
    int i, n;
    for (n = 0; n < 10; n++) {	/* 10 experiments */
	start = rdtsc();
	for (i = 0; i < MAX; i++) {
	    x[i] = t[i];	/* copy */
	}
	end = rdtsc();
	if (end - start < dt)
	    dt = end - start;	/* take minimum */
    }
    return dt;
}


void test_align(int offset)
{
    T *t = (T *) malloc(MAX * sizeof(T) + offset);
    T *x = (T *) malloc(MAX * sizeof(T) + offset);
    if (x == NULL || t == NULL) {
	printf("Not enough memory\n");
	exit(1);
    }
    /* fill cache(s) */
    test(t, x);
    /* aligned access */
    printf("   %lld / ", test(t, x));
    /* unaligned access with offset */
    t = (T *) (((char *) t) + offset);
    x = (T *) (((char *) x) + offset);
    printf("%lld  (offset=%d)\n", test(t, x), offset);
}


int main(void)
{
    int i;
    printf("Timing of (un)aligned data access\n");
    printf("copy %d items cache->cache\n", MAX);
    printf("item size = sizeof(%s) = %u\n", Tstring, (unsigned)sizeof(T));
    printf("\n");
    printf("aligned/unaligned time in CPU clocks\n");

    for (i = 0; i < sizeof(T); i++)
	test_align(i);

    return 0;
}
