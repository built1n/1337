#include "globals.h"

uint window_width, window_height;

char *datadir;

void *myfopen(const char *file, const char *mode)
{
    char filename[128];
    snprintf(filename, sizeof(filename), "%s/blocks", datadir);
    char *oldcwd = getcwd(NULL, 0);
    if(chdir(filename) < 0)
    {
        printf("blocks dir does not exist, creating...\n");
        mkdir(filename, 0755);
    }
    chdir(oldcwd);
    snprintf(filename, sizeof(filename), "%s/blocks/%s", datadir, file);
    return fopen(filename, mode);
}

size_t myfwrite(const void *buf, size_t bytes, void *filehandle)
{
    return fwrite(buf, 1, bytes, filehandle);
}

size_t myfread(void *buf, size_t bytes, void *filehandle)
{
    return fread(buf, 1, bytes, filehandle);
}

int myferror(void *filehandle)
{
    return ferror(filehandle);
}

void myfclose(void *filehandle)
{
    fclose(filehandle);
}

static void mygen(struct block_t *block)
{
    /* re-seed the RNG to make blocks the same across games */
    mysrand(block->coords.x / BLOCK_DIM, block->coords.y / BLOCK_DIM);

    for(int i = 0; i < 100; ++i)
    {
        uint8_t x = myrand() % BLOCK_DIM;
        uint8_t y = myrand() % BLOCK_DIM;
        uint8_t sprite = myrand() % ARRAYLEN(random_obstacles);
        block->tiles[x][y].sprite = random_obstacles[sprite];
    }

    for(int y = 0; y < BLOCK_DIM; ++y)
    {
        if(y == 0 || y == BLOCK_DIM - 1)
            for(int i = 0; i < BLOCK_DIM; ++i)
                block->tiles[i][y].sprite = SPRITE_TREE1;
        else
        {
            block->tiles[0][y].sprite = SPRITE_TREE1;
            block->tiles[BLOCK_DIM-1][y].sprite = SPRITE_TREE1;
        }
    }

#if 0
    /* add an animated enemy */
    uint enemy_x = myrand() % BLOCK_DIM, enemy_y = myrand() % BLOCK_DIM;
    struct tile_t *tile = &(block->tiles[enemy_x][enemy_y]);
    tile->sprite = enemies[myrand() % ARRAYLEN(enemies)];
    tile->data.anim.frame = 0;
    tile->data.anim.type_idx = anim_find(tile->sprite);

    struct anim_tilelist *node = malloc(sizeof(struct anim_tilelist));
    node->next = block->anim_tiles;
    node->coords.x = enemy_x;
    node->coords.y = enemy_y;
    block->anim_tiles = node;
#endif
}

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    /* no need to call srand() here, it's called for each block generated */

    datadir = getcwd(NULL, 0);

    struct world_t *world = malloc(sizeof(struct world_t));
    memset(world, 0, sizeof(*world));

    enum interface_t { SDL, CURS } interface = SDL;
    if(argc > 1 && strcmp(argv[1], "--curses") == 0)
        interface = CURS;

    switch(interface)
    {
    case SDL:
        init4sdl(world);
        break;
    case CURS:
        init4curses(world);
        break;
    }

    l_setgen(world, mygen);

    while(1)
    {
        l_gen(world);
        l_render(world);

        if(interface == SDL)
        {
            sdl2_update(world);
        }
        else if(interface == CURS)
        {
            switch(getch())
            {
            case KEY_LEFT:
                l_movecam(world, -32, 0);
                break;
            case KEY_RIGHT:
                l_movecam(world, 32, 0);
                break;
            case KEY_UP:
                l_movecam(world, 0, 32);
                break;
            case KEY_DOWN:
                l_movecam(world, 0, -32);
                break;
            }
        }
    } /* while */
}
