#include "mnemonic.h"
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>
#define SIZE 1024




int main(void)
{
    char stack[SIZE];
    linear_allocator* lin_alloc = mnemonic_linear_init(stack, SIZE, 0);
    
    int* a = mnemonic_linear_alloc(lin_alloc, sizeof(int), 4, 0);
    short* b = mnemonic_linear_alloc(lin_alloc, sizeof(short), 2, 0);
    char* c = mnemonic_linear_alloc(lin_alloc, sizeof(char), 1, 0);

    printf("stack begin: %p\n", stack);
    printf("alloc int:   %p\n", a);
    printf("alloc short: %p\n", b);
    printf("alloc char:  %p\n", c);
    
    char* heap = malloc(SIZE);
    lin_alloc = mnemonic_linear_init(heap, SIZE, 0);

    int* d = mnemonic_linear_alloc(lin_alloc, sizeof(int), 4, 0);
    short* e = mnemonic_linear_alloc(lin_alloc, sizeof(short), 2, 0);
    char* f = mnemonic_linear_alloc(lin_alloc, sizeof(char), 1, 0);

    printf("heap begin:  %p\n", heap);
    printf("alloc int:   %p\n", d);
    printf("alloc short: %p\n", e);
    printf("alloc char:  %p\n", f);

    free(heap);

    lin_alloc = mnemonic_linear_create(0);

    int* g = mnemonic_linear_alloc(lin_alloc, sizeof(int), 4, 0);
    short* h = mnemonic_linear_alloc(lin_alloc, sizeof(short), 2, 0);
    char* i = mnemonic_linear_alloc(lin_alloc, sizeof(char), 1, 0);

    printf("heap begin:  %p\n", lin_alloc);
    printf("alloc int:   %p\n", g);
    printf("alloc short: %p\n", h);
    printf("alloc char:  %p\n", i);

    mnemonic_linear_destroy(lin_alloc);

    pool_allocator* pool_alloc = mnemonic_pool_init(stack, SIZE, 4, 4, 0, 0);

    int* j = mnemonic_pool_alloc(pool_alloc);
    int* k = mnemonic_pool_alloc(pool_alloc);
    int* l = mnemonic_pool_alloc(pool_alloc);

    printf("stack begin: %p\n", stack);
    printf("alloc int:   %p\n", j);
    printf("alloc int:   %p\n", k);
    printf("alloc int:   %p\n", l);

    mnemonic_pool_free(pool_alloc, j);
    mnemonic_pool_free(pool_alloc, k);
    mnemonic_pool_free(pool_alloc, l);

    int* m = mnemonic_pool_alloc(pool_alloc);
    int* n = mnemonic_pool_alloc(pool_alloc);
    int* o = mnemonic_pool_alloc(pool_alloc);

    printf("alloc int:   %p\n", m);
    printf("alloc int:   %p\n", n);
    printf("alloc int:   %p\n", o);

    mnemonic_pool_free(pool_alloc, m);
    mnemonic_pool_free(pool_alloc, n);
    mnemonic_pool_free(pool_alloc, o);

    heap = malloc(SIZE);

    pool_alloc = mnemonic_pool_init(heap, SIZE, 4, 4, 0, 0);

    int* p = mnemonic_pool_alloc(pool_alloc);
    int* q = mnemonic_pool_alloc(pool_alloc);
    int* r = mnemonic_pool_alloc(pool_alloc);

    printf("heap begin:  %p\n", heap);
    printf("alloc int:   %p\n", p);
    printf("alloc int:   %p\n", q);
    printf("alloc int:   %p\n", r);

    mnemonic_pool_free(pool_alloc, p);
    mnemonic_pool_free(pool_alloc, q);
    mnemonic_pool_free(pool_alloc, r);

    free(heap);

    pool_alloc = mnemonic_pool_create(0, 4, 4, 0);

    mnemonic_pool_destroy(pool_alloc);

    getchar();

    return 0;
}