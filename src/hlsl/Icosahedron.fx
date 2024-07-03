#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"
#include "Utils.fx"

typedef VInBasic VIn;
typedef VOutIcosahedron VOut;
typedef GOutIcosahedron GOut;

cbuffer rarely
{
    DirectionalLight dir_lights[3];
    float3 eye_pos;
};

cbuffer per_frame
{
    float elapsed;
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
    vo.pos_l = vi.pos_l;
    vo.normal_l = vi.normal_l;
    vo.tex = vi.tex;
    return vo;
}

GOut vsNoGeo(VIn vi)
{
    GOut vo;
    const float4 pos_lh = homogenize(vi.pos_l);

    vo.pos_w = mul(pos_lh, world).xyz;
    vo.normal_w = mul(vi.normal_l, (float3x3)wit);
    vo.pos_h = mul(pos_lh, wvp);
    vo.tex = vi.tex;

    return vo;
}

void subdivide(VOut vi[3], out VOut vo[6])
{
    VOut vs[3];
    vs[0].pos_l = normalize(0.5f * (vi[0].pos_l + vi[1].pos_l));
    vs[1].pos_l = normalize(0.5f * (vi[1].pos_l + vi[2].pos_l));
    vs[2].pos_l = normalize(0.5f * (vi[2].pos_l + vi[0].pos_l));
    vs[0].normal_l = vs[0].pos_l;
    vs[1].normal_l = vs[1].pos_l;
    vs[2].normal_l = vs[2].pos_l;
    vs[0].tex = 0.5f * (vi[0].tex + vi[1].tex);
    vs[1].tex = 0.5f * (vi[1].tex + vi[2].tex);
    vs[2].tex = 0.5f * (vi[2].tex + vi[0].tex);

    vo[0] = vi[0];
    vo[1] = vs[0];
    vo[2] = vs[2];
    vo[3] = vs[1];
    vo[4] = vi[2];
    vo[5] = vi[1];
}

void outputSubdivision(VOut vs[6], inout TriangleStream<GOut> triangle_stream)
{
    GOut go[6];

    [unroll]
    for (int i = 0; i < 6; ++i)
    {
        const float4 pos_lh = homogenize(vs[i].pos_l);
        go[i].pos_h = mul(pos_lh, wvp);
        go[i].pos_w = mul(pos_lh, world).xyz;
        go[i].normal_w = mul(vs[i].normal_l, (float3x3)wit);
        go[i].tex = vs[i].tex;
    }

    [unroll]
    for (int j = 0; j < 5; ++j)
        triangle_stream.Append(go[j]);
    triangle_stream.RestartStrip();
    triangle_stream.Append(go[1]);
    triangle_stream.Append(go[5]);
    triangle_stream.Append(go[3]);
    triangle_stream.RestartStrip();
}

[maxvertexcount(8)]
void gsSubdivision1(triangle VOut vi[3], inout TriangleStream<GOut> triangle_stream)
{
    VOut vs[6];
    subdivide(vi, vs);
    outputSubdivision(vs, triangle_stream);
}

[maxvertexcount(32)]
void gsSubdivision2(triangle VOut vi[3], inout TriangleStream<GOut> triangle_stream)
{
    VOut vs1[6];
    subdivide(vi, vs1);

    VOut vs2_out[6];
    VOut vs2_in[3];

    vs2_in[0] = vs1[0];
    vs2_in[1] = vs1[1];
    vs2_in[2] = vs1[2];
    subdivide(vs2_in, vs2_out);
    outputSubdivision(vs2_out, triangle_stream);

    vs2_in[0] = vs1[1];
    vs2_in[1] = vs1[3];
    vs2_in[2] = vs1[2];
    subdivide(vs2_in, vs2_out);
    outputSubdivision(vs2_out, triangle_stream);

    vs2_in[0] = vs1[2];
    vs2_in[1] = vs1[3];
    vs2_in[2] = vs1[4];
    subdivide(vs2_in, vs2_out);
    outputSubdivision(vs2_out, triangle_stream);

    vs2_in[0] = vs1[1];
    vs2_in[1] = vs1[5];
    vs2_in[2] = vs1[3];
    subdivide(vs2_in, vs2_out);
    outputSubdivision(vs2_out, triangle_stream);
}

float3 faceNormal(VOut vs[3])
{
    return normalize((vs[0].normal_l + vs[1].normal_l + vs[2].normal_l) / 3.0f);
}

[maxvertexcount(3)]
void gsExplosion(triangle VOut vi[3], uint primitive_id : SV_PrimitiveID, inout TriangleStream<GOut> triangle_stream)
{
    const float3 face_normal = faceNormal(vi);
    const float speed = (primitive_id % 4 + 1) / 4.0f;
    const float3 explosion_vector = face_normal * speed;

    GOut go[3];
    [unroll] 
    for (int i = 0; i < 3; ++i)
    {
        if (trunc(elapsed) % 2 == 0)
            vi[i].pos_l += explosion_vector * frac(elapsed);
        else
            vi[i].pos_l += explosion_vector * (1.0f - frac(elapsed));

        const float4 pos_lh = homogenize(vi[i].pos_l);
        go[i].pos_h = mul(pos_lh, wvp);
        go[i].pos_w = mul(pos_lh, world).xyz;
        go[i].normal_w = mul(vi[i].normal_l, (float3x3)wit);
        go[i].tex = vi[i].tex;
        triangle_stream.Append(go[i]);
    }
}

float3 avgPosL(VOut vs[3])
{
    return (vs[0].pos_l + vs[1].pos_l + vs[2].pos_l) / 3.0f;
}

[maxvertexcount(2)]
void gsVertexNormals(point VOut vi[1], uint primitive_id : SV_PrimitiveID, inout LineStream<GOut> line_stream)
{
    GOut go;

    float4 pos_lh = homogenize(vi[0].pos_l);
    go.pos_h = mul(pos_lh, wvp);
    go.pos_w = mul(pos_lh, world).xyz;
    go.normal_w = mul(vi[0].normal_l, (float3x3)wit);
    go.tex = vi[0].tex;
    line_stream.Append(go);

    vi[0].pos_l += vi[0].normal_l;
    pos_lh = homogenize(vi[0].pos_l);
    go.pos_h = mul(pos_lh, wvp);
    go.pos_w = mul(pos_lh, world).xyz;
    go.normal_w = mul(vi[0].normal_l, (float3x3)wit);
    go.tex = vi[0].tex;
    line_stream.Append(go);
}

[maxvertexcount(2)]
void gsFaceNormals(triangle VOut vi[3], uint primitive_id : SV_PrimitiveID, inout LineStream<GOut> line_stream)
{
    GOut go;

    float3 avg_pos_l = avgPosL(vi);
    float4 pos_lh = float4(avg_pos_l, 1.0f);
    go.pos_h = mul(pos_lh, wvp);
    go.pos_w = mul(pos_lh, world).xyz;
    go.normal_w = mul(vi[0].normal_l, (float3x3)wit);
    go.tex = vi[0].tex;
    line_stream.Append(go);

    float3 face_normal = faceNormal(vi);
    avg_pos_l += face_normal;
    pos_lh = float4(avg_pos_l, 1.0f);
    go.pos_h = mul(pos_lh, wvp);
    go.pos_w = mul(pos_lh, world).xyz;
    go.normal_w = mul(vi[0].normal_l, (float3x3)wit);
    go.tex = vi[0].tex;
    line_stream.Append(go);
}

float4 ps(GOut pi, uniform int light_count) : SV_Target
{
    return psLightOnly(pi.normal_w, eye_pos, pi.pos_w, light_count, dir_lights, mat);
}

float4 psWhite(GOut pi) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 psBlue(GOut pi) : SV_Target
{
    return float4(0.5f, 0.5f, 1.0f, 1.0f);
}

technique11 icosahedron_subdivision0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vsNoGeo()));
        SetGeometryShader(NULL);
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}

technique11 icosahedron_subdivision1
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gsSubdivision1()));
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}

technique11 icosahedron_subdivision2
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gsSubdivision2()));
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}

technique11 icosahedron_explosion
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gsExplosion()));
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}

technique11 icosahedron_vertex_normals
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gsVertexNormals()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 icosahedron_face_normals
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetGeometryShader(CompileShader(gs_5_0, gsFaceNormals()));
        SetPixelShader(CompileShader(ps_5_0, psBlue()));
    }
}