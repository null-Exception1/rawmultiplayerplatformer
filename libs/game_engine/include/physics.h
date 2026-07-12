#ifndef PHYSICS_H
#define PHYSICS_H
#include <stdbool.h>

void update_pos(float *x, float *y, float *xvel, float *yvel);
void boundary_condition(float *x, float *y, float *xvel, float *yvel,
                        bool *touchedground);
#endif