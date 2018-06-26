#define SDL_MAIN_HANDLED

#include "rasterion.h"

#include <SDL2/SDL.h>
#include <iostream>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;

typedef void (*TestFunc)(SDL_Surface *);

void ClearSurface(SDL_Surface *surface) {
  assert(nullptr != surface);
  SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x0, 0x0, 0x0));
}

void Test1(SDL_Surface *surface) {
  SetPixel(surface, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
           {0xff, 0x0, 0x0, 0x0});
}

void Test2(SDL_Surface *surface) {
  DrawLine(surface, rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
           rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
           {0xff, 0x0, 0x0, 0x0});
}

void Test3(SDL_Surface *surface) { // vertical lines
  Uint32 x = rand() % SCREEN_WIDTH;
  DrawLine(surface, x, rand() % SCREEN_HEIGHT, x, rand() % SCREEN_HEIGHT,
           {0xff, 0x0, 0x0, 0x0});
}

void Test4(SDL_Surface *surface) { // horizontal lines
  Uint32 y = rand() % SCREEN_HEIGHT;
  DrawLine(surface, rand() % SCREEN_WIDTH, y, rand() % SCREEN_WIDTH, y,
           {0xff, 0x0, 0x0, 0x0});
}

int main(int argc, char **argv) {
  SDL_Window *window = nullptr;
  SDL_Surface *surface = nullptr;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "SDL could no initialize! SDL_Error: " << SDL_GetError()
              << "\n";
    return -1;
  }

  window = SDL_CreateWindow("Rasterion Tests", SDL_WINDOWPOS_UNDEFINED,
                            SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                            SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (nullptr == window) {
    std::cout << "Window could not be created! SDL_Error: " << SDL_GetError()
              << "\n";
    return -1;
  }

  surface = SDL_GetWindowSurface(window);
  ClearSurface(surface);

  bool running = true;
  // Event handler
  SDL_Event e;
  TestFunc testFunc = &Test1;

  while (running) {

    // Handle events on queue
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_KEYDOWN) {
        SDL_Keycode keyPressed = e.key.keysym.sym;

        switch (keyPressed) {
        case SDLK_ESCAPE:
          running = false;
          break;
        case SDLK_1:
          if (testFunc != &Test1) {
            testFunc = &Test1;
            ClearSurface(surface);
          }
          break;
        case SDLK_2:
          if (testFunc != &Test2) {
            testFunc = &Test2;
            ClearSurface(surface);
          }
          break;
        case SDLK_3:
          if (testFunc != &Test3) {
            testFunc = &Test3;
            ClearSurface(surface);
          }
          break;
        case SDLK_4:
          if (testFunc != &Test4) {
            testFunc = &Test4;
            ClearSurface(surface);
          }
          break;
        case SDLK_5:
          break;
        case SDLK_6:
          break;
        }
      }
      // User requests quit
      if (e.type == SDL_QUIT) {
        running = false;
      }
    }

    testFunc(surface);

    // Update the surface
    SDL_UpdateWindowSurface(window);
  }

  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
