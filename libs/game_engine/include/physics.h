#ifndef PHYSICS_H
#define PHYSICS_H
#include "SDL3/SDL_stdinc.h"
#include <config.h>
#include <stdbool.h>
void update_pos(float *x, float *y, float *xvel, float *yvel);
void boundary_condition(float *x, float *y, float *xvel, float *yvel,
                        bool *touchedground);
void collision_system(float *x, float *y, float *xvel, float *yvel,
                      bool *grounded, Block *blocks, int number_blocks);
#endif