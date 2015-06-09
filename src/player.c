#include "globals.h"

bool obstacle_check(struct world_t *world, llong x, llong y)
{
    enum sprite_t sprite = tile_get(world, x, y)->sprite;
    for(uint i = 0; i < ARRAYLEN(obstacles); ++i)
    {
        if(sprite == obstacles[i])
            return false;
    }
    return true;
}

bool player_move(struct world_t *world, llong dx, llong dy)
{
    if(obstacle_check(world, world->player.pos.x + dx, world->player.pos.y + dy))
    {
        world->player.pos.x += dx;
        world->player.pos.y += dy;

        world->camera.pos.x += dx;
        world->camera.pos.y += dy;
        return true;
    }
    else
        return false;
}
