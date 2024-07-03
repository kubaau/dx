#include "D3DInit.hpp"
#include <cassert>
#include "AppEngine.hpp"
#include "GraphicsDebugUtils.hpp"
#include "GraphicsOptions.hpp"

void initContext(DevicePtr& device, ContextPtr& context)
{
    auto flags = 0;
#if defined(DEBUG) || defined(_DEBUG)
    flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
    constexpr auto driver_type = D3D_DRIVER_TYPE_HARDWARE;

    D3D_FEATURE_LEVEL feature_level;
    const auto device_result = D3D11CreateDevice(0, // default adapter
                                                 driver_type,
                                                 0, // no software device
                                                 flags,
                                                 0,
                                                 0, // default feature level array
                                                 D3D11_SDK_VERSION,
                                                 &device,
                                                 &feature_level,
                                                 &context);

    assert(not FAILED(device_result) && "D3D11CreateDevice failed");
    assert(feature_level == D3D_FEATURE_LEVEL_11_0 && "Direct3D Feature Level 11 unsupported");

    // Check 4X MSAA quality support for our back buffer format.
    // All Direct3D 11 capable devices support 4X MSAA for all render target formats, so we only need to check quality
    // support.
    HR(device->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &_4x_msaa_quality));
}

void initSwapChain(SwapChainPtr& swap_chain, Device& device)
{
    DXGI_SWAP_CHAIN_DESC desc;
    desc.BufferDesc.Width = APP.width();
    desc.BufferDesc.Height = APP.height();
    desc.BufferDesc.RefreshRate.Numerator = 144;
    desc.BufferDesc.RefreshRate.Denominator = 1;
    desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
    desc.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
    desc.SampleDesc.Quality = enable_4x_msaa ? _4x_msaa_quality - 1 : 0;
    desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    desc.BufferCount = 1;
    desc.OutputWindow = APP.windowHandle();
    desc.Windowed = true;
    desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    desc.Flags = 0;

    // To correctly create the swap chain, we must use the IDXGIFactory that was used to create the device.
    // If we tried to use a different IDXGIFactory instance (by calling CreateDXGIFactory), we get an error:
    // "IDXGIFactory::CreateSwapChain: This function is being called with a device from a different IDXGIFactory."
    ComPtr<IDXGIDevice> dxgi_device;
    HR(device.QueryInterface(__uuidof(IDXGIDevice), (void**)&dxgi_device));

    ComPtr<IDXGIAdapter> dxgi_adapter;
    HR(dxgi_device->GetParent(__uuidof(IDXGIAdapter), (void**)&dxgi_adapter));

    ComPtr<IDXGIFactory> dxgi_factory;
    HR(dxgi_adapter->GetParent(__uuidof(IDXGIFactory), (void**)&dxgi_factory));

    HR(dxgi_factory->CreateSwapChain(&device, &desc, &swap_chain));
}
