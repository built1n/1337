#include "globals.h"

void vputsxy(SDL_Renderer *rend, int x, int y, const char *fmt, va_list ap)
{
    static TTF_Font *font = NULL;
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    if(!font)
    {
        char fontfile[128];
        snprintf(fontfile, sizeof(fontfile), "%s/LiberationMono-Regular.ttf", datadir);
        font = TTF_OpenFont(fontfile, 16);

        if(!font)
            fatal("failed to open font: %s", TTF_GetError());
    }

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
}

void putsxy(SDL_Renderer *rend, int x, int y, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vputsxy(rend, x, y, fmt, ap);
    va_end(ap);
}

static int curs_x = 0, curs_y = 0;

void vid_reset(void)
{
    curs_x = 0;
    curs_y = 0;
}

void vid_printf(SDL_Window *wind, SDL_Renderer *rend, const char *fmt, ...)
{
    int w, h;
    SDL_GetWindowSize(wind, &w, &h);
    va_list ap;
    va_start(ap, fmt);
    char buf[128];
    vsnprintf(buf, sizeof(buf), fmt, ap);

    char *ptr = buf;
    while(*ptr)
    {
        if(*ptr != '\n' && ptr)
        {
            putsxy(rend, curs_x, curs_y, "%c", *ptr);
            curs_x += 8;
            if(curs_x >= w)
            {
                curs_y = (curs_y + 20) % h;
                curs_x = 0;
            }
        }
        else if(*ptr == '\b')
        {
            curs_x = MAX(0, curs_x - 8);
            putsxy(rend, curs_x, curs_y, " ");
        }
        else if(*ptr)
        {
            curs_y = (curs_y + 20) % h;
            curs_x = 0;
        }
        ptr++;
    }

    SDL_RenderPresent(rend);

    va_end(ap);
}
