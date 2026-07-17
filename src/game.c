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
#include <client.h>
#include <config.h>
#include <keyboard.h>
#include <math.h>
#include <netdb.h>
#include <physics.h>
#include <pthread.h>
#include <rendering.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define ALIGN_PAGE_UP(sz, pg) (((sz) + ((pg) - 1)) & ~((pg) - 1))

static bool touchedground = false;
static float xvel = 0;
static float yvel = 0;
static bool isrunning = true;
static Uint8 leveldesign = 0;
static int number_blocks = 0;
static int lim_blocks = 100;
static int num_players = 0;
static int lim_players = 10;
enum { GrassBlock };

static int socketfd = -1;
static Player *players = NULL;

void generate_cool_map(Block *blocks, int *number_blocks, int lim_blocks,
                       ArenaAllocator *arena) {
#define PLACE_BLOCK(grid_x, grid_y)                                            \
  if (*number_blocks < lim_blocks) {                                           \
    Block b = {(float)(grid_x * block_size), (float)(grid_y * block_size),     \
               GrassBlock};                                                    \
    arena_allocate((unsigned char *)&b, sizeof(Block), arena);                 \
    (*number_blocks)++;                                                        \
  }

  for (int x = 0; x < 16; x++) {
    PLACE_BLOCK(x, 11);
  }

  for (int x = 5; x <= 10; x++) {
    PLACE_BLOCK(x, 7);
  }

  PLACE_BLOCK(0, 9);
  PLACE_BLOCK(1, 9);
  PLACE_BLOCK(0, 8);

  PLACE_BLOCK(15, 9);
  PLACE_BLOCK(14, 9);
  PLACE_BLOCK(15, 6);
  PLACE_BLOCK(14, 6);

  PLACE_BLOCK(7, 4);
  PLACE_BLOCK(8, 4);

#undef PLACE_BLOCK
}

void *network_listener_thread(void *arg) {
  int sock = *(int *)arg;
  Player incoming_player;

  while (isrunning && sock != -1) {
    int bytes_received =
        recv(sock, &incoming_player, sizeof(incoming_player), 0);

    if (bytes_received > 0 && players != NULL) {
      bool found = false;

      // update existing remote player details
      for (int i = 0; i < num_players; i++) {
        if (players[i].id == incoming_player.id) {
          players[i] = incoming_player;
          found = true;
          break;
        }
      }

      if (!found && num_players < lim_players) {
        players[num_players] = incoming_player;
        num_players++;
      }
    } else if (bytes_received == 0) {
      printf("Server connection lost.\n");
      num_players = 0;
      break;
    }
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  SDL_Window *window = NULL;
  SDL_Renderer *renderer = NULL;

  if (!init_rendering("game", WINDOW_WIDTH, WINDOW_HEIGHT, &window,
                      &renderer)) {
    return 1;
  }

  init(&socketfd);
  if (socketfd == -1) {
    printf("Warning: Connection failed. Operating in offline sandbox mode.\n");
  }

  size_t page_size = sysconf(_SC_PAGESIZE);
  size_t total_bytes = ALIGN_PAGE_UP(4096, page_size);
  void *memory = mmap(NULL, total_bytes, PROT_READ | PROT_WRITE,
                      MAP_ANON | MAP_PRIVATE, -1, 0);

  ArenaAllocator arena_allocator = arena_init(memory);

  Block *blocks = (Block *)memory;
  players = (Player *)((unsigned char *)memory + (sizeof(Block) * lim_blocks));

  generate_cool_map(blocks, &number_blocks, lim_blocks, &arena_allocator);

  int id;
  char buf[10];
  printf("ID PLEASE : ");
  if (fgets(buf, sizeof(buf), stdin)) {
    id = atoi(buf);
  } else {
    id = 1;
  }

  Player you = {id, 100.0f, 100.0f, RIGHT_IDLE, -1};

  sendtoserver(socketfd, you);

  pthread_t network_thread;
  if (socketfd != -1) {
    pthread_create(&network_thread, NULL, network_listener_thread, &socketfd);
  }

  SDL_Texture *leftidle = load_texture(renderer, "assets/leftidle.png");
  SDL_Texture *rightidle = load_texture(renderer, "assets/idle.png");
  SDL_Texture *rightwalk = load_texture(renderer, "assets/walkright.png");
  SDL_Texture *leftwalk = load_texture(renderer, "assets/walkleft.png");
  SDL_Texture *grass_block = load_texture(renderer, "assets/block.png");
  SDL_Texture *curr_player_texture = rightidle;

  Uint8 ticker = 0;
  Uint8 walkanim = 0;

  while (isrunning) {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_EVENT_QUIT) {
        isrunning = false;
      }
      if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN && leveldesign == 1) {
        if (event.button.button == SDL_BUTTON_LEFT) {
          float mousex, mousey;
          SDL_GetMouseState(&mousex, &mousey);

          const double nearestx =
              floor((double)mousex / block_size) * block_size;
          const double nearesty =
              floor((double)mousey / block_size) * block_size;

          Block block = {nearestx, nearesty, GrassBlock};
          if (number_blocks < lim_blocks) {
            arena_allocate((unsigned char *)&block, sizeof(block),
                           &arena_allocator);
            number_blocks++;
          }
        }
      }
      const bool *keystate = SDL_GetKeyboardState(NULL);

      if ((keystate[SDL_SCANCODE_W] || keystate[SDL_SCANCODE_SPACE]) &&
          touchedground == true) {
        yvel = -50;
      }
      if (keystate[SDL_SCANCODE_L]) {
        leveldesign = leveldesign ^ 1;
        printf("%d leveldesign\n", leveldesign);
      }
    }

    float mousex, mousey;
    SDL_GetMouseState(&mousex, &mousey);
    const bool *keystate = SDL_GetKeyboardState(NULL);

    handle_keyboard_events_player(keystate, &xvel, &yvel);
    screen_clear(renderer);

    if (leveldesign == 1) {
      const double nearestx = floor((double)mousex / block_size) * block_size;
      const double nearesty = floor((double)mousey / block_size) * block_size;
      const SDL_FRect rect = {nearestx, nearesty, block_size, block_size};
      SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
      SDL_RenderRect(renderer, &rect);
    }

    for (int i = 0; i < number_blocks; i++) {
      const Block curr_block = blocks[i];
      SDL_FRect dest_rect = {curr_block.x, curr_block.y, block_size,
                             block_size};
      SDL_RenderTexture(renderer, grass_block, NULL, &dest_rect);
    }

    yvel += g;
    xvel *= 0.95;

    collision_system(&you.x, &you.y, &xvel, &yvel, &touchedground, blocks,
                     number_blocks);
    update_pos(&you.x, &you.y, &xvel, &yvel);
    boundary_condition(&you.x, &you.y, &xvel, &yvel, &touchedground);

    sendtoserver(socketfd, you);

    for (int i = 0; i < num_players; i++) {
      if (players[i].id != you.id) {
        SDL_Texture *remote_texture = rightidle;
        if (players[i].animframe == LEFT_IDLE)
          remote_texture = leftidle;
        if (players[i].animframe == RIGHT_WALK)
          remote_texture = rightwalk;
        if (players[i].animframe == LEFT_WALK)
          remote_texture = leftwalk;

        draw_texture(renderer, remote_texture, players[i].x, players[i].y);
      }
    }

    draw_texture(renderer, curr_player_texture, you.x, you.y);

    SDL_RenderPresent(renderer);

    if (ticker == 10) {
      if (keystate[SDL_SCANCODE_D] && walkanim == 0) {
        curr_player_texture = rightidle;
        you.animframe = RIGHT_IDLE;
      } else if (keystate[SDL_SCANCODE_A] && walkanim == 0) {
        curr_player_texture = leftidle;
        you.animframe = LEFT_IDLE;
      } else if (keystate[SDL_SCANCODE_D] && walkanim == 1) {
        curr_player_texture = rightwalk;
        you.animframe = RIGHT_WALK;
      } else if (keystate[SDL_SCANCODE_A] && walkanim == 1) {
        curr_player_texture = leftwalk;
        you.animframe = LEFT_WALK;
      } else {
        if (curr_player_texture == rightwalk) {
          curr_player_texture = rightidle;
        } else if (curr_player_texture == leftwalk) {
          curr_player_texture = leftidle;
        }
      }
      walkanim = walkanim ^ 1;
      ticker = 0;
    }
    ticker++;

    SDL_Delay(16);
  }

  isrunning = false;
  if (socketfd != -1) {
    close(socketfd);
    pthread_join(network_thread, NULL);
  }

  munmap(memory, total_bytes);

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
