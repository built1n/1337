#include <1337/1337.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <curses.h>

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
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

extern sprite_t obstacles[8], random_obstacles[4], enemies[1];
extern const struct interface_t iface_sdl2, iface_curses;

void fatal(const char*, ...);
void console_enter(struct world_t*, SDL_Window*, SDL_Renderer*);
void vid_reset(void);
void vid_printf(SDL_Window*, SDL_Renderer*, const char*, ...);

/* custom RNG */
uint8_t myrand(void);
void mysrand(uint64_t, uint64_t);

/* file I/O wrappers */
void *myfopen(const char *file, const char *mode);
size_t myfwrite(const void *buf, size_t bytes, void *filehandle);
size_t myfread(void *buf, size_t bytes, void *filehandle);
int myferror(void *filehandle);
void myfclose(void *filehandle);

void init4sdl(struct world_t*);
void init4curses(struct world_t*);
void sdl2_update(struct world_t*);

/* tile data i/o */

void tiledata_write(void *filehandle, void *userdata);

void tiledata_read(void *filehandle, void **userdata);
