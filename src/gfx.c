#include "globals.h"

SDL_Texture *sprites[MAX_SPRITES] = { NULL };

void sprite_draw(enum sprite_t sprite, int x, int y, SDL_Renderer *rend)
{
    if(!sprites[sprite])
    {
        printf("load sprite %d\n", sprite);
        char spritename[128];
        snprintf(spritename, sizeof(spritename), "%s/sprites/%d.bmp", datadir, sprite);
        SDL_Surface *surf = SDL_LoadBMP(spritename);
        uint32_t colorkey = SDL_MapRGB(surf->format, 0xFF, 0x00, 0xFF);
        SDL_SetColorKey(surf, SDL_TRUE, colorkey);
        sprites[sprite] = SDL_CreateTextureFromSurface(rend, surf);
    }

    const SDL_Rect srcrect = { 0, 0, 32, 32 };
    const SDL_Rect dstrect = { x, y, 32, 32 };
    SDL_RenderCopy(rend, sprites[sprite], &srcrect, &dstrect);
}

void render(struct world_t *world, SDL_Renderer *rend)
{
    struct camera_t *camera = &world->camera;
    struct player_t *player = &world->player;
    for(llong x = camera->pos.x + camera->size.x, i = camera->size.x; x >= camera->pos.x; --x, --i)
        for(llong y = camera->pos.y, j = camera->size.y; y <= camera->pos.y + camera->size.y; ++y, --j)
        {
            enum sprite_t sprite, back;
            struct tile_t *tile = tile_get(world, x, y);
            if(tile)
            {
                sprite = tile->sprite;
                back = tile->background;
                llong offs_x = 0, offs_y = 0;
                if(x == player->pos.x && y == player->pos.y)
                {
                    sprite = player->sprite;
                    offs_x = player->offset.x;
                    offs_y = player->offset.y;
                }

                sprite_draw(back, i * 32, j * 32, rend);
                sprite_draw(sprite, i * 32 + offs_x, j * 32 + offs_y, rend);
            }
        }

    //putsxy(rend, 0, 0, "pos: (%lld, %lld)", player->pos.x, player->pos.y);
    //putsxy(rend, 0, 16, "block: (%d, %d)", ROUND_BLOCK(player->pos.x), ROUND_BLOCK(player->pos.y));
    //putsxy(rend, 0, 32, "blocklen: %d", world->blocklen);
    SDL_RenderPresent(rend);
}
