cbuffer fixed
{
    static const int blur_radius = 5;
    static const int blur_radius_x2 = blur_radius * 2;
    static const int thread_count = 256;
    static const int cache_size = thread_count + blur_radius_x2;
}; 

cbuffer rarely
{
    float gaussian_weights[blur_radius_x2 + 1];
    float color_sigma;
    bool bilateral;
};

Texture2D input;
RWTexture2D<float4> output;

groupshared float4 color_cache[cache_size];

float4 blurColor(int main_color_index, int i)
{
    const int current_color_index = main_color_index + i;

    const float4 current_color = color_cache[current_color_index];
    const float4 blur_color = current_color * gaussian_weights[blur_radius + i];

    if (bilateral)
    {
        const float4 main_color = color_cache[main_color_index];
        const float delta_color = distance(main_color.rgb, current_color.rgb);
        const float bilateral_weight = exp(-(delta_color * delta_color) / (color_sigma * color_sigma));
        return blur_color * bilateral_weight;
    }
    else
        return blur_color;
}

[numthreads(thread_count, 1, 1)]
void horizontalBlur(int3 group_thread_id : SV_GroupThreadID, int3 dispatch_thread_id : SV_DispatchThreadID)
{
    // Fill local thread storage to reduce bandwidth. To blur N pixels, we will need to load N + 2 * blur_radius pixels.
    // This thread group runs N threads. To get the extra 2 * blur_radius pixels, have 2 * blur_radius threads sample an
    // extra pixel.
    if (group_thread_id.x < blur_radius)
    {
        const int x = max(dispatch_thread_id.x - blur_radius, 0);
        color_cache[group_thread_id.x] = input[int2(x, dispatch_thread_id.y)];
    }
    if (group_thread_id.x >= thread_count - blur_radius)
    {
        const int x = min(dispatch_thread_id.x + blur_radius, input.Length.x - 1);
        color_cache[group_thread_id.x + blur_radius_x2] = input[int2(x, dispatch_thread_id.y)];
    }

    // to avoid treating pixels to the right of texture as black
    color_cache[group_thread_id.x + blur_radius] = input[min(dispatch_thread_id.xy, input.Length.xy - 1)];

    GroupMemoryBarrierWithGroupSync();
    
    float4 blur_color = 0;
    [unroll]
    for (int i = -blur_radius; i <= blur_radius; ++i)
        blur_color += blurColor(group_thread_id.x + blur_radius, i);    
    output[dispatch_thread_id.xy] = blur_color;
}

[numthreads(1, thread_count, 1)]
void verticalBlur(int3 group_thread_id : SV_GroupThreadID, int3 dispatch_thread_id : SV_DispatchThreadID)
{
    if (group_thread_id.y < blur_radius)
    {
        const int y = max(dispatch_thread_id.y - blur_radius, 0);
        color_cache[group_thread_id.y] = input[int2(dispatch_thread_id.x, y)];
    }
    if (group_thread_id.y >= thread_count - blur_radius)
    {
        const int y = min(dispatch_thread_id.y + blur_radius, input.Length.y - 1);
        color_cache[group_thread_id.y + blur_radius_x2] = input[int2(dispatch_thread_id.x, y)];
    }
    
    color_cache[group_thread_id.y + blur_radius] = input[min(dispatch_thread_id.xy, input.Length.xy - 1)];
    
    GroupMemoryBarrierWithGroupSync();

    float4 blur_color = 0;
    [unroll]
    for(int i = -blur_radius; i <= blur_radius; ++i)
        blur_color += blurColor(group_thread_id.y + blur_radius, i);    
    output[dispatch_thread_id.xy] = blur_color;
}

technique11 horizontal_blur
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, horizontalBlur()));
    }
}

technique11 vertical_blur
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, verticalBlur()));
    }
}