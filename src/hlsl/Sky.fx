#include "States.fx"
#include "Structs.fx"
#include "Utils.fx"

typedef VInSky VIn;
typedef VOutSky VOut;

cbuffer per_object
{
    float4x4 wvp;
};

TextureCube cube_map;

VOut vs(VIn vi)
{
    VOut vo;
        
    vo.pos_h = mul(homogenize(vi.pos_l), wvp).xyww; // set z = w so that z/w = 1 (skydome always on far plane)
    vo.pos_l = vi.pos_l;

    return vo;
}

float4 ps(VOut pi) : SV_Target
{
    return cube_map.Sample(anisotropic_sampler, pi.pos_l);
}

technique11 sky
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps()));

        SetRasterizerState(no_cull);
        SetDepthStencilState(less_equal, 0);
    }
}
