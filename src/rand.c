#include "globals.h"

static uint64_t rand_state;

#define RAND_A1 12
#define RAND_A2 25
#define RAND_A3 27

#define RAND_C1 4101842887655102017LL
#define RAND_C2 2685821657736338717LL

void mysrand(uint64_t seed)
{
    if(!seed)
    {
        seed = RAND_C1;
    }
    rand_state = seed;
}

uint64_t myrand(void)
{
    rand_state ^= rand_state >> RAND_A1;
    rand_state ^= rand_state << RAND_A2;
    rand_state ^= rand_state >> RAND_A3;
    return rand_state * RAND_C2;
}
