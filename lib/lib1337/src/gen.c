#include "1337_imp.h"

void l_setgen(struct world_t *world, genfunc_t genfunc)
{
    ((struct l33t_data*)(world->privatedata))->genfunc = genfunc;
}

void l_gen(struct world_t *world)
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
            l_loadblock(world, x, y);
        }
    }
}
