#include "Particle.fx"

cbuffer constant
{
	float3 shell_acceleration = { 0.0f, 0.0f, 0.0f };
	float3 particle_acceleration = { 0.0f, 0.8f, 0.0f };
};

static const uint shell_count = 10;
static const uint particle_count = 50;
static const float min_shell_delay = 1.0f;
static const float max_shell_delay = 5.0f;
static const float particle_ttl = 10.0f;

// stream_out

// The stream-out GS is just responsible for emitting new particles and destroying old particles.
// The logic programed here will generally vary from particle system to particle system, 
// as the destroy/spawn rules will be different.
[maxvertexcount(max(shell_count, particle_count))]
void gsStreamOutFunc(point Particle geometry_in[1], inout PointStream<Particle> point_stream)
{
	Particle particle_in = geometry_in[0];
	particle_in.age += time_step;

	switch (particle_in.type)
	{
		case SMOKE_EMITTER:
		{
			for (uint i = 0; i < shell_count; ++i)
			{
				float3 vec_random = getRandom3DVector(float(i) / shell_count);

				Particle p;
				p.initial_pos_w = emit_pos.xyz;
				p.initial_velocity_w = 1.0f * vec_random;
				p.size = float2(2.0f, 2.0f);
				p.age = 0.0f;
				p.type = SMOKE_SHELL;
				p.delay = lerp(min_shell_delay, max_shell_delay, (vec_random.x + 1.0f) * 0.5f);
				point_stream.Append(p);
			}
		} break;

		case SMOKE_SHELL:
		{
			if (particle_in.age > particle_in.delay)
				for (uint i = 0; i < particle_count; ++i)
				{
					float3 vec_random = getRandom3DVector(float(i) / particle_count);

					Particle p;
					p.initial_pos_w = calculate3DPosition(shell_acceleration, particle_in.age, particle_in.initial_velocity_w, particle_in.initial_pos_w);
					p.initial_velocity_w = 3.0f * vec_random;
					p.size = float2(0.5f, 0.5f);
					p.age = 0.0f;
					p.type = SMOKE_PARTICLE;
					p.delay = 0.0f;
					point_stream.Append(p);
				}
			else
				point_stream.Append(particle_in);
		} break;

		case SMOKE_PARTICLE:
		{
			if (particle_in.age < particle_ttl)
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
typedef VOutFire VOut;
typedef GOutFire GOut;

VOut vsDraw(VIn vi)
{
	VOut vo;

	float3 acceleration = { 0.0f, 0.0f, 0.0f };
	float time = vi.age;
	float opacity = 1.0f;

	switch (vi.type)
	{
		case SMOKE_SHELL: 
			acceleration = shell_acceleration;
			break;
		case SMOKE_PARTICLE: 
			acceleration = particle_acceleration; 
			opacity -= smoothstep(0.0f, opacity, time / particle_ttl);
			break;
	}

	vo.pos_w = calculate3DPosition(acceleration, time, vi.initial_velocity_w, vi.initial_pos_w);

	// fade color with time
	vo.color = float4(1.0f, 1.0f, 1.0f, opacity);

	vo.size = vi.size;
	vo.type = vi.type;

	return vo;
}

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void gsDraw(point VOut geometry_in[1], inout TriangleStream<GOut> triangle_stream)
{
	if (geometry_in[0].type != SMOKE_EMITTER)
	{
		// Compute world matrix so that billboard faces the camera.
		float3 look = normalize(eye_pos.xyz - geometry_in[0].pos_w);
		float3 right = normalize(cross(float3(0, 1, 0), look));
		float3 up = cross(look, right);

		// Compute triangle strip vertices (quad) in world space.
		float half_width = 0.5f * geometry_in[0].size.x;
		float half_height = 0.5f * geometry_in[0].size.y;

		float4 v[4];
		v[0] = float4(geometry_in[0].pos_w + half_width * right - half_height * up, 1.0f);
		v[1] = float4(geometry_in[0].pos_w + half_width * right + half_height * up, 1.0f);
		v[2] = float4(geometry_in[0].pos_w - half_width * right - half_height * up, 1.0f);
		v[3] = float4(geometry_in[0].pos_w - half_width * right + half_height * up, 1.0f);

		// Transform quad vertices to world space and output them as a triangle strip.
		GOut go;
		[unroll]
		for (int i = 0; i < 4; ++i)
		{
			go.pos_h = mul(v[i], vp);
			go.tex = quad_tex_coordinates[i];
			go.color = geometry_in[0].color;
			triangle_stream.Append(go);
		}
	}
}

float4 psDraw(GOut pi) : SV_TARGET
{
	return tex_array.Sample(anisotropic_sampler, float3(pi.tex, 0)) * pi.color;
}

technique11 draw
{
	pass P0
	{
		SetVertexShader(CompileShader(vs_5_0, vsDraw()));
		SetGeometryShader(CompileShader(gs_5_0, gsDraw()));
		SetPixelShader(CompileShader(ps_5_0, psDraw()));

		SetBlendState(additive_blending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		//SetBlendState(subtractive_blending, float4(0.0f, 0.0f, 0.0f, 0.0f), 0xffffffff);
		SetDepthStencilState(no_depth_writes, 0);
	}
}