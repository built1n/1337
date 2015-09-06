#include "1337_imp.h"

static struct block_t *block_new(llong x, llong y)
{
    struct block_t *block = malloc(sizeof(struct block_t));
    memset(block, 0, sizeof(struct block_t));
    for(uint i = 0; i < BLOCK_DIM*BLOCK_DIM; ++i)
    {
        ((struct tile_t*)(block->tiles))[i].background = 0;
    }

    block->coords.x = x;
    block->coords.y = y;
    return block;
}

static void block_swapout(const struct interface_t *interface, struct block_t *block)
{
    interface->logf(LOG_DEBUG, "swap out (%lld, %lld)\n",
                    block->coords.x, block->coords.y);
    char buf[64];
    snprintf(buf, sizeof(buf), "%016llx%016llx.block",
             block->coords.x / BLOCK_DIM, block->coords.y / BLOCK_DIM);
    void *f = interface->fopen(buf, "w");

    /* animation is now the responsibility of the user */
#if 0
    struct anim_tilelist *iter = block->anim_tiles;
    while(iter)
    {
        interface->logf(LOG_DEBUG, "writing anim tile data record\n");
        /* FIXME: assumes sizeof(llong) = 8 */
        unsigned char buf[16];
        assert(sizeof(llong) * 2 == 16);
        memcpy(buf, &iter->coords.x, sizeof(llong));
        memcpy(buf + 8, &iter->coords.y, sizeof(llong));
        if(interface->fwrite(buf, sizeof(buf), f) != sizeof(buf) || interface->ferror(f))
            interface->fatal("File I/O error");
        iter = iter->next;
    }

    /* write the sentinel value */
    llong marker = 0x7FFFFFFFFFFFFFFF;
    interface->fwrite(&marker, sizeof(marker), f);
    interface->fwrite(&marker, sizeof(marker), f);
#endif

    /* write the block's overlay tiles */
    struct overlaytile_t *iter = block->overlay;
    while(iter)
    {
        interface->fwrite(iter, sizeof(*iter), f);
        iter = iter->next;
    }

    /* now the sentinel */
    struct overlaytile_t sentinel;
    sentinel.id = -1;
    interface->fwrite(&sentinel, sizeof(sentinel), f);

    l_gz_write(interface,f, block->tiles, sizeof(block->tiles));

    interface->fclose(f);
}

static struct block_t *block_load(const struct interface_t *interface, llong x, llong y)
{
    interface->logf(LOG_DEBUG, "load block %lld, %lld\n", x, y);
    char filename[64];
    snprintf(filename, sizeof(filename), "%016llx%016llx.block",
             x / BLOCK_DIM, y / BLOCK_DIM);
    void *f = interface->fopen(filename, "r");
    if(!f)
        return NULL;
    struct block_t *block = block_new(x, y);

#if 0
    unsigned char buf[16];

    /* read in the linked list of animated tiles */
    /* disabled as animation is now in the hands of the user */
    while(1)
    {
        assert(sizeof(llong) * 2 == 16);
        if(interface->fread(buf, sizeof(buf), f) != sizeof(buf) || interface->ferror(f))
            interface->fatal("File I/O error");
        struct anim_tilelist *node = malloc(sizeof(struct anim_tilelist));
        node->next = block->anim_tiles;
        node->coords.x = *((llong*)buf);
        node->coords.y = *(((llong*)buf + 1));

        if(node->coords.x == 0x7FFFFFFFFFFFFFFF &&
           node->coords.y == 0x7FFFFFFFFFFFFFFF)
        {
            interface->logf(LOG_DEBUG, "encountered sentinel value\n");
            free(node);
            break;
        }

        assert(node->coords.x < BLOCK_DIM);
        assert(node->coords.x >= 0);
        assert(node->coords.y < BLOCK_DIM);
        assert(node->coords.y >= 0);

        interface->logf(LOG_DEBUG, "reading anim tile data %lld %lld\n", node->coords.x, node->coords.y);
        block->anim_tiles = node;
    }
#endif

    /* read the linked list of overlay tiles */
    /* note that order doesn't matter here */

    /* rebuild the linked list */

    struct overlaytile_t *prev = NULL;
    interface->logf(LOG_DEBUG, "loading linked-list of overlay tiles\n");
    while(1)
    {
        struct overlaytile_t *ov = malloc(sizeof(struct overlaytile_t));
        if(interface->fread(ov, sizeof(*ov), f) != sizeof(*ov) || ov->id == (uint)-1)
            break;
        ov->next = NULL;
        if(prev)
        {
            prev->next = ov;
        }
        else
        {
            block->overlay = ov;
        }
        prev = ov;
    }

    l_gz_read(interface, f, block->tiles, sizeof(block->tiles));

    return block;
}

/* gets the block starting at a coordinate */
/* coords must be multiples of BLOCK_DIM */
struct block_t *l_getblock(struct world_t *world, llong x, llong y)
{
    struct block_t *iter = ((struct l33t_data*)(world->privatedata))->blocks;
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

struct tile_t* l_gettile(struct world_t *world, llong x, llong y)
{
    /* get the coordinates of the block the tile is in */
    llong block_x = ROUND_BLOCK(x);
    llong block_y = ROUND_BLOCK(y);

    /* and now the tile offset */
    llong tile_x = x - block_x;
    llong tile_y = y - block_y;

    struct block_t *block = l_getblock(world, block_x, block_y);

    if(!block)
        return NULL;

    return &block->tiles[tile_x][tile_y];
}

void l_purge(struct world_t *world)
{
    /* iterate over the block list and remove any blocks outside of the "local" range */
    struct block_t *iter = ((struct l33t_data*)(world->privatedata))->blocks;
    struct block_t *prev = NULL;
    llong cam_x = world->camera.pos.x, cam_y = world->camera.pos.y;
    llong cam_w = world->camera.size.x, cam_h = world->camera.size.y;

    /* the dimensions of the region around the camera that blocks
       can exist in */
    uint local_x = ROUND_BLOCK(world->camera.size.x + BLOCK_DIM) + BLOCK_DIM;
    uint local_y = ROUND_BLOCK(world->camera.size.y + BLOCK_DIM) + BLOCK_DIM;

    /* determine if the length of the block list exceeds a threshold */
    if(CEIL((local_x * local_y) / (BLOCK_DIM * BLOCK_DIM)) < ((struct l33t_data*)(world->privatedata))->blocklen)
    {
        world->interface->logf(LOG_INFO, "Purging block list\n");
        while(iter)
        {
            /* determine if all or part of the block is in view */
            llong dx = iter->coords.x - cam_x;
            llong dy = iter->coords.y - cam_y;

            if(dx <= -BLOCK_DIM || cam_w < dx ||
               dy <= -BLOCK_DIM || cam_h < dy)
            {
                struct block_t *next = iter->next;
                block_swapout(world->interface, iter);
                if(!prev)
                    ((struct l33t_data*)(world->privatedata))->blocks = next;
                else
                    prev->next = next;
                free(iter);
                iter = next;
                --((struct l33t_data*)(world->privatedata))->blocklen;
            }
            else
            {
                prev = iter;
                iter = iter->next;
            }
        }
    }
}

void l_purgeall(struct world_t *world)
{
    world->interface->logf(LOG_INFO, "Purging entire block list\n");
    /* write every block to disk */
    struct block_t *iter = ((struct l33t_data*)(world->privatedata))->blocks;
    struct block_t *prev = NULL;
    while(iter)
    {
        struct block_t *next = iter->next;
        block_swapout(world->interface, iter);
        if(!prev)
            ((struct l33t_data*)(world->privatedata))->blocks = next;
        else
            prev->next = next;
        free(iter);
        iter = next;
        --((struct l33t_data*)(world->privatedata))->blocklen;
    }
}

static void block_add(struct world_t *world, struct block_t *block)
{
    struct l33t_data *data = ((struct l33t_data*)world->privatedata);

    block->next = data->blocks;
    data->blocks = block;
    data->blocklen++;
}

struct block_t *l_loadblock(struct world_t *world, llong x, llong y)
{
    struct block_t *ret = l_getblock(world, x, y);
    if(ret == NULL)
    {
        /* first try getting it from disk */
        ret = block_load(world->interface, x, y);
        if(ret)
            block_add(world, ret);
        else
        {
            /* it's not on disk, generate a new one */
            world->interface->logf(LOG_DEBUG, "Loading block failed");
            ret = block_new(x, y);

            /* add the block first to prevent an infinite loop
               in case the generator function calls tries to access the block indirectly */
            block_add(world, ret);

            genfunc_t genfunc = ((struct l33t_data*)(world->privatedata))->genfunc;
            if(genfunc)
            {
                world->interface->logf(LOG_DEBUG, "Calling user generator function\n");
                genfunc(world, ret);
            }
        }
    }

    return ret;
}
