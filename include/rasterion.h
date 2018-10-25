#pragma once

#include <vector>
#include <SDL2/SDL.h>

namespace rs
{
// resource types
typedef struct
{
    Uint32 id;
} Buffer;
typedef struct
{
    Uint32 id;
} Image;
typedef struct
{
    Uint32 id;
} Shader;
typedef struct
{
    Uint32 id;
} Pipeline;

enum class BufferType
{
    Vertex,  //
    Index    //
};

enum class CullMode
{
    None,  //
    Back,  //
    Front  //
};

enum class FaceWinding
{
    CCW,  // counter clockwise
    CW    //
};

enum class CompareFunc
{
    Never,         //
    Less,          //
    Equal,         //
    LessEqual,     //
    Greater,       //
    NotEqual,      //
    GreaterEqual,  //
    Always         //
};

enum class BlendFactor
{
    Zero,                //
    One,                 //
    SrcColor,            //
    OneMinusSrcColor,    //
    SrcAlpha,            //
    OneMinusSrcAlpha,    //
    DstColor,            //
    OneMinusDstColor,    //
    DstAlpha,            //
    OneMinusDstAlpha,    //
    SrcAlphaSaturated,   //
    BlendColor,          //
    OneMinusBlendColor,  //
    BlendAlpha,          //
    OneMinusBlendAlpha   //
};

enum class BlendOp
{
    Add,             //
    Subtract,        //
    ReverseSubtract  //
};

enum class VertexFormat
{
    Float,   //
    Float2,  //
    Float3,  //
    Float4   //
};

enum class PrimitiveType
{
    Points,        //
    Lines,         //
    LineStrip,     //
    Triangles,     //
    TriangleStrip  //
};

enum class IndexType
{
    Uint8,
    Uint16,
    Uint32
};

enum class PixelFormat
{
    None,
    RGBA8,
    RGB8,
    RGBA4,
    R5G6B5,
    R5G5B5A1,
    R10G10B10A2,
    RGBA32F,
    RGBA16F,
    R32F,
    R16F,
    L8,
    DXT1,
    DXT3,
    DXT5,
    DEPTH,
    DEPTHSTENCIL,
    PVRTC2_RGB,
    PVRTC4_RGB,
    PVRTC2_RGBA,
    PVRTC4_RGBA,
    ETC2_RGB8,
    ETC2_SRGB8,
    Count
};

// descriptors for resources
struct BufferDesc
{
    Uint32     size {0};
    BufferType type {BufferType::Vertex};
    void*      content {nullptr};  // initial content
};

struct BufferAttrDesc
{
};

struct BufferLayoutDesc
{
    std::vector<BufferAttrDesc> attributes;
};

struct ImageDesc
{
    Uint32      width {0};
    Uint32      height {0};
    Uint8       bpp {0};  // bytes per pixel
    PixelFormat pixelFormat;
    void*       content {nullptr};

    size_t size() const
    {
        return width * height * bpp;
    }
};
struct ShaderDesc
{
};

struct PipelineDesc
{
    BufferLayoutDesc bufferLayout;
    Shader           shader;
    PrimitiveType    primitiveType;
};

// draw state
struct DrawState
{
    Pipeline           pipeline;
    Buffer             vertexBuffer {0};
    Uint32             vertexBufferOffset {0};
    Buffer             indexBuffer {0};
    Uint32             indexBufferOffset {0};
    std::vector<Image> images;
};
// init & shutdown
bool Init(SDL_Surface* surface);
void ShutDown();

// low level drawing
void SetPixel(Uint32 x, Uint32 y, SDL_Color color);
void SetPixel(SDL_Point p, SDL_Color color);

SDL_Color GetPixel(Uint32 x, Uint32 y);
SDL_Color GetPixel(SDL_Point p);

void DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color, bool antialiased = false);
void DrawLine(SDL_Point p1, SDL_Point p2, SDL_Color color, bool antialiased = false);

void DrawCircle(Uint32 x, Uint32 y, Uint32 r, SDL_Color color);
void DrawCircle(SDL_Point p, Uint32 r, SDL_Color color);

void DrawRectangle(Uint32 x, Uint32 y, Uint32 w, Uint32 h, SDL_Color color);
void DrawRectangle(SDL_Point p, SDL_Point size, SDL_Color color);
void DrawRectangle(SDL_Rect r, SDL_Color color);

// resource creation
Buffer   MakeBuffer(const BufferDesc& bufferDesc);
Image    MakeImage(const ImageDesc& imageDesc);
Shader   MakeShader(const ShaderDesc& shaderDesc);
Pipeline MakePipeline(const PipelineDesc& pipelineDesc);

// resource update
void ClearBuffer(Buffer, Uint8 value);
void ClearImage(Image, SDL_Color);
void UpdateBuffer(Buffer, const void* data, size_t size);
void UpdateImage(Image, const void* data, size_t size);

// resource destruction
void DestroyBuffer(Buffer buffer);
void DestroyImage(Image image);
void DestroyShader(Shader shader);
void DestroyPipeline(Pipeline pipeline);

// high level drawing
void Draw(const DrawState& drawState);
}
