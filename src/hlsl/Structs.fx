#ifndef STRUCTS_FX
#define STRUCTS_FX

//////////////////////////////////////////////////// BASIC
struct VInBasic
{
	float3 pos_l : POSITION;
	float3 normal_l : NORMAL;
	float2 tex : TEXCOORD;
};

struct VOutBasic
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD0;
	float4 shadow_pos_h : TEXCOORD1;
};

//////////////////////////////////////////////////// TREE
struct VInTree
{
	float3 pos_w : POSITION;
	float2 size_w : SIZE;
};

struct VOutTree
{
	float3 center_w : POSITION;
	float2 size_w : SIZE;
};

struct GOutTree
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
	uint primitive_id : SV_PrimitiveID;
};

//////////////////////////////////////////////////// CYLINDER
struct VInCylinder
{
	float3 pos_l : POSITION;
	float3 normal_l: NORMAL;
};

struct VOutCylinder
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
};

struct GOutCylinder
{
	float3 pos_w : POSITION;
	float4 pos_wh : SV_POSITION;
	float3 normal_w : NORMAL;
	uint primitive_id : SV_PrimitiveID;
};

//////////////////////////////////////////////////// ICOSAHEDRON
struct VOutIcosahedron
{
	float3 pos_l : POSITION;
	float3 normal_l : NORMAL;
	float2 tex : TEXCOORD;
};

struct GOutIcosahedron
{
	float3 pos_w : POSITION;
	float4 pos_h : SV_POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
};

//////////////////////////////////////////////////// TESSELLATION
struct VInTess
{
	float3 pos_l : POSITION;
};

struct VOutTess
{
	float3 pos_l : POSITION;
};

struct PatchTessQuad
{
	float edge_tess[4] : SV_TessFactor;
	float inside_tess[2] : SV_InsideTessFactor;
};

struct PatchTessTriangle
{
	float edge_tess[3] : SV_TessFactor;
	float inside_tess : SV_InsideTessFactor;
};

struct HOutTess
{
	float3 pos_l : POSITION;
};

struct DOutTess
{
	float4 pos_h : SV_POSITION;
};

struct DOutTessLight
{
	float3 pos_w : POSITION;
	float4 pos_h : SV_POSITION;
	float3 normal_w : NORMAL;
};

//////////////////////////////////////////////////// INSTANCED BASIC
struct VInInstancedBasic
{
	float3 pos_l : POSITION;
	float3 normal_l : NORMAL;
	float2 tex : TEXCOORD;
	row_major float4x4 world : WORLD;
	float4 color : COLOR;
	uint instance_id : SV_InstanceID;
};

struct VOutInstancedBasic
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
	float4 color : COLOR;
};

//////////////////////////////////////////////////// SKY
struct VInSky
{
	float3 pos_l : POSITION;
};

struct VOutSky
{
	float4 pos_h : SV_POSITION;
	float3 pos_l : POSITION;
};

//////////////////////////////////////////////////// NORMAL MAP

struct VInNormalMap
{
	float3 pos_l : POSITION;
	float3 normal_l : NORMAL;
	float2 tex : TEXCOORD;
	float3 tangent_t : TANGENT;
};

struct VOutNormalMap
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
	float4 shadow_pos_h : TEXCOORD1;
	float3 tangent_w : TANGENT;
};

//////////////////////////////////////////////////// NORMAL MAP WORLD TO TANGENT

struct VOutNormalMapWorldToTangent
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL0;
	float3 normal_l : NORMAL1;
	float2 tex : TEXCOORD;
	float3 tangent_w : TANGENT0;
	float3 tangent_t : TANGENT1;
};

//////////////////////////////////////////////////// DISPLACEMENT MAP

struct VOutDisplacementMap
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
	float3 tangent_w : TANGENT;
	float tess : TESSFACTOR0;
};

struct HOutDisplacementMap
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float3 tangent_w : TANGENT;
	float2 tex : TEXCOORD;
};

//////////////////////////////////////////////////// WATER

struct VOutWater
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex_diffuse : TEXCOORD0;
	float2 tex_normal : TEXCOORD1;
	float2 tex_normal_sec : TEXCOORD2;
	float2 tex_height : TEXCOORD3;
	float2 tex_height_sec : TEXCOORD4;
	float3 tangent_w : TANGENT;
	float tess : TESSFACTOR0;
};

struct HOutWater
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex_diffuse : TEXCOORD0;
	float2 tex_normal : TEXCOORD1;
	float2 tex_normal_sec : TEXCOORD2;
	float2 tex_height : TEXCOORD3;
	float2 tex_height_sec : TEXCOORD4;
	float3 tangent_w : TANGENT;
};

struct DOutWater
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex_diffuse : TEXCOORD0;
	float2 tex_normal : TEXCOORD1;
	float2 tex_normal_sec : TEXCOORD2;
	float3 tangent_w : TANGENT;
};

//////////////////////////////////////////////////// TERRAIN

struct VInTerrain
{
	float3 pos_l : POSITION;
	float2 tex : TEXCOORD0;
	float2 y_bounds : TEXCOORD1;
};

struct VOutTerrain
{
	float3 pos_w : POSITION;
	float2 tex : TEXCOORD0;
	float2 y_bounds : TEXCOORD1;
	float density : DENSITY;
};

struct HOutTerrain
{
	float3 pos_w : POSITION;
	float2 tex : TEXCOORD0;
};

struct DOutTerrain
{
	float4 pos_h : SV_POSITION;
	float3 pos_w : POSITION;
	float2 tex : TEXCOORD0;
	float2 tiled_tex : TEXCOORD1;
};

//////////////////////////////////////////////////// PARTICLES

struct Particle
{
	float3 initial_pos_w : POSITION;
	float3 initial_velocity_w : VELOCITY;
	float2 size : SIZE;
	float age : AGE;
	uint type : TYPE;
	float delay : DELAY;
};

struct VOutFire
{
	float3 pos_w : POSITION;
	float2 size : SIZE;
	float4 color : COLOR;
	uint type : TYPE;
};

struct VOutRain
{
	float3 pos_w : POSITION;
	uint type : TYPE;
};

struct GOutFire
{
	float4 pos_h : SV_Position;
	float4 color : COLOR;
	float2 tex : TEXCOORD;
};

struct GOutRain
{
	float4 pos_h : SV_Position;
	float2 tex : TEXCOORD;
};

//////////////////////////////////////////////////// DEBUG

struct VOutDebug
{
	float4 pos_h : SV_POSITION;
	float2 tex : TEXCOORD0;
};

//////////////////////////////////////////////////// SHADOW

struct VOutDisplacementMapShadow
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
	float tess : TESSFACTOR0;
};

struct HOutDisplacementMapShadow
{
	float3 pos_w : POSITION;
	float3 normal_w : NORMAL;
	float2 tex : TEXCOORD;
};

#endif