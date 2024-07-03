#pragma once

#include <ext/d3dx11effect.h>

#include "ComPtr.hpp"
#include "ContainerTypedefs.hpp"

using Technique = ComPtr<ID3DX11EffectTechnique>;
using Techniques = Map<Technique>;

using MatrixVariable = ComPtr<ID3DX11EffectMatrixVariable>;
using MatrixVariables = Map<MatrixVariable>;

using VectorVariable = ComPtr<ID3DX11EffectVectorVariable>;
using VectorVariables = Map<VectorVariable>;

using ScalarVariable = ComPtr<ID3DX11EffectScalarVariable>;
using ScalarVariables = Map<ScalarVariable>;

using EffectVariable = ComPtr<ID3DX11EffectVariable>;
using EffectVariables = Map<EffectVariable>;

using EffectSRV = ComPtr<ID3DX11EffectShaderResourceVariable>;
using EffectSRVs = Map<EffectSRV>;

using EffectUAV = ComPtr<ID3DX11EffectUnorderedAccessViewVariable>;
using EffectUAVs = Map<EffectUAV>;

class Effect;
using Effects = Map<Effect>;

using EffectPtr = ComPtr<ID3DX11Effect>;
