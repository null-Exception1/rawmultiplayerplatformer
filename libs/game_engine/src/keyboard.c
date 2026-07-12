#include <SDL3/SDL.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
void handle_keyboard_events_player(const _Bool *keystate, float *xvel,
                                   float *yvel) {
  if (keystate[SDL_SCANCODE_A]) {
    *xvel -= 1;
  }
  if (keystate[SDL_SCANCODE_D]) {
    *xvel += 1;
  }
  if (keystate[SDL_SCANCODE_S]) {
    *yvel += 1;
  }
  if (keystate[SDL_SCANCODE_W]) {
    *yvel -= 1;
  }
}