#include "physics.h"
#include "config.h"
void update_pos(float *x, float *y, float *xvel, float *yvel) {

  *x += *xvel;
  *y += *yvel;
}
void boundary_condition(float *x, float *y, float *xvel, float *yvel,
                        bool *touchedground) {
  if (*x < 0) {
    *x = 0;
    *xvel = -(*xvel * 0.99f);
  }
  if (*x > WINDOW_WIDTH - w) {
    *x = (float)(WINDOW_WIDTH - w);
    *xvel = -(*xvel * 0.99f);
  }
  if (*y < 0) {
    *y = 0;
    *yvel = 0.0f;
  }
  if (*y >= WINDOW_HEIGHT - h) {
    *y = (float)(WINDOW_HEIGHT - h);
    *yvel = 0.0f;
    *touchedground = true;
  }
}

#include <physics.h>
#include <stdbool.h>

// stupid collision system i made when 9th grade
void collision_system(float *x, float *y, float *xvel, float *yvel,
                      bool *grounded, Block *blocks, int number_blocks) {
  *grounded = false;

  for (int i = 0; i < number_blocks; i++) {
    float cellx = blocks[i].x;
    float celly = blocks[i].y;

    float bruhx = *x + *xvel;
    float bruhy = *y;

    float points_x[4][2] = {{bruhx + w, bruhy},
                            {bruhx, bruhy},
                            {bruhx, bruhy + h},
                            {bruhx + w, bruhy + h}};

    for (int j = 0; j < 4; j++) {
      if (points_x[j][0] >= cellx && points_x[j][0] <= cellx + block_size &&
          points_x[j][1] >= celly && points_x[j][1] <= celly + block_size) {

        if (*xvel < 0.0f) {
          *x = cellx + block_size + 0.01f;
        }
        if (*xvel > 0.0f) {
          *x = cellx - w - 0.01f;
        }
        *xvel = 0.0f;
        break;
      }
    }

    bruhx = *x;
    bruhy = *y + *yvel;

    float points_y[4][2] = {{bruhx + w, bruhy},
                            {bruhx, bruhy},
                            {bruhx, bruhy + h},
                            {bruhx + w, bruhy + h}};

    for (int j = 0; j < 4; j++) {
      if (points_y[j][0] >= cellx && points_y[j][0] <= cellx + block_size &&
          points_y[j][1] >= celly && points_y[j][1] <= celly + block_size) {

        if (*yvel < 0.0f) {
          *y = celly + block_size + 0.01f;
          *yvel = 0.0f;
        } else if (*yvel > 0.0f) {
          *y = celly - h - 0.01f;
          *grounded = true;
          *yvel = 0.0f;
        }
        break;
      }
    }
  }
}
