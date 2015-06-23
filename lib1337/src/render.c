#include "1337_imp.h"

void l_render(struct world_t *world)
{
    struct camera_t *camera = &world->camera;
    //struct player_t *player = &world->player;
    for(llong x = camera->pos.x + camera->size.x, i = camera->size.x; x >= camera->pos.x; --x, --i)
        for(llong y = camera->pos.y, j = camera->size.y; y <= camera->pos.y + camera->size.y; ++y, --j)
        {
            sprite_t sprite, back;
            struct tile_t *tile = l_gettile(world, x, y);
            if(tile)
            {
                sprite = tile->sprite;
                back = tile->background;
                llong offs_x = tile->offset.x, offs_y = tile->offset.y;
                //if(x == player->pos.x && y == player->pos.y)
                //{
                //    sprite = player->sprite;
                //    offs_x = player->offset.x;
                //    offs_y = player->offset.y;
                //}

                world->interface->draw_sprite(world->userdata, i * 32, j * 32, back);
                world->interface->draw_sprite(world->userdata,
                                              i * 32 + offs_x, j * 32 + offs_y, sprite);
            }
        }

    //putsxy(rend, 0, 0, "pos: (%lld, %lld)", player->pos.x, player->pos.y);
    //putsxy(rend, 0, 16, "block: (%d, %d)", ROUND_BLOCK(player->pos.x), ROUND_BLOCK(player->pos.y));
    //putsxy(rend, 0, 32, "blocklen: %d", world->blocklen);
    world->interface->draw_update(world->userdata);
}
