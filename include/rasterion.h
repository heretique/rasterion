#pragma once

#include <SDL2/SDL.h>

void SetPixel(SDL_Surface *surface, Uint32 x, Uint32 y, SDL_Color color);
SDL_Color GetPixel(SDL_Surface *surface, Uint32 x, Uint32 y);
void DrawLine(SDL_Surface *surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1,
              SDL_Color color);
