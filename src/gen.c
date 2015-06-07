#include "globals.h"

void gen_randomize(struct block_t *block)
{
    for(int i = 0; i < GEN_MAX_TREES; ++i)
    {
        block->tiles[rand() % BLOCK_DIM][rand() % BLOCK_DIM].sprite =
            (rand() % 2 == 0) ? SPRITE_ROCK : SPRITE_TREE;
    }
}

void generate_view(struct world_t *world)
{
    struct camera_t *camera = &world->camera;
    for(llong x = ROUND_BLOCK(world->camera.pos.x);
        x < ROUND_BLOCK(camera->pos.x + camera->size.x + BLOCK_DIM);
        x += BLOCK_DIM)
    {
        for(llong y = ROUND_BLOCK(world->camera.pos.y);
            y < ROUND_BLOCK(camera->pos.y + camera->size.y + BLOCK_DIM);
            y += BLOCK_DIM)
        {
            if(!block_get(world, x, y))
            {
                struct block_t *loaded = block_load(x, y);
                if(loaded)
                    block_add(world, loaded);
                else
                {
                    printf("load failed.\n");
                    struct block_t *new = block_new(x, y);
                    block_add(world, new);
                    gen_randomize(new);
                }
            }
        }
    }
}
