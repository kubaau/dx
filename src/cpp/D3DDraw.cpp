#include <tools/Contains.hpp>
#include "D3DEngine.hpp"
#include "GraphicsDebugUtils.hpp"

void D3DEngine::drawScene()
{
    clearViews(*rtv, background_color);
    clearViews(*offscreen_rtv, background_color);

    context->OMSetRenderTargets(1, blur->isEnabled() ? offscreen_rtv.GetAddressOf() : rtv.GetAddressOf(), dsv.Get());

    const auto vp = XMLoadFloat4x4(&view_projection);

    for (auto& o : renderables)
        o.second.drawSolid(vp);

    for (auto& o : renderables)
        o.second.drawPlanarShadow(vp, dir_lights.front().direction);

    if (depth_visualizer == DepthVisualizer::none)
        context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_STENCIL, 1.0f, 0);

    for (auto& o : renderables)
        o.second.drawReflecting(vp);

    for (auto& o : renderables)
        o.second.drawReflectedSolid(vp, dir_lights);

    for (auto& o : renderables)
        o.second.drawReflectedPlanarShadow(vp, dir_lights, dir_lights.front().direction);

    for (auto& o : renderables)
        o.second.drawReflectedTransparent(vp, dir_lights);

    for (auto& o : renderables)
        o.second.drawTransparent(vp);

    for (auto& o : particles)
        o.second.draw(vp);

    using namespace DirectX;
    if (depth_visualizer == DepthVisualizer::box)
        visualizeDepth(vp);
    if (depth_visualizer == DepthVisualizer::quad)
        visualizeDepth(XMMatrixIdentity());

    if (blur->isEnabled())
        visualizeBlur();

    constexpr auto sync_interval = 0;
    constexpr auto present_flags = 0;
    HR(swap_chain->Present(sync_interval, present_flags));
}

void D3DEngine::clearViews(RTV& v, const Color& color)
{
    context->ClearRenderTargetView(&v, reinterpret_cast<const float*>(&color));
    context->ClearDepthStencilView(dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void D3DEngine::visualizeDepth(const Matrix& vp)
{
    const Color color = Colors::red;
    context->ClearRenderTargetView(rtv.Get(), reinterpret_cast<const float*>(&color));
    context->ClearRenderTargetView(offscreen_rtv.Get(), reinterpret_cast<const float*>(&color));

    const auto test_object_name = depth_visualizer == DepthVisualizer::box ? "depth_box" : "depth_quad";
    try
    {
        auto& test_object = renderables.at(test_object_name);

        test_object.overrideDSS(dss_default);

        const auto max_depth_count = 5;
        for (int i = 0; i <= max_depth_count; ++i)
        {
            test_object.mat = &materials["depth" + to_string(i)];
            test_object.drawSolid(vp, i, true);
        }
    }
    catch (out_of_range&)
    {
    }
}

void D3DEngine::visualizeBlur()
{
    using namespace DirectX;

    context->OMSetRenderTargets(1, rtv.GetAddressOf(), dsv.Get());
    constexpr auto blur_count = 4;
    blur->inPlace(*context, *offscreen_srv, *offscreen_uav, blur_count);
    if (contains(renderables, "blur_quad"))
        renderables.at("blur_quad").drawSolid(XMMatrixIdentity(), 0, true);
}
