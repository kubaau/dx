#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"

typedef VInTerrain VIn;
typedef VOutTerrain VOut;

static const int PLANES_IN_FRUSTUM = 6;

cbuffer rarely
{
	DirectionalLight dir_lights[3];
	float3 eye_pos;
	float fog_start;
	float fog_range;
	float4 fog_color;

	// When distance is minimum, the tessellation is maximum.
	// When distance is maximum, the tessellation is minimum.
	float max_tess_distance;
	float min_tess_distance;
	
	// Exponents for power of 2 tessellation. The tessellation range is [2^(gMinTess), 2^(gMaxTess)].
	// Since the maximum tessellation is 64, this means gMaxTess can be at most 6 since 2^6 = 64.
	float min_tess;
	float max_tess;

	float texel_cell_space_u;
	float texel_cell_space_v;
	float world_cell_space;
	float2 tex_scale = 50.0f;

	float4 world_frustum_planes[PLANES_IN_FRUSTUM];
};

cbuffer per_object
{
	float4x4 vp;
	Material mat;
};

Texture2DArray diffuse_maps;
Texture2D blend_map;
Texture2D height_map;
Texture2D density_map;

VOut vs(VIn vi)
{
	VOut vo;

	// Terrain specified directly in world space.
	vo.pos_w = vi.pos_l;

	// Displace the patch corners to world space. This is to make the eye to patch distance calculation more accurate.
	vo.pos_w.y = height_map.SampleLevel(point_linear_sampler, vi.tex, 0).r;

	vo.tex = vi.tex;
	vo.y_bounds = vi.y_bounds;

	vo.density = density_map.SampleLevel(anisotropic_sampler, vi.tex, 0).r;

	return vo;
}

float calculateTessFactor(float3 pos)
{
	float distance_to_eye = distance(pos, eye_pos);
	// max norm in xz plane (useful to see detail levels from a bird's eye).
	//float distance_to_eye = max(abs(pos.x - eye_pos.x), abs(pos.z - eye_pos.z));

	float interp_factor = saturate((distance_to_eye - min_tess_distance) / (max_tess_distance - min_tess_distance));

	return pow(2, (lerp(max_tess, min_tess, interp_factor)));
	//return pow(2, round(lerp(max_tess, min_tess, interp_factor)));
}

// Returns true if the box is completely behind (in negative half space) of plane.
bool isAABBBehindPlane(float3 center, float3 extents, float4 plane)
{
	float3 n = abs(plane.xyz);

	// This is always positive.
	float r = dot(extents, n);

	// signed distance from center point to plane.
	float s = dot(float4(center, 1.0f), plane);

	// If the center point of the box is a distance of e or more behind the plane (in which case s is negative since it is behind the plane),
	// then the box is completely in the negative half space of the plane.
	return (s + r) < 0.0f;
}

// Returns true if the box is completely outside the frustum.
bool isAABBOutsideFrustum(float3 center, float3 extents, float4 frustum_planes[PLANES_IN_FRUSTUM])
{
	for (int i = 0; i < PLANES_IN_FRUSTUM; ++i)
		if (isAABBBehindPlane(center, extents, frustum_planes[i]))
			return true;
	return false;
}

PatchTessQuad chs(InputPatch<VOut, 4> patch, uint patch_id : SV_PrimitiveID)
{
	PatchTessQuad pt;
	
	// We store the patch y_bounds in the first control point.
	float y_min = patch[0].y_bounds.x;
	float y_max = patch[0].y_bounds.y;

	// Build axis-aligned bounding box. patch[2] is lower-left corner and patch[1] is upper-right corner.
	float3 v_min = float3(patch[2].pos_w.x, y_min, patch[2].pos_w.z);
	float3 v_max = float3(patch[1].pos_w.x, y_max, patch[1].pos_w.z);

	float3 box_center = 0.5f * (v_min + v_max);
	float3 box_extents = 0.5f * (v_max - v_min);
	if (isAABBOutsideFrustum(box_center, box_extents, world_frustum_planes))
	{
		pt.edge_tess[0] = 0.0f;
		pt.edge_tess[1] = 0.0f;
		pt.edge_tess[2] = 0.0f;
		pt.edge_tess[3] = 0.0f;

		pt.inside_tess[0] = 0.0f;
		pt.inside_tess[1] = 0.0f;
	}
	else
	{
		// It is important to dmo the tess factor calculation based on the edge properties so that edges shared by more than one patch will
		// have the same tessellation factor. Otherwise, gaps can appear.

		// Compute midpoint on edges, and patch center
		float3 edge0 = 0.5f * (patch[0].pos_w + patch[2].pos_w);
		float3 edge1 = 0.5f * (patch[0].pos_w + patch[1].pos_w);
		float3 edge2 = 0.5f * (patch[1].pos_w + patch[3].pos_w);
		float3 edge3 = 0.5f * (patch[2].pos_w + patch[3].pos_w);
		float3 center = 0.25f * (patch[0].pos_w + patch[1].pos_w + patch[2].pos_w + patch[3].pos_w);

		pt.edge_tess[0] = calculateTessFactor(edge0);
		pt.edge_tess[1] = calculateTessFactor(edge1);
		pt.edge_tess[2] = calculateTessFactor(edge2);
		pt.edge_tess[3] = calculateTessFactor(edge3);

		pt.inside_tess[0] = calculateTessFactor(center);
		pt.inside_tess[1] = pt.inside_tess[0];

		// adjust for density
		/*
		pt.edge_tess[0] *= 0.5f * (patch[0].density + patch[2].density);
		pt.edge_tess[1] *= 0.5f * (patch[0].density + patch[1].density);
		pt.edge_tess[2] *= 0.5f * (patch[1].density + patch[3].density);
		pt.edge_tess[3] *= 0.5f * (patch[2].density + patch[3].density);

		pt.inside_tess[0] *= 0.25f * (patch[0].density + patch[1].density + patch[2].density + patch[3].density);
		pt.inside_tess[1] = pt.inside_tess[0];
		*/
	}

	return pt;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("chs")]
[maxtessfactor(64.0f)]
HOutTerrain hs(InputPatch<VOut, 4> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
	HOutTerrain ho;

	ho.pos_w = patch[i].pos_w;
	ho.tex = patch[i].tex;

	return ho;
}

// The domain shader is called for every vertex created by the tessellator. It is like the vertex shader after tessellation.
[domain("quad")]
DOutTerrain domainShader(PatchTessQuad pt, float2 uv : SV_DomainLocation, const OutputPatch<HOutTerrain, 4> quad)
{
	DOutTerrain dmo;

	// Bilinear interpolation.
	dmo.pos_w = lerp(
		lerp(quad[0].pos_w, quad[1].pos_w, uv.x), 
		lerp(quad[2].pos_w, quad[3].pos_w, uv.x), 
		uv.y);

	dmo.tex = lerp(
		lerp(quad[0].tex, quad[1].tex, uv.x),
		lerp(quad[2].tex, quad[3].tex, uv.x),
		uv.y);

	// Tile layer textures over terrain.
	dmo.tiled_tex = dmo.tex * tex_scale;

	// Displacement mapping
	dmo.pos_w.y = height_map.SampleLevel(point_linear_sampler, dmo.tex, 0).r;

	// NOTE: We tried computing the normal in the shader using finite difference,
	// but the vertices move continuously with fractional_even which creates
	// noticable light shimmering artifacts as the normal changes. 
	// Therefore, we moved the calculation to the pixel shader.  

	// Project to homogeneous clip space.
	dmo.pos_h = mul(float4(dmo.pos_w, 1.0f), vp);

	return dmo;
}

float4 blueMyself(float4 color)
{
	float add_r = -0.1f;
	float add_g = 0.0f;
	float add_b = 0.1f;
	float add_a = 0.0f;

	float mul_r = 0.5f;
	float mul_g = 0.6f;
	float mul_b = 1.3f;
	float mul_a = 1.0f;

	return saturate(float4(
		(color.r + add_r) * mul_r,
		(color.g + add_g) * mul_g,
		(color.b + add_b) * mul_b,
		(color.a + add_a) * mul_a));
}

float4 ps(DOutTerrain pi, uniform int light_count, uniform bool fog_enabled) : SV_Target
{
	// Estimate normal and tangent using central differences.
	float2 tex_left = pi.tex + float2(-texel_cell_space_u, 0.0f);
	float2 tex_right = pi.tex + float2(texel_cell_space_u, 0.0f);
	float2 tex_bottom = pi.tex + float2(0.0f, texel_cell_space_v);
	float2 tex_top = pi.tex + float2(0.0f, -texel_cell_space_v);

	float y_left = height_map.SampleLevel(point_linear_sampler, tex_left, 0).r;
	float y_right = height_map.SampleLevel(point_linear_sampler, tex_right, 0).r;
	float y_bottom = height_map.SampleLevel(point_linear_sampler, tex_bottom, 0).r;
	float y_top = height_map.SampleLevel(point_linear_sampler, tex_top, 0).r;

	float3 tangent = normalize(float3(2.0f * world_cell_space, y_right - y_left, 0.0f));
	float3 bitangent = normalize(float3(0.0f, y_bottom - y_top, -2.0f * world_cell_space));
	float3 normal_w = cross(tangent, bitangent);
	
	// The toEye vector is used in lighting.
	float3 to_eye = eye_pos - pi.pos_w;
	// Cache the distance to the eye from this surface point.
	float distance_to_eye = length(to_eye);
	// Normalize.
	to_eye /= distance_to_eye;

	// Texturing
	// Sample layers in texture array.
	float4 layer0 = diffuse_maps.Sample(anisotropic_sampler, float3(pi.tiled_tex, 0.0f));
	float4 layer1 = diffuse_maps.Sample(anisotropic_sampler, float3(pi.tiled_tex, 1.0f));
	float4 layer2 = diffuse_maps.Sample(anisotropic_sampler, float3(pi.tiled_tex, 2.0f));
	float4 layer3 = diffuse_maps.Sample(anisotropic_sampler, float3(pi.tiled_tex, 3.0f));
	float4 layer4 = diffuse_maps.Sample(anisotropic_sampler, float3(pi.tiled_tex, 4.0f));

	// Sample the blend map.
	float4 blend_factor = blend_map.Sample(anisotropic_sampler, pi.tex);

	// Blend the layers on top of each other.
	float4 texture_color = layer0;
	texture_color = lerp(texture_color, layer1, blend_factor.r);
	texture_color = lerp(texture_color, layer2, blend_factor.g);
	texture_color = lerp(texture_color, layer3, blend_factor.b);
	texture_color = lerp(texture_color, layer4, blend_factor.a);
	
	// Lighting.
	float4 pixel_color = texture_color;
	if (light_count > 0)
	{
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Sum the light contribution from each light source.  		
		[unroll]
		for (int i = 0; i < light_count; ++i)
		{
			LightColor light_color;
			light_color = directionalLight(mat, dir_lights[i], normal_w, to_eye);

			ambient += light_color.ambient;
			diffuse += light_color.diffuse;
			specular += light_color.specular;
		}

		// Modulate with late add.
		pixel_color = texture_color * (ambient + diffuse) + specular;
	}

	// Fogging	
	if (fog_enabled)
	{
		float fog_lerp = saturate((distance_to_eye - fog_start) / fog_range);

		// Blend the fog color and the lit color.
		pixel_color = lerp(pixel_color, fog_color, fog_lerp);
	}
	
	// Common to take alpha from diffuse mat and texture.
	pixel_color.a = mat.diffuse.a * texture_color.a;

	return pixel_color;
	//return blueMyself(pixel_color);
}

technique11 light3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetHullShader(CompileShader(hs_5_0, hs()));
		SetDomainShader(CompileShader(ds_5_0, domainShader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(3, false)));
	}
};