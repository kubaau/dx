#include <sstream>
#include "AppEngine.hpp"
#include <tools/Contains.hpp>
#include "D3DEngine.hpp"
#include "GraphicsDebugUtils.hpp"
#include <tools/Random.hpp>
#include "Vertex.hpp"

template <typename T>
auto frac(T val)
{
    return val - static_cast<int>(val);
}

void D3DEngine::updateScene()
{
    checkControls();

    // const auto color = static_cast<float>(frac(APP.total()));
    // background_color = {color, color, color, 1.0f};

    // effects.at("blur").scalar("color_sigma", 10 / APP.totalF());
    // effects.at("blur").scalar("color_sigma", 0.5f);
    if (contains(renderables, "blur_quad"))
    {
        if (blur)
            renderables.at("blur_quad").diffuse_override = &blur->output();
        else
            renderables.at("blur_quad").diffuse_override = nullptr;
    }

    if (contains(renderables, "bolt") and contains(textures, "bolt001"))
        animateBolt();

    if (contains(renderables, "waves_gpu") or contains(renderables, "waves_cpu"))
    {
        generateWave();
        animateWaterTexture();
    }

    for (auto& cs : compute_shaders)
        cs->run(*device, *context);

    for (auto& p : particles)
        p.second.update();

    updateEye();
    camera->updateView();
    XMStoreFloat4x4(&view_projection, camera->getVP());
}

void D3DEngine::animateBolt()
{
    using namespace DirectX;

    // animate bolt
    const auto bolt_scale = XMMatrixScaling(3.0f, 1.5f, 1.0f);
    const auto bolt_translation = XMMatrixTranslation(APP.totalF() * 0.02f, 0.0f, 0.0f);
    XMStoreFloat4x4(&renderables.at("bolt").diffuse_transform, bolt_scale * bolt_translation);

    static auto bolt_time = 0.0f;
    static unsigned bolt_tex = 0;
    const auto bolt_interval = 1.0f / 30;
    if (APP.totalF() - bolt_time >= bolt_interval)
    {
        stringstream n;
        n.width(3);
        n.fill('0');
        n << bolt_tex + 1;
        renderables.at("bolt").diffuse = textures.at("bolt" + n.str()).Get();

        bolt_time += bolt_interval;
        bolt_tex = (bolt_tex + 1) % 60;
    }
}

void D3DEngine::generateWave()
{
    static auto elapsed = 0.0;
    static const auto disturbance_interval = 0.01;
    if (APP.total() - elapsed >= disturbance_interval)
    {
        elapsed += disturbance_interval;

        const auto i_cpu = 5 + randomInt(0u, static_cast<unsigned>(waves_cpu->width()) - 10);
        const auto j_cpu = 5 + randomInt(0u, static_cast<unsigned>(waves_cpu->depth()) - 10);
        const auto i_gpu = 5 + randomInt(0u, waves_gpu->width() - 10);
        const auto j_gpu = 5 + randomInt(0u, waves_gpu->height() - 10);
        const auto r = randomFloat(0.25f, 1.0f);

        if (contains(renderables, "waves_cpu"))
            waves_cpu->disturb(i_cpu, j_cpu, r);
        if (contains(renderables, "waves_gpu"))
            waves_gpu->disturb(*context, i_gpu, j_gpu, r);
    }

    if (contains(renderables, "waves_cpu"))
    {
        waves_cpu->update();

        D3D11_MAPPED_SUBRESOURCE mapped_data;
        constexpr auto subresource = 0;
        constexpr auto map_flags = 0;
        auto& waves_cpu_mesh = meshes.at("waves_cpu");
        HR(context->Map(waves_cpu_mesh.vb.Get(), subresource, D3D11_MAP_WRITE_DISCARD, map_flags, &mapped_data));
        auto v = reinterpret_cast<Vertex::Basic*>(mapped_data.pData);
        for (auto i = 0u; i < waves_cpu_mesh.v_count; ++i)
        {
            v[i].position = waves_cpu->current(i);
            v[i].normal = waves_cpu->normal(i);
            // Derive tex-coords in [0,1] from position.
            v[i].tex.x = 0.5f + waves_cpu->current(i).x / waves_cpu->width();
            v[i].tex.y = 0.5f - waves_cpu->current(i).z / waves_cpu->depth();
        }
        context->Unmap(waves_cpu_mesh.vb.Get(), subresource);
    }
    if (contains(renderables, "waves_gpu"))
    {
        waves_gpu->update(*context);
        renderables.at("waves_gpu").displacement = &waves_gpu->displacement();
    }
}

void D3DEngine::animateWaterTexture()
{
    using namespace DirectX;

    static auto tex_offset_x = 0.0f;
    static auto tex_offset_y = 0.0f;
    const auto time_delta = APP.deltaF();
    tex_offset_x += 0.1f * time_delta;
    tex_offset_y += 0.05f * time_delta;
    const auto tex_offset = XMMatrixTranslation(tex_offset_x, tex_offset_y, 0.0f);
    static const auto tex_scale = XMMatrixScaling(5.0f, 5.0f, 0.0f);

    if (contains(renderables, "waves_cpu"))
    {
        auto& waves = renderables.at("waves_cpu");
        XMStoreFloat4x4(&waves.diffuse_transform, tex_scale * tex_offset);
    }
    if (contains(renderables, "waves_gpu"))
    {
        auto& waves = renderables.at("waves_gpu");
        XMStoreFloat4x4(&waves.diffuse_transform, tex_scale * tex_offset);
    }
}

void D3DEngine::resetParticles()
{
    for (auto& p : particles)
        p.second.reset();
}
