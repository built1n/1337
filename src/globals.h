#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <zlib.h>

/* some terminology:
   tile: contains a sprite and properties
   block: 64x64 section of tiles
   world: list of blocks
*/

/* all coordinates are world coordinates */

#define PROGRAM_NAME "Project 1337"

/* VERSION_INFO is supplied by the compiler */
#define PROGRAM_VERSION "prerelease (" VERSION_INFO ")"

#define ABS(x) (((x)<0)?-(x):(x))
#define ARRAYLEN(x) (sizeof(x) / sizeof(x[0]))
#define CEIL(x) (ceil((double)x))
#define FLOOR(x) (floor((double)x))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (( (a) > (b) ) ? (a) : (b))

#define LCD_WIDTH 1920
#define LCD_HEIGHT 1200

/* ROUND_BLOCK(x) rounds x to the next lowest multiple of BLOCK_DIM */
#define ROUND_BLOCK(x) (llong)((FLOOR((x)/BLOCK_DIM)*BLOCK_DIM))

typedef unsigned long long ullong;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef long long llong;

/* must be a power of two */
#define BLOCK_DIM 64

/* a maximum of LOCAL_DIM * LOCAL_DIM * 4 blocks can be stored in RAM at once */
/* should be bigger than the camera size */
#define LOCAL_DIM 2

/* keep this small for now */
#define MAX_SPRITES 32

#define GEN_MAX_TREES 1000000

/* distance in tiles from the edge of the screen the player needs to be for the
   camera to scroll */
#define SCROLL_DISTANCE 5

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

struct anim_def_t {
    enum sprite_t start;
    uint len;
};

struct anim_t {
    uchar frame;

    /* index in anim_data */
    uint type_idx;
};

struct tile_t {
    enum sprite_t sprite;
    enum sprite_t background;
    union {
        struct anim_t anim;
    } data;
};

struct coords_t {
    llong x, y;
};

struct anim_tilelist {
    struct coords_t coords;
    struct anim_tilelist *next;
};

struct block_t {
    struct tile_t tiles[BLOCK_DIM][BLOCK_DIM];
    struct anim_tilelist *anim_tiles;
    struct coords_t coords;
    struct block_t *next;
};

struct player_t {
    /* in tiles */
    struct coords_t pos;

    /* in pixels */
    struct coords_t offset;
    enum sprite_t sprite;
};

struct camera_t {
    struct coords_t pos;
    struct coords_t size;
};

struct world_t {
    struct block_t *blocks;
    uint blocklen;
    struct player_t player;
    struct camera_t camera;
    SDL_mutex *mutex;
};

extern uint window_width, window_height;
extern char *datadir;
extern char *blockdir;

enum sprite_t obstacles[8];
enum sprite_t random_obstacles[4];
enum sprite_t enemies[1];
struct anim_def_t anim_data[1];

/* utility functions */
void fatal(const char*, ...);
void vid_printf(SDL_Window*, SDL_Renderer*, const char*, ...);
void vid_reset(void);
uint32_t myrand(void);
void mysrand(uint32_t);

/* game-related functions */
bool player_move(struct world_t *world, llong dx, llong dy);
struct block_t *block_get(struct world_t*, llong, llong);
struct block_t *block_load(llong, llong);
struct block_t *block_new(llong, llong);
struct tile_t *tile_get(struct world_t*, llong, llong);
uint anim_find(enum sprite_t);
void animate_view(struct world_t*);
void block_add(struct world_t*, struct block_t*);
void block_purge(struct world_t*);
void block_setdir(const char*);
void block_update(struct block_t *);
void console_enter(struct world_t*, SDL_Window*, SDL_Renderer*);
void generate_view(struct world_t*);
void putsxy(SDL_Renderer*, int, int, const char*, ...);
void render(struct world_t *world, SDL_Renderer*);
void on_resize(struct world_t*);
