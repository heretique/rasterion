#pragma once

namespace util
{
inline float fpart(float v)
{
    return v - std::floor(v);
}

inline float rfpart(float v)
{
    return 1 - fpart(v);
}
}
