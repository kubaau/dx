#pragma once

#include <d3d11.h>
#include "ComPtr.hpp"
#include "ContainerTypedefs.hpp"

using Device = ID3D11Device;
using DevicePtr = ComPtr<Device>;

using Context = ID3D11DeviceContext;
using ContextPtr = ComPtr<Context>;

using SwapChain = IDXGISwapChain;
using SwapChainPtr = ComPtr<SwapChain>;

using SRV = ID3D11ShaderResourceView;
using SRVPtr = ComPtr<SRV>;
using Textures = Map<SRVPtr>;

using UAV = ID3D11UnorderedAccessView;
using UAVPtr = ComPtr<UAV>;

using RState = ID3D11RasterizerState;
using RStatePtr = ComPtr<RState>;
using RStates = Map<RStatePtr>;

using DSState = ID3D11DepthStencilState;
using DSStatePtr = ComPtr<DSState>;
using DSStates = Map<DSStatePtr>;

using BState = ID3D11BlendState;
using BStatePtr = ComPtr<BState>;
using BStates = Map<BStatePtr>;

using InputLayout = ID3D11InputLayout;
using InputLayoutPtr = ComPtr<InputLayout>;
using InputLayouts = Map<InputLayoutPtr>;

using RTV = ID3D11RenderTargetView;
using RTVPtr = ComPtr<RTV>;

using Texture1D = ID3D11Texture1D;
using Texture1DPtr = ComPtr<Texture1D>;
using Texture2D = ID3D11Texture2D;
using Texture2DPtr = ComPtr<Texture2D>;

using DXResource = ID3D11Resource;
using DXResourcePtr = ComPtr<DXResource>;

using D3DBuffer = ComPtr<ID3D11Buffer>;

using DSV = ID3D11DepthStencilView;
using DSVPtr = ComPtr<DSV>;
