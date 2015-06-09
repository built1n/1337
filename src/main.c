#include "globals.h"

SDL_Texture *sprites[MAX_SPRITES] = { NULL };

void sprite_draw(enum sprite_t sprite, int x, int y, SDL_Renderer *rend)
{
    if(!sprites[sprite])
    {
        printf("load sprite %d\n", sprite);
        char spritename[32];
        snprintf(spritename, sizeof(spritename), "sprites/%d.bmp", sprite);
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
    for(llong x = camera->pos.x + camera->size.x, i = 0; x >= camera->pos.x; --x, ++i)
        for(llong y = camera->pos.y, j = 0; y < camera->pos.y + camera->size.y; ++y, ++j)
        {
            enum sprite_t sprite, back;
            struct tile_t *tile = tile_get(world, x, y);
            sprite = tile->sprite;
            back = tile->background;
            if(x == player->pos.x && y == player->pos.y)
            {
                printf("draw player at %d, %d\n", x, y);
                sprite = player->sprite;
            }

            sprite_draw(back, i * 32, j * 32, rend);
            sprite_draw(sprite, i * 32, j * 32, rend);
        }
    SDL_RenderPresent(rend);
}

int main(int argc, char *argv[])
{
    struct world_t *world = malloc(sizeof(struct world_t));
    memset(world, 0, sizeof(*world));
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fatal("SDL init failed: %s", SDL_GetError());
    }

    SDL_Window *window = NULL;
    SDL_Renderer *rend = NULL;
    if(SDL_CreateWindowAndRenderer(640, 480, 0, &window, &rend) < 0)
        fatal("SDL init failed: %s", SDL_GetError());
    SDL_SetWindowTitle(window, PROGRAM_NAME);

    atexit(SDL_Quit);

    world->camera.size.x = 640/32;
    world->camera.size.y = 480/32;
    world->camera.pos.x = 0;
    world->camera.pos.y = 0;
    world->player.sprite = SPRITE_PLAYER;
    world->player.pos.x = 4;
    world->player.pos.y = 4;

    while(1)
    {
        generate_view(world);
        render(world, rend);

        SDL_Event ev;
        SDL_WaitEvent(NULL);
        while(SDL_PollEvent(&ev))
        {
            switch(ev.type)
            {
            case SDL_QUIT:
                return 1;
            case SDL_KEYDOWN:
                switch(ev.key.keysym.sym)
                {
                case SDLK_LEFT:
                    world->camera.pos.x += 1;
                    world->player.pos.x += 1;
                    break;
                case SDLK_RIGHT:
                    world->camera.pos.x -= 1;
                    world->player.pos.x -= 1;
                    break;
                case SDLK_UP:
                    world->camera.pos.y -= 1;
                    world->player.pos.y -= 1;
                    break;
                case SDLK_DOWN:
                    world->camera.pos.y += 1;
                    world->player.pos.y += 1;
                    break;
                }
            }
        }
    }
}
