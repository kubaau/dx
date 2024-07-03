#include "Lights.fx"
#include "Structs.fx"
#include "Utils.fx"

typedef VInCylinder VIn;
typedef VOutCylinder VOut;
typedef GOutCylinder GOut;

cbuffer rarely
{
    DirectionalLight dir_lights[3];
    float3 eye_pos;
};

cbuffer per_object
{
    float4x4 world;
    float4x4 wit;
    float4x4 wvp;
    Material mat;
};

VOut vs(VIn vi)
{
    VOut vo;

    const float4 pos_lh = homogenize(vi.pos_l);
    vo.pos_w = mul(pos_lh, world).xyz;
    vo.normal_w = mul(vi.normal_l, (float3x3)wit);

    return vo;
}

[maxvertexcount(4)]
void gs(line VOut vo[2], uint primitive_id : SV_PrimitiveID, inout TriangleStream<GOut> triangle_stream, uniform int size)
{
    float4 vertices[4];
    vertices[0] = homogenize(vo[0].pos_w);
    vertices[1] = homogenize(vo[0].pos_w) + float4(0.0f, size, 0.0f, 0.0f);
    vertices[2] = homogenize(vo[1].pos_w);
    vertices[3] = homogenize(vo[1].pos_w) + float4(0.0f, size, 0.0f, 0.0f);

    float3 normals[4];
    normals[0] = vo[0].normal_w;
    normals[1] = vo[0].normal_w;
    normals[2] = vo[1].normal_w;
    normals[3] = vo[1].normal_w;

    GOut go;
    [unroll]
    for (int i = 0; i < 4; ++i)
    {
        go.pos_w = vertices[i].xyz;
        go.pos_wh = mul(vertices[i], wvp);
        go.normal_w = normals[i];
        go.primitive_id = primitive_id;
        triangle_stream.Append(go);
    }
}

float4 ps(GOut pi, uniform int light_count) : SV_Target
{
    return psLightOnly(pi.normal_w, eye_pos, pi.pos_w, light_count, dir_lights, mat);
}

technique11 light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gs(2.0f)));
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}
