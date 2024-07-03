#include "WavesGPU.hpp"
#include "AppEngine.hpp"
#include "Effects.hpp"
#include "GraphicsDebugUtils.hpp"

void WavesGPU::init(Device& device,
                    Width width,
                    Height height,
                    double spatial_step,
                    double ts,
                    double speed,
                    double damping,
                    DXGI_FORMAT format,
                    Effect& effect)
{
    width_ = width;
    height_ = height;
    time_step = ts;

    effect.scalar("displacement_map_texel_size", 1.0f / width);
    effect.scalar("spatial_step", static_cast<float>(spatial_step));

    const auto c = damping * time_step;
    const auto d = c + 2;
    const auto e = pow(speed, 2) * pow(time_step, 2) / pow(spatial_step, 2);
    waves_effect = &effects.at("waves");
    waves_effect->scalar("k1", static_cast<float>((c - 2) / d));
    waves_effect->scalar("k2", static_cast<float>((4 - 8 * e) / d));
    waves_effect->scalar("k3", static_cast<float>(2 * e / d));

    D3D11_TEXTURE2D_DESC desc;
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = format;
    desc.SampleDesc.Count = 1;
    desc.SampleDesc.Quality = 0;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    desc.CPUAccessFlags = 0;
    desc.MiscFlags = 0;

    Texture2DPtr tex_prev;
    Texture2DPtr tex_curr;
    Texture2DPtr tex_next;
    HR(device.CreateTexture2D(&desc, 0, &tex_prev));
    HR(device.CreateTexture2D(&desc, 0, &tex_curr));
    HR(device.CreateTexture2D(&desc, 0, &tex_next));

    D3D11_SHADER_RESOURCE_VIEW_DESC srv_desc;
    srv_desc.Format = format;
    srv_desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srv_desc.Texture2D.MostDetailedMip = 0;
    srv_desc.Texture2D.MipLevels = 1;
    HR(device.CreateShaderResourceView(tex_prev.Get(), &srv_desc, &prev_srv));
    HR(device.CreateShaderResourceView(tex_curr.Get(), &srv_desc, &curr_srv));
    HR(device.CreateShaderResourceView(tex_next.Get(), &srv_desc, &next_srv));

    D3D11_UNORDERED_ACCESS_VIEW_DESC uav_desc;
    uav_desc.Format = format;
    uav_desc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
    uav_desc.Texture2D.MipSlice = 0;
    HR(device.CreateUnorderedAccessView(tex_prev.Get(), &uav_desc, &prev_uav));
    HR(device.CreateUnorderedAccessView(tex_curr.Get(), &uav_desc, &curr_uav));
    HR(device.CreateUnorderedAccessView(tex_next.Get(), &uav_desc, &next_uav));
}

void WavesGPU::disturb(Context& context, unsigned m, unsigned n, float magnitude)
{
    D3DX11_TECHNIQUE_DESC tech_desc;
    auto tech = waves_effect->technique("waves_disturb");
    tech->GetDesc(&tech_desc);
    for (auto i = 0u; i < tech_desc.Passes; ++i)
    {
        waves_effect->uav("curr_uav", *curr_uav);
        waves_effect->scalar("disturbance_magnitude", magnitude);
        waves_effect->scalar("x", m);
        waves_effect->scalar("y", n);
        tech->GetPassByIndex(i)->Apply(0, &context);
        context.Dispatch(1, 1, 1);
    }

    // Unbind output from compute shader (we are going to use this output as an input in the next pass, and a resource
    // cannot be both an output and input at the same time.
    UAVPtr null_uav;
    context.CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);
}

void WavesGPU::update(Context& context)
{
    elapsed += APP.delta();

    if (elapsed < time_step)
        return;

    elapsed = 0.0f;

    D3DX11_TECHNIQUE_DESC tech_desc;
    auto tech = waves_effect->technique("waves_update");
    tech->GetDesc(&tech_desc);
    for (auto i = 0u; i < tech_desc.Passes; ++i)
    {
        waves_effect->srv("prev_srv", *prev_srv);
        waves_effect->srv("curr_srv", *curr_srv);
        waves_effect->uav("next_uav", *next_uav);
        tech->GetPassByIndex(i)->Apply(0, &context);
        context.Dispatch(width_ / 16, height_ / 16, 1);
    }

    // Unbind the input texture from the CS for good housekeeping.
    SRVPtr null_srv;
    context.CSSetShaderResources(0, 1, &null_srv);

    // Unbind output from compute shader (we are going to use this output as an input in the next pass, and a resource
    // cannot be both an output and input at the same time.
    UAVPtr null_uav;
    context.CSSetUnorderedAccessViews(0, 1, &null_uav, nullptr);

    // Disable compute shader.
    context.CSSetShader(0, 0, 0);

    // Ping-pong buffers in preparation for the next update.
    // The previous solution is no longer needed and becomes the target of the next solution in the next update.
    // The current solution becomes the previous solution.
    // The next solution becomes the current solution.

    const auto srv_temp = prev_srv;
    prev_srv = curr_srv;
    curr_srv = next_srv;
    next_srv = srv_temp;

    const auto uav_temp = prev_uav;
    prev_uav = curr_uav;
    curr_uav = next_uav;
    next_uav = uav_temp;
}
