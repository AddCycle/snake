#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>

// for most countries
#define MAX_KEYBOARD_KEYS 105

#define LINE_WIDTH 2

#define DRAW_GRID draw_grid(surface, width, height, cell_size)
#define SNAKE(x, y) fill_cell(surface, x, y, snake_color, cell_size)
#define APPLE(x, y) fill_cell(surface, x, y, APPLE_COLOR, cell_size)

#define WHITE 0xffffffff
#define RED 0x00ff0000
#define YELLOW 0xffff00
#define GRID_COLOR 0x1f1f1f1f
#define APPLE_COLOR RED

struct GameState
{
  int score;
};

struct Apple
{
  int x, y;
};

struct SnakeElement
{
  int x, y;

  /* only last element is NULL */
  struct SnakeElement *next;
};

struct Direction
{
  int dx, dy;
};
