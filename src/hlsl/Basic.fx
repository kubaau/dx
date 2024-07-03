#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"
#include "Utils.fx"

typedef VInBasic VIn;
typedef VOutBasic VOut;

cbuffer rarely
{
    DirectionalLight dir_lights[3];
    float3 eye_pos;
    float fog_start;
    float fog_range;
    float4 fog_color;
    float displacement_map_texel_size;
    float spatial_step;
};

cbuffer per_object
{
    float4x4 world;
    float4x4 wit;
    float4x4 wvp;
    float4x4 diffuse_transform;
    float4x4 shadow_transform;
    Material mat;
};

Texture2D diffuse_map;
Texture2D displacement_map;
Texture2D shadow_map;
TextureCube cube_map;

VOut vs(VIn vi)
{
    VOut vo;

    const float4 pos_lh = homogenize(vi.pos_l);
    vo.pos_w = mul(pos_lh, world).xyz;
    vo.normal_w = mul(vi.normal_l, (float3x3)wit);
    vo.pos_h = mul(pos_lh, wvp);
    vo.tex = mul(float4(vi.tex, 0.0f, 1.0f), diffuse_transform).xy;
    vo.shadow_pos_h = mul(pos_lh, shadow_transform);

    return vo;
}

VOut vsWavesDisplacement(VIn vi)
{
    vi.pos_l.y = displacement_map.SampleLevel(anisotropic_sampler, vi.tex, 0.0f).r;

    const float du = displacement_map_texel_size;
    const float dv = displacement_map_texel_size;
    const float l = displacement_map.SampleLevel(point_sampler, vi.tex - float2(du, 0.0f), 0.0f).r;
    const float r = displacement_map.SampleLevel(point_sampler, vi.tex + float2(du, 0.0f), 0.0f).r;
    const float t = displacement_map.SampleLevel(point_sampler, vi.tex - float2(0.0f, dv), 0.0f).r;
    const float b = displacement_map.SampleLevel(point_sampler, vi.tex + float2(0.0f, dv), 0.0f).r;
    vi.normal_l = normalize(float3(-r + l, 2.0f * spatial_step, b - t));

    return vs(vi);
}

float4 psTest(VOut pi) : SV_Target
{
    return saturate(mat.diffuse);
}

float4 psDepthVisualizer(VOut pi) : SV_Target
{
    return float4(0.1f, 0.1f, 0.1f, 0.0f);
}

float4 ps(VOut pi,
          uniform uint light_count,
          uniform bool use_texture,
          uniform bool alpha_clip,
          uniform bool fog_enabled,
          uniform uint reflection_type) : SV_Target
{
    float4 pixel_color = 1;
    float4 tex_color = 1;

    if (use_texture)
    {        
        tex_color = addTextureColor(pixel_color, diffuse_map, anisotropic_sampler, pi.tex);

        if (alpha_clip)
            clip(tex_color.a - 0.1f);
    }
    
    pi.normal_w = normalize(pi.normal_w);
    const ToEye to_eye = toEye(eye_pos, pi.pos_w);

    if (light_count > 0)
    {        
        float3 shadow = 1;
        //shadow[0] = shadowFactor(shadow_sampler, shadow_map, pi.shadow_pos_h);
        addDirLightsColor(pixel_color, light_count, dir_lights, mat, pi.normal_w, to_eye.v, shadow);

        addReflectionColor(pixel_color, reflection_type, mat, pi.normal_w, to_eye.v, cube_map, anisotropic_sampler);
    }

    if (fog_enabled)
        addFogColor(pixel_color, to_eye.d, fog_start, fog_range, fog_color);

    pixel_color.a = mat.diffuse.a * tex_color.a;

    return pixel_color;
}

technique11 light0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, psTest()));
    }
}

technique11 light1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(1, false, false, false, 0)));
    }
}

technique11 light2
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(2, false, false, false, 0)));
    }
}

technique11 light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, false, false, false, 0)));
    }
}

technique11 light0_tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(0, true, false, false, 0)));
    }
}

technique11 light1_tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(1, true, false, false, 0)));
    }
}

technique11 light2_tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(2, true, false, false, 0)));
    }
}

technique11 light3_tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, false, 0)));
    }
}

technique11 light0_tex_alphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(0, true, true, false, 0)));
    }
}

technique11 light1_tex_alphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(1, true, true, false, 0)));
    }
}

technique11 light2_tex_alphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(2, true, true, false, 0)));
    }
}

technique11 light3_tex_alphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, true, false, 0)));
    }
}

technique11 light1_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(1, false, false, true, 0)));
    }
}

technique11 light2_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(2, false, false, true, 0)));
    }
}

technique11 light3_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, false, false, true, 0)));
    }
}

technique11 light0_tex_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(0, true, false, true, 0)));
    }
}

technique11 light1_tex_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(1, true, false, true, 0)));
    }
}

technique11 light2_tex_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(2, true, false, true, 0)));
    }
}

technique11 light3_tex_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, true, 0)));
    }
}

technique11 light0_tex_alphaClip_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(0, true, true, true, 0)));
    }
}

technique11 light1_tex_alphaClip_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(1, true, true, true, 0)));
    }
}

technique11 light2_tex_alphaClip_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(2, true, true, true, 0)));
    }
}

technique11 light3_tex_alphaClip_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, true, true, 0)));
    }
}

technique11 blend_depth_visualizer
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, psDepthVisualizer()));
    }
}

technique11 displacement0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vsWavesDisplacement()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(0, false, false, false, 0)));
    }
};

technique11 displacement3_tex
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vsWavesDisplacement()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, false, 0)));
    }
};

technique11 displacement3_tex_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vsWavesDisplacement()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, true, 0)));
    }
};

technique11 light3_reflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, false, false, false, 1)));
    }
};

technique11 light3_tex_reflect
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, false, 1)));
    }
};

technique11 light3_refract
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, false, false, false, 2)));
    }
};

technique11 light3_tex_refract
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, false, 2)));
    }
};