#include "globals.h"

void init4sdl(struct world_t *world)
{
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

    window_width = 640;
    window_height = 480;

    window = SDL_CreateWindow(PROGRAM_NAME " - " VERSION_INFO,
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              window_width, window_height,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
    rend = SDL_CreateRenderer(window, -1,
                              SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    world->interface = &iface_sdl2;
    world->userdata = rend;

    world->camera.pos.x = -10;
    world->camera.pos.y = -10;

    l_init(world, window_width, window_height);

    atexit(SDL_Quit);
}

static void draw_clear(void *userdata)
{
    (void) userdata;
    //SDL_RenderClear(userdata);
}

static void draw_sprite(void *userdata, uint x, uint y, sprite_t sprite)
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

    if(!sprites[sprite])
        fatal("failed to load sprite %d", sprite);

    const SDL_Rect srcrect = { 0, 0, 32, 32 };
    const SDL_Rect dstrect = { x, y, 32, 32 };
    SDL_RenderCopy(rend, sprites[sprite], &srcrect, &dstrect);
}

static void draw_text(void *userdata, uint x, uint y, const char *fmt, ...)
{
    SDL_Renderer *rend = userdata;
    static TTF_Font *font = NULL;
    if(!font)
    {
        char fontfile[128];
        snprintf(fontfile, sizeof(fontfile), "%s/LiberationMono-Regular.ttf", datadir);
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

static void draw_update(void *userdata)
{
    SDL_Renderer *rend = userdata;
    SDL_RenderPresent(rend);
}

static ullong mytime(void *userdata)
{
    (void) userdata;
    return SDL_GetTicks();
}

const struct interface_t iface_sdl2 = {
    draw_clear,
    draw_sprite,
    draw_text,
    draw_update,
    myfopen,
    myfwrite,
    myfread,
    myferror,
    myfclose,
    fatal,
    mytime,
    printf
};

void sdl2_update(struct world_t *world)
{
    static int elapsed = 0;
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
            exit(0);
        case SDL_KEYDOWN:
            switch(ev.key.keysym.sym)
            {
            case SDLK_LEFT:
                l_movecam(world, -8, 0);
                break;
            case SDLK_RIGHT:
                l_movecam(world, 8, 0);
                break;
            case SDLK_UP:
                l_movecam(world, 0, 8);
                break;
            case SDLK_DOWN:
                l_movecam(world, 0, -8);
                break;
#if 0
            case SDLK_BACKQUOTE:
                console_enter(world);
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
                break;
            }
            break;
        }
        elapsed += SDL_GetTicks() - before;
        if(elapsed >= 100)
        {
            elapsed = 0;
            //animate_view(world);
        } /* switch */
    } /* while */
}
