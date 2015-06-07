#include "globals.h"

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
                printf("adding block %d %d\n", x, y);
                block_add(world, block_new(x, y));
            }
        }
    }
}
