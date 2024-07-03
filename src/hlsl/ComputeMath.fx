struct DataVecAdd
{
    float3 v1;
    float2 v2;
};

StructuredBuffer<DataVecAdd> input_vec_add1;
StructuredBuffer<DataVecAdd> input_vec_add2;
RWStructuredBuffer<DataVecAdd> output_vec_add;

[numthreads(32, 1, 1)]
void computeShaderVecAdd(int3 dispatch_thread_id : SV_DispatchThreadID)
{
    output_vec_add[dispatch_thread_id.x].v1 = input_vec_add1[dispatch_thread_id.x].v1 + input_vec_add2[dispatch_thread_id.x].v1;
    output_vec_add[dispatch_thread_id.x].v2 = input_vec_add1[dispatch_thread_id.x].v2 + input_vec_add2[dispatch_thread_id.x].v2;
}

/* doesn't work - why?
Buffer<float3> input_vec_length;
RWBuffer<float> output_vec_length;

[numthreads(64, 1, 1)]
void computeShaderVecLength(int3 dispatch_thread_id : SV_DispatchThreadID)
{
    output_vec_length[dispatch_thread_id.x] = length(input_vec_length[dispatch_thread_id.x]);
}
*/

ConsumeStructuredBuffer<float3> input_vec_length;
AppendStructuredBuffer<float> output_vec_length;

[numthreads(64, 1, 1)]
void computeShaderVecLengthConsumeAppend()
{
    float3 vec = input_vec_length.Consume();
    float len = length(vec);
    output_vec_length.Append(len);
}

technique11 vec_add
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        SetComputeShader(CompileShader(cs_5_0, computeShaderVecAdd()));
    }
}

technique11 vec_length
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);
        //SetComputeShader(CompileShader(cs_5_0, computeShaderVecLength()));
        SetComputeShader(CompileShader(cs_5_0, computeShaderVecLengthConsumeAppend()));
    }
}