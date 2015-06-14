#include "globals.h"

void gen_randomize(struct block_t *block)
{
    uint64_t seed = 0;
    /* use the lower 32 bits from each coordinate of the block */
    seed |= ((block->coords.x / 64) << 32) & 0xFFFFFFFF;
    seed |= (block->coords.y / 64) & 0xFFFFFFFF;

    /* re-seed the RNG to make blocks the same across games */
    mysrand(seed);

    /* randomly place obstacles */
    for(int i = 0; i < GEN_MAX_TREES; ++i)
    {
        block->tiles[myrand() % BLOCK_DIM][myrand() % BLOCK_DIM].sprite =
            random_obstacles[myrand() % ARRAYLEN(random_obstacles)];
    }

    /* add an animated enemy */
    uint enemy_x = myrand() % BLOCK_DIM, enemy_y = myrand() % BLOCK_DIM;
    struct tile_t *tile = &(block->tiles[enemy_x][enemy_y]);
    tile->sprite = enemies[myrand() % ARRAYLEN(enemies)];
    tile->data.anim.frame = 0;
    tile->data.anim.type_idx = anim_find(tile->sprite);

    struct anim_tilelist *node = malloc(sizeof(struct anim_tilelist));
    node->next = block->anim_tiles;
    node->coords.x = enemy_x;
    node->coords.y = enemy_y;
    block->anim_tiles = node;
}

void generate_view(struct world_t *world)
{
    struct camera_t *camera = &world->camera;
    for(llong x = ROUND_BLOCK(world->camera.pos.x);
        x <= ROUND_BLOCK(camera->pos.x + camera->size.x + BLOCK_DIM);
        x += BLOCK_DIM)
    {
        for(llong y = ROUND_BLOCK(world->camera.pos.y);
            y <= ROUND_BLOCK(camera->pos.y + camera->size.y + BLOCK_DIM);
            y += BLOCK_DIM)
        {
            struct block_t *block = block_get(world, x, y);
            if(!block)
            {
                block = block_load(x, y);
                if(block)
                    block_add(world, block);
                else
                {
                    printf("load failed.\n");
                    block = block_new(x, y);
                    block_add(world, block);
                    gen_randomize(block);
                }
            }
        }
    }
}
