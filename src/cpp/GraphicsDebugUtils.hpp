#pragma once

#include <ext/dxerr.h>
#include <iostream>

#include "MathTypedefs.hpp"

#if defined(DEBUG) | defined(_DEBUG)
#define HR(x)                            \
    {                                    \
        const auto hr = (x);             \
        if (FAILED(hr))                  \
            DXTRACE_ERR_MSGBOX(L#x, hr); \
    }
#else
#define HR(x) (x)
#endif

inline ostream& operator<<(ostream& os, float2 f)
{
    return os << "{" << f.x << ", " << f.y << "}";
}

inline ostream& operator<<(ostream& os, float3 f)
{
    return os << "{" << f.x << ", " << f.y << ", " << f.z << "}";
}

inline ostream& operator<<(ostream& os, float4 f)
{
    return os << "{" << f.x << ", " << f.y << ", " << f.z << ", " << f.w << "}";
}

inline ostream& operator<<(ostream& os, const float4x4& m)
{
    os << "{" << m._11;
    os << ", " << m._12;
    os << ", " << m._13;
    os << ", " << m._14;
    os << "}";
    os << "{" << m._21;
    os << ", " << m._22;
    os << ", " << m._23;
    os << ", " << m._24;
    os << "}";
    os << "{" << m._31;
    os << ", " << m._32;
    os << ", " << m._33;
    os << ", " << m._34;
    os << "}";
    os << "{" << m._41;
    os << ", " << m._42;
    os << ", " << m._43;
    os << ", " << m._44;
    os << "}";
    return os;
}

ostream& operator<<(ostream&, const struct DirectionalLight&);
ostream& operator<<(ostream&, const struct Material&);
