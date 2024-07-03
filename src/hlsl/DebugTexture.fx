#include "States.fx"
#include "Structs.fx"

typedef VInBasic VIn;
typedef VOutDebug VOut;

cbuffer per_object
{
	float4x4 wvp;
};

Texture2D tex;

VOut vs(VIn vi)
{
	VOut vo;

	vo.pos_h = mul(float4(vi.pos_l, 1.0f), wvp);
	vo.tex = vi.tex;

	return vo;
}

float4 ps(VOut pi) : SV_Target
{
	return tex.Sample(linear_sampler, pi.tex);
}

float4 ps(VOut pi, uniform int index) : SV_Target // ?
{
	float4 c = tex.Sample(linear_sampler, pi.tex).r;

	// draw as grayscale
	return float4(c.rrr, 1);
}

technique11 rgba
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps()));
	}
}

technique11 red
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(0)));
	}
}

technique11 green
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(1)));
	}
}

technique11 blue
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(2)));
	}
}

technique11 alpha
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vs()));
		SetGeometryShader(NULL);
		SetPixelShader(CompileShader(ps_5_0, ps(3)));
	}
}