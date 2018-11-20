/*
 * test-vm.c -- test signalu a prace s virtualni pameti
 *
 * vyzaduje POSIX signaly, proto nelze pouzit -std=c99 -pedantic
 * testovano na Linuxu (Debian 3.1/Sarge GCC 3.3.5, Debian 6/Squeeze GCC 4.4.5)
 *
 */

#include <stdio.h>		/* printf() */
#include <signal.h>		/* sigaction(), atd. */
#include <setjmp.h>		/* setjmp(), longjmp() */
#include <stdlib.h>		/* malloc(), getenv() */

#define VMPAGESIZE 4096		/* velikost stranky virtualni pameti */

jmp_buf buf;			/* pro setjmp() a longjmp() */
struct sigaction act;		/* popis obsluhy signalu */

char *ptr=NULL;			/* ukazatel pro pristup do pameti */

/*
 * sigsegv() -- obsluha signalu SIGSEGV (poruseni ochrany pameti)
 */
void sigsegv(int sig)
{
    /* POSIX: pred longjmp se musi explicitne povolit */
    sigdelset(&act.sa_mask, SIGSEGV);
    sigprocmask(SIG_SETMASK, &act.sa_mask, &act.sa_mask);
    longjmp(buf, 1);		/* bez longjmp neprekroci instrukci */
}

int main()
{
    int automatic_var = 0;

    /* nastaveni obsluhy signalu SIGSEGV - poruseni ochrany pameti */
    act.sa_handler = &sigsegv;	/* nastavime signal handler */
    sigemptyset(&act.sa_mask);  /* neblokuje zadne signaly */
    if (sigaction(SIGSEGV, &act, NULL) < 0) {	/* nastaveni obsluhy */
	fprintf(stderr, "sigaction failed\n");
	return 1;
    }

#if 1
    /* nektere vybrane adresy v adresovem prostoru */
    printf("code:                   %p\n", &main);
    printf("constant data:          %p\n", "");
    printf("initialized var:        %p\n", &ptr);
    printf("uninitialized var:      %p\n", &buf);
    printf("small dynamic data:     %p\n", malloc(10)); /* memory leak */
    printf("libc PLT entry:         %p\n", &printf);
//    printf("standard library table: %p\n", *(void**)(((char*)&printf)+2));
//    printf("standard library:       %p\n", *(void**)((char*)(*(void**)(((char*)&printf)+2))+0));
    printf("big dynamic data:       %p\n", malloc(1024*1024L));
    printf("stack:                  %p\n",&automatic_var);
    printf("environment:            %p\n", getenv("PATH"));
    printf("\n");
#endif

    ptr = NULL;
    do {
	if (setjmp(buf) == 0) {
	    int x;
	    x = *ptr;		/* pristup do pameti -- CTENI */
	    printf("\n%p ", ptr);	/* tisk jen pro pristupne adresy */
	    printf("r");	/* muzeme cist */
	    *ptr = x;		/* pristup do pameti -- ZAPIS */
	    printf("w");	/* muzeme cist i psat */
	}
	ptr += VMPAGESIZE;
    } while (ptr != NULL);	/* projdeme cely virtualni adresovy prostor */
    printf("\n");

    return 0;
}
