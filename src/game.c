#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <config.h>
#include <keyboard.h>
#include <physics.h>
#include <rendering.h>
#include <stdlib.h>
static bool touchedground = false;

int main(int argc, char *argv[]) {

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window *window =
      SDL_CreateWindow("tmkc", WINDOW_WIDTH, WINDOW_HEIGHT,
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

      if (keystate[SDL_SCANCODE_W] && SDL_fabsf(y - WINDOW_HEIGHT + h) < 2.0f) {
        yvel = -50;
      }
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    const _Bool *keystate = SDL_GetKeyboardState(NULL);

    handle_keyboard_events_player(keystate, &xvel, &yvel);

    update_pos(&x, &y, &xvel, &yvel);

    boundary_condition(&x, &y, &xvel, &yvel, &touchedground);

    draw_rect(x, y, renderer);

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return EXIT_SUCCESS;
}
