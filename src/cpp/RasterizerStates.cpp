#include "RasterizerStates.hpp"
#include "GraphicsDebugUtils.hpp"

RStates rs;

void initRStates(Device& device)
{
    using namespace RS;
    initWireframe(device);
    initWireframeNoCull(device);
    initNoCull(device);
    initCullClockwise(device);
    initWireframeCullClockwise(device);
    initScissor(device);
}

namespace RS
{
    void initWireframe(Device& device)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = false;
        desc.DepthClipEnable = true;

        RStatePtr s;
        HR(device.CreateRasterizerState(&desc, &s));
        rs.emplace("wireframe", s);
    }

    void initWireframeNoCull(Device& device)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_NONE;
        desc.FrontCounterClockwise = false;
        desc.DepthClipEnable = true;

        RStatePtr s;
        HR(device.CreateRasterizerState(&desc, &s));
        rs.emplace("wireframe_no_cull", s);
    }

    void initNoCull(Device& device)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_NONE;
        desc.FrontCounterClockwise = false;
        desc.DepthClipEnable = true;

        RStatePtr s;
        HR(device.CreateRasterizerState(&desc, &s));
        rs.emplace("no_cull", s);
    }

    void initCullClockwise(Device& device)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = true;
        desc.DepthClipEnable = true;

        RStatePtr s;
        HR(device.CreateRasterizerState(&desc, &s));
        rs.emplace("cull_clockwise", s);
    }

    void initWireframeCullClockwise(Device& device)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_WIREFRAME;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = true;
        desc.DepthClipEnable = true;

        RStatePtr s;
        HR(device.CreateRasterizerState(&desc, &s));
        rs.emplace("wireframe_cull_clockwise", s);
    }

    void initScissor(Device& device)
    {
        D3D11_RASTERIZER_DESC desc{};
        desc.FillMode = D3D11_FILL_SOLID;
        desc.CullMode = D3D11_CULL_BACK;
        desc.FrontCounterClockwise = false;
        desc.DepthClipEnable = true;
        desc.ScissorEnable = true;

        RStatePtr s;
        HR(device.CreateRasterizerState(&desc, &s));
        rs.emplace("scissor", s);
    }
} // namespace RS
