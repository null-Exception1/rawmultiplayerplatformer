#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

const int h = 30;
const int w = 30;
const int g = 4;
void draw_rect(float x, float y, float w, float h, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_FRect rect = {x, y, w, h};
  SDL_RenderFillRect(renderer, &rect);
}

void boundary_condition(float *x, float *y, float *xvel, float *yvel) {
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
    *yvel = -(*yvel * 0.99f);
  }
  if (*y >= WINDOW_HEIGHT - h) {
    *y = (float)(WINDOW_HEIGHT - h);
    if (SDL_fabsf(*yvel) < 5.0f) {
      *yvel = 0.0f;
    } else {
      *yvel = -(*yvel * 0.8f);
    }
  }
}

int main(int argc, char *argv[]) {

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("Teri ma ki chut", WINDOW_WIDTH, WINDOW_HEIGHT,
                       SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);

  SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

  float x = 0;
  float y = 0;

  float xvel = 0;
  float yvel = 0;
  bool isrunning = true;
  while (isrunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        isrunning = false;
      }
      const _Bool *keystate = SDL_GetKeyboardState(NULL);

      if (keystate[SDL_SCANCODE_W] && SDL_fabsf(yvel) < 10.0f) {
        yvel = 100;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const _Bool *keystate = SDL_GetKeyboardState(NULL);

    if (keystate[SDL_SCANCODE_A]) {
      xvel -= 1;
    }
    if (keystate[SDL_SCANCODE_D]) {
      xvel += 1;
    }
    if (keystate[SDL_SCANCODE_S]) {
      yvel += 1;
    }
    if (keystate[SDL_SCANCODE_W]) {
      yvel -= 1;
    }

    yvel += g;
    xvel *= 0.90;
    yvel *= 0.90;

    x += xvel;
    y += yvel;

    boundary_condition(&x, &y, &xvel, &yvel);
    draw_rect(x, y, w, h, renderer);
    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return EXIT_SUCCESS;
}
