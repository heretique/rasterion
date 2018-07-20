#pragma once

#include <SDL2/SDL.h>
#include <vector>

namespace rs {

// resource types
typedef struct { Uint32 id; } Buffer;
typedef struct { Uint32 id; } Image;
typedef struct { Uint32 id; } Shader;
typedef struct { Uint32 id; } Pipeline;

enum class BufferType {
  Vertex, //
  Index   //
};

enum class CullMode {
  None, //
  Back, //
  Front //
};

enum class FaceWinding {
  CCW, // counter clockwise
  CW   //
};

enum class CompareFunc {
  Never,        //
  Less,         //
  Equal,        //
  LessEqual,    //
  Greater,      //
  NotEqual,     //
  GreaterEqual, //
  Always        //
};

enum class BlendFactor {
  Zero,               //
  One,                //
  SrcColor,           //
  OneMinusSrcColor,   //
  SrcAlpha,           //
  OneMinusSrcAlpha,   //
  DstColor,           //
  OneMinusDstColor,   //
  DstAlpha,           //
  OneMinusDstAlpha,   //
  SrcAlphaSaturated,  //
  BlendColor,         //
  OneMinusBlendColor, //
  BlendAlpha,         //
  OneMinusBlendAlpha  //
};

enum class BlendOp {
  Add,            //
  Subtract,       //
  ReverseSubtract //
};

enum class VertexFormat {
  Float,  //
  Float2, //
  Float3, //
  Float4  //
};

enum class PrimitiveType {
  Points,       //
  Lines,        //
  LineStrip,    //
  Triangles,    //
  TriangleStrip //
};

// descriptors for resources
struct BufferDesc {
  Uint32 size{0};
  BufferType type{BufferType::Vertex};
  const void *content{nullptr}; // initial content
};

struct BufferAttrDesc {};

struct BufferLayoutDesc {
  std::vector<BufferAttrDesc> attributes;
};

struct ImageDesc {
  Uint32 width{0};
  Uint32 height{0};
  Uint8 bpp{0}; // bytes per pixel
  const void *content{nullptr};
};
struct ShaderDesc {};

struct PipelineDesc {
  BufferLayoutDesc bufferLayout;
  Shader shader;
  PrimitiveType primitiveType;
};

// draw state
struct DrawState {
  Pipeline pipeline;
  Buffer vertexBuffer{0};
  Uint32 vertexBufferOffset{0};
  Buffer indexBuffer{0};
  Uint32 indexBufferOffset{0};
  std::vector<Image> images;
};
// init & shutdown
bool Init(SDL_Surface *surface);
void ShutDown();
// low level drawing
void SetPixel(Uint32 x, Uint32 y, SDL_Color color);
SDL_Color GetPixel(Uint32 x, Uint32 y);
void DrawLine(Uint32 x0, Uint32 y0, Uint32 x1, Uint32 y1, SDL_Color color);

// resource creation
Buffer MakeBuffer(const BufferDesc &bufferDesc);
Image MakeImage(const ImageDesc &imageDesc);
Shader MakeShader(const ShaderDesc &shaderDesc);
Pipeline MakePipeline(const PipelineDesc &pipelineDesc);

// high level drawing
void Draw(const DrawState &drawState);
}
