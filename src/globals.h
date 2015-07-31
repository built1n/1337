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
    SPRITE_STORE1,
    LAST_SPRITE
};

enum itemtype {
    ITEM_ARMOR_WOOD,
    ITEM_ARMOR_IRON,
    ITEM_ARMOR_STEEL,
    ITEM_ARMOR_LEGENDARY,

    ITEM_SWORD_WOODEN,
    ITEM_SWORD_IRON,
    ITEM_SWORD_STEEL,
    ITEM_SWORD_LEGENDARY,
    ITEM_BATTLEAXE,
    ITEM_MACE_IRON,
    ITEM_MACE_STEEL,

    ITEM_SLING,
    ITEM_LONGBOW,
    ITEM_CROSSBOW,
    ITEM_MUSKET,
    ITEM_PISTOL,
    ITEM_RIFLE,
    ITEM_HANDCANNON,
};

enum itemclass {
    ITEMCLASS_ARMOR,
    ITEMCLASS_WEAPON_MELEE,
    ITEMCLASS_WEAPON_RANGED,
    ITEMCLASS_GOOD,
};

struct weapondata_t {
    uint range; /* 0 = melee */
    uint dmg;
};

struct armordata_t {
    uint hp;
    uint maxhp;
};

struct tradegood_data {
    uint count;
    uint purchased;
};

struct tradegood_t {
    const char *name;
    uint baseprice;
    int gradient; /* in tenths of credits over x = [1,10] */
};

struct tradegood_t tradegoods[3];

struct item_t {
    enum itemtype item;
    uint weight; /* in grams */
    enum { GRAMS, KILOGRAMS } units;

    enum itemclass class;
    union {
        struct weapondata_t weapondata;
        struct armordata_t armordata;
        struct tradegood_data tradegood;
    } data;
};

struct aiplayer_t {
    enum aiclass { AI_FRIENDLY, AI_NEUTRAL, AI_ENEMY } class;
    uint hp;

    struct item_t *armor;
    struct itme_t *weapon1;
    struct item_t *weapon2;
    struct item_t *goods;
};

struct player_t {
    /* base hp */
    uint hp;
    uint credits; /* tenths of credits */

    /* armor takes damage first, then the player */
    struct item_t *armor;
    struct item_t *weapon1;
    struct item_t *weapon2;
    struct item_t *goods;
};

struct userdata_t {
    struct player_t player;
    void *iface_data;
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
