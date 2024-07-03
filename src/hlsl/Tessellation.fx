#include "Lights.fx"
#include "Structs.fx"
#include "Utils.fx"

typedef VInTess VIn;
typedef VOutTess VOut;

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

// in the tessellation pipeline, vs is mostly used for animation to animate a coarse mesh before tessellating it
VOut vs(VIn vi)
{
    VOut vo;
    vo.pos_l = vi.pos_l;
    return vo;
}

PatchTessQuad chsQuad(InputPatch<VOut, 4> patch, uint patch_id : SV_PrimitiveID)
{
    PatchTessQuad pt;

    const float3 center_local = (patch[0].pos_l + patch[1].pos_l + patch[2].pos_l + patch[3].pos_l) / 4;
    const float3 center_w = mul(float4(center_local, 1.0f), world).xyz;

    const float distance_to_eye = distance(center_w, eye_pos);

    // the interval [d0, d1] defines the range we tessellate in
    const float d0 = 20.0f;
    const float d1 = 150.0f;
    const float tess = max(1.0f, 64.0f * saturate((d1 - distance_to_eye) / (d1 - d0)));
    
    pt.edge_tess[0] = tess;
    pt.edge_tess[1] = tess;
    pt.edge_tess[2] = tess;
    pt.edge_tess[3] = tess;

    pt.inside_tess[0] = tess;
    pt.inside_tess[1] = tess;

    return pt;
}

PatchTessTriangle chsTriangle(InputPatch<VOut, 3> patch, uint patch_id : SV_PrimitiveID)
{
    PatchTessTriangle pt;

    const float3 center_local = (patch[0].pos_l + patch[1].pos_l + patch[2].pos_l) / 3;
    const float3 center_w = mul(float4(center_local, 1.0f), world).xyz;

    const float distance_to_eye = distance(center_w, eye_pos);

    const float d0 = 20.0f;
    const float d1 = 150.0f;
    const float tess = max(1.0f, 64.0f * saturate((d1 - distance_to_eye) / (d1 - d0)));

    pt.edge_tess[0] = tess;
    pt.edge_tess[1] = tess;
    pt.edge_tess[2] = tess;

    pt.inside_tess = tess;

    return pt;
}	

PatchTessQuad chsCubicBezierQuad(InputPatch<VOut, 16> patch, uint patch_id : SV_PrimitiveID)
{
    PatchTessQuad pt;

    pt.edge_tess[0] = 25;
    pt.edge_tess[1] = 25;
    pt.edge_tess[2] = 25;
    pt.edge_tess[3] = 25;

    pt.inside_tess[0] = 25;
    pt.inside_tess[1] = 25;

    return pt;
}

PatchTessTriangle chsCubicBezierTriangle(InputPatch<VOut, 10> patch, uint patch_id : SV_PrimitiveID)
{
    PatchTessTriangle pt;

    pt.edge_tess[0] = 25;
    pt.edge_tess[1] = 25;
    pt.edge_tess[2] = 25;

    pt.inside_tess = 25;

    return pt;
}

PatchTessQuad chsQuadraticBezierQuad(InputPatch<VOut, 9> patch, uint patch_id : SV_PrimitiveID)
{
    PatchTessQuad pt;

    pt.edge_tess[0] = 25;
    pt.edge_tess[1] = 25;
    pt.edge_tess[2] = 25;
    pt.edge_tess[3] = 25;

    pt.inside_tess[0] = 25;
    pt.inside_tess[1] = 25;

    return pt;
}

PatchTessTriangle chsQuadraticBezierTriangle(InputPatch<VOut, 6> patch, uint patch_id : SV_PrimitiveID)
{
    PatchTessTriangle pt;

    pt.edge_tess[0] = 25;
    pt.edge_tess[1] = 25;
    pt.edge_tess[2] = 25;

    pt.inside_tess = 25;

    return pt;
}

HOutTess hsPassthrough(VOut vo)
{
    HOutTess ho;
    ho.pos_l = vo.pos_l;
    return ho;
}

// this Hull Shader part is commonly used for a coordinate basis change, for example changing from a quad to a Bezier bi-cubic
[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("chsQuad")]
[maxtessfactor(64.0f)]
HOutTess hsQuad(InputPatch<VOut, 4> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
    return hsPassthrough(patch[i]);
}

[domain("tri")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("chsTriangle")]
[maxtessfactor(64.0f)]
HOutTess hsTriangle(InputPatch<VOut, 3> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
    return hsPassthrough(patch[i]);
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(16)]
[patchconstantfunc("chsCubicBezierQuad")]
[maxtessfactor(64.0f)]
HOutTess hsCubicBezierQuad(InputPatch<VOut, 16> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
    return hsPassthrough(patch[i]);
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(10)]
[patchconstantfunc("chsCubicBezierTriangle")]
[maxtessfactor(64.0f)]
HOutTess hsCubicBezierTriangle(InputPatch<VOut, 10> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
    return hsPassthrough(patch[i]);
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(9)]
[patchconstantfunc("chsQuadraticBezierQuad")]
[maxtessfactor(64.0f)]
HOutTess hsQuadraticBezierQuad(InputPatch<VOut, 9> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
    return hsPassthrough(patch[i]);
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(6)]
[patchconstantfunc("chsQuadraticBezierTriangle")]
[maxtessfactor(64.0f)]
HOutTess hsQuadraticBezierTriangle(InputPatch<VOut, 6> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
    return hsPassthrough(patch[i]);
}

float inv(float t)
{
    return 1 - t;
}

float4 bernsteinBasisCubicBezier(float t)
{
    const float inv_t = inv(t);

    return float4(inv_t * inv_t * inv_t,
                  3.0f * t * inv_t * inv_t,
                  3.0f * t * t * inv_t,
                  t * t * t);
}

float3 bernsteinBasisQuadraticBezier(float t)
{
    const float inv_t = inv(t);

    return float3(inv_t * inv_t,
                  2.0f * t * inv_t,
                  t * t);
}

float4 dBernsteinBasisCubicBezier(float t)
{
    const float inv_t = inv(t);

    return float4(-3 * inv_t * inv_t,
                  3 * inv_t * inv_t - 6 * t * inv_t,
                  6 * t * inv_t - 3 * t * t,
                  3 * t * t);
}

float3 dBernsteinBasisQuadraticBezier(float t)
{
    const float inv_t = inv(t);

    return float3(-2 * inv_t,
                  2 * inv_t - 2 * t,
                  2 * t);
}

float3 cubicBezierSumQuad(const OutputPatch<HOutTess, 16> bezier_patch, float4 u_basis, float4 v_basis)
{
    float3 sum = 0;
    sum =  v_basis.x * (u_basis.x * bezier_patch[0].pos_l  + u_basis.y * bezier_patch[1].pos_l  + u_basis.z * bezier_patch[2].pos_l  + u_basis.w * bezier_patch[3].pos_l);
    sum += v_basis.y * (u_basis.x * bezier_patch[4].pos_l  + u_basis.y * bezier_patch[5].pos_l  + u_basis.z * bezier_patch[6].pos_l  + u_basis.w * bezier_patch[7].pos_l);
    sum += v_basis.z * (u_basis.x * bezier_patch[8].pos_l  + u_basis.y * bezier_patch[9].pos_l  + u_basis.z * bezier_patch[10].pos_l + u_basis.w * bezier_patch[11].pos_l);
    sum += v_basis.w * (u_basis.x * bezier_patch[12].pos_l + u_basis.y * bezier_patch[13].pos_l + u_basis.z * bezier_patch[14].pos_l + u_basis.w * bezier_patch[15].pos_l);
    return sum;
}

float3 cubicBezierSumTriangle(const OutputPatch<HOutTess, 10> bezier_patch, float3 uvw)
{
    const float s = uvw.x;
    const float t = uvw.y;
    const float u = uvw.z;
    const float s2 = s * s;
    const float s3 = s2 * s;
    const float t2 = t * t;
    const float t3 = t2 * t;
    const float u2 = u * u;
    const float u3 = u2 * u;

    float3 sum = 0;
    sum += bezier_patch[0].pos_l * s3;
    sum += bezier_patch[1].pos_l * 3 * s2 * t;
    sum += bezier_patch[2].pos_l * 3 * s * t2;
    sum += bezier_patch[3].pos_l * t3;
    sum += bezier_patch[4].pos_l * 3 * t2 * u;
    sum += bezier_patch[5].pos_l * 3 * t * u2;
    sum += bezier_patch[6].pos_l * u3;
    sum += bezier_patch[7].pos_l * 3 * s * u2;
    sum += bezier_patch[8].pos_l * 3 * s2 * u;
    sum += bezier_patch[9].pos_l * 6 * s * t * u;
    return sum;
}

float3 quadraticBezierSumQuad(const OutputPatch<HOutTess, 9> bezier_patch, float3 u_basis, float3 v_basis)
{
    float3 sum = 0;
    sum =  v_basis.x * (u_basis.x * bezier_patch[0].pos_l + u_basis.y * bezier_patch[1].pos_l + u_basis.z * bezier_patch[2].pos_l);
    sum += v_basis.y * (u_basis.x * bezier_patch[3].pos_l + u_basis.y * bezier_patch[4].pos_l + u_basis.z * bezier_patch[5].pos_l);
    sum += v_basis.z * (u_basis.x * bezier_patch[6].pos_l + u_basis.y * bezier_patch[7].pos_l + u_basis.z * bezier_patch[8].pos_l);
    return sum;
}

float3 quadraticBezierSumTriangle(const OutputPatch<HOutTess, 6> bezier_patch, float u, float v, float w)
{
    float3 sum = 0;
    sum += bezier_patch[0].pos_l * u * u;
    sum += bezier_patch[1].pos_l * 2 * u * v;
    sum += bezier_patch[2].pos_l * v * v;
    sum += bezier_patch[3].pos_l * 2 * v * w;
    sum += bezier_patch[4].pos_l * w * w;
    sum += bezier_patch[5].pos_l * 2 * u * w;
    return sum;
}

// the domain shader is called for every vertex created by the tessellator - it is like the vertex shader after tessellation
[domain("quad")]
DOutTess domainShaderQuad(PatchTessQuad pt, float2 uv : SV_DomainLocation, const OutputPatch<HOutTess, 4> quad)
{
    DOutTess dmo;

    // bilinear interpolation
    const float3 position_u1 = lerp(quad[0].pos_l, quad[1].pos_l, uv.x);
    const float3 position_u2 = lerp(quad[2].pos_l, quad[3].pos_l, uv.x);
    float3 pos = lerp(position_u1, position_u2, uv.y);

    // displacement
    pos.y = 0.3f * (pos.z * sin(pos.x) + pos.x * cos(pos.z));

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("tri")]
DOutTess domainShaderTriangle(PatchTessTriangle pt, float3 /* barycentric */ uvw : SV_DomainLocation, const OutputPatch<HOutTess, 3> tri)
{
    DOutTess dmo;

    const float3 pos = uvw.x * tri[0].pos_l + uvw.y * tri[1].pos_l + uvw.z * tri[2].pos_l;

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("tri")]
DOutTess domainShaderIcosahedron(PatchTessTriangle pt, float3 /* barycentric */ uvw : SV_DomainLocation, const OutputPatch<HOutTess, 3> tri)
{
    DOutTess dmo;

    float3 pos = uvw.x * tri[0].pos_l + uvw.y * tri[1].pos_l + uvw.z * tri[2].pos_l;

    pos = normalize(pos);

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("quad")]
DOutTess domainShaderCubicBezierQuad(PatchTessQuad pt, float2 uv : SV_DomainLocation, const OutputPatch<HOutTess, 16> bezier_patch)
{
    DOutTess dmo;

    const float4 u_basis = bernsteinBasisCubicBezier(uv.x);
    const float4 v_basis = bernsteinBasisCubicBezier(uv.y);

    const float3 pos = cubicBezierSumQuad(bezier_patch, u_basis, v_basis);

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("tri")]
DOutTess domainShaderCubicBezierTriangle(PatchTessTriangle pt, float3 /* barycentric */ uvw : SV_DomainLocation, const OutputPatch<HOutTess, 10> bezier_patch)
{
    DOutTess dmo;

    const float3 pos = cubicBezierSumTriangle(bezier_patch, uvw);	

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("quad")]
DOutTess domainShaderQuadraticBezierQuad(PatchTessQuad pt, float2 uv : SV_DomainLocation, const OutputPatch<HOutTess, 9> bezier_patch)
{
    DOutTess dmo;

    const float3 u_basis = bernsteinBasisQuadraticBezier(uv.x);
    const float3 v_basis = bernsteinBasisQuadraticBezier(uv.y);

    const float3 pos = quadraticBezierSumQuad(bezier_patch, u_basis, v_basis);

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("quad")]
DOutTessLight domainShaderQuadraticBezierQuadLight(PatchTessQuad pt, float2 uv : SV_DomainLocation, const OutputPatch<HOutTess, 9> bezier_patch)
{
    DOutTessLight dmo;

    const float3 u_basis = bernsteinBasisQuadraticBezier(uv.x);
    const float3 v_basis = bernsteinBasisQuadraticBezier(uv.y);

    const float3 pos = quadraticBezierSumQuad(bezier_patch, u_basis, v_basis);
    float4 pos_lh = homogenize(pos);

    dmo.pos_h = mul(pos_lh, wvp);
    dmo.pos_w = mul(pos_lh, world).xyz;

    // Modify the "Bezier Patch" demo to light and shade the Bezier surface. You will need to compute vertex normals in
    // the domain shader. A normal at a vertex pos can be found by taking the cross product of the partial derivatives
    // at the pos.

    const float3 du_basis = dBernsteinBasisQuadraticBezier(uv.x);
    const float3 dv_basis = dBernsteinBasisQuadraticBezier(uv.y);

    const float3 du = normalize(quadraticBezierSumQuad(bezier_patch, du_basis, v_basis));
    const float3 dv = normalize(quadraticBezierSumQuad(bezier_patch, u_basis, dv_basis));

    const float3 normal_l = normalize(cross(du, dv));
    dmo.normal_w = mul(normal_l, (float3x3)wit);

    return dmo;
}

[domain("tri")]
DOutTess domainShaderQuadraticBezierTriangle(PatchTessTriangle pt, float3 /* barycentric */ uvw : SV_DomainLocation, const OutputPatch<HOutTess, 6> bezier_patch)
{
    DOutTess dmo;
    
    const float3 pos = quadraticBezierSumTriangle(bezier_patch, uvw.x, uvw.y, uvw.z);

    dmo.pos_h = mul(homogenize(pos), wvp);

    return dmo;
}

[domain("tri")]
DOutTessLight domainShaderQuadraticBezierTriangleLight(PatchTessTriangle pt, float3 uvw : SV_DomainLocation, const OutputPatch<HOutTess, 6> bezier_patch)
{
    DOutTessLight dmo;
    
    const float3 pos = quadraticBezierSumTriangle(bezier_patch, uvw.x, uvw.y, uvw.z);
    const float4 pos_lh = homogenize(pos);

    dmo.pos_h = mul(pos_lh, wvp);
    dmo.pos_w = mul(pos_lh, world).xyz;

    const float3 du = normalize(bezier_patch[0].pos_l * uvw.x + bezier_patch[1].pos_l * uvw.y + bezier_patch[5].pos_l * uvw.z);
    const float3 dv = normalize(bezier_patch[1].pos_l * uvw.x + bezier_patch[2].pos_l * uvw.y + bezier_patch[3].pos_l * uvw.z);

    const float3 normal_l = normalize(cross(du, dv));
    dmo.normal_w = mul(normal_l, (float3x3)wit);

    return dmo;
}

float4 psWhite(DOutTess pi) : SV_Target
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}

float4 ps(DOutTessLight pi, uniform int light_count) : SV_Target
{
    return psLightOnly(pi.normal_w, eye_pos, pi.pos_w, light_count, dir_lights, mat);
}

technique11 tessellation_quad
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsQuad()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderQuad()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_triangle
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsTriangle()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderTriangle()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_icosahedron
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsTriangle()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderIcosahedron()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_cubic_bezier_quad
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsCubicBezierQuad()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderCubicBezierQuad()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_cubic_bezier_triangle
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsCubicBezierTriangle()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderCubicBezierTriangle()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_quadratic_bezier_quad
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsQuadraticBezierQuad()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderQuadraticBezierQuad()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_quadratic_bezier_quad_light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsQuadraticBezierQuad()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderQuadraticBezierQuadLight()));
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}

technique11 tessellation_quadratic_bezier_triangle
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsQuadraticBezierTriangle()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderQuadraticBezierTriangle()));
        SetPixelShader(CompileShader(ps_5_0, psWhite()));
    }
}

technique11 tessellation_quadratic_bezier_triangle_light3
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, vs()));
        SetHullShader(CompileShader(hs_5_0, hsQuadraticBezierTriangle()));
        SetDomainShader(CompileShader(ds_5_0, domainShaderQuadraticBezierTriangleLight()));
        SetPixelShader(CompileShader(ps_5_0, ps(3)));
    }
}