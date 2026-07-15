#ifndef CONFIG
#define CONFIG

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define h 30
#define w 30
#define g 5
#include "SDL3/SDL_stdinc.h"
#define block_size 50
typedef struct {
  float x;
  float y;
  Uint8 type;
} Block;

#endif