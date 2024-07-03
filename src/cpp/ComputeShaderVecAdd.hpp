#pragma once

#include "DXTypedefs.hpp"
#include "IComputeShader.hpp"
#include "MathTypedefs.hpp"

class ComputeShaderVecAdd : public ComputeShaderBase
{
public:
    void run(Device&, Context&) override;

private:
    void init(Device&);
    void compute(Context&);

    struct DataVecAdd
    {
        float3 vector1;
        float2 vector2;
    };

    D3DBuffer output_vec_add;
    D3DBuffer output_vec_add_debug;
    SRVPtr input_vec_add1_srv;
    SRVPtr input_vec_add2_srv;
    UAVPtr output_vec_add_uav;
};
