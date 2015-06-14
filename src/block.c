#include "globals.h"

struct block_t *block_new(llong x, llong y)
{
    struct block_t *block = malloc(sizeof(struct block_t));
    memset(block, 0, sizeof(struct block_t));
    for(uint i = 0; i < BLOCK_DIM*BLOCK_DIM; ++i)
    {
        ((struct tile_t*)(block->tiles))[i].background = SPRITE_GRASS;
    }

    block->coords.x = x;
    block->coords.y = y;
    return block;
}

void block_update(struct block_t *block)
{
    struct anim_tilelist *iter = block->anim_tiles;
    while(iter)
    {
        struct tile_t *tile = &(block->tiles[iter->coords.x][iter->coords.y]);
        tile->sprite = (tile->data.anim.frame++ % anim_data[tile->data.anim.type_idx].len) +
            anim_data[tile->data.anim.type_idx].start;
        iter = iter->next;
    }
}

#define GZIP_CHUNK 0x1000

void block_swapout(struct block_t *block)
{
    printf("swap out\n");
    char buf[64];
    snprintf(buf, sizeof(buf), "%016llx%016llx.block",
             block->coords.x / BLOCK_DIM, block->coords.y / BLOCK_DIM);
    FILE *f = fopen(buf, "w");

    struct anim_tilelist *iter = block->anim_tiles;
    while(iter)
    {
        printf("writing anim tile data record\n");
        /* FIXME: assumes sizeof(llong) = 8 */
        unsigned char buf[16];
        assert(sizeof(llong) * 2 == 16);
        memcpy(buf, &iter->coords.x, sizeof(llong));
        memcpy(buf + 8, &iter->coords.y, sizeof(llong));
        if(fwrite(buf, 1, sizeof(buf), f) != sizeof(buf) || ferror(f))
            fatal("file I/O error");
        iter = iter->next;
    }

    llong marker = 0x7FFFFFFFFFFFFFFF;
    fwrite(&marker, 1, sizeof(marker), f);
    fwrite(&marker, 1, sizeof(marker), f);

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
        if(fwrite(outbuf, 1, have, f) != have || ferror(f))
            fatal("file I/O error");
    } while (out.avail_in > 0);

    deflateEnd(&out);
    free(outbuf);

    fclose(f);
}

struct block_t *block_load(llong x, llong y)
{
    printf("load block %lld, %lld\n", x, y);
    char filename[64];
    snprintf(filename, sizeof(filename), "%016llx%016llx.block",
             x / BLOCK_DIM, y / BLOCK_DIM);
    FILE *f = fopen(filename, "r");
    if(!f)
        return NULL;
    struct block_t *new = block_new(x, y);

    unsigned char buf[16];

    /* read in the linked list of animated tiles */
    while(1)
    {
        assert(sizeof(llong) * 2 == 16);
        if(fread(buf, 1, sizeof(buf), f) != sizeof(buf) || ferror(f))
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
        in.avail_in = fread(inbuf, 1, GZIP_CHUNK, f);
        if(!in.avail_in)
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

    fclose(f);
    return new;
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

void block_purge(struct world_t *world)
{
    printf("purging block list\n");
    /* iterate over the block list and remove any blocks outside of the "local" range */
    struct block_t *iter = world->blocks;
    struct block_t *prev = NULL;
    llong cam_x = world->camera.pos.x, cam_y = world->camera.pos.y;
    while(iter)
    {
        if((ABS(iter->coords.x - cam_x) > LOCAL_DIM * BLOCK_DIM ||
            ABS(iter->coords.y - cam_y) > LOCAL_DIM * BLOCK_DIM))
        {
            struct block_t *next = iter->next;
            block_swapout(iter);
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

void block_purgeall(struct world_t *world)
{
    printf("purging entire block list\n");
    /* iterate over the block list and remove any blocks outside of the "local" range */
    struct block_t *iter = world->blocks;
    struct block_t *prev = NULL;
    while(iter)
    {
        struct block_t *next = iter->next;
        block_swapout(iter);
        if(!prev)
            world->blocks = next;
        else
            prev->next = next;
        free(iter);
        iter = next;
        --world->blocklen;
    }
}

void block_add(struct world_t *world, struct block_t *block)
{
    block->next = world->blocks;
    world->blocks = block;
    if(world->blocklen++ > 4 * LOCAL_DIM * LOCAL_DIM)
    {
        block_purge(world);
    }
}

void block_setdir(const char *dir)
{
    if(chdir(dir) < 0)
    {
        printf("%s\n", strerror(errno));
        mode_t old_umask = umask(0);
        mkdir(dir, 0744);
        umask(old_umask);
        if(chdir(dir) < 0)
        {
            fatal("can't create directory: %s", strerror(errno));
        }
    }
}
