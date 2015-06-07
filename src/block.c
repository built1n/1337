#include "globals.h"

struct block_t *block_new(llong x, llong y)
{
    struct block_t *block = malloc(sizeof(struct block_t));
    memset(block, 0, sizeof(struct block_t));
    block->coords.x = x;
    block->coords.y = y;
    return block;
}

/* gets the block starting at a coordinate */
/* coords must be multiples of BLOCK_DIM */
struct block_t *block_get(struct world_t *world, llong x, llong y)
{
    struct block_t *iter = world->blocks;
    while(iter)
    {
        if(iter->coords.x == x &&
           iter->coords.y == y)
        {
            return iter;
        }

        iter = iter->next;
    }

    return NULL;
}

struct tile_t* tile_get(struct world_t *world, llong x, llong y)
{
    /* get the coordinates of the block the tile is in */
    llong block_x = ROUND_BLOCK(x);
    llong block_y = ROUND_BLOCK(y);

    /* in C99, the sign of the remainder comes from the DIVIDEND */
    /* we want a positive value, so take it's absolute value */
    llong tile_x = x - block_x;
    llong tile_y = y - block_y;
    //printf("%d, %d --> %d, %d + %d, %d\n", x, y,
    //       block_x, block_y,
    //       tile_x, tile_y);

    struct block_t *block = block_get(world, block_x, block_y);

    if(!block)
        return NULL;

    return &block->tiles[tile_x][tile_y];
}

void block_swapout(struct block_t *block)
{

}

void block_purge(struct world_t *world)
{

}

void block_add(struct world_t *world, struct block_t *block)
{
    block->next = world->blocks;
    world->blocks = block;
    if(world->blocklen++ > 4 * LOCAL_DIM * LOCAL_DIM)
    {
        block_purge(world);
    }
    printf("blocklen %d\n", world->blocklen);
}
