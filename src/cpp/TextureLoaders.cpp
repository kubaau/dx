#include "TextureLoaders.hpp"

#include <codecvt>
#include <ext/DDSTextureLoader.h>
#include <ext/WICTextureLoader.h>
#include <locale>
#include <string>
#include <tools/RandomContainers.hpp>

#include "GraphicsDebugUtils.hpp"

using namespace DirectX;

constexpr auto tex_max_size = 0u;

static auto toWString(string str)
{
    using namespace std;
    wstring_convert<codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}

SRVPtr loadDDSTexture(Device& device, Context& context, FilePath path)
{
    DXResourcePtr rsrc;
    SRVPtr srv;
    HR(CreateDDSTextureFromFile(
        &device, &context, path.c_str(), rsrc.GetAddressOf(), srv.GetAddressOf(), tex_max_size));
    return srv;
}

SRVPtr loadBMPTexture(Device& device, Context& context, FilePath path)
{
    DXResourcePtr tex;
    SRVPtr srv;
    HR(CreateWICTextureFromFile(&device, &context, path.c_str(), tex.GetAddressOf(), srv.GetAddressOf(), tex_max_size));
    return srv;
}

SRVPtr create2DTextureArray(Device& device, Context& context, const FilePaths& paths)
{
    // Load the texture elements individually from file. These textures won't be used by the GPU (0 bind flags), they
    // are just used to load the image data from file. We use the STAGING usage so the CPU can read the resource.

    using namespace std;

    const auto size = paths.size();

    vector<DXResourcePtr> rsrc(size);
    for (auto i = 0u; i < size; ++i)
    {
        constexpr auto bind_flags = 0;
        constexpr auto cpu_flags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
        constexpr auto misc_flags = 0;
        constexpr auto null_srv = nullptr;
        HR(CreateDDSTextureFromFileEx(&device,
                                      &context,
                                      paths[i].c_str(),
                                      tex_max_size,
                                      D3D11_USAGE_STAGING,
                                      bind_flags,
                                      cpu_flags,
                                      misc_flags,
                                      DDS_LOADER_DEFAULT,
                                      rsrc[i].GetAddressOf(),
                                      null_srv));
    }

    // Create the texture array. Each element in the texture array has the same format/dimensions.
    D3D11_TEXTURE2D_DESC element_desc;
    Texture2DPtr tex2d;
    rsrc[0].As<Texture2D>(&tex2d);
    tex2d->GetDesc(&element_desc);

    D3D11_TEXTURE2D_DESC array_desc;
    array_desc.Width = element_desc.Width;
    array_desc.Height = element_desc.Height;
    array_desc.MipLevels = element_desc.MipLevels;
    array_desc.ArraySize = static_cast<unsigned>(size);
    array_desc.Format = element_desc.Format;
    array_desc.SampleDesc.Count = 1;
    array_desc.SampleDesc.Quality = 0;
    array_desc.Usage = D3D11_USAGE_DEFAULT;
    array_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    array_desc.CPUAccessFlags = 0;
    array_desc.MiscFlags = 0;

    Texture2DPtr tex_array;
    HR(device.CreateTexture2D(&array_desc, 0, &tex_array));

    // Copy individual texture elements into texture array.
    for (auto element = 0u; element < size; ++element)
    {
        for (auto mip_level = 0u; mip_level < element_desc.MipLevels; ++mip_level)
        {
            D3D11_MAPPED_SUBRESOURCE mapped_tex_2d{};
            HR(context.Map(rsrc[element].Get(), mip_level, D3D11_MAP_READ, 0, &mapped_tex_2d));

            context.UpdateSubresource(tex_array.Get(),
                                      D3D11CalcSubresource(mip_level, element, element_desc.MipLevels),
                                      0,
                                      mapped_tex_2d.pData,
                                      mapped_tex_2d.RowPitch,
                                      mapped_tex_2d.DepthPitch);

            context.Unmap(rsrc[element].Get(), mip_level);
        }
    }

    // Create a resource view to the texture array.
    D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;
    view_desc.Format = array_desc.Format;
    view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
    view_desc.Texture2DArray.MostDetailedMip = 0;
    view_desc.Texture2DArray.MipLevels = array_desc.MipLevels;
    view_desc.Texture2DArray.FirstArraySlice = 0;
    view_desc.Texture2DArray.ArraySize = static_cast<unsigned>(size);

    SRVPtr tex;
    HR(device.CreateShaderResourceView(tex_array.Get(), &view_desc, &tex));
    return tex;
}

SRVPtr createRandomTexture(Device& device)
{
    constexpr auto size = 1024;

    vector<float4> vals(size);
    const auto random_x = randomFloats(-1.0f, 1.0f, size);
    const auto random_y = randomFloats(-1.0f, 1.0f, size);
    const auto random_z = randomFloats(-1.0f, 1.0f, size);
    const auto random_w = randomFloats(-1.0f, 1.0f, size);
    for (int i = 0; i < size; ++i)
    {
        vals[i].x = random_x[i];
        vals[i].y = random_y[i];
        vals[i].z = random_z[i];
        vals[i].w = random_w[i];
    }

    D3D11_SUBRESOURCE_DATA init_data;
    init_data.pSysMem = vals.data();
    init_data.SysMemPitch = sizeof(float4) * size;
    init_data.SysMemSlicePitch = 0;

    D3D11_TEXTURE1D_DESC tex_desc;
    tex_desc.Width = size;
    tex_desc.MipLevels = 1;
    tex_desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    tex_desc.Usage = D3D11_USAGE_IMMUTABLE;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = 0;
    tex_desc.ArraySize = 1;

    Texture1DPtr tex;
    HR(device.CreateTexture1D(&tex_desc, &init_data, &tex));

    D3D11_SHADER_RESOURCE_VIEW_DESC view_desc;
    view_desc.Format = tex_desc.Format;
    view_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE1D;
    view_desc.Texture1D.MipLevels = tex_desc.MipLevels;
    view_desc.Texture1D.MostDetailedMip = 0;

    SRVPtr srv;
    HR(device.CreateShaderResourceView(tex.Get(), &view_desc, &srv));
    return srv;
}
