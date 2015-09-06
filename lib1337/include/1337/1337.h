/* 1337: a 2-D graphics engine */
/* Copyright (C) 2015 Franklin Wei */

/*
 * Some Terminology:
 * "tile" - contains a 32x32 sprite and its offset
 * "block" - a 64x64 section of tiles, addressed by its upper-left tile
 * "world" - a list of blocks and other data
 */

/* The library is designed to manage and render two types of tiles:
 * - tiles
 * - overlay tiles
 *
 * Everything else, including game logic and I/O is the user's responsibility
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

#define LAYER_TOP 0
#define LAYER_BOTTOM 1024

struct overlaytile_t {
    uint id;

    /* [ LAYER_TOP, LAYER_BOTTOM ) */
    /* NOTE: layers only apply within the tile the overlay falls on */
    /* the overlapping behavior with adjacent overlay tiles is undefined */
    uint layer;

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

#define LOG_ERROR   0
#define LOG_WARNING 1
#define LOG_INFO    2
#define LOG_VERBOSE 3
#define LOG_DEBUG   4

    int (*logf)(int level, const char *fmt, ...);
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


/******************************************************************************/
/************************ BEGIN FUNCTION DECLARATIONS *************************/
/******************************************************************************/


/*********** startup/shutdown *************/
void l_init(struct world_t*, uint window_w, uint window_h);                      // initializes a world context with the given resolution
void l_free(struct world_t*);                                                    // frees the world context and associated memory

/************ generation *************/
void l_setgen(struct world_t*, genfunc_t);                                       // sets the block generation function
void l_gen(struct world_t*);                                                     // loads/generates blocks in view of the camera, then tries to purge blocklist

/*************** camera ****************/
void l_resize(struct world_t*, uint new_w, uint new_h);                          // resize camera to a new screen size in pixels
void l_movecam(struct world_t*, llong dx, llong dy);                             // moves the camera by PIXELS

/************* tile/block access **************/
struct tile_t *l_gettile(struct world_t*, llong x, llong y);                     // returns a pointer to the tile at (x,y) or NULL if it's not in RAM
struct block_t *l_getblock(struct world_t*, llong x, llong y);                   // returns a pointer to the block at exactly (x,y) or NULL if invalid or not loaded
struct block_t *l_loadblock(struct world_t*, llong x, llong y);                  // loads or generates the block at exactly (x,y), returns pointer to block

/*************** overlay ****************/
uint l_addoverlay(struct world_t*, llong x, llong y, uint layer);                // returns the id of a new overlay tile at (x,y)
struct overlaytile_t *l_getoverlay(struct world_t*, uint id);                    // gets the overlay with the specified id
void l_moveoverlay(struct world_t*, uint id, llong x, llong y);                  // moves an overlay tile to a specific location (in px)
void l_shiftoverlay(struct world_t*, uint id, llong dx, llong dy);               // moves an overlay tile relative to it's current location (in px) NOTE: may be subject to overflow errors
void l_deloverlay(struct world_t*, uint id);                                     // deletes an overlay tile, freeing its memory

/************ block management *************/
void l_purge(struct world_t*);                                                   // swaps out blocks not visible to the camera
void l_purgeall(struct world_t*);                                                // swaps out all blocks out to disk
void l_purgeoverlay(struct world_t*);                                            // saves the chunk list to disk, needed for overlays

/*********** rendering ************/
void l_render(struct world_t*);                                                  // renders all tiles, overlays, and messages

/********** miscellaneous **********/
const char *l_version(void);                                                     // returns a pointer to a string containing the library version, should match LEET_VERSION

#endif
