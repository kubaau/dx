#include "WavesCPU.hpp"
#include <cassert>
#include <cmath>
#include "AppEngine.hpp"

void WavesCPU::init(unsigned rows, unsigned columns, float dx, float dt, float speed, float damping)
{
    row_count = rows;
    column_count = columns;

    vertex_count = rows * columns;
    triangle_count = (rows - 1) * (columns - 1) * 2;

    spatial_step = dx;
    time_step = dt;

    const auto c = damping * time_step;
    const auto d = c + 2;
    const auto e = pow(speed, 2) * pow(time_step, 2) / pow(spatial_step, 2);
    k1 = static_cast<float>((c - 2) / d);
    k2 = static_cast<float>((4 - 8 * e) / d);
    k3 = static_cast<float>(2 * e / d);

    prev.resize(vertex_count);
    curr.resize(vertex_count);
    normals.resize(vertex_count);
    tangent_x.resize(vertex_count);

    const auto half_width = (columns - 1) * dx * 0.5f;
    const auto half_depth = (rows - 1) * dx * 0.5f;
    for (auto i = 0u; i < rows; ++i)
    {
        const auto z = half_depth - i * dx;
        for (auto j = 0u; j < columns; ++j)
        {
            const auto index = i * columns + j;
            const auto x = -half_width + j * dx;
            curr[index] = float3(x, 0.0f, z);
            prev[index] = float3(x, 0.0f, z);
            normals[index] = float3(0.0f, 1.0f, 0.0f);
            tangent_x[index] = float3(1.0f, 0.0f, 0.0f);
        }
    }
}

void WavesCPU::disturb(unsigned i, unsigned j, float magnitude)
{
    // Don't disturb boundaries.
    assert(i > 1 && i < row_count - 2);
    assert(j > 1 && j < column_count - 2);

    // Disturb the ijth vertex height and its neighbors.
    const auto half_magnitude = 0.5f * magnitude;
    curr[i * column_count + j].y += magnitude;
    curr[i * column_count + j + 1].y += half_magnitude;
    curr[i * column_count + j - 1].y += half_magnitude;
    curr[(i + 1) * column_count + j].y += half_magnitude;
    curr[(i - 1) * column_count + j].y += half_magnitude;
}

void WavesCPU::update()
{
    static auto elapsed = 0.0f;

    elapsed += APP.deltaF();

    if (elapsed >= time_step)
    {
        elapsed = 0.0f;

        // Only update interior points; we use zero boundary conditions.
        for (auto i = 1u; i < row_count - 1; ++i)
        {
            for (auto j = 1u; j < column_count - 1; ++j)
            {
                // After this update we will be discarding the old previous buffer, so overwrite that buffer with the
                // new update. Note how we can do this inplace (read/write to same element) because we won't need
                // prev_ij again and the assignment happens last. Note j indexes x and i indexes z: h(x_j, z_i, t_k).
                // Moreover, our +z axis goes "down"; this is just to keep consistent with our row indices going down.
                prev[i * column_count + j].y = k1 * prev[i * column_count + j].y + k2 * curr[i * column_count + j].y +
                    k3 *
                        (curr[(i + 1) * column_count + j].y + curr[(i - 1) * column_count + j].y +
                         curr[i * column_count + j + 1].y + curr[i * column_count + j - 1].y);
            }
        }

        // We just overwrote the previous buffer with the new data, so this data needs to become the current solution
        // and the old current solution becomes the new previous solution.
        swap(prev, curr);

        // Compute normals using finite difference scheme.
        for (auto i = 1u; i < row_count - 1; ++i)
        {
            for (auto j = 1u; j < column_count - 1; ++j)
            {
                const auto l = curr[i * column_count + j - 1].y;
                const auto r = curr[i * column_count + j + 1].y;
                const auto t = curr[(i - 1) * column_count + j].y;
                const auto b = curr[(i + 1) * column_count + j].y;
                normals[i * column_count + j].x = -r + l;
                normals[i * column_count + j].y = 2.0f * spatial_step;
                normals[i * column_count + j].z = b - t;

                using namespace DirectX;

                const auto n = XMVector3Normalize(XMLoadFloat3(&normals[i * column_count + j]));
                XMStoreFloat3(&normals[i * column_count + j], n);

                tangent_x[i * column_count + j] = float3(2.0f * spatial_step, r - l, 0.0f);
                const auto tangent = XMVector3Normalize(XMLoadFloat3(&tangent_x[i * column_count + j]));
                XMStoreFloat3(&tangent_x[i * column_count + j], tangent);
            }
        }
    }
}
