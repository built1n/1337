#include "1337_imp.h"

void l_movecam(struct world_t *world, llong dx, llong dy)
{
    struct camera_t *cam = &world->camera;
    cam->offset.x += cam->pos.x * 32 + dx;
    cam->pos.x = cam->offset.x / 32;
    cam->offset.x %= 32;

    cam->offset.y += cam->pos.y * 32 + dy;
    cam->pos.y = cam->offset.y / 32;
    cam->offset.y %= 32;
}
