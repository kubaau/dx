#pragma once

#include "MathTypedefs.hpp"

inline auto inverseTranspose(const Matrix& m)
{
    using namespace DirectX;

    auto a = m;
    a.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
    auto det = XMMatrixDeterminant(a);
    return XMMatrixTranspose(XMMatrixInverse(&det, a));
}

// Returns the polar angle of the point (x,y) in [0, 2*pi).
inline auto angleFromXY(float x, float y)
{
    float theta = 0.0f;

    // Quadrant I or IV
    if (x >= 0.0f)
    {
        // If x = 0, then atanf(y/x) = +pi/2 if y > 0
        //                atanf(y/x) = -pi/2 if y < 0
        theta = atanf(y / x); // in [-pi/2, +pi/2]

        if (theta < 0.0f)
            theta += 2.0f * pi; // in [0, 2*pi).
    }
    // Quadrant II or III
    else
        theta = atanf(y / x) + pi; // in [0, 2*pi).

    return theta;
}
