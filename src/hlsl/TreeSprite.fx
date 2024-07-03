#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"
#include "Utils.fx"

typedef VInTree VIn;
typedef VOutTree VOut;
typedef GOutTree GOut;

cbuffer rarely
{
    DirectionalLight dir_lights[3];
    float3 eye_pos;
    float fog_start;
    float fog_range;
    float4 fog_color;
};

cbuffer per_object
{
    float4x4 vp;
    Material mat;
};

Texture2DArray tex_array;

VOut vs(VIn vi)
{
    VOut vo;
    vo.center_w = vi.pos_w;
    vo.size_w = vi.size_w;
    return vo;
}

VOut vsID(VIn vi, uint vertex_id : SV_VertexID)
{
    VOut vo;
    vo.center_w = vi.pos_w;
    vo.size_w = float2(vertex_id + 2, vertex_id + 2);
    return vo;
}

static const uint v_count = 4;
[maxvertexcount(v_count)]
void gs(point VOut vi[1], uint primitive_id : SV_PrimitiveID, inout TriangleStream<GOut> triangle_stream)
{
    const float3 up = float3(0.0f, 1.0f, 0.0f);
    const float3 pos = vi[0].center_w;
    float3 look = pos - eye_pos;
    look.y = 0.0f;
    look = normalize(look);
    const float3 right = cross(up, look);

    const float3 halfway_right = 0.5f * vi[0].size_w.x * right;
    const float3 halfway_up = 0.5f * vi[0].size_w.y * up;

    float3 vs[v_count];
    vs[0] = pos - halfway_right + halfway_up; // tl
    vs[1] = pos + halfway_right + halfway_up; // tr
    vs[2] = pos - halfway_right - halfway_up; // bl
    vs[3] = pos + halfway_right - halfway_up; // br

    static const float2 tex_coordinates[v_count] =
    {
        float2(0.0f, 0.0f), // tl
        float2(1.0f, 0.0f), // tr
        float2(0.0f, 1.0f), // bl
        float2(1.0f, 1.0f)  // br
    };

    GOut go;
    [unroll]
    for (uint i = 0; i < v_count; ++i)
    {
        go.pos_h = mul(homogenize(vs[i]), vp);
        go.pos_w = vs[i].xyz;
        go.normal_w = look;
        go.tex = tex_coordinates[i];
        go.primitive_id = primitive_id;

        triangle_stream.Append(go);
    }
}

float4 ps(GOut pi, uniform int light_count, uniform bool use_texture, uniform bool alpha_clip, uniform bool fog_enabled) : SV_Target
{
    float4 pixel_color = 1;
    float4 tex_color = 1;

    if (use_texture)
    {        
        tex_color = addTextureColor(pixel_color, tex_array, anisotropic_sampler, float3(pi.tex, pi.primitive_id % 4));

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
    }

    if (fog_enabled)
        addFogColor(pixel_color, to_eye.d, fog_start, fog_range, fog_color);

    pixel_color.a = mat.diffuse.a * tex_color.a;

    return pixel_color;
}

technique11 light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gs()));
        SetPixelShader(CompileShader(ps_5_0, ps(3, false, false, false)));
    }
}

technique11 light3_tex_alphaClip
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gs()));
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, true, false)));
    }
}

technique11 light3_tex_alphaClip_fog
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gs()));
        SetPixelShader(CompileShader(ps_5_0, ps(3, true, true, true)));
    }
}
