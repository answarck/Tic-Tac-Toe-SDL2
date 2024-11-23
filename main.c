#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#define WIN_W 600
#define WIN_H 600
#define WIN_H_BY_3 WIN_H / 3
#define WIN_W_BY_3 WIN_W / 3
#define X_PADDING 30
#define X_THICKNESS 5
#define O_PADDING 30
#define O_THICKNESS 5
#define GRID_THICKNESS 3

int checkState (int grid[3][3], int row, int col) {
  for (int i = 0; i < 3; i++) {
    if ((grid[i][0] == grid[i][1] && grid[i][1] == grid[i][2]) && grid[i][1] != 0) return 0;
    else if ((grid[0][i] == grid[1][i] && grid[1][i] == grid[2][i]) && grid[1][i] != 0) return 0;
    if (grid[1][1] != 0) {
      if (grid[0][0] == grid[1][1] && grid[1][1] == grid[2][2]) return 0; 
      else if (grid[0][2] == grid[1][1] && grid[1][1] == grid[2][0]) return 0; 
    }
  }
  return 1;
}

void DrawX(SDL_Renderer *renderer, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
  for (int i = 0; i <= X_THICKNESS; i++) SDL_RenderDrawLine(renderer, X_PADDING + WIN_W_BY_3 * (x - 1) + i, X_PADDING + WIN_H_BY_3 * (y - 1), WIN_W_BY_3 * x - X_PADDING + i, WIN_H_BY_3 * y - X_PADDING);
  for (int i = 0; i <= X_THICKNESS; i++) SDL_RenderDrawLine(renderer, WIN_W_BY_3 * x - X_PADDING + i, X_PADDING + WIN_H_BY_3 * (y - 1), X_PADDING + WIN_W_BY_3 * (x - 1) + i, WIN_H_BY_3 * y - X_PADDING);
  SDL_RenderPresent(renderer);
}

void DrawO(SDL_Renderer *renderer, int x, int y) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); 
  int centerX = (WIN_W_BY_3 * (x - 1)) + WIN_W_BY_3 / 2; 
  int centerY = (WIN_H_BY_3 * (y - 1)) + WIN_H_BY_3 / 2; 
  for (int i = 0; i <= O_THICKNESS; i++) {
    int radius = (WIN_W_BY_3 / 2) - O_PADDING + i;            
    for (int angle = 0; angle < 360; angle++) { 
      float angleRad = angle * M_PI / 180.0f; 
      int pointX = centerX + (int)(radius * cos(angleRad));
      int pointY = centerY + (int)(radius * sin(angleRad));
      SDL_RenderDrawPoint(renderer, pointX, pointY);
    }
  }
  SDL_RenderPresent(renderer);
}


void DrawGrid(SDL_Renderer *renderer) {
  int X = WIN_W / 3, Y = WIN_H / 3, i;
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  for(i = 1; i <= GRID_THICKNESS; i++) SDL_RenderDrawLine(renderer, X + i, 0, X + i, WIN_H);
  for(i = 1; i <= GRID_THICKNESS; i++) SDL_RenderDrawLine(renderer, (X * 2) + i, 0, (X * 2) + i, WIN_H);
  for(i = 1; i <= GRID_THICKNESS; i++) SDL_RenderDrawLine(renderer, 0, Y + i, WIN_W, Y + i);
  for(i = 1; i <= GRID_THICKNESS; i++) SDL_RenderDrawLine(renderer, 0, (Y * 2) + i, WIN_W, (Y * 2) + i);
}

int main() {
  if(SDL_Init(SDL_INIT_VIDEO) != 0) {
    printf("Error: SDL is not initialised! %s\n", SDL_GetError());
    return 1;
  }

  SDL_Window *window = SDL_CreateWindow("Tic Tac Toe", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_W, WIN_H, SDL_WINDOW_SHOWN);
  if (!window) {
    printf("Error: can't create window %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
  }

  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if(!renderer) {
    printf("Error: can't initialise renderer %s\n", SDL_GetError());
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 1;
  }

  TTF_Init();
  TTF_Font *font = TTF_OpenFont("./font.ttf", 90);
  SDL_Color color = {0, 0, 0, 255};
  SDL_Surface *surface1 = TTF_RenderText_Solid(font, "Player X won!", color);
  SDL_Surface *surface2 = TTF_RenderText_Solid(font, "Player O won!", color);
  SDL_Surface *surface3 = TTF_RenderText_Solid(font, "Press q to quit.", color);
  SDL_Texture *texture1 = SDL_CreateTextureFromSurface(renderer, surface1);
  SDL_Texture *texture2 = SDL_CreateTextureFromSurface(renderer, surface2);
  SDL_Texture *texture3 = SDL_CreateTextureFromSurface(renderer, surface3);
  SDL_FreeSurface(surface1);
  SDL_FreeSurface(surface2);
  SDL_FreeSurface(surface3);

  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderClear(renderer);

  DrawGrid(renderer);
  SDL_RenderPresent(renderer);



  SDL_Event event;
  int running = 1, gameOver = 0, row, column, MouseX, MouseY, player = 1, count = 0, grid[3][3] = {0};
  while(running && count != 9) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_MOUSEBUTTONDOWN) {
        SDL_GetMouseState(&MouseX, &MouseY);
        row = (MouseX / 200) + 1; // TODO: FIX THE PROBLEM HERE
        column = (MouseY / 200) + 1;
        if (player == 1 && grid[row - 1][column - 1] == 0){
          grid[row - 1][column - 1] = 1;
          DrawX(renderer, row, column);
          count += 1;
          player *= -1;
        } 
        else if (grid[row - 1][column - 1] == 0){
          grid[row - 1][column - 1] = 2;
          DrawO(renderer, row, column);
          count += 1;
          player *= -1;
        } 
        if (checkState(grid, row, column) == 0) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
          SDL_RenderClear(renderer);
          SDL_Rect dest1 = {140, 250, 310, 70}; // Position and size of the text
          SDL_Rect dest2 = {180, 350, 220, 50}; // Position and size of the text
          if (player == -1) {
            SDL_RenderCopy(renderer, texture1, NULL, &dest1);
          }
          else {
            SDL_RenderCopy(renderer, texture2, NULL, &dest1);
          }
          SDL_RenderCopy(renderer, texture3, NULL, &dest2);
          SDL_RenderPresent(renderer);
          gameOver = 1;
        }
      }
      if (event.type == SDL_QUIT) {
        running = 0;
      } 
      else if (gameOver && event.key.keysym.sym == SDLK_q) {
        running = 0;
      }
    }
  }
  SDL_DestroyTexture(texture1);
  SDL_DestroyTexture(texture2);
  TTF_CloseFont(font);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
  TTF_Quit();
  return 0;
}

