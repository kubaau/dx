#include "Mesh.hpp"
#include <fstream>
#include <vector>
#include "GraphicsDebugFlags.hpp"
#include "GraphicsDebugUtils.hpp"
#include <tools/Print.hpp>
#include "Vertex.hpp"

Mesh::Mesh(Device& device, Filename filename)
{
    if (debug_mesh_load)
        print("Loading", filename);

    using namespace std;
    using Vertex = Vertex::Basic;

    constexpr auto base_filename = "../../../assets/models/";
    ifstream file_in{base_filename + filename};
    assert(file_in && "Model not found");

    stride = sizeof(Vertex);

    unsigned triangle_count = 0;
    string ignored;
    file_in >> ignored >> v_count;
    file_in >> ignored >> triangle_count;
    i_count = 3 * triangle_count;

    vector<Vertex> vs(v_count);
    vector<unsigned> indices(i_count);

    file_in >> ignored >> ignored >> ignored >> ignored;

    for (auto i = 0u; i < v_count; ++i)
    {
        file_in >> vs[i].position.x >> vs[i].position.y >> vs[i].position.z;
        file_in >> vs[i].normal.x >> vs[i].normal.y >> vs[i].normal.z;
    }

    file_in >> ignored;
    file_in >> ignored;
    file_in >> ignored;

    for (auto i = 0u; i < triangle_count; ++i)
        file_in >> indices[i * 3 + 0] >> indices[i * 3 + 1] >> indices[i * 3 + 2];

    file_in.close();

    D3D11_BUFFER_DESC vertex_buffer_desc;
    vertex_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
    vertex_buffer_desc.ByteWidth = sizeof(Vertex) * v_count;
    vertex_buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vertex_buffer_desc.CPUAccessFlags = 0;
    vertex_buffer_desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA vertex_init_data;
    vertex_init_data.pSysMem = &vs[0];
    HR(device.CreateBuffer(&vertex_buffer_desc, &vertex_init_data, &vb));

    D3D11_BUFFER_DESC index_buffer_desc;
    index_buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
    index_buffer_desc.ByteWidth = sizeof(unsigned) * i_count;
    index_buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
    index_buffer_desc.CPUAccessFlags = 0;
    index_buffer_desc.MiscFlags = 0;
    D3D11_SUBRESOURCE_DATA index_init_data;
    index_init_data.pSysMem = &indices[0];
    HR(device.CreateBuffer(&index_buffer_desc, &index_init_data, &ib));

    i_format = DXGI_FORMAT_R32_UINT;
}
