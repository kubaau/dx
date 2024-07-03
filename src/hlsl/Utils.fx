#ifndef UTILS_FX
#define UTILS_FX

#include "Lights.fx"

float4 homogenize(float3 v)
{
    return float4(v, 1.0f);
}

struct ToEye
{
    float3 v;
    float d;
};

ToEye toEye(float3 eye_pos, float3 obj_pos)
{
    ToEye to_eye;
    to_eye.v = eye_pos - obj_pos;
    to_eye.d = length(to_eye.v);
    to_eye.v /= to_eye.d;
    return to_eye;
}

float4 addTextureColor(inout float4 p, Texture2D tex, SamplerState ss, float2 uv)
{
    float4 ret = tex.Sample(ss, uv);
    p *= ret;
    return ret;
}

float4 addTextureColor(inout float4 p, Texture2DArray tex_array, SamplerState ss, float3 uvw)
{
    float4 ret = tex_array.Sample(ss, uvw);
    p *= ret;
    return ret;
}

void addDirLightsColor(inout float4 p,
                       uint light_count,
                       DirectionalLight lights[3],
                       Material mat,
                       float3 normal,
                       float3 to_eye,
                       float3 shadow)
{
    float4 ambient = 0;
    float4 diffuse = 0;
    float4 specular = 0;

    [unroll] 
    for (uint i = 0; i < light_count; ++i)
    {
        const LightColor light_color = directionalLight(mat, lights[i], normal, to_eye);
        ambient += light_color.ambient;
        diffuse += shadow[i] * light_color.diffuse;
        specular += shadow[i] * light_color.specular;
    }

    p = p * (ambient + diffuse) + specular;
}

static const uint REFLECTION_NONE = 0;
static const uint REFLECTION_REFLECT = 1;
static const uint REFLECTION_REFRACT = 2;

void addReflectionColor(inout float4 p,
                        uint reflection_type,
                        Material mat,
                        float3 normal,
                        float3 to_eye,
                        TextureCube cube_map,
                        SamplerState ss)
{
    if (reflection_type != REFLECTION_NONE)
    {
        const float3 incident = -to_eye;
        float3 reflection_vector = 0;

        if (reflection_type == REFLECTION_REFLECT)
            reflection_vector = reflect(incident, normal);
        else
            reflection_vector = refract(incident, normal, 0.98f);

        float4 reflection_color = cube_map.Sample(ss, reflection_vector);
        p += mat.reflect * reflection_color;
    }
}

void addFogColor(inout float4 p, float distance, float fog_start, float fog_range, float4 fog_color)
{    
    const float fog_lerp = saturate((distance - fog_start) / fog_range);
    p = lerp(p, fog_color, fog_lerp);
}

float4 psLightOnly(
    float3 normal, float3 eye_pos, float3 pos, uint light_count, DirectionalLight dir_lights[3], Material mat)
{
    float4 pixel_color = 1;
    
    normal = normalize(normal);    
    const ToEye to_eye = toEye(eye_pos, pos);

    if (light_count > 0)
    {
        float3 shadow = 1;
        addDirLightsColor(pixel_color, light_count, dir_lights, mat, normal, to_eye.v, shadow);
    }

    pixel_color.a = mat.diffuse.a;

    return pixel_color;
}

#endif