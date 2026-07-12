
#include "rendering.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <config.h>
bool init_rendering(const char *title, int width, int height,
                    SDL_Window **out_window, SDL_Renderer **out_renderer) {
  if (!SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("SDL Video Init Failed: %s", SDL_GetError());
    return false;
  }
  return SDL_CreateWindowAndRenderer(title, width, height, 0, out_window,
                                     out_renderer);
}
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *file_path) {
  SDL_Texture *texture = IMG_LoadTexture(renderer, file_path);
  if (!texture) {
    SDL_Log("Failed to load texture at %s: %s", file_path, SDL_GetError());
  }
  return texture;
}
void draw_texture(SDL_Renderer *renderer, SDL_Texture *texture, float x,
                  float y) {
  if (!texture)
    return;

  SDL_FRect dest_rect = {x, y, w, h};
  // Pass NULL for source rect to draw the entire image
  SDL_RenderTexture(renderer, texture, NULL, &dest_rect);
}

void draw_rect(float x, float y, SDL_Renderer *renderer) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_FRect rect = {x, y, w, h};
  SDL_RenderFillRect(renderer, &rect);
}

void screen_clear(SDL_Renderer *renderer) {
<<<<<<< HEAD
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
=======
  SDL_SetRenderDrawColor(renderer, 30, 144, 255, 255);
>>>>>>> 7da37c8 (some level design + rendering textures)
  SDL_RenderClear(renderer);
}
