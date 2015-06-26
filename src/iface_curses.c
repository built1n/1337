#include "globals.h"

static void curses_cleanup(void)
{
    endwin();
}

void init4curses(struct world_t *world)
{
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    atexit(curses_cleanup);
    int x, y;
    getmaxyx(stdscr, y, x);
    world->interface = &iface_curses;
    l_init(world, x * 32, y * 32);
}

static void draw_clear(void *userdata)
{
    (void) userdata;
    clear();
}

static void draw_sprite(void *userdata, uint x, uint y, sprite_t sprite)
{
    (void) userdata;
    char c = ' ';
    switch(sprite)
    {
    case SPRITE_ROCK1:
        c = 'R';
        break;
    case SPRITE_TREE1:
    case SPRITE_TREE2:
    case SPRITE_TREE3:
        c = 'T';
        break;
    }
    mvaddch(y/32, x/32, c);
}

static void draw_text(void *userdata, uint x, uint y, const char *fmt, ...)
{
    (void) userdata;
    va_list ap;
    va_start(ap, fmt);
    char buf[128];
    vsnprintf(buf, sizeof(buf), fmt, ap);

    mvaddstr(y/32, x/32, buf);

    va_end(ap);
}

static void draw_update(void *userdata)
{
    (void) userdata;
    refresh();
}

static ullong my_mstime(void *userdata)
{
    (void) userdata;
    struct timespec t;

    clock_gettime(CLOCK_MONOTONIC, &t);

    ullong ms = t.tv_sec * 1000 + t.tv_nsec / 1000000;
    return ms;
}

int dummy_printf(const char *fmt, ...)
{
    (void) fmt;
    return 0;
}

const struct interface_t iface_curses = {
    draw_clear,
    draw_sprite,
    draw_text,
    draw_update,
    myfopen,
    myfwrite,
    myfread,
    myferror,
    myfclose,
    fatal,
    my_mstime,
    dummy_printf
};
