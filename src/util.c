#include "globals.h"

void fatal(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    fprintf(stderr, "FATAL: %s\n", buf);

    exit(EXIT_FAILURE);
}


void putsxy(SDL_Renderer *rend, int x, int y, const char *fmt, ...)
{
    static TTF_Font *font = NULL;
    va_list ap;
    va_start(ap, fmt);
    char buf[256];
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if(!font)
    {
        font = TTF_OpenFont("LiberationMono-Regular.ttf", 16);

        if(!font)
            fatal("failed to open font: %s", TTF_GetError());
    }

    const SDL_Color color = { 0xff, 0xff, 0xff, 0xff };
    SDL_Surface *text = TTF_RenderText_Solid(font, buf, color);

    SDL_Texture *t = SDL_CreateTextureFromSurface(rend, text);
    SDL_Rect dest = { x, y, text->w, text->h };
    SDL_RenderCopy(rend, t, NULL, &dest);

    SDL_FreeSurface(text);
    SDL_DestroyTexture(t);
}
