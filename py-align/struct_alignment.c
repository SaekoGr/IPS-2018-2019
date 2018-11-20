/*
 *
 * Simple test of structure members alignment
 * Public Domain
 *
 */

#include <stdio.h>
#include <stddef.h>

#define DEFINE_STRUCT(S)  S st; char *str=#S;
#if 1
DEFINE_STRUCT( struct test { char c1; int i; char c2; } )
#else
DEFINE_STRUCT( struct test { char c1; long i; char c2; } )
#endif

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ == 199901L)
# define OFFSET(st,item) (unsigned)offsetof(struct test,item)
#else
# define OFFSET(st,item) ((unsigned)((char*)&st.item  - (char*)&st))
#endif

int main() {

    printf("\n%s st;\n", str);
    printf("\nsizeof(st) = %u\n", (unsigned) sizeof(st));
    printf("&st      = %p\n", (void*) &st);
    printf("&st.c1   = %p (%u)\n", (void*) &st.c1,     OFFSET(st,c1));
    printf("&st.i    = %p (%u)\n", (void*) &st.i,      OFFSET(st,i));
    printf("&st.c2   = %p (%u)\n", (void*) &st.c2,     OFFSET(st,c2));
    printf("(&st)+1  = %p (%u)\n", (void*) (&st + 1),  (unsigned) sizeof(st));

    return 0;
}

