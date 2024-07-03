#include "Particle.fx"

cbuffer constant
{
	float3 gravity_acceleration = { 0.0f, -9.81f, 0.0f };
};

static const float emit_delay = 0.001f;
static const float emit_xzrange = 3.0f;
static const float emit_ymin = 10.0f;
static const float emit_ymax = emit_ymin + 5.0f;
static const uint emit_count = 5;

// stream_out

// The stream-out GS is just responsible for emitting new particles and destroying old particles.
// The logic programed here will generally vary from particle system to particle system, 
// as the destroy/spawn rules will be different.
[maxvertexcount(emit_count + 1)]
void gsStreamOutFunc(point Particle geometry_in[1], inout PointStream<Particle> point_stream)
{
	Particle particle_in = geometry_in[0];
	particle_in.age += time_step;

	switch (particle_in.type)
	{
		case RAIN_EMITTER:
		{
			if (particle_in.age > emit_delay)
			{
				for (uint i = 0; i < emit_count; ++i)
				{
					float3 vec_random = (getRandom3DVector(float(i) / emit_count) + float3(1.0f, 1.0f, 1.0f)) * 0.5f;
					vec_random.x = lerp(-emit_xzrange, emit_xzrange, vec_random.x);
					vec_random.y = lerp(emit_ymin, emit_ymax, vec_random.y);
					vec_random.z = lerp(-emit_xzrange, emit_xzrange, vec_random.z);

					Particle p;
					p.initial_pos_w = emit_pos.xyz;
					p.initial_velocity_w = vec_random;
					p.size = float2(1.0f, 1.0f);
					p.age = 0.0f;
					p.type = RAIN_DROP;
					p.delay = 0.0f;
					point_stream.Append(p);
				}

				particle_in.age = 0.0f;
			}

			point_stream.Append(particle_in);
		} break;
		case RAIN_DROP:
		{
			float3 pos = calculate3DPosition(gravity_acceleration, particle_in.age, particle_in.initial_velocity_w, particle_in.initial_pos_w);
			if (pos.y > 0.0f)
				point_stream.Append(particle_in);
		} break;
	}
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

	vo.pos_w = calculate3DPosition(gravity_acceleration, vi.age, vi.initial_velocity_w, vi.initial_pos_w);
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
		float3 p1 = geometry_in[0].pos_w + 0.01f * gravity_acceleration;

		GOut v0;
		v0.pos_h = mul(float4(p0, 1.0f), vp);
		v0.tex = float2(0.0f, 0.0f);
		line_stream.Append(v0);

		GOut v1;
		v1.pos_h = mul(float4(p1, 1.0f), vp);
		v1.tex = float2(1.0f, 1.0f);
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