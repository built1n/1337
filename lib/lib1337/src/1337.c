#include "1337_imp.h"

void l_init(struct world_t *world, uint w, uint h)
{
    l_resize(world, w, h);
    world->privatedata = malloc(sizeof(struct l33t_data));
    ((struct l33t_data*)(world->privatedata))->genfunc = NULL;
}

void l_resize(struct world_t *world, uint w, uint h)
{
    world->camera.size.x = CEIL(w / 32);
    world->camera.size.y = CEIL(h / 32);
}

void l_free(struct world_t *world)
{
    if(world->privatedata)
        free(world->privatedata);
}
