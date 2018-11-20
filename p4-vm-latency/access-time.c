// this is simple test of memeory access time
// Public domain. Author: peringer AT fit.vutbr.cz

#include <stdio.h>
#include <stdlib.h>

// size of allocated memory block
const size_t SIZE = 100uL*1024*1024;

#ifndef __GNUC__
#error "use GNU C, please"
#endif

#define rdtsc() __builtin_ia32_rdtscp(&xxx)

int main(int argc, char *argv[])
{
    unsigned xxx;
    // parse arguments
    if (argc > 3) {
	printf("Usage: %s step [N] \n", argv[0]);
	return 1;
    }
    size_t STEP = 4;           // addressing step size
    if (argc > 1)
	STEP = atol(argv[1]);
    if (STEP < 1 || STEP > 65536) {
	printf("Bad step (1..65536 is allowed)\n");
	return 1;
    }
    size_t N = 10;              // number of samples
    const size_t MAX = (SIZE-4096)/STEP - 1;
    if (argc > 2)
	N = atol(argv[2]);
    if (N < 1 || N > MAX) {
	printf("Bad N (1..%zd is allowed)\n", MAX);
	return 2;
    }
    // allocate memory
    unsigned char *m;       // access time
    m = malloc(SIZE);
    if (m == NULL) {
	printf("Error: not enough memory\n");
	return 3;
    }
    int i;
#if 0
    // touch memory buffer to eliminate page-faults
    for (i = 0; i < SIZE; i++) {
	m[i] = 0;
    }
#endif
    unsigned atime[N];
    for (i = 0; i < N; i++) { // fill cache
	atime[i] = 0;
    }
    // get 4096B page-aligned address  (TODO: use sysconf()/getconf)
    unsigned char *address = (unsigned char*)((size_t)(m+4096) & ~0x0FFFuL);
    const char *name = "R";
    long long a, b, sum=0;
    unsigned long correction;
#if 0
    do { // compute average rdtscp time (eliminate extra big values)
        correction=0;
        for (i=0;i<10;i++) {
            a = rdtsc();
            b = rdtsc();
            correction += b-a;
        }
        correction/=10;
    }while(correction<=0 || correction>100); // TODO: use better limits
#else
    // compute min rdtscp time
    correction=1000;
    for (i=0;i<100;i++) {
        a = rdtsc();
        b = rdtsc();
        unsigned long dt = b-a;
        if(dt < correction)
            correction = dt;
    }
#endif
    printf("# RDTSC correction=%d\n",correction);

    // test of memory access time
    for (i=0;i<N;i++) {
        char c;
        a = rdtsc();
        c = *address;                   // READ
	b = rdtsc();
        long long dt = (b - a) - correction;
        if(dt<0) dt=0;
        atime[i] = dt;
        address += STEP;
        sum+=c;
    }
    // print times:
    printf("# %s sum=%lld\n",name,sum);
    printf("# step=%zd\n",STEP);
    printf("# N=%zd\n",N);
    printf("# offset[-] time[clk] \n");
    for (i = 0; i < N; i++)
	printf("%8d %8d \n", (int)(i*STEP), (int)atime[i]);
    return 0;
}

