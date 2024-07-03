#ifndef LIGHTS_FX
#define LIGHTS_FX

struct DirectionalLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 direction;
    float pad;
};

struct PointLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 pos;
    float range;

    float3 attenuation;
    float pad;
};

struct SpotLight
{
    float4 ambient;
    float4 diffuse;
    float4 specular;

    float3 pos;
    float range;

    float3 direction;
    float spot;

    float3 attenuation;
    float pad;
};

struct Material
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float4 reflect;
};

struct LightColor
{
    float4 ambient;
    float4 diffuse;
    float4 specular;
};

float toonShade(float diffuse_factor)
{
    if (diffuse_factor < 0.0f)
        diffuse_factor = 0.4f;
    else if (diffuse_factor <= 0.5f)
        diffuse_factor = 0.6f;
    else
        diffuse_factor = 1.0f;

    return diffuse_factor;
}

LightColor directionalLight(Material mat, DirectionalLight light, float3 normal, float3 to_eye)
{
    LightColor light_color;
    light_color.ambient = mat.ambient * light.ambient;

    const float3 light_v = -light.direction;
    float diffuse_factor = dot(light_v, normal);
    //diffuse_factor = toonShade(diffuse_factor);

    [flatten]
    if (diffuse_factor > 0)
    {
        light_color.diffuse = diffuse_factor * mat.diffuse * light.diffuse;

        const float3 reflection = reflect(light.direction, normal);
        const float specular_factor = pow(max(dot(reflection, to_eye), 0.0f), mat.specular.w);
        light_color.specular = specular_factor * mat.specular * light.specular;
    }
    else
    {
        light_color.diffuse = 0;
        light_color.specular = 0;
    }

    return light_color;
}

LightColor pointLight(Material mat, PointLight light, float3 pos, float3 normal, float3 to_eye)
{
    LightColor light_color;
    light_color.ambient = mat.ambient * light.ambient;

    float3 light_v = light.pos - pos;
    const float distance = length(light_v);

    if (distance > light.range)
    {
        light_color.diffuse = 0;
        light_color.specular = 0;
        return light_color;
    }
    light_v /= distance;

    const float diffuse_factor = dot(light_v, normal);

    [flatten]
    if (diffuse_factor > 0.0f)
    {
        light_color.diffuse = diffuse_factor * mat.diffuse * light.diffuse;

        const float3 reflection = reflect(-light_v, normal);
        const float specular_factor = pow(max(dot(reflection, to_eye), 0.0f), mat.specular.w);
        light_color.specular = specular_factor * mat.specular * light.specular;

        const float attenuation = 1.0f / dot(light.attenuation, float3(1.0f, distance, distance * distance));
        light_color.diffuse *= attenuation;
        light_color.specular *= attenuation;
    }
    else
    {
        light_color.diffuse = 0;
        light_color.specular = 0;
    }

    return light_color;
}

LightColor spotLight(Material mat, SpotLight spot_light, float3 pos, float3 normal, float3 to_eye)
{
    LightColor light_color;
    light_color.ambient = mat.ambient * spot_light.ambient;

    float3 light_v = spot_light.pos - pos;
    const float distance = length(light_v);

    if (distance > spot_light.range)
    {
        light_color.diffuse = 0;
        light_color.specular = 0;
        return light_color;
    }
    light_v /= distance;

    const float diffuse_factor = dot(light_v, normal);

    [flatten]
    if (diffuse_factor > 0.0f)
    {
        light_color.diffuse = diffuse_factor * mat.diffuse * spot_light.diffuse;

        const float3 reflection = reflect(-light_v, normal);
        const float specular_factor = pow(max(dot(reflection, to_eye), 0.0f), mat.specular.w);
        light_color.specular = specular_factor * mat.specular * spot_light.specular;

        const float spot = pow(max(dot(-light_v, spot_light.direction), 0.0f), spot_light.spot);
        const float attenuation = spot / dot(spot_light.attenuation, float3(1.0f, distance, distance * distance));

        light_color.ambient *= spot;
        light_color.diffuse *= attenuation;
        light_color.specular *= attenuation;
    }
    else
    {
        light_color.diffuse = 0;
        light_color.specular = 0;

        // Scale by spotlight factor and attenuate.
        const float spot = pow(max(dot(-light_v, spot_light.direction), 0.0f), spot_light.spot);
        light_color.ambient *= spot;
    }

    return light_color;
}

float3 normalSampleToTangentSpace(float3 normal_map_sample)
{
    // [0, 1] to [-1, 1]
    const float3 normal_tangent_space = 2.0f * normal_map_sample - 1.0f;
    return normalize(normal_tangent_space);
}

float3 tbnTransform(float3 vec, float3 normal, float3 tangent)
{
    tangent = normalize(tangent - dot(tangent, normal) * normal);
    const float3 binormal = cross(normal, tangent);

    return mul(vec, float3x3(tangent, binormal, normal));
}

float3 normalSampleToWorldSpace(float3 normal_map_sample, float3 normal_w, float3 tangent_w)
{
    const float3 normal_tangent_space = normalSampleToTangentSpace(normal_map_sample);
    return normalize(tbnTransform(normal_tangent_space, normal_w, tangent_w));
}

float shadowFactor(SamplerComparisonState shadow_sampler, Texture2D shadow_map, float4 shadow_pos_h)
{
    // complete projection by doing division by w
    shadow_pos_h.xyz /= shadow_pos_h.w;

    // depth in NDC space
    const float depth = shadow_pos_h.z;

    // texel size
    static const float shadow_max_size = 2048.0f;
    const float dx = 1.0f / shadow_max_size;

    float percent_lit = 0.0f;
    static const uint offset_count = 9;
    const float2 offsets[offset_count] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(dx, -dx),
        float2(-dx, 0.0f), float2(0.0f, 0.0f), float2(dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(dx, +dx)
    };

    [unroll]
    for (uint i = 0; i < offset_count; ++i)
        percent_lit += shadow_map.SampleCmpLevelZero(shadow_sampler, shadow_pos_h.xy + offsets[i], depth).r;

    return percent_lit /= offset_count;
}

#endif