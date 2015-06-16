#include "globals.h"

/* (x,y) is a tile */
bool obstacle_check(struct world_t *world, llong x, llong y)
{
    printf("=check collide with %lld, %lld\n", x,y );
    enum sprite_t sprite = tile_get(world, x, y)->sprite;
    for(uint i = 0; i < ARRAYLEN(obstacles); ++i)
    {
        if(sprite == obstacles[i])
            return false;
    }
    return true;
}

/* player movement is by pixels */
bool player_move(struct world_t *world, llong dx, llong dy)
{
    dx *= 4;
    dy *= 4;
    struct player_t *player = &world->player;
    printf("player is at %lld, %lld + %lld, %lld\n",
           player->pos.x, player->pos.y,
           dx, dy);

    struct coords_t newtile = { player->pos.x, player->pos.y };

    newtile.x += dx / 32;
    newtile.y += dy / 32;

    struct coords_t newoffs = { player->offset.x, player->offset.y };

    if(SIGN(dx) > 0)
        newoffs.x += ABS(dx % 32);
    else
        newoffs.x -= ABS(dx % 32);

    if(SIGN(dy) > 0)
        newoffs.y += ABS(dy % 32);
    else
        newoffs.y -= ABS(dy % 32);

    if(newoffs.x < 0)
    {
        --newtile.x;
        newoffs.x = 31;
    }
    else if(newoffs.x >= 32)
    {
        ++newtile.x;
        newoffs.x = 0;
    }

    if(newoffs.y < 0)
    {
        ++newtile.y;
        newoffs.y = 31;
    }
    else if(newoffs.y >= 32)
    {
        --newtile.y;
        newoffs.y = 0;
    }

    printf("move player to %lld, %lld\n", newtile.x, newtile.y);
    if(obstacle_check(world, newtile.x, newtile.y))
    {
        player->pos.x = newtile.x;
        player->pos.y = newtile.y;
        player->offset.x = newoffs.x;
        player->offset.y = newoffs.y;

        if(player->pos.x <= world->camera.pos.x + SCROLL_DISTANCE ||
           player->pos.y <= world->camera.pos.y + SCROLL_DISTANCE ||
           player->pos.x >= world->camera.pos.x + world->camera.size.x - SCROLL_DISTANCE ||
           player->pos.y >= world->camera.pos.y + world->camera.size.y - SCROLL_DISTANCE)
        {
            world->camera.pos.x += dx;
            world->camera.pos.y += dy;
        }
        return true;
    }
    else
        return false;
}
