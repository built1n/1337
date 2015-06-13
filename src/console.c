#include "globals.h"

static int add_char(SDL_Window *wind, SDL_Renderer *rend, char *buf, size_t buflen, int bufidx, int key)
{
    if((('a' <= key && key <= 'z') || key == ' ' || key == '-' || ('0' <= key && key <= '9')) && bufidx < buflen)
    {
        buf[bufidx++] = (char)key;
        vid_printf(wind, rend, "%c", key);
    }
    if(key == '\b')
    {
        printf("got backspace %d\n", bufidx);
        bufidx = MAX(0, bufidx - 1);
        printf("%d\n", bufidx);
    }
    return bufidx;
}

void console_enter(struct world_t *world, SDL_Window *wind, SDL_Renderer *rend)
{
    char buf[128];
    int bufidx = 0;
    vid_reset();
    vid_printf(wind, rend, "Entered debug console.\nPress tilde again to exit.\n");
    while(1)
    {
        vid_printf(wind, rend, "> ");
        bufidx = 0;
        memset(buf, 0, sizeof(buf));

        while(1)
        {
            SDL_WaitEvent(NULL);
            SDL_Event ev;
            while(SDL_PollEvent(&ev))
            {
                switch(ev.type)
                {
                case SDL_QUIT:
                    exit(1);
                case SDL_KEYDOWN:
                    switch(ev.key.keysym.sym)
                    {
                    case SDLK_BACKQUOTE:
                        return;
                    case SDLK_RETURN:
                        goto exec_cmd;
                    default:
                        bufidx = add_char(wind, rend, buf, sizeof(buf), bufidx, ev.key.keysym.sym);
                        break;
                    }
                    break;
                case SDL_WINDOWEVENT:
                    switch(ev.window.event)
                    {
                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        window_width = ev.window.data1;
                        window_height = ev.window.data2;
                         on_resize(world);
                        SDL_RenderPresent(rend);
                        break;
                    }
                    break;
                }

            }
        }
        bufidx = add_char(wind, rend, buf, sizeof(buf), bufidx, '\0');
    exec_cmd:
        vid_printf(wind, rend, "\n");
        char *cmd = strtok(buf, " ");
        if(strcmp(cmd, "teleport") == 0)
        {
            char *xs = strtok(NULL, " ");
            vid_printf(wind, rend, "x is %s\n", xs);
            if(xs)
            {
                llong x = strtoll(xs, NULL, 0);
                char *ys = strtok(NULL, " ");
                vid_printf(wind, rend, "y is %s\n", ys);
                if(ys)
                {
                    llong y = strtoll(ys, NULL, 0);
                    world->camera.pos.x = x - 8;
                    world->camera.pos.y = y - 8;
                    world->player.pos.x = x;
                    world->player.pos.y = y;
                    generate_view(world);
                    render(world, rend);
                    vid_printf(wind, rend, "teleport successful\n");
                }
            }
        }
        else if(strcmp(cmd, "purge") == 0)
        {
            extern void block_purgeall(struct world_t*);
            block_purgeall(world);
            vid_printf(wind, rend, "all blocks written to disk\n");
            generate_view(world);
        }
        else if(strcmp(cmd, "info") == 0)
        {
            vid_printf(wind, rend, "player position: (%lld, %lld)\n",
                       world->player.pos.x, world->player.pos.y);
            vid_printf(wind, rend, "blocklen: %d\n", world->blocklen);
        }
        else
        {
            vid_printf(wind, rend, "unknown command: %s\n", cmd);
        }
    }
}
