#ifndef RENDERING
#define RENDERING

#include "SDL3/SDL_render.h"
#include <SDL3/SDL.h>

void draw_rect(float x, float y, SDL_Renderer *renderer);
bool init_rendering(const char *title, int width, int height,
                    SDL_Window **out_window, SDL_Renderer **out_renderer);
SDL_Texture *load_texture(SDL_Renderer *renderer, const char *file_path);
void draw_texture(SDL_Renderer *renderer, SDL_Texture *texture, float x,
                  float y);
void screen_clear(SDL_Renderer *renderer);
#endif