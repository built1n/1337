#include "1337_imp.h"

static void chunk_swapout(const struct interface_t *iface, struct overlay_chunk *c)
{
    char filename[32];
    snprintf(filename, sizeof(filename), "chunk%d", c->start / CHUNK_SIZE);

    void *fd = iface->fopen(filename, "w");
    iface->fwrite(c, sizeof(*c), fd);
    iface->fclose(fd);
}

#define ROUND_CHUNK(x) (uint)((FLOOR((x)/CHUNK_SIZE)*CHUNK_SIZE))

static void chunk_swapin(const struct interface_t *iface, struct overlay_chunk **chunk, uint want)
{
    printf("chunk_swapin %x, %d\n", chunk, want);
    printf("rounded: %d\n", ROUND_CHUNK(want));
    printf("*chunk: %d\n", *chunk);
    if(!*chunk)
    {
        /* chunk is uninitialized */
        *chunk = malloc(sizeof(**chunk));
        printf("allocated memory for **chunk\n");
        goto try_load;
    }
    else
    {
        if(ROUND_CHUNK(want) == (*chunk)->start)
        {
            return;
        }

        chunk_swapout(iface, *chunk);

        char filename[32];

    try_load:

        snprintf(filename, sizeof(filename), "chunk%d", ROUND_CHUNK(want));
        void *fd = iface->fopen(filename, "r");

        if(!fd)
        {
            (*chunk)->start = ROUND_CHUNK(want);
            memset((*chunk)->tiles, 0xFF, sizeof((*chunk)->tiles));
        }
        else
        {
            iface->printf("swapping in chunk from disk\n");
            iface->fread(*chunk, sizeof(**chunk), fd);
            assert((*chunk)->start == ROUND_CHUNK(want));
            iface->fclose(fd);
        }
    }
}

void l_purgeoverlay(struct world_t *world)
{
    struct overlay_chunk *chunk = ((struct l33t_data*)world->privatedata)->chunk;
    /* a chunk might not be loaded, so check before calling */
    if(chunk)
        chunk_swapout(world->interface, chunk);
}

uint l_addoverlay(struct world_t *world, llong x, llong y, uint layer)
{
    printf("l_addoverlay\n");
    llong bx = ROUND_BLOCK(x), by = ROUND_BLOCK(y);
    struct block_t *block = l_getblock(world, bx, by);
    if(!block)
    {
        l_loadblock(world, bx, by);
        block = l_getblock(world, bx, by);
    }

    /* insert the tile into the block's list of overlay tiles */

    struct overlaytile_t *ov = malloc(sizeof(*ov));
    memset(ov, 0, sizeof(*ov));
    ov->_coords.x = x - bx;
    ov->_coords.y = y - by;
    ov->layer = layer;

    /* insert it in the proper place */
    //ov->next = block->overlay;
    //block->overlay = ov;
    struct overlaytile_t *iter = block->overlay, *last = NULL;
    if(!iter)
    {
        ov->next = block->overlay;
        block->overlay = ov;
    }
    while(iter)
    {
        if(ov->layer >= iter->layer)
        {
            printf("inserting overlay in block list\n");
            if(!last)
                block->overlay = ov;
            else
                last->next = ov;
            ov->next = iter;
            break;
        }
        last = iter;
        iter = iter->next;
    }

    /* put a key into the overlay list pointing to this tile's block */
    struct l33t_data *data = ((struct l33t_data*)world->privatedata);

    /* do a linear search for the lowest open slot */
    chunk_swapin(world->interface, &data->chunk, 0);
    while(1)
    {
        for(uint i = 0; i < CHUNK_SIZE; ++i)
        {
            if(data->chunk->tiles[i].x == -1 &&
               data->chunk->tiles[i].y == -1)
            {
                data->chunk->tiles[i].x = bx;
                data->chunk->tiles[i].y = by;
                ov->id = data->chunk->start * CHUNK_SIZE + i;
                return ov->id;
            }
        }
        chunk_swapin(world->interface, &data->chunk, data->chunk->start + CHUNK_SIZE);
    }
}

struct overlaytile_t *l_getoverlay(struct world_t *world, uint id)
{
    printf("l_getoverlay %x, %d\n", world, id);
    /* load the overlay's chunk */
    struct l33t_data *data = ((struct l33t_data*)world->privatedata);

    chunk_swapin(world->interface, &data->chunk, id);

    printf("swap in chunk\n");

    uint rem = id - ROUND_CHUNK(id);

    llong bx = data->chunk->tiles[rem].x;
    llong by = data->chunk->tiles[rem].y;
    if(bx == -1 && by == -1)
    {
        printf("overlay not found in chunk\n");
        return NULL;
    }

    /* found it's block, now search for the overlay tile */
    struct block_t *block = l_loadblock(world, bx, by);

    struct overlaytile_t *iter = block->overlay;
    while(iter)
    {
        if(iter->id == id)
            return iter;
        iter = iter->next;
    }

    printf("could not find overlay in block list\n");

    return NULL;
}

void l_deloverlay(struct world_t *world, uint id)
{
    /* load the overlay's chunk */
    struct l33t_data *data = ((struct l33t_data*)world->privatedata);

    chunk_swapin(world->interface, &data->chunk, id);

    uint rem = id - ROUND_CHUNK(id);

    llong bx = data->chunk->tiles[rem].x;
    llong by = data->chunk->tiles[rem].y;
    if(bx == -1 && by == -1)
        return NULL;

    /* found it's block, now search for the overlay tile */
    struct block_t *block = l_loadblock(world, bx, by);

    struct overlaytile_t *iter = block->overlay, *prev = NULL;
    while(iter)
    {
        if(iter->id == id)
        {
            /* remove the tile */
            if(prev)
                prev->next = iter->next;
            else
                block->overlay = iter->next;
            free(iter);

            /* remove it's list entry */
            data->chunk->tiles[rem].x = -1;
            data->chunk->tiles[rem].y = -1;

            return;
        }
        prev = iter;
        iter = iter->next;
    }
}
