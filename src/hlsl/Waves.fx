cbuffer rarely
{
    float k1, k2, k3;
    float disturbance_magnitude; 
    int x;
    int y;
};

Texture2D prev_srv;
Texture2D curr_srv;
RWTexture2D<float> curr_uav;
RWTexture2D<float> next_uav;

[numthreads(16, 16, 1)]
void update(int3 group_thread_id : SV_GroupThreadID, int3 dispatch_thread_id : SV_DispatchThreadID)
{
    const int x = dispatch_thread_id.x;
    const int y = dispatch_thread_id.y;

    next_uav[dispatch_thread_id.xy] =
        k1 * prev_srv[int2(x, y)].r +
        k2 * curr_srv[int2(x, y)].r +
        k3 * (
        curr_srv[int2(x + 1, y)].r +
        curr_srv[int2(x - 1, y)].r +
        curr_srv[int2(x, y + 1)].r +
        curr_srv[int2(x, y - 1)].r
        );
}

[numthreads(1, 1, 1)]
void disturb(int3 group_thread_id : SV_GroupThreadID, int3 dispatch_thread_id : SV_DispatchThreadID)
{	
    const float disturbance_magnitude_half = 0.5f * disturbance_magnitude;

    curr_uav[int2(x, y)] += disturbance_magnitude;
    curr_uav[int2(x + 1, y)] += disturbance_magnitude_half;
    curr_uav[int2(x - 1, y)] += disturbance_magnitude_half;
    curr_uav[int2(x, y + 1)] += disturbance_magnitude_half;
    curr_uav[int2(x, y - 1)] += disturbance_magnitude_half;	
}

technique11 waves_update
{
    pass P0
    {
        SetVertexShader( NULL );
        SetPixelShader( NULL );
        SetComputeShader(CompileShader(cs_5_0, update()));
    }
}

technique11 waves_disturb
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, disturb()));
    }
}