#include "ComputeShaderVecAdd.hpp"
#include "Effects.hpp"
#include "GraphicsDebugFlags.hpp"
#include "GraphicsDebugUtils.hpp"

void ComputeShaderVecAdd::run(Device& device, Context& context)
{
    if (already_ran)
        return;

    init(device);
    compute(context);
    already_ran = true;
}

void ComputeShaderVecAdd::init(Device& device)
{
    element_count = 32;

    vector<DataVecAdd> data1(element_count);
    vector<DataVecAdd> data2(element_count);
    for (auto i = 0u; i < element_count; ++i)
    {
        const auto j = static_cast<float>(i);
        data1[i].vector1 = float3(j, j, j);
        data1[i].vector2 = float2(j, 0);
        data2[i].vector1 = float3(-j, j, 0.0f);
        data2[i].vector2 = float2(0, -j);
    }

    // Create a buffer to be bound as a shader input (D3D11_BIND_SHADER_RESOURCE).
    D3D11_BUFFER_DESC input_desc;
    input_desc.Usage = D3D11_USAGE_DEFAULT;
    input_desc.ByteWidth = sizeof(DataVecAdd) * element_count;
    input_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    input_desc.CPUAccessFlags = 0;
    input_desc.StructureByteStride = sizeof(DataVecAdd);
    input_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA vertex_init_data1;
    vertex_init_data1.pSysMem = data1.data();
    D3DBuffer vb1;
    HR(device.CreateBuffer(&input_desc, &vertex_init_data1, &vb1));

    D3D11_SUBRESOURCE_DATA vertex_init_data2;
    vertex_init_data2.pSysMem = data2.data();
    D3DBuffer vb2;
    HR(device.CreateBuffer(&input_desc, &vertex_init_data2, &vb2));

    // Create a read-write buffer the compute shader can write to (D3D11_BIND_UNORDERED_ACCESS).
    D3D11_BUFFER_DESC output_desc;
    output_desc.Usage = D3D11_USAGE_DEFAULT;
    output_desc.ByteWidth = sizeof(DataVecAdd) * element_count;
    output_desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
    output_desc.CPUAccessFlags = 0;
    output_desc.StructureByteStride = sizeof(DataVecAdd);
    output_desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    HR(device.CreateBuffer(&output_desc, 0, &output_vec_add));

    // Create a system memory version of the buffer to read the results back from.
    output_desc.Usage = D3D11_USAGE_STAGING;
    output_desc.BindFlags = 0;
    output_desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    HR(device.CreateBuffer(&output_desc, 0, &output_vec_add_debug));

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = DXGI_FORMAT_UNKNOWN;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
    srv_desc.BufferEx.FirstElement = 0;
    srv_desc.BufferEx.Flags = 0;
    srv_desc.BufferEx.NumElements = element_count;

    device.CreateShaderResourceView(vb1.Get(), &srv_desc, &input_vec_add1_srv);
    device.CreateShaderResourceView(vb2.Get(), &srv_desc, &input_vec_add2_srv);

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    uav_desc.Format = DXGI_FORMAT_UNKNOWN;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
    uav_desc.Buffer.FirstElement = 0;
    uav_desc.Buffer.Flags = 0;
    uav_desc.Buffer.NumElements = element_count;

    device.CreateUnorderedAccessView(output_vec_add.Get(), &uav_desc, &output_vec_add_uav);
}

void ComputeShaderVecAdd::compute(Context& context)
{
    auto& effect = effects.at("compute_math");
    effect.srv("input_vec_add1", *input_vec_add1_srv);
    effect.srv("input_vec_add2", *input_vec_add2_srv);
    effect.uav("output_vec_add", *output_vec_add_uav);

    D3DX11_TECHNIQUE_DESC tech_desc;
    auto tech = effect.technique("vec_add");
    tech->GetDesc(&tech_desc);
    for (auto i = 0u; i < tech_desc.Passes; ++i)
    {
        constexpr auto pass_flags = 0;
        tech->GetPassByIndex(i)->Apply(pass_flags, &context);
        context.Dispatch(1, 1, 1);
    }

    // Unbind the input textures from the CS for good housekeeping.
    SRVPtr null_srv;
    context.CSSetShaderResources(0, 1, &null_srv);

    // Unbind output from compute shader (we are going to use this output as an input in the next pass, and a resource
    // cannot be both an output and input at the same time.
    UAVPtr null_uav;
    context.CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);

    // Disable compute shader.
    context.CSSetShader(0, 0, 0);

    // Copy the output buffer to system memory.
    context.CopyResource(output_vec_add_debug.Get(), output_vec_add.Get());

    // Map the data for reading.
    D3D11_MAPPED_SUBRESOURCE mapped_data{};
    constexpr auto subresource = 0;
    constexpr auto map_flags = 0;
    context.Map(output_vec_add_debug.Get(), subresource, D3D11_MAP_READ, map_flags, &mapped_data);

    const auto data_view = reinterpret_cast<DataVecAdd*>(mapped_data.pData);

    if (debug_compute)
        for (auto i = 0u; i < element_count; ++i)
            print(data_view[i].vector1, data_view[i].vector2);

    context.Unmap(output_vec_add_debug.Get(), subresource);
}
