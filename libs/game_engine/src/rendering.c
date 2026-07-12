
#include "rendering.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>
#include <config.h>

void draw_rect(float x, float y, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_FRect rect = {x, y, w, h};
  SDL_RenderFillRect(renderer, &rect);
}