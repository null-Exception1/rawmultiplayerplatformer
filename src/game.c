#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keyboard.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_scancode.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_video.h"
#include <SDL3/SDL.h>
#include <arena_alloc.h>
#include <config.h>
#include <keyboard.h>
#include <math.h>
#include <physics.h>
#include <rendering.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#define block_size 50
typedef struct {
  float x;
  float y;
  Uint8 type;
} Block;

static bool touchedground = false;

static float x = 0;
static float y = 0;
static float xvel = 0;
static float yvel = 0;
static bool isrunning = true;
static Uint8 leveldesign = 0;
static int number_blocks = 0;
static int lim_blocks = 10;
enum { GrassBlock };
int main(int argc, char *argv[]) {

  Block *blocks = malloc(sizeof(Block) * lim_blocks);
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;
  if (!init_rendering("game", 800, 600, &window, &renderer)) {
    return 1;
  }

  size_t total_bytes = 64 * 1024 * 1024;
  void *big_block = mmap(NULL, total_bytes, PROT_READ | PROT_WRITE,
                         MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);

  if (big_block == MAP_FAILED) {
    printf("Map failed");
  }

  if (munmap(big_block, total_bytes) == -1) {
    perror("munmap failed");
    return 1;
  }

  SDL_Texture *player_texture = load_texture(renderer, "assets/leftidle.png");
  SDL_Texture *grass_block = load_texture(renderer, "assets/block.png");

  while (isrunning) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        isrunning = false;
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          printf("number_blocks %d \n", number_blocks);
          float mousex, mousey;

          SDL_GetMouseState(&mousex, &mousey);

          const double nearestx =
              floor((double)mousex / block_size) * block_size;
          const double nearesty =
              floor((double)mousey / block_size) * block_size;

          const Block block = {nearestx, nearesty, GrassBlock};
          memcpy(&blocks[number_blocks], &block, sizeof block);
          number_blocks++;
          if (number_blocks >= lim_blocks) {
            lim_blocks *= 2;
            printf("lim blocks %d\n", lim_blocks);
            Block *temp = realloc(blocks, sizeof(Block) * lim_blocks);

            if (blocks != NULL) {
              blocks = temp;
            } else {
              printf("Memory reallocation failed! Keeping old data.\n");
              free(temp);
              return 1;
            }
          }
        }
      }
      const _Bool *keystate = SDL_GetKeyboardState(NULL);

      if ((keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_SPACE]) &&
          SDL_fabsf(y - WINDOW_HEIGHT + h) < 2.0f) {
        yvel = -50;
      }
      if (keystate[SDL_SCANCODE_L]) {
        leveldesign = leveldesign ^ 1;
        printf("%d leveldesign", leveldesign);
      }
    }
    float mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);

    const _Bool *keystate = SDL_GetKeyboardState(NULL);

    handle_keyboard_events_player(keystate, &xvel, &yvel);

    screen_clear(renderer);

    if (leveldesign == 1) { // level design is on

      const double nearestx = floor((double)mousex / block_size) * block_size;
      const double nearesty = floor((double)mousey / block_size) * block_size;

      const SDL_FRect rect = {nearestx, nearesty, block_size, block_size};
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 0);
      SDL_RenderRect(renderer, &rect);
    }

    // render the leveldesign
    for (int i = 0; i < number_blocks; i++) {
      const Block curr_block = blocks[i];
      SDL_FRect dest_rect = {curr_block.x, curr_block.y, block_size,
                             block_size};
      SDL_RenderTexture(renderer, grass_block, NULL, &dest_rect);
    }

    update_pos(&x, &y, &xvel, &yvel);

    boundary_condition(&x, &y, &xvel, &yvel, &touchedground);

    draw_texture(renderer, player_texture, x, y);

    SDL_RenderPresent(renderer);
    SDL_Delay(16);
  }

  free(blocks);
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return EXIT_SUCCESS;
}
