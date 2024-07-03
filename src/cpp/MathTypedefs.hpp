#pragma once

#include <DirectXMath.h>

constexpr auto pi = DirectX::XM_PI;

using float2 = DirectX::XMFLOAT2;
using float3 = DirectX::XMFLOAT3;
using float4 = DirectX::XMFLOAT4;

using float4x4 = DirectX::XMFLOAT4X4;

using Matrix = DirectX::XMMATRIX;
using Vector = DirectX::XMVECTOR;

using Rotation = float3;
using Scale = Rotation;
using Translation = Rotation;
