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

#define GZIP_CHUNK 0x1000

static void block_swapout(struct interface_t *interface, struct block_t *block)
{
    printf("swap out\n");
    char buf[64];
    snprintf(buf, sizeof(buf), "%016llx%016llx.block",
             block->coords.x / BLOCK_DIM, block->coords.y / BLOCK_DIM);
    void *f = interface->fopen(buf, "w");

    /* animation is the responsibility of the user */
#if 0
    struct anim_tilelist *iter = block->anim_tiles;
    while(iter)
    {
        printf("writing anim tile data record\n");
        /* FIXME: assumes sizeof(llong) = 8 */
        unsigned char buf[16];
        assert(sizeof(llong) * 2 == 16);
        memcpy(buf, &iter->coords.x, sizeof(llong));
        memcpy(buf + 8, &iter->coords.y, sizeof(llong));
        if(interface->fwrite(buf, sizeof(buf), f) != sizeof(buf) || interface->ferror(f))
            fatal("file I/O error");
        iter = iter->next;
    }

    /* write the sentinel value */
    llong marker = 0x7FFFFFFFFFFFFFFF;
    interface->fwrite(&marker, sizeof(marker), f);
    interface->fwrite(&marker, sizeof(marker), f);
#endif

    unsigned char *outbuf = malloc(GZIP_CHUNK);

    z_stream out;
    out.zalloc = Z_NULL;
    out.zfree = Z_NULL;
    out.opaque = Z_NULL;
    if(deflateInit(&out, Z_DEFAULT_COMPRESSION) != Z_OK)
    {
      fatal("failed to open gzip stream for output");
    }

    out.avail_in = sizeof(block->tiles);
    out.next_in = (uchar*)block->tiles;
    do {
        out.avail_out = GZIP_CHUNK;
        out.next_out = outbuf;
        assert(deflate(&out, Z_FINISH) != Z_STREAM_ERROR);
        size_t have = GZIP_CHUNK - out.avail_out;
        if(interface->fwrite(outbuf, have, f) != have || interface->ferror(f))
            fatal("file I/O error");
    } while (out.avail_in > 0);

    deflateEnd(&out);
    free(outbuf);

    interface->fclose(f);
}

static struct block_t *block_load(struct interface_t *interface, llong x, llong y)
{
    printf("load block %lld, %lld\n", x, y);
    char filename[64];
    snprintf(filename, sizeof(filename), "%016llx%016llx.block",
             x / BLOCK_DIM, y / BLOCK_DIM);
    void *f = interface->fopen(filename, "r");
    if(!f)
        return NULL;
    struct block_t *new = block_new(x, y);

#if 0
    unsigned char buf[16];

    /* read in the linked list of animated tiles */
    /* disabled as animation is now in the hands of the user */
    while(1)
    {
        assert(sizeof(llong) * 2 == 16);
        if(interface->fread(buf, sizeof(buf), f) != sizeof(buf) || interface->ferror(f))
            fatal("file I/O error");
        struct anim_tilelist *node = malloc(sizeof(struct anim_tilelist));
        node->next = new->anim_tiles;
        node->coords.x = *((llong*)buf);
        node->coords.y = *(((llong*)buf + 1));

        if(node->coords.x == 0x7FFFFFFFFFFFFFFF &&
           node->coords.y == 0x7FFFFFFFFFFFFFFF)
        {
            printf("encountered sentinel value\n");
            free(node);
            break;
        }

        assert(node->coords.x < BLOCK_DIM);
        assert(node->coords.x >= 0);
        assert(node->coords.y < BLOCK_DIM);
        assert(node->coords.y >= 0);

        printf("reading anim tile data %lld %lld\n", node->coords.x, node->coords.y);
        new->anim_tiles = node;
    }
#endif

    z_stream in;
    in.zalloc = Z_NULL;
    in.zfree = Z_NULL;
    in.opaque = Z_NULL;
    in.avail_in = 0;
    in.next_in = Z_NULL;

    if(inflateInit(&in) != Z_OK)
        fatal("failed to open gzip stream");

    unsigned char *inbuf = malloc(GZIP_CHUNK);
    int ret;
    uchar *out = (uchar*)new->tiles;
    size_t avail = sizeof(new->tiles);

    do {
        in.avail_in = interface->fread(inbuf, GZIP_CHUNK, f);
        if(!in.avail_in || interface->ferror(f))
            break;
        in.next_in = inbuf;

        do {
            in.avail_out = MIN(GZIP_CHUNK, avail);
            in.next_out = out;
            ret = inflate(&in, Z_NO_FLUSH);

            assert(ret != Z_STREAM_ERROR);

            switch(ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR; /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&in);
                fatal("gzip memory error");
            }
            size_t have = GZIP_CHUNK - in.avail_out;
            out += have;
            avail -= have;
        } while (in.avail_out == 0);
    } while (ret != Z_STREAM_END);

    inflateEnd(&in);
    free(inbuf);

    interface->fclose(f);
    return new;
}

/* gets the block starting at a coordinate */
/* coords must be multiples of BLOCK_DIM */
struct block_t *l_getblock(struct world_t *world, llong x, llong y)
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
    printf("purging block list\n");
    /* iterate over the block list and remove any blocks outside of the "local" range */
    struct block_t *iter = world->blocks;
    struct block_t *prev = NULL;
    llong cam_x = world->camera.pos.x, cam_y = world->camera.pos.y;
    uint local_dim = MAX(CEIL(world->camera.size.x / 64),
                         CEIL(world->camera.size.y / 64)) + 2;
    uint local_tiles = local_dim * BLOCK_DIM;
    while(iter)
    {
        if((ABS(iter->coords.x - cam_x) > local_tiles ||
            ABS(iter->coords.y - cam_y) > local_tiles))
        {
            struct block_t *next = iter->next;
            block_swapout(world->interface, iter);
            if(!prev)
                world->blocks = next;
            else
                prev->next = next;
            free(iter);
            iter = next;
            --world->blocklen;
        }
        else
        {
            prev = iter;
            iter = iter->next;
        }
    }
}

void l_purgeall(struct world_t *world)
{
    printf("purging entire block list\n");
    /* iterate over the block list and remove any blocks outside of the "local" range */
    struct block_t *iter = world->blocks;
    struct block_t *prev = NULL;
    while(iter)
    {
        struct block_t *next = iter->next;
        block_swapout(world->interface, iter);
        if(!prev)
            world->blocks = next;
        else
            prev->next = next;
        free(iter);
        iter = next;
        --world->blocklen;
    }
}

static void block_add(struct world_t *world, struct block_t *block)
{
    block->next = world->blocks;
    world->blocks = block;
    uint local_dim = MAX(CEIL(world->camera.size.x / 64),
                         CEIL(world->camera.size.y / 64)) + 2;
    if(world->blocklen++ > 4 * local_dim * local_dim)
    {
        l_purge(world);
    }
}

void l_loadblock(struct world_t *world, llong x, llong y)
{
    if(l_getblock(world, x, y) == NULL)
    {
        /* first try getting it from disk */
        struct block_t *block = block_load(world->interface, x, y);
        if(block)
            block_add(world, block);
        else
        {
            /* it's not on disk, generate a new one */
            printf("load failed.\n");
            block = block_new(x, y);
            block_add(world, block);
            genfunc_t genfunc = ((struct l33t_data*)(world->privatedata))->genfunc;
            if(genfunc)
                genfunc(block);
        }
    }
}
