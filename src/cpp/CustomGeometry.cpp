#include "CustomGeometry.hpp"
#include <tools/ContainerOperators.hpp>
#include <tools/Random.hpp>
#include <tools/Typedefs.hpp>
#include "GeometryGenerator.h"
#include "GraphicsDebugUtils.hpp"
#include "IWavesCPU.hpp"
#include "IWavesGPU.hpp"
#include "Mesh.hpp"
#include "Vertex.hpp"

Meshes buildCustomMeshes(Device& device, IWavesCPU& waves_cpu, IWavesGPU& waves_gpu)
{
    using namespace RangeOperators;

    Meshes meshes;
    meshes += buildSkullRoom(device);
    meshes += buildCircle(device, 1.0, 20);
    meshes += buildIcosahedron(device);
    meshes += buildLand(device);
    meshes += buildWavesCPU(device, waves_cpu);
    meshes += buildWavesGPU(device, waves_gpu);
    meshes += buildBox(device);
    meshes += buildCylinder(device);
    meshes += buildTreeSprites(device, 16);
    meshes += buildScreenQuad(device);
    meshes += buildQuadPatch(device);
    meshes += buildTrianglePatch(device);
    meshes += buildIcosahedronPatch(device);
    meshes += buildCubicBezierQuadPatch(device);
    meshes += buildCubicBezierTrianglePatch(device);
    meshes += buildQuadraticBezierQuadPatch(device);
    meshes += buildQuadraticBezierTrianglePatch(device);
    meshes += buildSky(device, 5000);
    return meshes;
}

Meshes buildSkullRoom(Device& device)
{
    using Vertex = Vertex::Basic;

    Vertex v[30];

    // Floor: Observe we tile texture coordinates.
    v[0] = {-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f};
    v[1] = {-3.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f};
    v[2] = {7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f};

    v[3] = {-3.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 0.0f, 4.0f};
    v[4] = {7.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 4.0f, 0.0f};
    v[5] = {7.5f, 0.0f, -10.0f, 0.0f, 1.0f, 0.0f, 4.0f, 4.0f};

    // Wall: Observe we tile texture coordinates, and that we leave a gap in the middle for the mirror.
    v[6] = {-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    v[7] = {-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    v[8] = {-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f};

    v[9] = {-3.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    v[10] = {-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.0f};
    v[11] = {-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.5f, 2.0f};

    v[12] = {2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    v[13] = {2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    v[14] = {7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f};

    v[15] = {2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 2.0f};
    v[16] = {7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 0.0f};
    v[17] = {7.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 2.0f, 2.0f};

    v[18] = {-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    v[19] = {-3.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    v[20] = {7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f};

    v[21] = {-3.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    v[22] = {7.5f, 6.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 0.0f};
    v[23] = {7.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 6.0f, 1.0f};

    // Mirror
    v[24] = {-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    v[25] = {-2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f};
    v[26] = {2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f};

    v[27] = {-2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f};
    v[28] = {2.5f, 4.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f};
    v[29] = {2.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f};

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;

    D3DBuffer floor_vb;
    vbd.ByteWidth = sizeof(Vertex) * 6;
    vertex_init_data.pSysMem = v;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &floor_vb));

    D3DBuffer wall_vb;
    vbd.ByteWidth = sizeof(Vertex) * 18;
    vertex_init_data.pSysMem = v + 6;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &wall_vb));

    D3DBuffer mirror_vb;
    vbd.ByteWidth = sizeof(Vertex) * 6;
    vertex_init_data.pSysMem = v + 6 + 18;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &mirror_vb));

    Meshes ret;
    ret.emplace("floor", Mesh{floor_vb, 6, sizeof(Vertex)});
    ret.emplace("wall", Mesh{wall_vb, 18, sizeof(Vertex)});
    ret.emplace("mirror", Mesh{mirror_vb, 6, sizeof(Vertex)});
    return ret;
}

Meshes buildCircle(Device& device, float radius, unsigned v_count)
{
    using Vertex = Vertex::Basic;

    const auto step = 2 * pi / (v_count - 1);

    vector<Vertex> vs(v_count);

    float angle = 0.0;
    for (auto i = 0u; i < v_count - 1; ++i)
    {
        const auto x = radius * cos(angle);
        const auto z = radius * sin(angle);
        angle += step;
        vs[i].position = float3(x, 0.0f, z);
    }
    vs[v_count - 1].position = vs[0].position;

    for (auto i = 0u; i < v_count - 1; ++i)
    {
        vs[i].normal.x = vs[i + 1].position.x - vs[i].position.x;
        vs[i].normal.y = 0.0f;
        vs[i].normal.z = vs[i + 1].position.z - vs[i].position.z;
    }
    vs[v_count - 1].normal.x = vs[0].position.x - vs[v_count - 1].position.x;
    vs[v_count - 1].normal.y = 0.0f;
    vs[v_count - 1].normal.z = vs[0].position.z - vs[v_count - 1].position.z;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;

    D3DBuffer circle_vb;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vertex_init_data.pSysMem = vs.data();
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &circle_vb));

    Meshes ret;
    ret.emplace("circle", Mesh{circle_vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildIcosahedron(Device& device)
{
    using Vertex = Vertex::Basic;

    GeometryGenerator::MeshData icosahedron;
    GeometryGenerator geo_gen;
    geo_gen.CreateGeosphere(1.0f, 0, icosahedron);

    vector<Vertex> vs(icosahedron.Vertices.size());

    for (auto i = 0u; i < icosahedron.Vertices.size(); ++i)
    {
        vs[i].position = icosahedron.Vertices[i].Position;
        vs[i].normal = icosahedron.Vertices[i].Normal;
        vs[i].tex = icosahedron.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;

    D3DBuffer icosahedron_vb;
    vbd.ByteWidth = static_cast<unsigned>(sizeof(Vertex) * icosahedron.Vertices.size());
    vertex_init_data.pSysMem = vs.data();
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &icosahedron_vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;

    D3DBuffer icosahedron_ib;
    ibd.ByteWidth = static_cast<unsigned>(sizeof(unsigned) * icosahedron.Indices.size());
    index_init_data.pSysMem = icosahedron.Indices.data();
    HR(device.CreateBuffer(&ibd, &index_init_data, &icosahedron_ib));

    Meshes ret;
    ret.emplace("icosahedron",
                Mesh{icosahedron_vb,
                     static_cast<unsigned>(icosahedron.Vertices.size()),
                     sizeof(Vertex),
                     icosahedron_ib,
                     static_cast<unsigned>(icosahedron.Indices.size())});
    return ret;
}

auto hillHeight(float x, float z)
{
    return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}

auto hillNormal(float x, float z)
{
    using namespace DirectX;

    // n = (-df/dx, 1, -df/dz)
    float3 n(
        -0.03f * z * cosf(0.1f * x) - 0.3f * cosf(0.1f * z), 1.0f, -0.3f * sinf(0.1f * x) + 0.03f * x * sinf(0.1f * z));
    auto unitNormal = XMVector3Normalize(XMLoadFloat3(&n));
    XMStoreFloat3(&n, unitNormal);
    return n;
}

Meshes buildLand(Device& device)
{
    using Vertex = Vertex::Basic;

    GeometryGenerator::MeshData grid;
    GeometryGenerator geo_gen;

    geo_gen.CreateGrid(160.0f, 160.0f, 50, 50, grid);

    const auto v_count = static_cast<unsigned>(grid.Vertices.size());
    const auto i_count = static_cast<unsigned>(grid.Indices.size());

    // Extract the vertex elements we are interested and apply the height function to each vertex.
    vector<Vertex> vs(v_count);
    for (auto i = 0u; i < v_count; ++i)
    {
        auto pos = grid.Vertices[i].Position;

        pos.y = hillHeight(pos.x, pos.z);

        vs[i].position = pos;
        vs[i].normal = hillNormal(pos.x, pos.z);
        vs[i].tex = grid.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vs.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = grid.Indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("land", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildWavesCPU(Device& device, IWavesCPU& waves)
{
    using Vertex = Vertex::Basic;

    // Create the vertex buffer. Note that we allocate space only, as we will be updating the data every time step of
    // the simulation.

    const auto v_count = static_cast<unsigned>(waves.vertexCount());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    vbd.MiscFlags = 0;
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, 0, &vb));

    // Create the index buffer. The index buffer is fixed, so we only need to create and set once.
    vector<unsigned> indices(3 * waves.triangleCount());

    // Iterate over each quad.
    const auto m = waves.rowCount();
    const auto n = waves.columnCount();
    auto k = 0;
    for (auto i = 0u; i < m - 1; ++i)
    {
        for (auto j = 0u; j < n - 1; ++j)
        {
            indices[k] = i * n + j;
            indices[k + 1] = i * n + j + 1;
            indices[k + 2] = (i + 1) * n + j;

            indices[k + 3] = (i + 1) * n + j;
            indices[k + 4] = i * n + j + 1;
            indices[k + 5] = (i + 1) * n + j + 1;

            k += 6; // next quad
        }
    }

    const auto i_count = static_cast<unsigned>(indices.size());

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("waves_cpu", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildWavesGPU(Device& device, IWavesGPU& waves)
{
    using Vertex = Vertex::Basic;

    GeometryGenerator::MeshData grid;
    GeometryGenerator geo_gen;

    geo_gen.CreateGrid(160, 160, waves.width(), waves.height(), grid);
    // geo_gen.CreateGrid(waves.width(), waves.height(), waves.width(), waves.height(), grid);

    const auto v_count = static_cast<unsigned>(grid.Vertices.size());
    const auto i_count = static_cast<unsigned>(grid.Indices.size());

    vector<Vertex> vertices(v_count);
    for (auto i = 0u; i < v_count; ++i)
    {
        vertices[i].position = grid.Vertices[i].Position;
        vertices[i].normal = grid.Vertices[i].Normal;
        vertices[i].tex = grid.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = grid.Indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("waves_gpu", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildBox(Device& device)
{
    using Vertex = Vertex::Basic;

    GeometryGenerator::MeshData box;
    GeometryGenerator geo_gen;

    geo_gen.CreateBox(1.0f, 1.0f, 1.0f, box);

    const auto v_count = static_cast<unsigned>(box.Vertices.size());
    const auto i_count = static_cast<unsigned>(box.Indices.size());

    vector<Vertex> vs(v_count);

    for (auto i = 0u; i < v_count; ++i)
    {
        vs[i].position = box.Vertices[i].Position;
        vs[i].normal = box.Vertices[i].Normal;
        vs[i].tex = box.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vs.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = box.Indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("box", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildCylinder(Device& device)
{
    using Vertex = Vertex::Basic;

    // Make a cylinder with one stack and no end-caps.
    const auto radius = 15, length = 20;
    const auto slice_count = 32u;
    const auto circle_v_count = slice_count + 1u;
    const auto v_count = 2 * circle_v_count;
    const auto i_count = 6 * slice_count;
    const auto d_theta = 2.0f * pi / slice_count;

    Vertex vs[v_count];

    for (auto i = 0u; i < circle_v_count; ++i)
    {
        const auto x = radius * cosf(i * d_theta);
        const auto z = radius * sinf(i * d_theta);

        vs[i].position = float3(x, 0.0f, z);
        vs[i].normal = float3(x, 0.0f, z);
        vs[i].tex = float2(i * d_theta / (2.0f * pi), 0.95f);

        vs[i + circle_v_count].position = float3(x, (float)length, z);
        vs[i + circle_v_count].normal = float3(x, 0.0f, z);
        vs[i + circle_v_count].tex = float2(i * d_theta / (2.0f * pi), 0.05f);
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vs;
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    vector<unsigned> indices(i_count);
    for (auto i = 0u; i < slice_count; ++i)
    {
        indices[i * 6 + 0] = i;
        indices[i * 6 + 1] = circle_v_count + i;
        indices[i * 6 + 2] = circle_v_count + i + 1;

        indices[i * 6 + 3] = i;
        indices[i * 6 + 4] = circle_v_count + i + 1;
        indices[i * 6 + 5] = i + 1;
    }

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("cylinder", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildTreeSprites(Device& device, unsigned count)
{
    using Vertex = Vertex::PointSprite;

    vector<Vertex> vs(count);
    const auto x_values = randomFloats<float>(-3.5, 3.5, count);
    const auto z_values = randomFloats<float>(-3.5, 3.5, count);
    for (auto i = 0u; i < count; ++i)
    {
        auto y = hillHeight(x_values[i], z_values[i]);
        y += 1.0f; // move slightly above land height
        vs[i].position = {x_values[i], y, z_values[i]};
        vs[i].position.x += -13;
        vs[i].position.z += -8;
        const auto size = randomFloat(2.0f, 3.0f);
        vs[i].size = {size, size};
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vs.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    vector<u16> indices(count);
    iota(indices);

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(u16) * count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("tree_sprites", Mesh{vb, count, sizeof(Vertex), ib, count, DXGI_FORMAT_R16_UINT});
    return ret;
}

Meshes buildScreenQuad(Device& device)
{
    using Vertex = Vertex::Basic;

    GeometryGenerator::MeshData quad;
    GeometryGenerator geo_gen;

    geo_gen.CreateFullscreenQuad(quad);

    const auto v_count = static_cast<unsigned>(quad.Vertices.size());
    const auto i_count = static_cast<unsigned>(quad.Indices.size());

    vector<Vertex> vs(v_count);

    for (auto i = 0u; i < v_count; ++i)
    {
        vs[i].position = quad.Vertices[i].Position;
        vs[i].normal = quad.Vertices[i].Normal;
        vs[i].tex = quad.Vertices[i].TexC;
    }

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vs.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = static_cast<unsigned>(sizeof(unsigned) * quad.Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = quad.Indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("screen_quad", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildQuadPatch(Device& device)
{
    using Vertex = Vertex::PositionOnly;

    vector<Vertex> vertices(4);
    vertices[0].position = float3{-10.0f, 0.0f, +10.0f};
    vertices[1].position = float3{+10.0f, 0.0f, +10.0f};
    vertices[2].position = float3{-10.0f, 0.0f, -10.0f};
    vertices[3].position = float3{+10.0f, 0.0f, -10.0f};

    const auto v_count = static_cast<unsigned>(vertices.size());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    Meshes ret;
    ret.emplace("quad_patch", Mesh{vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildTrianglePatch(Device& device)
{
    using Vertex = Vertex::PositionOnly;

    vector<Vertex> vertices(3);
    vertices[0].position = float3{-10.0f, 0.0f, +10.0f};
    vertices[1].position = float3{+10.0f, 0.0f, +10.0f};
    vertices[2].position = float3{0.0f, 0.0f, -10.0f};

    const auto v_count = static_cast<unsigned>(vertices.size());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    Meshes ret;
    ret.emplace("triangle_patch", Mesh{vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildIcosahedronPatch(Device& device)
{
    using Vertex = Vertex::PositionOnly;

    GeometryGenerator::MeshData icosahedron;
    GeometryGenerator geo_gen;

    geo_gen.CreateGeosphere(1.0f, 0, icosahedron);

    const auto v_count = static_cast<unsigned>(icosahedron.Vertices.size());
    const auto i_count = static_cast<unsigned>(icosahedron.Indices.size());

    vector<Vertex> vs(v_count);

    for (auto i = 0u; i < v_count; ++i)
        vs[i].position = icosahedron.Vertices[i].Position;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vs.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(UINT) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = &icosahedron.Indices[0];
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &index_init_data, &ib));

    Meshes ret;
    ret.emplace("icosahedron_patch", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}

Meshes buildCubicBezierQuadPatch(Device& device)
{
    using Vertex = float3;

    const vector<Vertex> vertices = {// Row 0
                                     float3(-10.0f, -10.0f, +15.0f),
                                     float3(-5.0f, 0.0f, +15.0f),
                                     float3(+5.0f, 0.0f, +15.0f),
                                     float3(+10.0f, 0.0f, +15.0f),

                                     // Row 1
                                     float3(-15.0f, 0.0f, +5.0f),
                                     float3(-5.0f, 0.0f, +5.0f),
                                     float3(+5.0f, 20.0f, +5.0f),
                                     float3(+15.0f, 0.0f, +5.0f),

                                     // Row 2
                                     float3(-15.0f, 0.0f, -5.0f),
                                     float3(-5.0f, 0.0f, -5.0f),
                                     float3(+5.0f, 0.0f, -5.0f),
                                     float3(+15.0f, 0.0f, -5.0f),

                                     // Row 3
                                     float3(-10.0f, 10.0f, -15.0f),
                                     float3(-5.0f, 0.0f, -15.0f),
                                     float3(+5.0f, 0.0f, -15.0f),
                                     float3(+25.0f, 10.0f, -15.0f)};

    const auto v_count = static_cast<unsigned>(vertices.size());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(float3) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    Meshes ret;
    ret.emplace("cubic_bezier_quad_patch", Mesh{vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildCubicBezierTrianglePatch(Device& device)
{
    using Vertex = float3;

    const vector<Vertex> vertices = {
        float3(-15.0f, +5.0f, +15.f), // top leftmost
        float3(-5.0f, 0.0f, +15.f),   // top left
        float3(+5.0f, 0.0f, +15.f),   // top right
        float3(+15.0f, +5.0f, +15.f), // top rightmost / bottom rightmost
        float3(+5.0f, -5.0f, +5.f),   // bottom right
        float3(-5.0f, +5.0f, -5.f),   // bottom left
        float3(-15.0f, -5.0f, -15.f), // bottom leftmost / left bottommost
        float3(-15.0f, 0.0f, -5.f),   // left bottom
        float3(-15.0f, 0.0f, +5.f),   // left top
        float3(0.0f, -5.0f, 0.f)      // center
    };

    const auto v_count = static_cast<unsigned>(vertices.size());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(float3) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    Meshes ret;
    ret.emplace("cubic_bezier_triangle_patch", Mesh{vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildQuadraticBezierQuadPatch(Device& device)
{
    using Vertex = float3;

    const vector<Vertex> vertices = {// Row 0
                                     float3(-10.0f, -5.0f, 10.0f),
                                     float3(0.0f, 0.0f, 10.0f),
                                     float3(+10.0f, +5.0f, 10.0f),

                                     // Row 1
                                     float3(-10.0f, 0.0f, 0.0f),
                                     float3(0.0f, +5.0f, 0.0f),
                                     float3(+10.0f, +5.0f, 0.0f),

                                     // Row 2
                                     float3(-10.0f, 0.0f, -10.0f),
                                     float3(0.0f, -5.0f, -10.0f),
                                     float3(+10.0f, -5.0f, -10.0f)};

    const auto v_count = static_cast<unsigned>(vertices.size());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(float3) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    Meshes ret;
    ret.emplace("quadratic_bezier_quad_patch", Mesh{vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildQuadraticBezierTrianglePatch(Device& device)
{
    using Vertex = float3;

    const vector<Vertex> vertices = {
        float3(-10.0f, +5.0f, +10.f), // top left
        float3(0.0f, +5.0f, +10.f),   // top
        float3(+10.0f, 0.0f, +10.f),  // top right
        float3(0.0f, -5.0f, 0.f),     // center
        float3(-10.0f, 0.0f, -10.f),  // bottom left
        float3(-10.0f, +9.0f, 0.f)    // left
    };

    const auto v_count = static_cast<unsigned>(vertices.size());

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(float3) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &vertex_init_data, &vb));

    Meshes ret;
    ret.emplace("quadratic_bezier_triangle_patch", Mesh{vb, v_count, sizeof(Vertex)});
    return ret;
}

Meshes buildSky(Device& device, float sky_sphere_radius)
{
    using Vertex = float3;

    GeometryGenerator::MeshData sphere;
    GeometryGenerator geo_gen;
    geo_gen.CreateSphere(sky_sphere_radius, 30, 30, sphere);

    const auto v_count = static_cast<unsigned>(sphere.Vertices.size());
    const auto i_count = static_cast<unsigned>(sphere.Indices.size());

    vector<Vertex> vertices(v_count);

    for (size_t i = 0; i < v_count; ++i)
        vertices[i] = sphere.Vertices[i].Position;

    D3D11_BUFFER_DESC vbd;
    vbd.Usage = D3D11_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * v_count;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;
    D3D11_SUBRESOURCE_DATA v_init_data;
    v_init_data.pSysMem = vertices.data();
    D3DBuffer vb;
    HR(device.CreateBuffer(&vbd, &v_init_data, &vb));

    D3D11_BUFFER_DESC ibd;
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned) * i_count;
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.StructureByteStride = 0;
    ibd.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA i_init_data;
    i_init_data.pSysMem = sphere.Indices.data();
    D3DBuffer ib;
    HR(device.CreateBuffer(&ibd, &i_init_data, &ib));

    Meshes ret;
    ret.emplace("sky", Mesh{vb, v_count, sizeof(Vertex), ib, i_count});
    return ret;
}
