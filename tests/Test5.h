#pragma once

#include "TestInterface.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "rasterion.h"
#include "tinyOBJLoader.h"
#include <iostream>

class Test5 : public TestInterface
{
    // TestInterface interface
public:
    struct Point
    {
        float x;
        float y;
    };

    Test5(int width, int height)
        : _width(width)
        , _height(height)
    {
    }

    virtual void init() override
    {
        std::string inputfile = "assets/diablo3_pose.obj";

        std::string err;
        if (tinyobj::LoadObj(&_attrib, &_shapes, &_materials, &err, inputfile.c_str()))
        {
        }
        else
        {
            if (!err.empty())
            {  // `err` may contain warning message.
                std::cerr << err << std::endl;
            }
        }
    }
    virtual void run(SDL_Surface* surface) override
    {
        // Loop over shapes
        for (size_t s = 0; s < _shapes.size(); s++)
        {
            // Loop over faces(polygon)
            size_t index_offset = 0;
            for (size_t f = 0; f < _shapes[s].mesh.num_face_vertices.size(); f++)
            {
                int fv = _shapes[s].mesh.num_face_vertices[f];

                // Loop over vertices in the face.
                for (size_t v = 1; v < fv; v++)
                {
                    // access to vertex
                    tinyobj::index_t idx  = _shapes[s].mesh.indices[index_offset + v];
                    tinyobj::index_t idx0 = _shapes[s].mesh.indices[index_offset + v - 1];
                    tinyobj::real_t  vx   = _attrib.vertices[3 * idx.vertex_index + 0];
                    tinyobj::real_t  vy   = _attrib.vertices[3 * idx.vertex_index + 1];
                    //          tinyobj::real_t vz = _attrib.vertices[3 * idx.vertex_index
                    //          + 2];
                    tinyobj::real_t vx0 = _attrib.vertices[3 * idx0.vertex_index + 0];
                    tinyobj::real_t vy0 = _attrib.vertices[3 * idx0.vertex_index + 1];
                    //          tinyobj::real_t vz0 = _attrib.vertices[3 *
                    //          idx0.vertex_index + 2];
                    Point p0 = transform({vx0, vy0});
                    Point p1 = transform({vx, vy});
                    rs::DrawLine(p0.x, p0.y, p1.x, p1.y, {0xff, 0x0, 0x0, 0x0}, false);
                    // Optional: vertex colors
                    // tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
                    // tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
                    // tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
                }
                index_offset += fv;

                // per-face material
                _shapes[s].mesh.material_ids[f];
            }
        }
    }
    virtual void input(SDL_Keycode keyPressed) override
    {
    }

    Point transform(Point p)
    {
        Point ret;
        ret.x = (1 + p.x) * _width / 2.f;
        ret.y = (1 - p.y) * _height / 2.f;
        return ret;
    }

private:
    int                              _width;
    int                              _height;
    tinyobj::attrib_t                _attrib;
    std::vector<tinyobj::shape_t>    _shapes;
    std::vector<tinyobj::material_t> _materials;
};
