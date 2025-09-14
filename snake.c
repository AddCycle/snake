#include "snake.h"

void draw_grid(SDL_Surface *surface, int width, int height, int cell_size)
{
  SDL_Rect row_line = {0, 0, width, LINE_WIDTH};
  for (row_line.y = 0; row_line.y < height; row_line.y += cell_size)
    SDL_FillSurfaceRect(surface, &row_line, GRID_COLOR);

  SDL_Rect col_line = {0, 0, LINE_WIDTH, height};
  for (col_line.x = 0; col_line.x < width; col_line.x += cell_size)
    SDL_FillSurfaceRect(surface, &col_line, GRID_COLOR);
}

void fill_cell(SDL_Surface *surface, int x, int y, Uint32 color, int cell_size)
{
  SDL_Rect rect = {x * cell_size, y * cell_size, cell_size, cell_size};
  SDL_FillSurfaceRect(surface, &rect, color);
}

void reset_apple(struct SnakeElement *snakeElement, struct Apple *apple, int cols, int rows)
{
  apple->x = cols * ((double)rand() / RAND_MAX);
  apple->y = rows * ((double)rand() / RAND_MAX);

  /* if the apple coords collides with snake ones, try again */
  do
  {
    if (snakeElement->x == apple->x && snakeElement->y == apple->y)
    {
      reset_apple(snakeElement, apple, cols, rows);
    }
    snakeElement = snakeElement->next;
  } while (snakeElement != NULL);
}

void draw_apple(SDL_Surface *surface, struct Apple *apple, int cell_size)
{
  APPLE(apple->x, apple->y);
}

void draw_snake(SDL_Surface *surface, struct SnakeElement *snakeElement, int cell_size, int snake_color)
{
  SNAKE(snakeElement->x, snakeElement->y);
  if (snakeElement->next)
  {
    draw_snake(surface, snakeElement->next, cell_size, snake_color);
  }
}

void move_snake(struct SnakeElement *snake, struct Direction *direction, int cols, int rows)
{
  int prev_x = snake->x;
  int prev_y = snake->y;

  // move head
  snake->x += direction->dx;
  snake->y += direction->dy;

  // wrap around (portal style)
  if (snake->x >= cols)
    snake->x = 0;
  if (snake->x < 0)
    snake->x = cols - 1;
  if (snake->y >= rows)
    snake->y = 0;
  if (snake->y < 0)
    snake->y = rows - 1;

  // move body: each part takes the previous one's old coords
  struct SnakeElement *current = snake->next;
  while (current)
  {
    int tmp_x = current->x;
    int tmp_y = current->y;
    current->x = prev_x;
    current->y = prev_y;
    prev_x = tmp_x;
    prev_y = tmp_y;

    current = current->next;
  }
}

void init_snake(struct SnakeElement *snake, int x, int y)
{
  snake->x = x;
  snake->y = y;
  snake->next = NULL;
}

void free_snake(struct SnakeElement *snake)
{
  if (snake != NULL)
  {
    free_snake(snake->next);
    free(snake);
  }
}

void lengthen_snake(struct SnakeElement *snake)
{
  // traverse to the last element
  while (snake->next != NULL)
    snake = snake->next;

  // allocate new segment
  struct SnakeElement *new_part = malloc(sizeof(struct SnakeElement));
  if (!new_part)
  {
    printf("failed to allocate the next body part\n");
    return;
  }

  new_part->x = snake->x;
  new_part->y = snake->y;
  new_part->next = NULL;

  snake->next = new_part;
}

bool check_collision(struct SnakeElement *snake)
{
  int head_x = snake->x;
  int head_y = snake->y;

  struct SnakeElement *current = snake->next;
  while (current)
  {
    if (current->x == head_x && current->y == head_y)
    {
      return true;
    }
    current = current->next;
  }
  return false;
}

void init_game(struct GameState *gamestate)
{
  // initialize the game (difficulty, level, ...)
  gamestate->score = 0;
}

void score(struct GameState *gamestate)
{
  gamestate->score++;
  printf("score: %d\n", gamestate->score);
}

bool is_allowed_key(SDL_Keycode key, SDL_Keycode allowed_keys[MAX_KEYBOARD_KEYS])
{
  for (int i = 0; i < MAX_KEYBOARD_KEYS; i++)
  {
    if (key == allowed_keys[i])
      return true;
  }
  return false;
}

void init_keys(SDL_Keycode allowed_keys[MAX_KEYBOARD_KEYS])
{
  int index = 0;
  allowed_keys[index++] = SDLK_LEFT;
  allowed_keys[index++] = SDLK_RIGHT;
  allowed_keys[index++] = SDLK_UP;
  allowed_keys[index++] = SDLK_DOWN;
  allowed_keys[index++] = SDLK_SPACE;
  allowed_keys[index++] = SDLK_ESCAPE;
  allowed_keys[index++] = SDLK_F;
}

// the main menu of the game
int main_menu(SDL_Window *window, SDL_Surface *surface, int width, int height)
{
  SDL_Event event;
  int choice = 0; // 1 = Play, 2 = Quit
  int running = 1;
  TTF_Font *font = TTF_OpenFont("PressStart2P.ttf", 20);
  if (!font)
  {
    SDL_Log("Failed to load font: %s", SDL_GetError());
    return 2;
  }

  SDL_Color white = {255, 255, 255, 255};
  SDL_Color black = {0, 0, 0, 255};

  while (running)
  {
    // Clear screen
    SDL_ClearSurface(surface, 0, 0, 0, 0);

    SDL_Surface *text_surface = TTF_RenderText_Blended(font, "SNAKE CLASSIC", 0, white);
    if (text_surface)
    {
      SDL_Rect text_rect;
      text_rect.x = width / 2 - text_surface->w / 2;
      text_rect.y = height / 4 - text_surface->h / 2;
      SDL_BlitSurface(text_surface, NULL, surface, &text_rect);
      SDL_DestroySurface(text_surface);
    }

    // Draw simple rectangles as buttons with outlines
    SDL_Rect outline_rect_play = {width / 2 - 110, height / 2 - 55, 220, 60};
    SDL_Rect play_rect = {width / 2 - 100, height / 2 - 50, 200, 50};

    SDL_Rect outline_rect_quit = {width / 2 - 110, height / 2 - 30 + play_rect.h, 220, 60};
    SDL_Rect quit_rect = {width / 2 - 100, height / 2 - 25 + play_rect.h, 200, 50};

    if (choice == 1)
      SDL_FillSurfaceRect(surface, &outline_rect_play, 0xffffff);
    SDL_FillSurfaceRect(surface, &play_rect, 0x00ff00); // green
    if (choice == 2)
      SDL_FillSurfaceRect(surface, &outline_rect_quit, 0xffffff);
    SDL_FillSurfaceRect(surface, &quit_rect, 0xff0000); // red

    SDL_Surface *play_text_surface = TTF_RenderText_Blended(font, "PLAY", 0, black);
    if (play_text_surface)
    {
      SDL_Rect text_rect;
      text_rect.x = width / 2 - play_text_surface->w / 2;
      text_rect.y = height / 4 - play_text_surface->h / 2 + 125;
      SDL_BlitSurface(play_text_surface, NULL, surface, &text_rect);
      SDL_DestroySurface(play_text_surface);
    }

    SDL_Surface *quit_text_surface = TTF_RenderText_Blended(font, "EXIT", 0, black);
    if (quit_text_surface)
    {
      SDL_Rect text_rect;
      text_rect.x = width / 2 - quit_text_surface->w / 2;
      text_rect.y = height / 4 - quit_text_surface->h / 2 + 200;
      SDL_BlitSurface(quit_text_surface, NULL, surface, &text_rect);
      SDL_DestroySurface(quit_text_surface);
    }

    SDL_UpdateWindowSurface(window);

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        TTF_CloseFont(font);
        return 2; // quit
      }
      if (event.type == SDL_EVENT_KEY_DOWN)
      {
        if (event.key.key == SDLK_UP || event.key.key == SDLK_W)
          choice = 1; // Play selected
        if (event.key.key == SDLK_DOWN || event.key.key == SDLK_S)
          choice = 2; // Quit selected
        if (event.key.key == SDLK_RETURN || event.key.key == SDLK_KP_ENTER || event.key.key == SDLK_SPACE)
        {
          TTF_CloseFont(font);
          return choice; // return selection
        }
        if (event.key.key == SDLK_ESCAPE)
        {
          TTF_CloseFont(font);
          return 2; // quit immediately
        }
      }
    }
  }
  TTF_CloseFont(font);
  return 2; // default quit
}

int main(int argc, char *argv[])
{
  if (!SDL_Init(SDL_INIT_VIDEO))
  {
    printf("SDL video init failed !\n");
    return 1;
  }

  TTF_Init();

  TTF_Font *font = TTF_OpenFont("PressStart2P.ttf", 20);

  int width = 900;
  int height = 600;
  int cell_size = 15;

  int cols = width / cell_size;
  int rows = height / cell_size;

  SDL_Window *window = SDL_CreateWindow("Snake Classic C", width, height, SDL_WINDOW_RESIZABLE);

  SDL_Surface *surface = SDL_GetWindowSurface(window);
  SDL_Rect rect = {200, 200, 200, 200};

  // main menu play/quit
  int menu_choice = main_menu(window, surface, width, height);
  if (menu_choice == 2) // Quit selected
  {
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
  }

  SDL_Event event;

  int game = 1;
  bool f_key_press = false;

  // initializing the allowed keys in our game
  SDL_Keycode allowed_keys[MAX_KEYBOARD_KEYS];
  init_keys(allowed_keys);

  struct GameState gamestate;
  init_game(&gamestate);

  int snake_color = WHITE;
  struct SnakeElement *snake = malloc(sizeof(struct SnakeElement));
  init_snake(snake, 5, 5);

  struct Apple apple = {0, 0};
  struct Direction direction = {0, 0};

  const int target_fps = 60;                 // maximum FPS
  int snake_fps = 10;                        // snake moves 10 times per second
  const int frame_delay = 1000 / target_fps; // ms per frame
  int snake_frame_delay = 1000 / snake_fps;  // ms per snake move

  Uint32 last_snake_time = SDL_GetTicks();
  Uint32 last_fps_time = SDL_GetTicks();
  int frames = 0;

  while (game)
  {
    Uint32 frame_start = SDL_GetTicks();

    while (SDL_PollEvent(&event))
    {
      if (event.type == SDL_EVENT_QUIT)
      {
        game = 0;
      }
      if (event.type == SDL_EVENT_WINDOW_RESIZED)
      {
        width = event.window.data1;
        height = event.window.data2;

        int target_cols = 60;
        int target_rows = 40;
        cell_size = (width / target_cols < height / target_rows)
                        ? width / target_cols
                        : height / target_rows;

        cols = width / cell_size;
        rows = height / cell_size;
        surface = SDL_GetWindowSurface(window);
      }
      if (event.type == SDL_EVENT_KEY_DOWN && is_allowed_key(event.key.key, allowed_keys))
      {
        if (event.key.key == SDLK_RIGHT && direction.dx != -1)
        {
          direction.dx = 0;
          direction.dy = 0;
          direction.dx = 1;
        }
        if (event.key.key == SDLK_LEFT && direction.dx != 1)
        {
          direction.dx = 0;
          direction.dy = 0;
          direction.dx = -1;
        }
        if (event.key.key == SDLK_DOWN && direction.dy != -1)
        {
          direction.dx = 0;
          direction.dy = 0;
          direction.dy = 1;
        }
        if (event.key.key == SDLK_UP && direction.dy != 1)
        {
          direction.dx = 0;
          direction.dy = 0;
          direction.dy = -1;
        }
        if (event.key.key == SDLK_SPACE)
        {
          printf("snake speed boost\n");
          snake_fps = 30;
          snake_frame_delay = 1000 / snake_fps;
          snake_color = YELLOW;
        }
        if (event.key.key == SDLK_F && !f_key_press)
        {
          printf("f_key_pressed\n");
          SDL_MaximizeWindow(window);
          f_key_press = true;
        }
        if (event.key.key == SDLK_ESCAPE)
          game = 0;
      }

      if (event.type == SDL_EVENT_KEY_UP && is_allowed_key(event.key.key, allowed_keys))
      {
        if (event.key.key == SDLK_SPACE)
        {
          printf("boost gone\n");
          snake_fps = 10;
          snake_frame_delay = 1000 / snake_fps;
          snake_color = WHITE;
        }
        if (event.key.key == SDLK_F)
        {
          f_key_press = false;
        }
      }
    }
    Uint32 now = SDL_GetTicks();
    Uint32 delta_snake = now - last_snake_time;
    if (delta_snake >= snake_frame_delay)
    {
      move_snake(snake, &direction, cols, rows);

      if (check_collision(snake))
      {
        printf("Game Over! You were too strong for yourself.\n");
        game = 0;
      }

      if (snake->x == apple.x && snake->y == apple.y)
      {
        reset_apple(snake, &apple, cols, rows);
        lengthen_snake(snake);
        score(&gamestate);
      }
      last_snake_time = now;
    }

    draw_apple(surface, &apple, cell_size);
    draw_snake(surface, snake, cell_size, snake_color);
    DRAW_GRID;

    char buffer[64]; // enough space for "Score: <big number>"
    snprintf(buffer, sizeof(buffer), "Score: %d", gamestate.score);
    SDL_Color white = {255, 255, 255, 150};
    SDL_Surface *text_surface = TTF_RenderText_Blended(font, buffer, 0, white);
    if (text_surface)
    {
      SDL_Rect text_rect;
      text_rect.x = width / 2 - text_surface->w / 2;
      text_rect.y = height / 4 - text_surface->h / 2;
      SDL_BlitSurface(text_surface, NULL, surface, &text_rect);
      SDL_DestroySurface(text_surface);
    }

    SDL_UpdateWindowSurface(window);
    SDL_ClearSurface(surface, 0, 0, 0, 0);

    // --- FPS counter ---
    frames++;
    if (now - last_fps_time >= 1000)
    {
      printf("FPS: %d\n", frames);
      frames = 0;
      last_fps_time = now;
    }

    // --- Frame cap ---
    Uint32 frame_time = SDL_GetTicks() - frame_start;
    if (frame_time < frame_delay)
      SDL_Delay(frame_delay - frame_time);
  }

  free_snake(snake);
  TTF_CloseFont(font);
  TTF_Quit();

  SDL_DestroyWindow(window);
  SDL_Quit();
  return 0;
}