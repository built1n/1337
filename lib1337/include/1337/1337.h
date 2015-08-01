/* 1337: a 2-D graphics engine */
/* Copyright (C) 2015 Franklin Wei */

/*
 * Some Terminology:
 * "tile" - contains a 32x32 sprite and its offset
 * "block" - a 64x64 section of tiles, addressed by its upper-left tile
 * "world" - a list of blocks and other data
 */

#ifndef LEET_EXPORT_HEADER
#define LEET_EXPORT_HEADER

#define LEET_VERSION VERSION_INFO

#include <stddef.h> /* for size_t */

#define BLOCK_DIM 64

typedef unsigned long long ullong;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef unsigned short ushort;
typedef unsigned char uchar;

typedef long long llong;

typedef uint sprite_t;

struct coords_t {
    llong x, y;
};

struct camera_t {
    struct coords_t pos;
    struct coords_t offset;
    struct coords_t size;
};

struct tile_t {
    sprite_t sprite;

    struct coords_t offset;

    sprite_t background;
};

struct overlaytile_t {
    uint id;

    /* DO NOT modify _coords directly, use l_moveoverlay instead */
    struct coords_t _coords;

    struct coords_t offset;

    sprite_t sprite;

    struct overlaytile_t *next;
};

struct block_t {
    struct coords_t coords;

    struct tile_t tiles[BLOCK_DIM][BLOCK_DIM];

    /* linked-list of overlay tiles in this block */
    struct overlaytile_t *overlay;

    struct block_t *next;
};

/*
 * a structure of this type needs to be filled out
 * and assigned to world->interface
 *
 * each function called is passed the value of world->userdata
 */
struct interface_t {
    /* drawing */
    void (*draw_clear)(void *userdata);
    void (*draw_sprite)(void *userdata, uint x, uint y, sprite_t);
    void (*draw_text)(void *userdata, uint x, uint y, const char*, ...);
    void (*draw_update)(void *userdata);

    /* file input/output */

    /*
     * files are opened with out directory names, it is the responsibility
     * of the I/O functions to determine their locations
     */

    void* (*fopen)(const char *file, const char *mode);
    size_t (*fwrite)(const void *buf, size_t bytes, void *filehandle);
    size_t (*fread)(void *buf, size_t bytes, void *filehandle);
    int (*ferror)(void *filehandle);
    void (*fclose)(void *filehandle);

    /* miscellaneous */
    void (*fatal)(const char *fmt, ...);
    ullong (*get_mstime)(void *userdata);
    int (*printf)(const char *fmt, ...);
};

struct world_t {
    /* private: */
    void *privatedata;

    /* user-assigned: */
    struct camera_t camera;

    const struct interface_t *interface;

    /* arbitrary, user-settable pointers */
    void *userdata;
};

typedef void (*genfunc_t)(struct world_t*, struct block_t*);

/* initializes a world context with the given resolution */
void l_init(struct world_t*, uint window_w, uint window_h);

/*
 * should be called on camera resize
 *  - sets camera size to fit the new resolution
 *  - also generates blocks in view of the new camera
 */
void l_resize(struct world_t*, uint new_w, uint new_h);

/*
 * moves the camera by an offset in PIXELS
 *  - assumes that tilesize = 32px
 */
void l_movecam(struct world_t*, llong dx, llong dy);

/* sets the block generation function */
void l_setgen(struct world_t*, genfunc_t);

/*
 * generates or loads all blocks in view of the camera
 *  - attempts to purge the block list when done
 */
void l_gen(struct world_t*);

/*
 * returns to a pointer to a tile at (x,y)
 *  - returns NULL if the block the tile is in is not loaded in memory
 */
struct tile_t *l_gettile(struct world_t*, llong x, llong y);

/*
 * returns a pointer to the block starting at (x,y)
 *  - both x and y MUST be multiples of 64
 *  - returns NULL if (x,y) does not refer to a valid block
 */
struct block_t *l_getblock(struct world_t*, llong x, llong y);

/*
 * adds an overlay tile in the block (x,y) is in
 *  - returns an integer identifier which can be used to access
 *    the tile through l_getoverlay
 */
uint l_addoverlay(struct world_t*, llong x, llong y);

/* returns a pointer to the overlay tile with the specified id */
struct overlaytile_t *l_getoverlay(struct world_t*, uint id);

/* moves an overlay to a certain tile */
/* the offset needs to be set manualloy */
void l_moveoverlay(struct world_t*, uint id, llong x, llong y);

/* deletes an overlay tile, freeing memory for it */
void l_deloverlay(struct world_t*, uint id);

/* loads or generates the block starting at (x,y) if it is not already loaded */
struct block_t *l_loadblock(struct world_t*, llong x, llong y);

/* purges blocks outside a certain distance from the camera */
void l_purge(struct world_t*);

/* purges all blocks to disk */
void l_purgeall(struct world_t*);

/* purges the chunk list, needed for overlay consistency across saves */
void l_purgeoverlay(struct world_t*);

/* renders the visible tiles to screen via calls to draw_sprite */
void l_render(struct world_t*);

/* frees the world context and associated memory */
void l_free(struct world_t*);

/*
 * returns a pointer to a statically allocated string containing the library version
 *  - the string MUST match LEET_VERSION exactly
 */
const char *l_version(void);

#endif
