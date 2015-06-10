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
            if(tile)
            {
                sprite = tile->sprite;
                back = tile->background;
                if(x == player->pos.x && y == player->pos.y)
                {
                    sprite = player->sprite;
                }

                sprite_draw(back, i * 32, j * 32, rend);
                sprite_draw(sprite, i * 32, j * 32, rend);
            }
        }

    putsxy(rend, 0, 0, "pos: (%lld, %lld)", player->pos.x, player->pos.y);
    putsxy(rend, 0, 16, "block: (%d, %d)", ROUND_BLOCK(player->pos.x), ROUND_BLOCK(player->pos.y));
    putsxy(rend, 0, 32, "blocklen: %d", world->blocklen);
    SDL_RenderPresent(rend);
}

uint window_width, window_height;

void on_resize(struct world_t *world)
{
    world->camera.size.x = window_width/32;
    world->camera.size.y = window_height/32;
    printf("window resized to %d %d\n", world->camera.size.x, world->camera.size.y);
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
                case SDLK_p:
                    block_purgeall(world);
                    break;
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
        }
    }
}
