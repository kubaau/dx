#include "DepthStencilStates.hpp"
#include "GraphicsDebugUtils.hpp"

DSStates dss;

void initDSStates(Device& device)
{
    using namespace DSS;
    initMarkStencil(device);
    initDrawReflection(device);
    initNoDoubleBlend(device);
    initDepthWriteOff(device);
    initDepthCounter(device);
    initDepthCounterVisualizer(device);
    initLessEqual(device);
}

namespace DSS
{
    void initMarkStencil(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = true;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // We are not rendering backfacing polygons, so these settings do not matter.
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("mark_stencil", s);
    }

    void initDrawReflection(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = true;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // We are not rendering backfacing polygons, so these settings do not matter.
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("draw_reflection", s);
    }

    void initNoDoubleBlend(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = true;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // We are not rendering backfacing polygons, so these settings do not matter.
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("no_double_blend", s);
    }

    void initDepthWriteOff(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        desc.DepthFunc = D3D11_COMPARISON_LESS;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("depth_write_off", s);
    }

    void initDepthCounter(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = true;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        // We are not rendering backfacing polygons, so these settings do not matter.
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_INCR_SAT;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR_SAT;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("depth_counter", s);
    }

    void initDepthCounterVisualizer(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = false;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS;
        desc.StencilEnable = true;
        desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
        desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

        desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.FrontFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        // We are not rendering backfacing polygons, so these settings do not matter.
        desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        desc.BackFace.StencilFunc = D3D11_COMPARISON_EQUAL;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("depth_counter_visualizer", s);
    }

    void initLessEqual(Device& device)
    {
        D3D11_DEPTH_STENCIL_DESC desc{};
        desc.DepthEnable = true;
        desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
        desc.StencilEnable = false;

        DSStatePtr s;
        HR(device.CreateDepthStencilState(&desc, &s));
        dss.emplace("less_equal", s);
    }
} // namespace DSS
