#include "Lights.fx"
#include "States.fx"
#include "Structs.fx"

// commented out code was for chapter 18, exercises 4 and 5

typedef VInNormalMap VIn;
typedef VOutNormalMap VOut;
//typedef VOutNormalMapWorldToTangent VOut;

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
	float4x4 world;
	float4x4 wit;
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
	//vo.normal_l = vi.normal_l;
	//vo.tangent_t = vi.tangent_t;

	// Transform to homogeneous clip space.
	vo.pos_h = mul(pos_lh, wvp);

	// Output vertex attributes for interpolation across triangle.
	vo.tex = mul(float4(vi.tex, 0.0f, 1.0f), diffuse_transform).xy;

	// Generate projective tex-coords to project shadow map onto scene.
	vo.shadow_pos_h = mul(pos_lh, shadow_transform);

	return vo;
}

float4 ps(VOut pi, uniform int light_count, uniform bool use_texture, uniform bool alpha_clip, uniform bool fog_enabled, uniform int reflection_type) : SV_Target
{
	// Interpolating normal can unnormalize it, so normalize it.
	pi.normal_w = normalize(pi.normal_w);
	//pi.normal_l = normalize(pi.normal_l);

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
	//float3 tangent_w_transformed = normalize(mul(float4(pi.tangent_t, 1.0f), diffuse_transform).xyz);
	//float3 bumped_normal_w = normalSampleToWorldSpace(normal_map_sample, pi.normal_w, tangent_w_transformed);
	//float3 bumped_normal_tangent = normalSampleToTangentSpace(normal_map_sample);

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

			//DirectionalLight light_tangent = dir_lights[i];
			//light_tangent.direction = tbnTransform(light_tangent.direction, pi.normal_l, pi.tangent_t);
			//float3 to_eye_tangent = tbnTransform(to_eye, pi.normal_l, pi.tangent_t);
			//light_color = directionalLight(mat, light_tangent, bumped_normal_tangent, to_eye_tangent);
				
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

technique11 light3_tex_normal
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(3, true, false, false, 0)));
	}
};