#include "physics.h"
#include "config.h"
void update_pos(float *x, float *y, float *xvel, float *yvel) {
  *yvel += g;

  *xvel *= 0.95;
  *yvel *= 0.95;

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
