#include <1337/1337.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define ABS(x) (((x)<0)?-(x):(x))
#define ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))
#define CEIL(x) (ceil((double)x))
#define FLOOR(x) (floor((double)x))
#define MIN(a,b) (( (a) < (b) ) ? (a) : (b))
#define MAX(a,b) (( (a) > (b) ) ? (a) : (b))

extern char *datadir;
extern uint window_width, window_height;

enum sprite_t {
    SPRITE_GRASS = 0,
    SPRITE_ROCK1,
    SPRITE_TREE1,
    SPRITE_PLAYER,
    SPRITE_TREE2,
    SPRITE_TREE3,
    SPRITE_ENEMY1_FRAME1,
    SPRITE_ENEMY1_FRAME2,
    SPRITE_ENEMY1_FRAME3,
    SPRITE_ENEMY1_FRAME4
};

void fatal(const char*, ...);
uint64_t myrand(void);
void mysrand(uint64_t, uint64_t);
void console_enter(struct world_t*, SDL_Window*, SDL_Renderer*);
