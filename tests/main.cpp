#define SDL_MAIN_HANDLED

#include "Test5.h"
#include "TestInterface.h"
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

class Test1 : public TestInterface {
  // TestInterface interface
public:
  virtual void init() override {}
  virtual void run(SDL_Surface *surface) override {
    rs::SetPixel(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
                 {0xff, 0x0, 0x0, 0x0});
  }
  virtual void input(SDL_Keycode keyPressed) override {}
};

class Test2 : public TestInterface {
  // TestInterface interface
public:
  virtual void init() override {}
  virtual void run(SDL_Surface *surface) override {
    rs::DrawLine(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
                 rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,
                 {0xff, 0x0, 0x0, 0x0});
  }
  virtual void input(SDL_Keycode keyPressed) override {}
};

class Test3 : public TestInterface {
  // TestInterface interface
public:
  virtual void init() override {}
  virtual void run(SDL_Surface *surface) override {
    Uint32 x = rand() % SCREEN_WIDTH;
    rs::DrawLine(x, rand() % SCREEN_HEIGHT, x, rand() % SCREEN_HEIGHT,
                 {0xff, 0x0, 0x0, 0x0});
  }
  virtual void input(SDL_Keycode keyPressed) override {}
};

class Test4 : public TestInterface {
  // TestInterface interface
public:
  virtual void init() override {}
  virtual void run(SDL_Surface *surface) override {
    Uint32 y = rand() % SCREEN_HEIGHT;
    rs::DrawLine(rand() % SCREEN_WIDTH, y, rand() % SCREEN_WIDTH, y,
                 {0xff, 0x0, 0x0, 0x0});
  }
  virtual void input(SDL_Keycode keyPressed) override {}
};

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

  rs::Init(surface);

  bool running = true;
  // Event handler
  SDL_Event e;
  std::unique_ptr<TestInterface> testInterface = std::make_unique<Test1>();

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
          testInterface = std::make_unique<Test1>();
          testInterface->init();
          ClearSurface(surface);
          break;
        case SDLK_2:
          testInterface = std::make_unique<Test2>();
          testInterface->init();
          ClearSurface(surface);
          break;
        case SDLK_3:
          testInterface = std::make_unique<Test3>();
          testInterface->init();
          ClearSurface(surface);
          break;
        case SDLK_4:
          testInterface = std::make_unique<Test4>();
          testInterface->init();
          ClearSurface(surface);
          break;
        case SDLK_5:
          testInterface = std::make_unique<Test5>(SCREEN_WIDTH, SCREEN_HEIGHT);
          testInterface->init();
          ClearSurface(surface);
          break;
        case SDLK_6:
          break;
        default:
          testInterface->input(keyPressed);
        }
      }
      // User requests quit
      if (e.type == SDL_QUIT) {
        running = false;
      }
    }

    testInterface->run(surface);

    // Update the surface
    SDL_UpdateWindowSurface(window);
  }

  rs::ShutDown();

  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
