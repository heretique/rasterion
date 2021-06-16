#define SDL_MAIN_HANDLED

#include "Test5.h"
#include "TestInterface.h"
#include "rasterion.h"
#include <iostream>
#include <Hq/Math/Vector.h>
#include <SDL.h>
#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

const int SCREEN_WIDTH  = 800;
const int SCREEN_HEIGHT = 600;

typedef void (*TestFunc)(SDL_Surface*);

void ClearSurface(SDL_Surface* surface)
{
    assert(nullptr != surface);
    SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0x0, 0x0, 0x0));
}

class Test1 : public TestInterface
{
    // TestInterface interface
public:
    virtual void init() override
    {
        using namespace hq::math;
        // TEST VECTORS
        Vector3f v1(1.f, 1.f, 1.f);
        Vector3f v2 = {1.f, 2.f, 3.f};
        Vector3f v3 {1.f, 2.f, 3.f};
        Vector3f v4 = {{1.f, 2.f}, 3.f};
        Vector3f x {1.f, 0.f, 0.f};
        Vector3f y {0.f, 1.f, 0.f};
        Vector3f z = cross(x, y);
        float    d = dot(x, z);
    }
    virtual void run(SDL_Surface* surface) override
    {
        rs::SetPixel(rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT, {0xff, 0x0, 0x0, 0x0});
    }
    virtual void input(SDL_Keycode keyPressed) override {}
};

class Test2 : public TestInterface
{
    // TestInterface interface
public:
    virtual void init() override {}
    virtual void run(SDL_Surface* surface) override
    {
        rs::DrawLine(rand() % (SCREEN_WIDTH - 1), rand() % (SCREEN_HEIGHT - 1), rand() % (SCREEN_WIDTH - 1),
                     rand() % (SCREEN_HEIGHT - 1), {0xff, 0x0, 0x0, 0x0}, true);
    }
    virtual void input(SDL_Keycode keyPressed) override {}
};

class Test3 : public TestInterface
{
    // TestInterface interface
public:
    virtual void init() override {}
    virtual void run(SDL_Surface* surface) override
    {
        Uint32 x = rand() % SCREEN_WIDTH;
        rs::DrawLine(x, rand() % SCREEN_HEIGHT, x, rand() % SCREEN_HEIGHT, {0xff, 0x0, 0x0, 0x0});
    }
    virtual void input(SDL_Keycode keyPressed) override {}
};

class Test4 : public TestInterface
{
    // TestInterface interface
public:
    virtual void init() override {}
    virtual void run(SDL_Surface* surface) override
    {
        Uint32 y = rand() % SCREEN_HEIGHT;
        rs::DrawLine(rand() % SCREEN_WIDTH, y, rand() % SCREEN_WIDTH, y, {0xff, 0x0, 0x0, 0x0});
    }
    virtual void input(SDL_Keycode keyPressed) override {}
};

int main(int argc, char** argv)
{
    int result = Catch::Session().run(argc, argv);

    std::cout << "Catch tests result: " << result << "\n";

    SDL_Window*  window  = nullptr;
    SDL_Surface* surface = nullptr;

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cout << "SDL could no initialize! SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    window = SDL_CreateWindow("Rasterion Tests", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                              SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (nullptr == window)
    {
        std::cout << "Window could not be created! SDL_Error: " << SDL_GetError() << "\n";
        return -1;
    }

    surface = SDL_GetWindowSurface(window);
    ClearSurface(surface);

    rs::Init(surface);

    bool running = true;
    // Event handler
    SDL_Event                      e;
    std::unique_ptr<TestInterface> testInterface = std::make_unique<Test1>();
    testInterface->init();

    while (running)
    {
        // Handle events on queue
        while (SDL_PollEvent(&e) != 0)
        {
            if (e.type == SDL_KEYDOWN)
            {
                SDL_Keycode keyPressed = e.key.keysym.sym;

                switch (keyPressed)
                {
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
                        testInterface = std::make_unique<Test5>(SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
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
            if (e.type == SDL_QUIT)
            {
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
