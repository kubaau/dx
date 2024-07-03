#pragma once

#include "DXTypedefs.hpp"
#include "IComputeShader.hpp"

class ComputeShaderVecLength : public ComputeShaderBase
{
public:
    void run(Device&, Context&) override;

private:
    void init(Device&, Context&);
    void compute(Context&);

    D3DBuffer output_vec_length;
    D3DBuffer output_vec_length_debug;
    UAVPtr input_vec_length_uav;
    UAVPtr output_vec_length_uav;
};
