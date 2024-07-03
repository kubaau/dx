#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"

typedef VInNormalMap VIn;
typedef VOutDisplacementMap VOut;
typedef VOutNormalMap DOut;

cbuffer rarely
{
	DirectionalLight dir_lights[3];
	float3 eye_pos;
	float fog_start;
	float fog_range;
	float4 fog_color;
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
	float4x4 shadow_transform;
	Material mat;
};

Texture2D diffuse_map;
Texture2D normal_map;
Texture2D shadow_map;
TextureCube cube_map;

VOut vs(VIn vi)
{
	VOut vo;

	float4 pos_lh = float4(vi.pos_l, 1.0f);

	// Transform to world space.
	vo.pos_w = mul(pos_lh, world).xyz;
	vo.normal_w = mul(vi.normal_l, (float3x3)wit);
	vo.tangent_w = mul(vi.tangent_t, (float3x3)world);

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

PatchTessTriangle chs(InputPatch<VOut, 3> patch, uint patch_id : SV_PrimitiveID)
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
HOutDisplacementMap hs(InputPatch<VOut, 3> patch, uint i : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID)
{
	HOutDisplacementMap ho;

	// Pass through shader.
	ho.pos_w = patch[i].pos_w;
	ho.normal_w = patch[i].normal_w;
	ho.tangent_w = patch[i].tangent_w;
	ho.tex = patch[i].tex;

	return ho;
}

// The domain shader is called for every vertex created by the tessellator. It is like the vertex shader after tessellation.
[domain("tri")]
DOut domainShader(PatchTessTriangle pt, float3 uvw : SV_DomainLocation, const OutputPatch<HOutDisplacementMap, 3> tri)
{
	DOut dmo;

	// Interpolate patch attributes to generated vertices.
	dmo.pos_w = uvw.x * tri[0].pos_w + uvw.y * tri[1].pos_w + uvw.z * tri[2].pos_w;
	dmo.normal_w = uvw.x * tri[0].normal_w + uvw.y * tri[1].normal_w + uvw.z * tri[2].normal_w;
	dmo.tangent_w = uvw.x * tri[0].tangent_w + uvw.y * tri[1].tangent_w + uvw.z * tri[2].tangent_w;
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

	// Generate projective tex-coords to project shadow map onto scene.
	dmo.shadow_pos_h = mul(float4(dmo.pos_w, 1.0f), shadow_transform);

	// Project to homogeneous clip space.
	dmo.pos_h = mul(float4(dmo.pos_w, 1.0f), vp);

	return dmo;
}

float4 ps(VOutNormalMap pi, uniform int light_count, uniform bool use_texture, uniform bool alpha_clip, uniform bool fog_enabled, uniform int reflection_type) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pi.normal_w = normalize(pi.normal_w);

	// The toEye vector is used in lighting.
	float3 to_eye = eye_pos - pi.pos_w;
	// Cache the distance to the eye from this surface point.
	float distance_to_eye = length(to_eye);
	// Normalize.
	to_eye /= distance_to_eye;

	// Default to multiplicative identity.
	float4 texture_color = float4(1, 1, 1, 1);
	if (use_texture)
	{
		// Sample texture.
		texture_color = diffuse_map.Sample(anisotropic_sampler, pi.tex);

		if (alpha_clip)
		{
			// Discard pixel if texture alpha < 0.1.  Note that we dmo this
			// test as soon as possible so that we can potentially exit the shader 
			// early, thereby skipping the rest of the shader code.
			clip(texture_color.a - 0.1f);
		}
	}

	// Normal mapping
	float3 normal_map_sample = normal_map.Sample(anisotropic_sampler, pi.tex).rgb;
	float3 bumped_normal_w = normalSampleToWorldSpace(normal_map_sample, pi.normal_w, pi.tangent_w);

	// Lighting.
	float4 pixel_color = texture_color;
	if (light_count > 0)
	{
		// Start with a sum of zero. 
		float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

		// Only the first light casts a shadow.
		float3 shadow = float3(1.0f, 1.0f, 1.0f);
		shadow[0] = shadowFactor(shadow_sampler, shadow_map, pi.shadow_pos_h);

		// Sum the light contribution from each light source.  		
		[unroll]
		for (int i = 0; i < light_count; ++i)
		{
			LightColor light_color;
			light_color = directionalLight(mat, dir_lights[i], bumped_normal_w, to_eye);

			ambient += light_color.ambient;
			diffuse += shadow[i] * light_color.diffuse;
			specular += shadow[i] * light_color.specular;
		}

		// Modulate with late add.
		pixel_color = texture_color * (ambient + diffuse) + specular;

		if (reflection_type > 0)
		{
			float3 incident = -to_eye;
			float3 reflection_vector = float3(0.0f, 0.0f, 0.0f);

			if (reflection_type == 1)
				reflection_vector = reflect(incident, bumped_normal_w);
			else
				reflection_vector = refract(incident, bumped_normal_w, 0.98f);

			float4 reflection_color = cube_map.Sample(anisotropic_sampler, reflection_vector);
			pixel_color += mat.reflect * reflection_color;
		}
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
}

technique11 light3_tex_displacement
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetHullShader(CompileShader(hs_5_0, hs()));
		SetDomainShader(CompileShader(ds_5_0, domainShader()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, false, 0)));
	}
};