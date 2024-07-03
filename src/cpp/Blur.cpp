#include "Blur.hpp"
#include "AppEngine.hpp"
#include "Effects.hpp"
#include "GraphicsDebugUtils.hpp"
#include <tools/RangeStlAlgorithms.hpp>

void Blur::init(Device& device, DXGI_FORMAT format)
{
    output_srv.Reset();
    output_uav.Reset();

    // Note, compressed formats cannot be used for UAV. We get error like: ERROR: Device::CreateTexture2D: The
    // format (0x4d, BC3_UNORM) cannot be bound as an UnorderedAccessView, or cast to a format that could be bound as an
    // UnorderedAccessView. Therefore this format does not support D3D11_BIND_UNORDERED_ACCESS.

    D3D11_TEXTURE2D_DESC tex_desc;
    tex_desc.Width = APP.width();
    tex_desc.Height = APP.height();
    tex_desc.MipLevels = 1;
    tex_desc.ArraySize = 1;
    tex_desc.Format = format;
    tex_desc.SampleDesc.Count = 1;
    tex_desc.SampleDesc.Quality = 0;
    tex_desc.Usage = D3D11_USAGE_DEFAULT;
    tex_desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    tex_desc.CPUAccessFlags = 0;
    tex_desc.MiscFlags = 0;

    Texture2DPtr tex;
    HR(device.CreateTexture2D(&tex_desc, 0, &tex));

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;
    HR(device.CreateShaderResourceView(tex.Get(), &srv_desc, &output_srv));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    uav_desc.Format = format;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uav_desc.Texture2D.MipSlice = 0;
    HR(device.CreateUnorderedAccessView(tex.Get(), &uav_desc, &output_uav));
}

void Blur::gaussianWeights(float sigma)
{
    constexpr auto w_count = 11;
    vector<float> weights(w_count);

    constexpr auto w_count_half = w_count / 2;
    const auto sigma_sq = powf(sigma, 2);
    for (int i = 0; i < w_count; ++i)
    {
        const auto x = i - w_count_half;
        weights[i] = expf(-(x * x) / sigma_sq);
    }

    const auto sum = accumulate(weights, 0.0f);
    for (auto& w : weights)
        w /= sum;

    effects.at("blur").scalar("gaussian_weights", weights);
}

void Blur::inPlace(Context& context, SRV& input_srv, UAV& input_uav, unsigned blur_count)
{
    // How many groups do we need to dispatch to cover a row of pixels, where each group covers 256 pixels (the 256 is
    // defined in the ComputeShader).
    const auto groups_x_count = static_cast<unsigned>(ceilf(APP.width() / 256.0f));
    const auto groups_y_count = static_cast<unsigned>(ceilf(APP.height() / 256.0f));
    const auto groups_z_count = 1;

    D3DX11_TECHNIQUE_DESC tech_desc;
    auto& blur_effect = effects.at("blur");
    constexpr auto pass_flags = 0;
    constexpr auto start_slot = 0;
    constexpr auto view_count = 1;
    constexpr auto uav_counts = nullptr;

    SRVPtr null_srv;
    UAVPtr null_uav;

    // Run the compute shader to blur the offscreen texture.
    while (blur_count--)
    {
        // HORIZONTAL blur pass.
        auto tech = blur_effect.technique("horizontal_blur");
        tech->GetDesc(&tech_desc);
        for (auto i = 0u; i < tech_desc.Passes; ++i)
        {
            blur_effect.srv("input", input_srv);
            blur_effect.uav("output", *output_uav);
            tech->GetPassByIndex(i)->Apply(pass_flags, &context);
            context.Dispatch(groups_x_count, APP.height(), groups_z_count);
        }

        // Unbind the input texture from the CS for good housekeeping.
        context.CSSetShaderResources(start_slot, view_count, &null_srv);

        // Unbind output from compute shader (we are going to use this output as an input in the next pass, and a
        // resource cannot be both an output and input at the same time.
        context.CSSetUnorderedAccessViews(start_slot, view_count, &null_uav, uav_counts);

        // VERTICAL blur pass.
        tech = blur_effect.technique("vertical_blur");
        tech->GetDesc(&tech_desc);
        for (auto i = 0u; i < tech_desc.Passes; ++i)
        {
            blur_effect.srv("input", *output_srv);
            blur_effect.uav("output", input_uav);
            tech->GetPassByIndex(i)->Apply(pass_flags, &context);
            context.Dispatch(APP.width(), groups_y_count, groups_z_count);
        }

        context.CSSetShaderResources(start_slot, view_count, &null_srv);
        context.CSSetUnorderedAccessViews(start_slot, view_count, &null_uav, uav_counts);
    }

    // Disable compute shader.
    context.CSSetShader(0, 0, 0);
}
