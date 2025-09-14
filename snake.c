#include <SDL3/SDL.h>
#include <stdio.h>

#define WIDTH 900
#define HEIGHT 600

#define LINE_WIDTH 2
#define CELL_SIZE 15

#define COLS WIDTH / CELL_SIZE
#define ROWS HEIGHT / CELL_SIZE

#define DRAW_GRID draw_grid(surface)
#define SNAKE(x, y) fill_cell(surface, x, y, SNAKE_COLOR)
#define APPLE(x, y) fill_cell(surface, x, y, APPLE_COLOR)

#define WHITE 0xffffffff
#define RED 0x00ff0000
#define GRID_COLOR 0x1f1f1f1f
#define APPLE_COLOR RED
#define SNAKE_COLOR WHITE

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

void draw_grid(SDL_Surface *surface)
{
  SDL_Rect row_line = {0, 0, WIDTH, LINE_WIDTH};
  for (row_line.y = 0; row_line.y < HEIGHT; row_line.y += CELL_SIZE)
    SDL_FillSurfaceRect(surface, &row_line, GRID_COLOR);

  SDL_Rect col_line = {0, 0, LINE_WIDTH, HEIGHT};
  for (col_line.x = 0; col_line.x < WIDTH; col_line.x += CELL_SIZE)
    SDL_FillSurfaceRect(surface, &col_line, GRID_COLOR);
}

void fill_cell(SDL_Surface *surface, int x, int y, Uint32 color)
{
  SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
  SDL_FillSurfaceRect(surface, &rect, color);
}

void reset_apple(struct SnakeElement *snakeElement, struct Apple *apple)
{
  apple->x = COLS * ((double)rand() / RAND_MAX);
  apple->y = ROWS * ((double)rand() / RAND_MAX);

  /* if the apple coords collides with snake ones, try again */
  do
  {
    if (snakeElement->x == apple->x && snakeElement->y == apple->y)
    {
      reset_apple(snakeElement, apple);
    }
    snakeElement = snakeElement->next;
  } while (snakeElement != NULL);
}

void draw_apple(SDL_Surface *surface, struct Apple *apple)
{
  APPLE(apple->x, apple->y);
}

void draw_snake(SDL_Surface *surface, struct SnakeElement *snakeElement)
{
  if (snakeElement)
  {
    SNAKE(snakeElement->x, snakeElement->y);
    draw_snake(surface, snakeElement->next);
  }
}

void move_snake(struct SnakeElement *snakeElement, struct Direction *direction)
{
  snakeElement->x += direction->dx;
  snakeElement->y += direction->dy;
}

int main(int argc, char *argv[])
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    printf("SDL video init failed !\n");
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Snake Classic C", WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);

  SDL_Surface *surface = SDL_GetWindowSurface(window);
  SDL_Rect rect = {200, 200, 200, 200};
  SDL_Event event;

  int game = 1;

  struct SnakeElement snake = {5, 5, NULL};
  struct Apple apple = {9, 5};
  struct Direction direction;

  while (game)
  {
    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        game = 0;
      }
      if (event.type == SDL_EVENT_KEY_DOWN)
      {
        direction.dx = 0;
        direction.dy = 0;
        if (event.key.key == SDLK_RIGHT)
          direction.dx = 1;
        if (event.key.key == SDLK_LEFT)
          direction.dx = -1;
        if (event.key.key == SDLK_DOWN)
          direction.dy = 1;
        if (event.key.key == SDLK_UP)
          direction.dy = -1;
        if (event.key.key == SDLK_ESCAPE)
          game = 0;
      }
    }

    move_snake(&snake, &direction);
    if (snake.x == apple.x && snake.y == apple.y)
    {
      reset_apple(&snake, &apple);
    }

    draw_apple(surface, &apple);
    draw_snake(surface, &snake);
    DRAW_GRID;
    SDL_UpdateWindowSurface(window);
    SDL_ClearSurface(surface, 0, 0, 0, 0);
    SDL_Delay(50);
  }

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}