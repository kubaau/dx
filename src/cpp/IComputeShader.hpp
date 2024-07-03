#pragma once

#include "DXTypedefs.hpp"
#include "Interface.hpp"

class IComputeShader : public Interface
{
public:
    virtual void run(Device&, Context&) = 0;
};

class ComputeShaderBase : public IComputeShader
{
protected:
    unsigned element_count;
    bool already_ran{};
};
