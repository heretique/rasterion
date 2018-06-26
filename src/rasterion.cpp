#include "rasterion.h"
#include <assert.h>
#include <cstdlib>

void SetPixel(SDL_Surface *surface, Uint32 x, Uint32 y, SDL_Color color) {
  assert(surface != nullptr);
  Uint32 col = SDL_MapRGB(surface->format, color.r, color.g, color.b);
  Uint8 bytesPerPixel = surface->format->BytesPerPixel;
  Uint8 *pixel = static_cast<Uint8 *>(surface->pixels);
  pixel += surface->pitch * y + bytesPerPixel * x;
  memcpy(pixel, &col, bytesPerPixel);
}

SDL_Color GetPixel(SDL_Surface *surface, Uint32 x, Uint32 y) {
  assert(surface != nullptr);
  SDL_Color color;
  Uint32 col = 0;
  Uint8 bytesPerPixel = surface->format->BytesPerPixel;
  Uint8 *pixel = static_cast<Uint8 *>(surface->pixels);
  pixel += surface->pitch * y + bytesPerPixel * x;
  memcpy(&col, pixel, bytesPerPixel);
  SDL_GetRGB(col, surface->format, &color.r, &color.g, &color.b);
  return color;
}

void DrawLine(SDL_Surface *surface, Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1,
              SDL_Color color) {

  if (x0 == x1) // vertical line
  {
    if (y0 > y1) {
      std::swap(y0, y1);
    }
    for (Uint32 y = y0; y <= y1; ++y)
      SetPixel(surface, x0, y, color);

    return;
  }

  if (y0 == y1) { // horizontal line
    if (x0 > x1) {
      std::swap(x0, x1);
    }
    for (Uint32 x = x0; x <= x1; ++x)
      SetPixel(surface, x, y0, color);

    return;
  }

  bool steep = false;
  if (std::abs((int)(x0 - x1)) < std::abs((int)(y0 - y1))) {
    std::swap(x0, y0);
    std::swap(x1, y1);
    steep = true;
  }
  if (x0 > x1) {
    std::swap(x0, x1);
    std::swap(y0, y1);
  }
  int dx = x1 - x0;
  int dy = y1 - y0;
  int derror = std::abs(dy) * 2;
  int error = 0;
  int y = y0;

  for (Uint32 x = x0; x <= x1; ++x) {
    if (steep) {
      SetPixel(surface, y, x, color);
    } else {
      SetPixel(surface, x, y, color);
    }
    error += derror;
    if (error > dx) {
      y += (y1 > y0 ? 1 : -1);
      error -= dx * 2;
    }
  }
}
