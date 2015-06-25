#include <1337/1337.h>

#include <assert.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <zlib.h>

#define ABS(x) (((x)<0)?-(x):(x))
#define ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))
#define CEIL(x) (ceil((double)x))
#define FLOOR(x) (floor((double)x))
#define MIN(a,b) (( (a) < (b) ) ? (a) : (b))
#define MAX(a,b) (( (a) > (b) ) ? (a) : (b))

/* ROUND_BLOCK(x) rounds x to the next lowest multiple of BLOCK_DIM */
#define ROUND_BLOCK(x) (llong)((FLOOR((x)/BLOCK_DIM)*BLOCK_DIM))

#define SIGN(x) ( ((x) < 0) ? -1 : 1)

struct l33t_data {
    genfunc_t genfunc;
    struct block_t *blocks;
    uint blocklen;
};
