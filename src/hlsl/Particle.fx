#include "States.fx"
#include "Structs.fx"

// Texture coordinates used to stretch texture over quad when we expand point particle into a quad.
static const float2 quad_tex_coordinates[4] =
{
	float2(0.0f, 1.0f),
	float2(1.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f)
};

cbuffer per_frame
{
	float3 eye_pos;
	float game_time;
	float time_step;
};

cbuffer per_object
{
	float4x4 vp;
	float3 emit_pos;
	float3 emit_dir;
};

Texture2DArray tex_array;
Texture1D random_tex;

static const uint FIRE_EMITTER = 0;
static const uint FIRE_FLARE = 1;

static const uint RAIN_EMITTER = 0;
static const uint RAIN_DROP = 1;

static const uint SMOKE_EMITTER = 0;
static const uint SMOKE_SHELL = 1;
static const uint SMOKE_PARTICLE = 2;

float3 getRandom3DVector(float offset)
{
	return random_tex.SampleLevel(linear_sampler, game_time + offset, 0).xyz;
}

float3 getRandomUnit3DVector(float offset)
{
	return normalize(getRandom3DVector(offset));
}

float3 calculate3DPosition(float3 acceleration, float time, float3 velocity, float3 pos)
{
	return acceleration * time * time * 0.5f
		+ velocity * time
		+ pos;
}

Particle vsStreamOut(Particle vi)
{
	return vi;
}