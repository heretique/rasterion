#pragma once

#include <SDL.h>

class TestInterface
{
public:
    virtual void init()                        = 0;
    virtual void run(SDL_Surface* surface)     = 0;
    virtual void input(SDL_Keycode keyPressed) = 0;
};
