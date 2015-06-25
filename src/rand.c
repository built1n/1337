#include "globals.h"

/* a simple combined xorshift-MLCG generator */

/* constants from Numerical Recipes 3rd Ed. */

void xorshift(uint64_t *n)
{
    *n ^= *n >> 21;
    *n ^= *n << 35;
    *n ^= *n >> 4;
}

void mlcg(uint64_t *n)
{
    *n *= 2685821657736338717LL;
}

static uint64_t a, b;

uint8_t myrand(void)
{
    xorshift(&a);
    mlcg(&b);
    uint8_t ret = (a >> 21) + (b >> 16);

    return ret;
}

void mysrand(uint64_t hi, uint64_t lo)
{
    /* initialize with some bit-rich constants */
    a = hi ^ 4101842887655102017LL;
    if(!a)
        a = 1;
    myrand();
    b = lo ^ a;
    if(!b)
        b = 1;
    myrand();
}
