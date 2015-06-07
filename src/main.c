#include "globals.h"

SDL_Texture *sprites[MAX_SPRITES] = { NULL };

void render(struct world_t *world, SDL_Window *window, SDL_Renderer *rend)
{
    char spritename[32];
    struct camera_t *camera = &world->camera;
    for(llong x = camera->pos.x + camera->size.x, i = 0; x >= camera->pos.x; --x, ++i)
        for(llong y = camera->pos.y, j = 0; y < camera->pos.y + camera->size.y; ++y, ++j)
        {
            enum sprite_t sprite;
            sprite = tile_get(world, x, y)->sprite;
            //sprite = 1;

            snprintf(spritename, sizeof(spritename), "sprites/%d.bmp", sprite);
            if(!sprites[sprite])
                sprites[sprite] = SDL_CreateTextureFromSurface(rend, SDL_LoadBMP(spritename));

            const SDL_Rect srcrect = { 0, 0, 32, 32 };
            const SDL_Rect dstrect = { i * 32, j * 32, 32, 32 };
            SDL_RenderCopy(rend, sprites[sprite], &srcrect, &dstrect);
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

    world->camera.size.x = 80;
    world->camera.size.y = 24;
    world->camera.pos.x = 0;
    world->camera.pos.y = 0;

    while(1)
    {
        generate_view(world);
        render(world, window, rend);

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
                    break;
                case SDLK_RIGHT:
                    world->camera.pos.x -= 1;
                    break;
                case SDLK_UP:
                    world->camera.pos.y -= 1;
                    break;
                case SDLK_DOWN:
                    world->camera.pos.y += 1;
                    break;
                }
            }
        }
    }
}
