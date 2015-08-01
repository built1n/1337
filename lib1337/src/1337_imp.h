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

#define CHUNK_SIZE 1024

/* this structure is used to find the coordinates of the block
   an overlay tile is in */
struct overlay_chunk {
    uint start;
    struct coords_t tiles[CHUNK_SIZE]; /* set to -1, -1 to indicate empty */
};

struct l33t_data {
    genfunc_t genfunc;
    struct block_t *blocks;
    uint blocklen;
    /* currently loaded chunk */
    struct overlay_chunk *chunk;
};

void l_gz_write(const struct interface_t*, void *fd, const void*, size_t);
size_t l_gz_read(const struct interface_t*, void *fd, void*, size_t max);
