#include "Lights.fx"
#include "Structs.fx"

typedef VInInstancedBasic VIn;
typedef VOutInstancedBasic VOut;

cbuffer rarely
{
	DirectionalLight dir_lights[3];
	float3 eye_pos;
}

cbuffer per_object
{
	float4x4 vp;
	Material mat;
};

VOut vs(VIn vi)
{
	VOut vo;

	// Transform to world space space.
	vo.pos_w = mul(float4(vi.pos_l, 1.0f), vi.world).xyz;
	vo.normal_w = mul(vi.normal_l, (float3x3)vi.world);

	// Transform to homogeneous clip space.
	vo.pos_h = mul(float4(vo.pos_w, 1.0f), vp);

	// Output vertex attributes for interpolation across triangle.
	vo.tex = vi.tex; //mul(float4(vi.Tex, 0.0f, 1.0f), gTexTransform).xy;
	vo.color = vi.color;

	return vo;
}

float4 ps(VOut pi, uniform int light_count) : SV_Target
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
			light_color = directionalLight(mat, dir_lights[i], pi.normal_w, to_eye);

			ambient += light_color.ambient * pi.color;
			diffuse += light_color.diffuse * pi.color;
			specular += light_color.specular;
		}

		// Modulate with late add.
		pixel_color = texture_color * (ambient + diffuse) + specular;
	}

	// Common to take alpha from diffuse mat and texture.
	pixel_color.a = mat.diffuse.a * texture_color.a;

	return pixel_color;
}

technique11 light3
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(3)));
	}
}