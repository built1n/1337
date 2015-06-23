#include "globals.h"

void xorshift(uint64_t *n)
{
    *n ^= *n >> 21;
    *n ^= *n << 35;
    *n ^= *n >> 4;
}

static uint64_t a, b;

uint64_t myrand(void)
{
    xorshift(&a);
    xorshift(&b);
    uint64_t ret = a & 0xFFFFFFFF;
    ret |= b << 32;
    return ret;
}

void mysrand(uint64_t hi, uint64_t lo)
{
    a = hi + 42;
    if(!a)
        a = 1;
    b = lo - 7;
    if(!b)
        b = 2;
}
