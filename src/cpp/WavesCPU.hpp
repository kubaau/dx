#pragma once

#include <vector>
#include "IWavesCPU.hpp"

class WavesCPU : public IWavesCPU
{
public:
    void init(unsigned row_count,
              unsigned column_count,
              float spatial_step,
              float time_step,
              float speed,
              float damping) override;
    void disturb(unsigned i, unsigned j, float magnitude) override;
    void update() override;

    const float3& current(unsigned i) const override { return curr[i]; }
    const float3& normal(unsigned i) const override { return normals[i]; }
    unsigned vertexCount() const override { return vertex_count; }
    unsigned triangleCount() const override { return triangle_count; }
    unsigned rowCount() const override { return row_count; }
    unsigned columnCount() const override { return column_count; }
    float width() const override { return column_count * spatial_step; }
    float depth() const override { return row_count * spatial_step; }

private:
    unsigned row_count;
    unsigned column_count;
    unsigned vertex_count;
    unsigned triangle_count;
    float time_step;
    float spatial_step;
    float k1, k2, k3;

    vector<float3> prev;
    vector<float3> curr;
    vector<float3> normals;
    vector<float3> tangent_x;
};
