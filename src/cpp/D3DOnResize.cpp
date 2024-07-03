#include "AppEngine.hpp"
#include "D3DEngine.hpp"
#include "GraphicsDebugUtils.hpp"
#include "GraphicsOptions.hpp"

void D3DEngine::onResize()
{
    releaseOldCOMs();
    updateSwapChain();
    updateRenderTargetView();
    updateDepthStencilBuffer();
    updateDepthStencilView();
    updateViewport();
    updateProjectionMatrix();
    updateOffscreenViews();

    camera->setLens(pi * 0.25, APP.aspectRatio(), 0.1f, 1000.0f);
    blur->init(*device);
}

void D3DEngine::releaseOldCOMs()
{
    rtv.Reset();
    dsv.Reset();
    depth_stencil_buffer.Reset();
    offscreen_srv.Reset();
    offscreen_uav.Reset();
    offscreen_rtv.Reset();
}

void D3DEngine::updateSwapChain()
{
    HR(swap_chain->ResizeBuffers(1, APP.width(), APP.height(), DXGI_FORMAT_R8G8B8A8_UNORM, 0));
}

void D3DEngine::updateRenderTargetView()
{
    Texture2DPtr back_buffer;
    HR(swap_chain->GetBuffer(0, __uuidof(Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf())));
    HR(device->CreateRenderTargetView(back_buffer.Get(), 0, &rtv));
}

void D3DEngine::updateDepthStencilBuffer()
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = APP.width();
    desc.Height = APP.height();
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    desc.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
    desc.SampleDesc.Quality = enable_4x_msaa ? _4x_msaa_quality - 1 : 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    HR(device->CreateTexture2D(&desc, 0, &depth_stencil_buffer));
}

void D3DEngine::updateDepthStencilView()
{
    HR(device->CreateDepthStencilView(depth_stencil_buffer.Get(), 0, &dsv));
}

void D3DEngine::updateViewport()
{
    static D3D11_VIEWPORT viewport = {0, 0, 0, 0, 0, 1};
    viewport.Width = APP.widthF();
    viewport.Height = APP.heightF();
    context->RSSetViewports(1, &viewport);
}

void D3DEngine::updateProjectionMatrix()
{
    using namespace DirectX;
    XMStoreFloat4x4(&projection, XMMatrixPerspectiveFovLH(0.25f * pi, APP.aspectRatio(), 1.0f, 1000.0f));
    updateViewProjectionMatrix();
}

void D3DEngine::updateOffscreenViews()
{
    D3D11_TEXTURE2D_DESC desc;
    desc.Width = APP.width();
    desc.Height = APP.height();
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    Texture2DPtr tex;
    HR(device->CreateTexture2D(&desc, 0, &tex));

    // Null description means to create a view to all mipmap levels using the format the texture was created with.
    HR(device->CreateShaderResourceView(tex.Get(), 0, &offscreen_srv));
    HR(device->CreateUnorderedAccessView(tex.Get(), 0, &offscreen_uav));
    HR(device->CreateRenderTargetView(tex.Get(), 0, &offscreen_rtv));
}
