#pragma once

#include "Interface.hpp"
#include "MathTypedefs.hpp"

class IWavesCPU : public Interface
{
public:
    virtual void init(
        unsigned row_count, unsigned column_count, float spatial_step, float time_step, float speed, float damping) = 0;
    virtual void disturb(unsigned i, unsigned j, float magnitude) = 0;
    virtual void update() = 0;

    virtual const float3& current(unsigned i) const = 0;
    virtual const float3& normal(unsigned i) const = 0;
    virtual unsigned vertexCount() const = 0;
    virtual unsigned triangleCount() const = 0;
    virtual unsigned rowCount() const = 0;
    virtual unsigned columnCount() const = 0;
    virtual float width() const = 0;
    virtual float depth() const = 0;
};
