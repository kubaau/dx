#include "Renderable.hpp"
#include "AppEngine.hpp"
#include "BlendStates.hpp"
#include <tools/Contains.hpp>
#include "DepthStencilStates.hpp"
#include "Effect.hpp"
#include "GraphicsDebugFlags.hpp"
#include "GraphicsDebugUtils.hpp"
#include "MathUtils.hpp"
#include "Mesh.hpp"
#include "RasterizerStates.hpp"

constexpr float blend_factor[] = {0.0f, 0.0f, 0.0f, 0.0f};
constexpr auto blend_mask = 0xFFFFFFFF;
constexpr auto no_planar_shadow = nullptr;
constexpr auto stencil_1 = 1;
constexpr auto no_reflection = nullptr;
constexpr auto no_dir_lights = nullptr;
using namespace DirectX;
const auto xy_plane_reflection_matrix = XMMatrixReflect(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));

void Renderable::drawSolid(const Matrix& vp, unsigned stencil, bool force_draw)
{
    if (not force_draw and (not enabled or transparent() or reflecting))
        return;

    if (debug_drawing)
        print("Drawing solid", id);
    draw(vp, rs_primary, dss_primary, bs_primary, no_planar_shadow, stencil, no_reflection, no_dir_lights);
}

void Renderable::drawPlanarShadow(const Matrix& vp, const float3& main_light_pos)
{
    if (not enabled or transparent() or not mat_shadow)
        return;

    if (debug_drawing)
        print("Drawing planar shadow for", id);
    draw(vp,
         rs_primary,
         dss["no_double_blend"].Get(),
         bs["transparent"].Get(),
         &main_light_pos,
         no_stencil,
         no_reflection,
         no_dir_lights);
}

void Renderable::drawReflecting(const Matrix& vp)
{
    if (not enabled or not reflecting)
        return;

    if (debug_drawing)
        print("Drawing reflecting", id);
    draw(vp,
         rs_primary,
         dss["mark_stencil"].Get(),
         bs["no_render_target_write"].Get(),
         no_planar_shadow,
         stencil_1,
         no_reflection,
         no_dir_lights);
}

void Renderable::drawReflectedSolid(const Matrix& vp, const DirectionalLights& dir_lights)
{
    if (not enabled or transparent() or reflecting or not reflectable)
        return;

    if (debug_drawing)
        print("Drawing reflected solid", id);
    draw(vp,
         rs_primary == rs["wireframe"].Get() ? rs["wireframe_cull_clockwise"].Get() : rs["cull_clockwise"].Get(),
         dss["draw_reflection"].Get(),
         bs_primary,
         no_planar_shadow,
         stencil_1,
         &xy_plane_reflection_matrix,
         &dir_lights);
}

void Renderable::drawReflectedPlanarShadow(const Matrix& vp,
                                           const DirectionalLights& dir_lights,
                                           const float3& main_light_pos)
{
    if (not enabled or transparent() or reflecting or not reflectable or not mat_shadow)
        return;

    if (debug_drawing)
        print("Drawing reflected planar shadow for", id);
    draw(vp,
         rs["cull_clockwise"].Get(),
         dss["no_double_blend"].Get(),
         bs["transparent"].Get(),
         &main_light_pos,
         stencil_1,
         &xy_plane_reflection_matrix,
         &dir_lights);
}

void Renderable::drawTransparent(const Matrix& vp)
{
    if (not enabled or not transparent())
        return;

    if (debug_drawing)
        print("Drawing transparent", id);
    draw(vp,
         rs_primary,
         dss_primary,
         additive() ? bs_primary : bs["transparent"].Get(),
         no_planar_shadow,
         no_stencil,
         no_reflection,
         no_dir_lights);
}

void Renderable::drawReflectedTransparent(const Matrix& vp, const DirectionalLights& dir_lights)
{
    if (not enabled or not transparent() or reflecting or not reflectable)
        return;

    if (debug_drawing)
        print("Drawing reflected transparent", id);
    draw(vp,
         rs["cull_clockwise"].Get(),
         dss["draw_reflection"].Get(),
         additive() ? bs_primary : bs["transparent"].Get(),
         no_planar_shadow,
         stencil_1,
         &xy_plane_reflection_matrix,
         &dir_lights);
}

void Renderable::draw(const Matrix& vp,
                      RState* rstate,
                      DSState* dsstate,
                      BState* bstate,
                      const float3* main_light_pos,
                      unsigned stencil,
                      const Matrix* reflection_matrix,
                      const DirectionalLights* dir_lights)
{
    const auto planar_shadow = main_light_pos != nullptr;

    context.IASetInputLayout(il);
    context.IASetPrimitiveTopology(topology);
    constexpr auto vb_start_slot = 0;
    constexpr auto vb_count = 1;
    constexpr auto i_offset = 0;
    context.IASetVertexBuffers(vb_start_slot, vb_count, mesh->vb.GetAddressOf(), &mesh->stride, &mesh_offset);
    if (mesh->ib)
        context.IASetIndexBuffer(mesh->ib.Get(), mesh->i_format, i_offset);

    context.RSSetState(debug_wireframe ? ::rs["wireframe"].Get() : rs_override ? rs_override : rstate);

    context.OMSetDepthStencilState(dss_override ? dss_override : dsstate, stencil);
    context.OMSetBlendState(bs_override ? bs_override : bstate, blend_factor, blend_mask);

    auto w = XMLoadFloat4x4(&world);
    if (planar_shadow)
    {
        const auto shadow_plane = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); // xz plane
        const auto to_main_light = -XMLoadFloat3(main_light_pos);
        const auto shadow_offset_y = XMMatrixTranslation(0.0f, 0.001f, 0.0f);
        const auto shadow = XMMatrixShadow(shadow_plane, to_main_light) * shadow_offset_y;
        w *= shadow;
    }
    if (reflection_matrix)
        w *= *reflection_matrix;
    const auto wit = inverseTranspose(w);
    const auto wvp = w * vp;

    effect->matrix("world", w);
    effect->matrix("wit", wit);
    effect->matrix("wvp", wvp);
    effect->matrix("vp", vp);
    if (planar_shadow)
        effect->variable("mat", *mat_shadow);
    else if (mat)
        effect->variable("mat", *mat);
    if (diffuse or diffuse_override)
    {
        effect->srv("diffuse_map", diffuse_override ? *diffuse_override : *diffuse);
        effect->matrix("diffuse_transform", XMLoadFloat4x4(&diffuse_transform));
    }
    if (displacement)
    {
        effect->srv("displacement_map", *displacement);
    }
    if (tex_array)
    {
        effect->srv("tex_array", *tex_array);
    }
    if (cube_map)
    {
        effect->srv("cube_map", *cube_map);
    }
    effect->scalar("elapsed", static_cast<float>(APP.total()));

    if (reflection_matrix)
    {
        // reflect the light directions
        auto reflected_lights = *dir_lights;
        for (auto& light : reflected_lights)
        {
            const auto light_dir = XMLoadFloat3(&light.direction);
            const auto reflected_light_dir = XMVector3TransformNormal(light_dir, *reflection_matrix);
            XMStoreFloat3(&light.direction, reflected_light_dir);
        }
        effect->variable("dir_lights", reflected_lights.front(), static_cast<unsigned>(reflected_lights.size()));
    }

    constexpr auto pass_flags = 0;
    constexpr auto start_index = 0;
    constexpr auto base_vertex = 0;
    constexpr auto start_vertex = 0;

    D3DX11_TECHNIQUE_DESC tech_desc;
    tech->GetDesc(&tech_desc);
    for (auto i = 0u; i < tech_desc.Passes; ++i)
    {
        tech->GetPassByIndex(i)->Apply(pass_flags, &context);
        if (mesh->ib)
            context.DrawIndexed(mesh->i_count, start_index, base_vertex);
        else
            context.Draw(mesh->v_count, start_vertex);
    }

    if (contains(techs, "tech_face_normals") and mesh->ib)
    {
        const auto tech_fn = techs["tech_face_normals"];
        tech_fn->GetDesc(&tech_desc);
        for (auto i = 0u; i < tech_desc.Passes; ++i)
        {
            tech_fn->GetPassByIndex(i)->Apply(pass_flags, &context);
            context.DrawIndexed(mesh->i_count, start_index, base_vertex);
        }
    }
    if (contains(techs, "tech_vertex_normals"))
    {
        const auto tech_vn = techs["tech_vertex_normals"];
        context.IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        tech_vn->GetDesc(&tech_desc);
        for (auto i = 0u; i < tech_desc.Passes; ++i)
        {
            tech_vn->GetPassByIndex(i)->Apply(pass_flags, &context);
            context.Draw(mesh->v_count, start_vertex);
        }
        context.IASetPrimitiveTopology(topology);
    }

    if (dir_lights)
        // restore light directions
        effect->variable("dir_lights", dir_lights->front(), static_cast<unsigned>(dir_lights->size()));

    if (displacement)
    {
        // Unbind displacement map from vertex shader, as we will be binding it as a compute shader resource in the next
        // update.
        SRVPtr null_srv;
        context.VSSetShaderResources(0, 1, &null_srv);
    }

    // FX sets tessellation stages, but it does not disable them. So do that here to turn off tessellation.
    context.HSSetShader(0, 0, 0);
    context.DSSetShader(0, 0, 0);

    // context.RSSetState(rs_default);

    // context.OMSetDepthStencilState(dss_default, stencil);
    // context.OMSetBlendState(bs_default, blend_factor, blend_mask);
}

auto distanceV3(const float3& v1, const float3& v2)
{
    return XMVectorGetX(XMVector3Length(XMLoadFloat3(&v1) - XMLoadFloat3(&v2)));
}

void Renderable::updateEye(const float3& eye_pos)
{
    distance_to_eye = distanceV3(eye_pos, t);
    updateTechnique();
}

void Renderable::renderOption(RenderOption ro)
{
    render_option = ro;
    updateTechnique();
}

void Renderable::translate(const float3& v)
{
    t.x += v.x;
    t.y += v.y;
    t.z += v.z;
    updateWorld();
}

void Renderable::updateTechnique()
{
    Name tech_prefix;
    switch (render_option)
    {
        case RenderOption::no_tex: tech_prefix = "no_tex_"; break;
        case RenderOption::fog: tech_prefix = "fog_"; break;
        case RenderOption::depth_test_blend: tech = effect->technique("blend_depth_visualizer"); return;
        default: break;
    }

    Name tech_suffix = "mid";
    if (distance_to_eye <= nearr)
        tech_suffix = "near";
    else if (distance_to_eye >= farr)
        tech_suffix = "far";

    tech = techs[tech_prefix + tech_suffix];
}

void Renderable::updateWorld()
{
    const auto rm = XMMatrixRotationRollPitchYaw(r.x, r.y, r.z);
    const auto sm = XMMatrixScaling(s.x, s.y, s.z);
    const auto tm = XMMatrixTranslation(t.x, t.y, t.z);
    XMStoreFloat4x4(&world, rm * sm * tm);
}

bool Renderable::transparent()
{
    return mat and mat->diffuse.w < 1.0f or additive();
}

bool Renderable::additive()
{
    return bs_primary == bs["additive"].Get();
}
