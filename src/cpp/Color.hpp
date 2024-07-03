#pragma once

#include "MathTypedefs.hpp"

using Color = DirectX::XMVECTORF32;
namespace Colors
{
    XMGLOBALCONST Color white = {1.0f, 1.0f, 1.0f, 1.0f};
    XMGLOBALCONST Color black = {0.0f, 0.0f, 0.0f, 1.0f};
    XMGLOBALCONST Color red = {1.0f, 0.0f, 0.0f, 1.0f};
    XMGLOBALCONST Color green = {0.0f, 1.0f, 0.0f, 1.0f};
    XMGLOBALCONST Color blue = {0.0f, 0.0f, 1.0f, 1.0f};
    XMGLOBALCONST Color yellow = {1.0f, 1.0f, 0.0f, 1.0f};
    XMGLOBALCONST Color cyan = {0.0f, 1.0f, 1.0f, 1.0f};
    XMGLOBALCONST Color magenta = {1.0f, 0.0f, 1.0f, 1.0f};
    XMGLOBALCONST Color silver = {0.75f, 0.75f, 0.75f, 1.0f};
    XMGLOBALCONST Color light_steel_blue = {0.69f, 0.77f, 0.87f, 1.0f};
}