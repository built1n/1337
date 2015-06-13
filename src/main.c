#include "globals.h"

uint window_width, window_height;

void on_resize(struct world_t *world)
{
    world->camera.size.x = CEIL(window_width/32);
    world->camera.size.y = CEIL(window_height/32);
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    mysrand(42);

    struct world_t *world = malloc(sizeof(struct world_t));
    memset(world, 0, sizeof(*world));
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    {
        fatal("SDL init failed: %s", SDL_GetError());
    }

    if(TTF_Init() < 0)
    {
        fatal("TTF init failed: %s", TTF_GetError());
    }

    SDL_Window *window = NULL;
    SDL_Renderer *rend = NULL;
    window = SDL_CreateWindow(PROGRAM_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              LCD_WIDTH, LCD_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    rend = SDL_CreateRenderer(window, -1,
                              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);


    window_width = LCD_WIDTH;
    window_height = LCD_HEIGHT;

    on_resize(world);

    atexit(SDL_Quit);

    world->camera.pos.x = 0;
    world->camera.pos.y = 0;
    world->player.sprite = SPRITE_PLAYER;
    world->player.pos.x = 8;
    world->player.pos.y = 8;

    int elapsed = 0;
    while(1)
    {
        generate_view(world);
        render(world, rend);

        SDL_Event ev;
        int before = SDL_GetTicks();
        SDL_WaitEventTimeout(NULL, 100);
        elapsed += SDL_GetTicks() - before;
        if(elapsed >= 100)
        {
            elapsed = 0;
            animate_view(world);
        }
        while(SDL_PollEvent(&ev))
        {
            before = SDL_GetTicks();
            switch(ev.type)
            {
            case SDL_QUIT:
                return 1;
            case SDL_KEYDOWN:
                switch(ev.key.keysym.sym)
                {
                case SDLK_LEFT:
                    player_move(world, 1, 0);
                    break;
                case SDLK_RIGHT:
                    player_move(world, -1, 0);
                    break;
                case SDLK_UP:
                    player_move(world, 0, -1);
                    break;
                case SDLK_DOWN:
                    player_move(world, 0, 1);
                    break;
#ifndef NDEBUG
                case SDLK_BACKQUOTE:
                    console_enter(world, window, rend);
                    break;
#endif
                }
                break;
            case SDL_WINDOWEVENT:
                switch(ev.window.event)
                {
                case SDL_WINDOWEVENT_SIZE_CHANGED:
                    window_width = ev.window.data1;
                    window_height = ev.window.data2;
                    on_resize(world);
                    SDL_RenderPresent(rend);
                    break;
                }
                break;
            }
            elapsed += SDL_GetTicks() - before;
            if(elapsed >= 100)
            {
                elapsed = 0;
                animate_view(world);
            }
        }
    }
}
