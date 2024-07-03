#include "Particle.fx"

cbuffer constant
{
	float3 acceleration = { -1.0f, -9.8f, 0.0f };
};

// stream_out

// The stream-out GS is just responsible for emitting new particles and destroying old particles.
// The logic programed here will generally vary from particle system to particle system, 
// as the destroy/spawn rules will be different.
[maxvertexcount(6)]
void gsStreamOutFunc(point Particle geometry_in[1], inout PointStream<Particle> point_stream)
{	
	geometry_in[0].age += time_step;

	if (geometry_in[0].type == RAIN_EMITTER)
	{	
		// time to emit a new particle?
		if (geometry_in[0].age > 0.002f)
		{
			for (int i = 0; i < 5; ++i)
			{
				// Spread rain drops out above the camera.
				float3 vec_random = 35.0f * getRandom3DVector((float)i / 5.0f);
				vec_random.y = 20.0f;
			
				Particle p;
				p.initial_pos_w = emit_pos.xyz + vec_random;
				p.initial_velocity_w = float3(0.0f, 0.0f, 0.0f);
				p.size = float2(1.0f, 1.0f);
				p.age = 0.0f;
				p.type = RAIN_DROP;
				p.delay = 0.0f;
			
				point_stream.Append(p);
			}
			
			// reset the time to emit
			geometry_in[0].age = 0.0f;
		}

		// always keep emitters
		point_stream.Append(geometry_in[0]);
	}
	else
		// Specify conditions to keep particle; this may vary from system to system.
		if (geometry_in[0].age <= 3.0f)
			point_stream.Append(geometry_in[0]);
}

GeometryShader gsStreamOut = ConstructGSWithSO(
	CompileShader(gs_5_0, gsStreamOutFunc()),
	"POSITION.xyz; VELOCITY.xyz; SIZE.xy; AGE.x; TYPE.x; DELAY.x");
	
technique11 stream_out
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsStreamOut()));
		SetGeometryShader(gsStreamOut);
		SetPixelShader(NULL);
		// we must also disable the depth buffer for stream-out only
		SetDepthStencilState(disable_depth, 0);
	}
}

// draw

typedef Particle VIn;
typedef VOutRain VOut;
typedef GOutRain GOut;

VOut vsDraw(VIn vi)
{
	VOut vo;
	
	float t = vi.age;
	
	// constant acceleration equation
	vo.pos_w = 0.5f * t * t * acceleration 
								+ t * vi.initial_velocity_w 
								+ vi.initial_pos_w;
	
	vo.type = vi.type;
	
	return vo;
}

// The draw GS just expands points into lines.
[maxvertexcount(2)]
void gsDraw(point VOut geometry_in[1], inout LineStream<GOut> line_stream)
{	
	// dmo not draw emitter particles.
	if (geometry_in[0].type != RAIN_EMITTER)
	{
		// Slant line in acceleration direction.
		float3 p0 = geometry_in[0].pos_w;
		float3 p1 = geometry_in[0].pos_w + 0.07f * acceleration;
		
		GOut v0;
		v0.pos_h = mul(float4(p0, 1.0f), vp);
		v0.tex = float2(0.0f, 0.0f);
		line_stream.Append(v0);
		
		GOut v1;
		v1.pos_h = mul(float4(p1, 1.0f), vp);
		v1.tex  = float2(1.0f, 1.0f);
		line_stream.Append(v1);
	}
}

float4 psDraw(GOut pi) : SV_TARGET
{
	return tex_array.Sample(anisotropic_sampler, float3(pi.tex, 0));
}

technique11 draw
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsDraw()));
		SetGeometryShader(CompileShader(gs_5_0, gsDraw()));
		SetPixelShader(CompileShader(ps_5_0, psDraw()));
		SetDepthStencilState(no_depth_writes, 0);
	}
}