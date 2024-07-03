#include "Particle.fx"

cbuffer constant
{
	float3 acceleration = { 0.0f, 7.8f, 0.0f };
};

// stream_out

// The stream-out GS is just responsible for emitting new particles and destroying old particles.
// The logic programed here will generally vary from particle system to particle system, 
// as the destroy/spawn rules will be different.
[maxvertexcount(2)]
void gsStreamOutFunc(point Particle geometry_in[1], inout PointStream<Particle> point_stream)
{	
	geometry_in[0].age += time_step;
	
	if (geometry_in[0].type == FIRE_EMITTER)
	{	
		// time to emit a new particle?
		if (geometry_in[0].age > 0.005f)
		{
			float3 vec_random = getRandomUnit3DVector(0.0f);
			vec_random.x *= 0.5f;
			vec_random.z *= 0.5f;
			
			Particle p;
			p.initial_pos_w = emit_pos.xyz;
			p.initial_velocity_w = 4.0f * vec_random;
			p.size = float2(3.0f, 3.0f);
			p.age = 0.0f;
			p.type = FIRE_FLARE;
			p.delay = 0.0f;
			
			point_stream.Append(p);
			
			// reset the time to emit
			geometry_in[0].age = 0.0f;
		}
		
		// always keep emitters
		point_stream.Append(geometry_in[0]);
	}
	else
		// Specify conditions to keep particle; this may vary from system to system.
		if (geometry_in[0].age <= 1.0f)
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
typedef VOutFire VOut;
typedef GOutFire GOut;

VOut vsDraw(VIn vi)
{
	VOut vo;
	
	float t = vi.age;
	
	// constant acceleration equation
	vo.pos_w = 0.5f * t * t * acceleration 
								+ t * vi.initial_velocity_w 
								+ vi.initial_pos_w;
	
	// fade color with time
	float opacity = 1.0f - smoothstep(0.0f, 1.0f, t / 1.0f);
	vo.color = float4(1.0f, 1.0f, 1.0f, opacity);
	
	vo.size = vi.size;
	vo.type = vi.type;
	
	return vo;
}

// The draw GS just expands points into camera facing quads.
[maxvertexcount(4)]
void gsDraw(point VOut geometry_in[1], inout TriangleStream<GOut> triangle_stream)
{	
	// dmo not draw emitter particles.
	if (geometry_in[0].type != FIRE_EMITTER)
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
		SetDepthStencilState(no_depth_writes, 0);
    }
}