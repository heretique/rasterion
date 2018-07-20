#include "rasterion.h"
#include <assert.h>
#include <cstdlib>

namespace rs {

class Rasterion {
public:
  bool Init(SDL_Surface *surface);
  void SetPixel(Uint32 x, Uint32 y, SDL_Color color);
  SDL_Color GetPixel(Uint32 x, Uint32 y);
  void DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color);
  void Shutdown();

private:
  SDL_Surface *_surface;
  void *_pixels;
  int _surfacePitch;
  Uint8 _surfaceBytesPerPixel;
  SDL_PixelFormat *_surfaceFormat;
};

static Rasterion rasterion;

bool Rasterion::Init(SDL_Surface *surface) {
  _surface = surface;
  _pixels = surface->pixels;
  _surfacePitch = surface->pitch;
  _surfaceBytesPerPixel = surface->format->BytesPerPixel;
  _surfaceFormat = surface->format;

  return true;
}

void Rasterion::SetPixel(Uint32 x, Uint32 y, SDL_Color color) {
  assert(_surface != nullptr);
  Uint32 col = SDL_MapRGB(_surfaceFormat, color.r, color.g, color.b);
  Uint8 *pixel = static_cast<Uint8 *>(_pixels);
  pixel += _surfacePitch * y + _surfaceBytesPerPixel * x;
  memcpy(pixel, &col, _surfaceBytesPerPixel);
}

SDL_Color Rasterion::GetPixel(Uint32 x, Uint32 y) {
  assert(_surface != nullptr);
  SDL_Color color;
  Uint32 col = 0;
  Uint8 *pixel = static_cast<Uint8 *>(_pixels);
  pixel += _surfacePitch * y + _surfaceBytesPerPixel * x;
  memcpy(&col, pixel, _surfaceBytesPerPixel);
  SDL_GetRGB(col, _surfaceFormat, &color.r, &color.g, &color.b);
  return color;
}

void Rasterion::DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1,
                         SDL_Color color) {

  if (x0 == x1) // vertical line
  {
    if (y0 > y1) {
      std::swap(y0, y1);
    }
    for (Uint32 y = y0; y <= y1; ++y)
      SetPixel(x0, y, color);

    return;
  }

  if (y0 == y1) { // horizontal line
    if (x0 > x1) {
      std::swap(x0, x1);
    }
    for (Uint32 x = x0; x <= x1; ++x)
      SetPixel(x, y0, color);

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
      SetPixel(y, x, color);
    } else {
      SetPixel(x, y, color);
    }
    error += derror;
    if (error > dx) {
      y += (y1 > y0 ? 1 : -1);
      error -= dx * 2;
    }
  }
}

void Rasterion::Shutdown() {
  _surface = nullptr;
  _pixels = nullptr;
  _surfacePitch = 0;
  _surfaceBytesPerPixel = 0;
  _surfaceFormat = nullptr;
}

bool Init(SDL_Surface *surface) { return rasterion.Init(surface); }

void SetPixel(Uint32 x, Uint32 y, SDL_Color color) {
  rasterion.SetPixel(x, y, color);
}

SDL_Color GetPixel(Uint32 x, Uint32 y) { return rasterion.GetPixel(x, y); }

void DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color) {
  rasterion.DrawLine(x0, y0, x1, y1, color);
}

void ShutDown() { rasterion.Shutdown(); }
}
