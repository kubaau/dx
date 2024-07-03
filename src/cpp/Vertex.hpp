#pragma once

#include "MathTypedefs.hpp"

namespace Vertex
{
    struct Basic
    {
        float3 position{};
        float3 normal{};
        float2 tex{};

        Basic() {}

        Basic(const float3& p, const float3& n, const float2& uv) : position(p), normal(n), tex(uv) {}

        Basic(float px, float py, float pz, float nx, float ny, float nz, float u, float v)
            : position(px, py, pz), normal(nx, ny, nz), tex(u, v)
        {
        }
    };

    struct PointSprite
    {
        float3 position{};
        float2 size{};
    };

    struct PositionOnly
    {
        float3 position{};
    };
} // namespace Vertex
