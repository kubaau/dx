#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"

typedef VInBasic VIn;
typedef VOutDebug VOut;
typedef VOutDisplacementMapShadow VOutShadowTess;
typedef HOutDisplacementMapShadow HOut;
typedef GOutIcosahedron DOut;

cbuffer rarely
{
	float3 eye_pos;
	float height_scale;
	float max_tess_distance;
	float min_tess_distance;
	float min_tess;
	float max_tess;
};

cbuffer per_object
{
	float4x4 world;
	float4x4 wit;
	float4x4 vp;
	float4x4 wvp;
	float4x4 diffuse_transform;
};

Texture2D diffuse_map;
Texture2D normal_map;

VOut vs(VIn vi)
{
	VOut vo;

	vo.pos_h = mul(float4(vi.pos_l, 1.0f), wvp);
	vo.tex = mul(float4(vi.tex, 0.0f, 1.0f), diffuse_transform).xy;

	return vo;
}

VOutShadowTess vsTess(VIn vi)
{
	VOutShadowTess vo;

	float4 pos_lh = float4(vi.pos_l, 1.0f);

	// Transform to world space.
	vo.pos_w = mul(pos_lh, world).xyz;
	vo.normal_w = mul(vi.normal_l, (float3x3)wit);

	// Output vertex attributes for interpolation across triangle.
	vo.tex = mul(float4(vi.tex, 0.0f, 1.0f), diffuse_transform).xy;

	float eye_distance = distance(vo.pos_w, eye_pos);

	// Normalized tessellation factor. 
	// The tessellation is 
	//   0 if d >= gMinTessDistance and
	//   1 if d <= gMaxTessDistance.  
	float tess = saturate((min_tess_distance - eye_distance) / (min_tess_distance - max_tess_distance));

	// Rescale [0,1] --> [gMinTessFactor, gMaxTessFactor].
	vo.tess = min_tess + tess * (max_tess - min_tess);

	return vo;
}

PatchTessTriangle chs(InputPatch<VOutShadowTess, 3> patch, uint patch_id : SV_PrimitiveID)
{
	PatchTessTriangle pt;

	// Average tess factors along edges, and pick an edge tess factor for the interior tessellation.  
	// It is important to dmo the tess factor calculation based on the edge properties so that edges shared by
	// more than one triangle will have the same tessellation factor. Otherwise, gaps can appear.
	pt.edge_tess[0] = 0.5f * (patch[1].tess + patch[2].tess);
	pt.edge_tess[1] = 0.5f * (patch[2].tess + patch[0].tess);
	pt.edge_tess[2] = 0.5f * (patch[0].tess + patch[1].tess);
	pt.inside_tess = pt.edge_tess[0];

	return pt;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("chs")]
[maxtessfactor(64.0f)]
HOut hs(InputPatch<VOutShadowTess, 3> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
	HOut ho;

	// Pass through shader.
	ho.pos_w = patch[i].pos_w;
	ho.normal_w = patch[i].normal_w;
	ho.tex = patch[i].tex;

	return ho;
}

// The domain shader is called for every vertex created by the tessellator. It is like the vertex shader after tessellation.
[domain("tri")]
DOut domainShader(PatchTessTriangle pt, float3 uvw : SV_DomainLocation, const OutputPatch<HOut, 3> tri)
{
	DOut dmo;

	// Interpolate patch attributes to generated vertices.
	dmo.pos_w = uvw.x * tri[0].pos_w + uvw.y * tri[1].pos_w + uvw.z * tri[2].pos_w;
	dmo.normal_w = uvw.x * tri[0].normal_w + uvw.y * tri[1].normal_w + uvw.z * tri[2].normal_w;
	dmo.tex = uvw.x * tri[0].tex + uvw.y * tri[1].tex + uvw.z * tri[2].tex;

	// Interpolating normal can unnormalize it, so normalize it.
	dmo.normal_w = normalize(dmo.normal_w);

	// Displacement mapping.

	// Choose the mipmap level based on distance to the eye; specifically, choose the next miplevel every MipInterval units, and clamp the miplevel in [0,6].
	const float mip_interval = 20.0f;
	float mip_level = clamp((distance(dmo.pos_w, eye_pos) - mip_interval) / mip_interval, 0.0f, 6.0f);

	// Sample height map (stored in alpha channel).
	float height = normal_map.SampleLevel(anisotropic_sampler, dmo.tex, mip_level).a;

	// Offset vertex along normal.
	dmo.pos_w += (height_scale * (height - 1.0)) * dmo.normal_w;

	// Project to homogeneous clip space.
	dmo.pos_h = mul(float4(dmo.pos_w, 1.0f), vp);

	return dmo;
}

// This is only used for alpha cut out geometry, so that shadows show up correctly. 
// Geometry that does not need to sample a texture can use a NULL pixel shader for depth pass.
void ps(VOut pi)
{
	float4 diffuse = diffuse_map.Sample(linear_sampler, pi.tex);

	// Don't write transparent pixels to the shadow map.
	clip(diffuse.a - 0.15f);
}

// This is only used for alpha cut out geometry, so that shadows show up correctly.
// Geometry that does not need to sample a texture can use a NULL pixel shader for depth pass.
void psTess(DOut pi)
{
	float4 diffuse = diffuse_map.Sample(linear_sampler, pi.tex);

	// Don't write transparent pixels to the shadow map.
	clip(diffuse.a - 0.15f);
}

technique11 build_shadow_map
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(depth);
	}
}

technique11 build_shadow_map_alpha_clip
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps()));
	}
}

technique11 build_shadow_map_tess
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsTess()));
		SetHullShader(CompileShader(hs_5_0, hs()));
		SetDomainShader(CompileShader(ds_5_0, domainShader()));
		SetGeometryShader(NULL);
		SetPixelShader(NULL);

		SetRasterizerState(depth);
	}
}

technique11 build_shadow_map_tess_alpha_clip
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsTess()));
		SetHullShader(CompileShader(hs_5_0, hs()));
		SetDomainShader(CompileShader(ds_5_0, domainShader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, psTess()));
	}
}