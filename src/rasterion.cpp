#include "rasterion.h"
#include "id_pool.h"

#include <assert.h>

#include <cstdlib>
#include <unordered_map>
#include "math/util.h"

namespace rs
{
class Rasterion
{
public:
    bool init(SDL_Surface* surface);
    void shutdown();

    // direct surface access
    void setPixel(Uint32 x, Uint32 y, SDL_Color color, float blendFactor = 1.f);
    SDL_Color getPixel(Uint32 x, Uint32 y);

    void drawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color);
    void drawLineAntialiased(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color);
    void drawCircle(Uint32 x, Uint32 y, Uint32 r, SDL_Color color);
    void drawRectangle(Uint32 x, Uint32 y, Uint32 w, Uint32 h, SDL_Color color);

    // resource creation
    Buffer makeBuffer(const BufferDesc& bufferDesc);
    Image makeImage(const ImageDesc& imageDesc);
    Shader makeShader(const ShaderDesc& shaderDesc);
    Pipeline makePipeline(const PipelineDesc& pipelineDesc);

    // resource update
    void clearBuffer(Buffer buffer, Uint8 value);
    void clearImage(Image image, SDL_Color color);
    void updateBuffer(Buffer buffer, const void* data, size_t size);
    void updateImage(Image image, const void* data, size_t size);

private:
    SDL_Surface*     _surface;
    void*            _pixels;
    int              _surfacePitch;
    Uint8            _surfaceBytesPerPixel;
    SDL_PixelFormat* _surfaceFormat;
    std::unordered_map<Uint32, BufferDesc>   _buffers;
    std::unordered_map<Uint32, ImageDesc>    _images;
    std::unordered_map<Uint32, ShaderDesc>   _shaders;
    std::unordered_map<Uint32, PipelineDesc> _pipelines;
    IdPool _buffersIdPool;
    IdPool _imageIdPool;
    IdPool _shadersIdPool;
    IdPool _pipelinesIdPool;
};

static Rasterion rasterion;

bool Rasterion::init(SDL_Surface* surface)
{
    _surface              = surface;
    _pixels               = surface->pixels;
    _surfacePitch         = surface->pitch;
    _surfaceBytesPerPixel = surface->format->BytesPerPixel;
    _surfaceFormat        = surface->format;

    return true;
}

void Rasterion::shutdown()
{
    _surface              = nullptr;
    _pixels               = nullptr;
    _surfacePitch         = 0;
    _surfaceBytesPerPixel = 0;
    _surfaceFormat        = nullptr;
}

void Rasterion::setPixel(Uint32 x, Uint32 y, SDL_Color color, float blendFactor)
{
    assert(_surface != nullptr);
    assert(blendFactor <= 1.f && blendFactor >= 0.f);
    Uint8* pixel = static_cast<Uint8*>(_pixels);
    pixel += _surfacePitch * y + _surfaceBytesPerPixel * x;
    if (blendFactor == 1.f)
    {
        // convert and copy
        Uint32 col = SDL_MapRGB(_surfaceFormat, color.r, color.g, color.b);
        memcpy(pixel, &col, _surfaceBytesPerPixel);
        return;
    }

    // else we need to get existing color for blending
    Uint32    bufferCol = 0;
    SDL_Color bufferColor;
    memcpy(&bufferCol, pixel, _surfaceBytesPerPixel);
    SDL_GetRGB(bufferCol, _surfaceFormat, &bufferColor.r, &bufferColor.g, &bufferColor.b);
    SDL_Color blendedColor;
    // and blend
    blendedColor.r = color.r * blendFactor + (1.f - blendFactor) * bufferColor.r;
    blendedColor.g = color.g * blendFactor + (1.f - blendFactor) * bufferColor.g;
    blendedColor.b = color.b * blendFactor + (1.f - blendFactor) * bufferColor.b;
    // convert and copy
    Uint32 col = SDL_MapRGB(_surfaceFormat, blendedColor.r, blendedColor.g, blendedColor.b);
    memcpy(pixel, &col, _surfaceBytesPerPixel);
}

SDL_Color Rasterion::getPixel(Uint32 x, Uint32 y)
{
    assert(_surface != nullptr);
    SDL_Color color;
    Uint32    col   = 0;
    Uint8*    pixel = static_cast<Uint8*>(_pixels);
    pixel += _surfacePitch * y + _surfaceBytesPerPixel * x;
    memcpy(&col, pixel, _surfaceBytesPerPixel);
    SDL_GetRGB(col, _surfaceFormat, &color.r, &color.g, &color.b);
    return color;
}

/// Drawing lines using Bresenham's alrgorithm
/// https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm
void Rasterion::drawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color)
{
    if (x0 == x1)  // vertical line
    {
        if (y0 > y1)
        {
            std::swap(y0, y1);
        }
        for (Uint32 y = y0; y <= y1; ++y)
            setPixel(x0, y, color);

        return;
    }

    if (y0 == y1)
    {  // horizontal line
        if (x0 > x1)
        {
            std::swap(x0, x1);
        }
        for (Uint32 x = x0; x <= x1; ++x)
            setPixel(x, y0, color);

        return;
    }

    bool steep = false;
    if (std::abs((int)(x0 - x1)) < std::abs((int)(y0 - y1)))
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }
    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    int dx     = x1 - x0;
    int dy     = y1 - y0;
    int derror = std::abs(dy) * 2;
    int error  = 0;
    int y      = y0;

    for (Uint32 x = x0; x <= x1; ++x)
    {
        if (steep)
        {
            setPixel(y, x, color);
        }
        else
        {
            setPixel(x, y, color);
        }
        error += derror;
        if (error > dx)
        {
            y += (y1 > y0 ? 1 : -1);
            error -= dx * 2;
        }
    }
}

/// Draw antialised lines using Xiaolin Wu's algorithm
/// https://en.wikipedia.org/wiki/Xiaolin_Wu%27s_line_algorithm
/// https://rosettacode.org/wiki/Xiaolin_Wu%27s_line_algorithm#C
void Rasterion::drawLineAntialiased(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color)
{
    bool steep = std::abs((int)(y1 - y0)) > std::abs((int)(x1 - x0));
    if (steep)
    {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }

    if (x0 > x1)
    {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float dx       = (float)x1 - x0;
    float dy       = (float)y1 - y0;
    float gradient = dy / dx;

    if (dx == 0.f)
        gradient = 1.f;

    float xend = std::roundf(x0);
    float yend = y0 + gradient * (xend - x0);
    float xgap = util::rfpart(x0 + 0.5f);

    int xpxl1 = (int)xend;  // this will be used in the main loop
    int ypxl1 = (int)yend;
    if (steep)
    {
        setPixel(ypxl1, xpxl1, color, util::rfpart(yend) * xgap);
        setPixel(ypxl1 + 1, xpxl1, color, util::fpart(yend) * xgap);
    }

    else
    {
        setPixel(xpxl1, ypxl1, color, util::rfpart(yend) * xgap);
        setPixel(xpxl1, ypxl1 + 1, color, util::fpart(yend) * xgap);
    }

    float intery = yend + gradient;  // first y-intersection for the main loop

    // handle second endpoint
    xend      = std::roundf(x1);
    yend      = y1 + gradient * (xend - x1);
    xgap      = util::fpart(x1 + 0.5f);
    int xpxl2 = (int)xend;  // this will be used in the main loop
    int ypxl2 = (int)yend;

    if (steep)
    {
        setPixel(ypxl2, xpxl2, color, util::rfpart(yend) * xgap);
        setPixel(ypxl2 + 1, xpxl2, color, util::fpart(yend) * xgap);
    }
    else
    {
        setPixel(xpxl2, ypxl2, color, util::rfpart(yend) * xgap);
        setPixel(xpxl2, ypxl2 + 1, color, util::fpart(yend) * xgap);
    }

    // main loop
    if (steep)
        for (int x = xpxl1 + 1; x < xpxl2; ++x)
        {
            setPixel((int)intery, x, color, util::rfpart(intery));
            setPixel((int)intery + 1, x, color, util::fpart(intery));
            intery = intery + gradient;
        }
    else
        for (int x = xpxl1 + 1; x < xpxl2; ++x)
        {
            setPixel(x, (int)intery, color, util::rfpart(intery));
            setPixel(x, (int)intery + 1, color, util::fpart(intery));
            intery = intery + gradient;
        }
}

void Rasterion::drawCircle(Uint32 x, Uint32 y, Uint32 r, SDL_Color color)
{
}

void Rasterion::drawRectangle(Uint32 x, Uint32 y, Uint32 w, Uint32 h, SDL_Color color)
{
}

Buffer Rasterion::makeBuffer(const BufferDesc& bufferDesc)
{
    Uint32 id = _buffersIdPool.getId();
    if (id != 0)
    {
        BufferDesc descriptor = bufferDesc;
        descriptor.content    = std::malloc(descriptor.size);
        if (nullptr == descriptor.content)
        {
            _buffersIdPool.freeId(id);
            return {0};
        }

        if (nullptr != bufferDesc.content)
        {
            std::memcpy(descriptor.content, bufferDesc.content, bufferDesc.size);
        }
        _buffers.insert(std::make_pair(id, descriptor));
    }
    return {id};
}

Image Rasterion::makeImage(const ImageDesc& imageDesc)
{
    Uint32 id = _imageIdPool.getId();
    if (id != 0)
    {
        ImageDesc descriptor = imageDesc;
        descriptor.content   = std::malloc(descriptor.width * descriptor.height * descriptor.bpp);
        if (nullptr == descriptor.content)
        {
            _imageIdPool.freeId(id);
            return {0};
        }

        if (nullptr != imageDesc.content)
        {
            std::memcpy(descriptor.content, imageDesc.content, descriptor.width * descriptor.height * descriptor.bpp);
        }
        _images.insert(std::make_pair(id, descriptor));
    }
    return {id};
}

Shader Rasterion::makeShader(const ShaderDesc& shaderDesc)
{
    Uint32 id = _shadersIdPool.getId();

    return {id};
}

Pipeline Rasterion::makePipeline(const PipelineDesc& pipelineDesc)
{
    Uint32 id = _pipelinesIdPool.getId();

    return {id};
}

void Rasterion::clearBuffer(Buffer buffer, Uint8 value)
{
    if (0 == buffer.id)
        return;

    auto bufferIt = _buffers.find(buffer.id);
    if (bufferIt == _buffers.end())
        return;

    BufferDesc& descriptor = bufferIt->second;
    std::memset(descriptor.content, value, descriptor.size);
}

void Rasterion::clearImage(Image image, SDL_Color color)
{
    if (0 == image.id)
        return;

    auto imageIt = _images.find(image.id);
    if (imageIt == _images.end())
        return;

    // TODO Convert color to proper bitformat then memset
}

void Rasterion::updateBuffer(Buffer buffer, const void* data, size_t size)
{
    if (0 == buffer.id)
        return;

    auto bufferIt = _buffers.find(buffer.id);
    if (bufferIt == _buffers.end())
        return;

    BufferDesc& descriptor = bufferIt->second;
    assert(size < descriptor.size);
    std::memcpy(descriptor.content, data, size);
}

void Rasterion::updateImage(Image image, const void* data, size_t size)
{
    if (0 == image.id)
        return;

    auto imageIt = _images.find(image.id);
    if (imageIt == _images.end())
        return;

    ImageDesc& descriptor = imageIt->second;
    assert(size < descriptor.size());

    std::memcpy(descriptor.content, data, size);
}

//////// API ////////

bool Init(SDL_Surface* surface)
{
    return rasterion.init(surface);
}
void ShutDown()
{
    rasterion.shutdown();
}

void SetPixel(Uint32 x, Uint32 y, SDL_Color color)
{
    rasterion.setPixel(x, y, color);
}
void SetPixel(SDL_Point p, SDL_Color color)
{
    rasterion.setPixel(p.x, p.y, color);
}

SDL_Color GetPixel(Uint32 x, Uint32 y)
{
    return rasterion.getPixel(x, y);
}

SDL_Color GetPixel(SDL_Point p)
{
    return rasterion.getPixel(p.x, p.y);
}

void DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color, bool antialiased)
{
    antialiased ? rasterion.drawLineAntialiased(x0, y0, x1, y1, color) : rasterion.drawLine(x0, y0, x1, y1, color);
}

void DrawLine(SDL_Point p1, SDL_Point p2, SDL_Color color, bool antialiased)
{
    antialiased ? rasterion.drawLineAntialiased(p1.x, p1.y, p2.x, p2.y, color)
                : rasterion.drawLine(p1.x, p1.y, p2.x, p2.y, color);
}

void DrawCircle(Uint32 x, Uint32 y, Uint32 r, SDL_Color color)
{
    rasterion.drawCircle(x, y, r, color);
}

void DrawCircle(SDL_Point p, Uint32 r, SDL_Color color)
{
    rasterion.drawCircle(p.x, p.y, r, color);
}

void DrawRectangle(Uint32 x, Uint32 y, Uint32 w, Uint32 h, SDL_Color color)
{
    rasterion.drawRectangle(x, y, w, h, color);
}

void DrawRectangle(SDL_Point topLeft, SDL_Point bottomRight, SDL_Color color)
{
    rasterion.drawRectangle(topLeft.x, topLeft.y, bottomRight.x - topLeft.x, bottomRight.y - topLeft.y, color);
}

void DrawRectangle(SDL_Rect r, SDL_Color color)
{
}

Buffer MakeBuffer(const BufferDesc& bufferDesc)
{
    return rasterion.makeBuffer(bufferDesc);
}

Image MakeImage(const ImageDesc& imageDesc)
{
    return rasterion.makeImage(imageDesc);
}

Shader MakeShader(const ShaderDesc& shaderDesc)
{
    return rasterion.makeShader(shaderDesc);
}

Pipeline MakePipeline(const PipelineDesc& pipelineDesc)
{
    return rasterion.makePipeline(pipelineDesc);
}

void DestroyBuffer(Buffer buffer)
{
}

void DestroyImage(Image image)
{
}

void DestroyShader(Shader shader)
{
}

void DestroyPipeline(Pipeline pipeline)
{
}
}
