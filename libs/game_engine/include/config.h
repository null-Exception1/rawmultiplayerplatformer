#ifndef CONFIG
#define CONFIG

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

#define h 30
#define w 30
#define g 5

#define block_size 50

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
typedef struct {
  float x;
  float y;
  unsigned char type;
} Block;

typedef enum { LEFT_IDLE, RIGHT_IDLE, LEFT_WALK, RIGHT_WALK } WALK_FRAME;
typedef struct {
  int id;
  float x;
  float y;
  WALK_FRAME animframe;
  int newfd;
} Player;

#endif