#include <1337/1337.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define ABS(x) (((x)<0)?-(x):(x))
#define ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))
#define CEIL(x) (ceil((double)x))
#define FLOOR(x) (floor((double)x))
#define MIN(a,b) (( (a) < (b) ) ? (a) : (b))
#define MAX(a,b) (( (a) > (b) ) ? (a) : (b))

extern char *datadir;
extern uint window_width, window_height;
