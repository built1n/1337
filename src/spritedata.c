#include "globals.h"

uint obstacles[] = {
    SPRITE_ROCK1,
    SPRITE_TREE1,
    SPRITE_TREE2,
    SPRITE_TREE3,
    SPRITE_ENEMY1_FRAME1,
    SPRITE_ENEMY1_FRAME2,
    SPRITE_ENEMY1_FRAME3,
    SPRITE_ENEMY1_FRAME4,
};

uint random_obstacles[] = {
    SPRITE_ROCK1,
    SPRITE_TREE1,
    SPRITE_TREE2,
    SPRITE_TREE3,
};

uint enemies[] = {
    SPRITE_ENEMY1_FRAME1,
};

struct anim_def_t anim_data[] = {
    { SPRITE_ENEMY1_FRAME1, 4 },
};
