#include "globals.h"

uint window_width, window_height;

char *datadir;

void draw_clear(void *userdata)
{

}

void draw_sprite(void *userdata, uint x, uint y, sprite_t sprite)
{
    SDL_Renderer *rend = userdata;
    static SDL_Texture *sprites[32] = { NULL };
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

void draw_text(void *userdata, uint x, uint y, const char *fmt, ...)
{
    SDL_Renderer *rend = userdata;
    static TTF_Font *font = NULL;
    if(!font)
    {
        char fontfile[128];
        snprintf(fontfile, sizeof(fontfile), "LiberationMono-Regular.ttf");
        font = TTF_OpenFont(fontfile, 16);

        if(!font)
            fatal("failed to open font: %s", TTF_GetError());
    }

    va_list ap;
    va_start(ap, fmt);
    char buf[128];
    vsnprintf(buf, sizeof(buf), fmt, ap);

    const SDL_Color color = { 0xff, 0xff, 0xff, 0xff };
    SDL_Surface *text = TTF_RenderText_Solid(font, buf, color);

    if(text)
    {
        SDL_Texture *t = SDL_CreateTextureFromSurface(rend, text);
        SDL_Rect dest = { x, y, text->w, text->h };
        SDL_RenderCopy(rend, t, NULL, &dest);
        SDL_DestroyTexture(t);
    }

    SDL_FreeSurface(text);

    va_end(ap);
}

void draw_update(void *userdata)
{
    SDL_Renderer *rend = userdata;
    SDL_RenderPresent(rend);
}

size_t myfwrite(const void *buf, size_t bytes, void *filehandle)
{
    return fwrite(buf, 1, bytes, filehandle);
}

size_t myfread(void *buf, size_t bytes, void *filehandle)
{
    return fread(buf, 1, bytes, filehandle);
}

const struct interface_t sdl2_iface = {
    draw_clear,
    draw_sprite,
    draw_text,
    draw_update,
    fopen,
    myfwrite,
    myfread,
    ferror,
    fclose
};

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    /* no need to call srand() here, it's called for each block generated */

    datadir = getcwd(NULL, 0);

    //block_setdir("blocks");


    struct world_t *world = malloc(sizeof(struct world_t));
    memset(world, 0, sizeof(*world));
    world->interface = &sdl2_iface;

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
    window = SDL_CreateWindow("a", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              1920, 1200, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    rend = SDL_CreateRenderer(window, -1,
                              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    world->userdata = rend;

    window_width = 1920;
    window_height = 1200;

    l_init(world, window_width, window_height);

    atexit(SDL_Quit);

    world->camera.pos.x = -10;
    world->camera.pos.y = -10;

    int elapsed = 0;
    while(1)
    {
        l_gen(world);
        l_render(world);

        SDL_Event ev;
        int before = SDL_GetTicks();
        SDL_WaitEventTimeout(NULL, 100);
        elapsed += SDL_GetTicks() - before;
        if(elapsed >= 100)
        {
            elapsed = 0;
            //animate_view(world);
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
                    //player_move(world, -1, 0);
                    break;
                case SDLK_RIGHT:
                    //player_move(world, 1, 0);
                    break;
                case SDLK_UP:
                    //player_move(world, 0, -1);
                    break;
                case SDLK_DOWN:
                    //player_move(world, 0, 1);
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
                    l_resize(world, window_width, window_height);
                    SDL_RenderPresent(rend);
                    break;
                }
                break;
            }
            elapsed += SDL_GetTicks() - before;
            if(elapsed >= 100)
            {
                elapsed = 0;
                //animate_view(world);
            }
        }
    }
}
