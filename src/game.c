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
#include <unistd.h>
#define ALIGN_UP(size, alignment)                                              \
  (((size) + ((alignment) - 1)) & ~((alignment) - 1))
#define ALIGN_PAGE_UP(sz, pg) (((sz) + ((pg) - 1)) & ~((pg) - 1))

static bool touchedground = false;

static float x = 0;
static float y = 0;
static float xvel = 0;
static float yvel = 0;
static bool isrunning = true;
static Uint8 leveldesign = 0;
static int number_blocks = 0;
static int lim_blocks = 100; // 50*9 = 450 bytes roughly
enum { GrassBlock };

int main(int argc, char *argv[]) {
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  if (!init_rendering("game", 800, 600, &window, &renderer)) {
    return 1;
  }

  size_t total_bytes = 4096;

  void *memory = mmap(NULL, ALIGN_PAGE_UP(4096, sysconf(_SC_PAGESIZE)),
                      PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);

  ArenaAllocator arena_allocator = arena_init(memory);

  // pointers
  // Block *blocks = malloc(sizeof(Block) * lim_blocks);
  Block *blocks = memory; // 0th position
  SDL_Texture *leftidle = load_texture(renderer, "assets/leftidle.png");
  SDL_Texture *rightidle = load_texture(renderer, "assets/idle.png");
  SDL_Texture *rightwalk = load_texture(renderer, "assets/walkright.png");
  SDL_Texture *leftwalk = load_texture(renderer, "assets/walkleft.png");
  SDL_Texture *grass_block = load_texture(renderer, "assets/block.png");
  SDL_Texture *curr_player_texture;

  curr_player_texture = leftidle;
  Uint8 ticker = 0;
  Uint8 walkanim = 0;
  while (isrunning) {
    SDL_Event event;

    // event loop
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        isrunning = false;
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && leveldesign == 1) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          printf("number_blocks %d \n", number_blocks);
          float mousex, mousey;

          SDL_GetMouseState(&mousex, &mousey);

          const double nearestx =
              floor((double)mousex / block_size) * block_size;
          const double nearesty =
              floor((double)mousey / block_size) * block_size;

          Block block = {nearestx, nearesty, GrassBlock};
          arena_allocate((unsigned char *)&block, sizeof(block),
                         &arena_allocator);

          // memcpy(&blocks[number_blocks], &block, sizeof block);
          number_blocks++;
          if (number_blocks >= lim_blocks) {
            printf("Memory reallocation failed! Keeping old data.\n");
          }
        }
      }
      const _Bool *keystate = SDL_GetKeyboardState(NULL);

      if ((keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_SPACE]) &&
          touchedground == true) {
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

    yvel += g;

    xvel *= 0.95;
    collision_system(&x, &y, &xvel, &yvel, &touchedground, blocks,
                     number_blocks);

    update_pos(&x, &y, &xvel, &yvel);

    boundary_condition(&x, &y, &xvel, &yvel, &touchedground);

    draw_texture(renderer, curr_player_texture, x, y);

    SDL_RenderPresent(renderer);

    if (ticker == 10) {

      if (keystate[SDL_SCANCODE_D] && walkanim == 0) {
        curr_player_texture = rightidle;
      }
      if (keystate[SDL_SCANCODE_A] && walkanim == 0) {
        curr_player_texture = leftidle;
      }
      if (keystate[SDL_SCANCODE_D] && walkanim == 1) {
        curr_player_texture = rightwalk;
      }
      if (keystate[SDL_SCANCODE_A] && walkanim == 1) {
        curr_player_texture = leftwalk;
      }

      walkanim = walkanim ^ 1;
      ticker = 0;
    }
    ticker++;

    SDL_Delay(16);
  }

  free(blocks);
  SDL_DestroyTexture(leftidle);
  SDL_DestroyTexture(rightidle);
  SDL_DestroyTexture(rightwalk);
  SDL_DestroyTexture(leftwalk);
  SDL_DestroyTexture(grass_block);
  SDL_DestroyWindow(window);
  SDL_DestroyRenderer(renderer);
  SDL_Quit();
  return EXIT_SUCCESS;
}
