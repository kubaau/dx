#pragma once

#include "DXTypedefs.hpp"
#include "GraphicsTypedefs.hpp" // won't LINK without it - find out why
#include "IOTypedefs.hpp"

struct Mesh
{
    Mesh(D3DBuffer vb,
         unsigned v_count,
         unsigned stride,
         D3DBuffer ib = nullptr,
         unsigned i_count = 0,
         DXGI_FORMAT i_format = DXGI_FORMAT_R32_UINT)
        : vb(vb), v_count(v_count), stride(stride), ib(ib), i_count(i_count), i_format(i_format)
    {
    }

    Mesh(Device&, Filename);

    D3DBuffer vb;
    unsigned v_count;
    unsigned stride;

    D3DBuffer ib;
    unsigned i_count;
    DXGI_FORMAT i_format;
};
