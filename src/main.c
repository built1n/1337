#include "globals.h"

void render(struct world_t *world)
{
    char buf[80][24];
    for(int i = 0; i < 80; ++i)
    {
        for(int j = 0; j < 24; ++j)
        {
            buf[i][j] = (tile_get(world, i, j)->sprite == SPRITE_EMPTY) ? 'E' : '?';
            //putchar(buf[i][j]);
        }
    }

}

int main(int argc, char *argv[])
{
    struct world_t *world = malloc(sizeof(struct world_t));
    memset(world, 0, sizeof(*world));
    //if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
    //{
    //    fatal("SDL init failed");
    //}

    //atexit(SDL_Quit);

    world->camera.size.x = 80;
    world->camera.size.y = 24;
    world->camera.pos.x = 0;
    world->camera.pos.y = 0;
    tile_get(world, 0, 0);
    tile_get(world, -1, -1);
    while(1)
    {
        generate_view(world);
        render(world);
        world->camera.pos.x += 1;
        world->camera.pos.y += 1;
    }
}
