#include "globals.h"

uint anim_find(enum sprite_t start)
{
    for(uint i = 0; i < ARRAYLEN(anim_data); ++i)
    {
        if(anim_data[i].start == start)
            return i;
    }
    return -1;
}

void animate_view(struct world_t *world)
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
            block_update(block_get(world, x, y));
        }
    }
}
