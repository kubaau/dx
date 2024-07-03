#pragma once

#include "DXTypedefs.hpp"
#include "GraphicsTypedefs.hpp"
#include <tools/Typedefs.hpp>

class Effect;
struct ID3DX11EffectTechnique;

constexpr auto rs_default = nullptr;
constexpr auto dss_default = nullptr;
constexpr auto bs_default = nullptr;
constexpr auto no_stencil = 0;

enum class RenderOption : u8
{
    none,
    no_tex,
    fog,
    depth_test_blend
};

class Renderable
{
    friend class D3DEngine;

public:
    Renderable(Context& ctx) : context(ctx) {}

    void drawSolid(const Matrix& vp, unsigned stencil = no_stencil, bool force_draw = false);
    void drawPlanarShadow(const Matrix& vp, const float3& main_light_pos);
    void drawReflecting(const Matrix& vp);
    void drawReflectedSolid(const Matrix& vp, const DirectionalLights&);
    void drawReflectedPlanarShadow(const Matrix& vp, const DirectionalLights&, const float3& main_light_pos);
    void drawTransparent(const Matrix& vp);
    void drawReflectedTransparent(const Matrix& vp, const DirectionalLights&);

    void updateEye(const float3& eye_pos);
    void renderOption(RenderOption);

    void overrideRS(RState* state) { rs_override = state; }
    void overrideDSS(DSState* state) { dss_override = state; }
    void overrideBS(BState* state) { bs_override = state; }

    void translate(const float3& v);

private:
    void draw(const Matrix& vp,
              RState*,
              DSState*,
              BState*,
              const float3* main_light_pos,
              unsigned stencil,
              const Matrix* reflection_matrix,
              const DirectionalLights* dir_lights);

    void updateTechnique();
    void updateWorld();

    bool transparent();
    bool additive();

    Context& context;

    ID id;

    Mesh* mesh;
    unsigned mesh_offset;
    D3D_PRIMITIVE_TOPOLOGY topology;

    SRV* diffuse{};
    SRV* diffuse_override{};
    SRV* normal{};
    SRV* normal_sec{};
    SRV* displacement{};
    SRV* cube_map{};
    SRV* tex_array{};
    float4x4 diffuse_transform;
    float4x4 normal_transform;
    float4x4 normal_sec_transform;
    float4x4 height_transform;
    float4x4 height_sec_transform;

    Material* mat{};
    Material* mat_shadow{};
    bool reflecting;
    bool reflectable;
    bool enabled;

    Effect* effect;
    ID3DX11EffectTechnique* tech;
    Map<ID3DX11EffectTechnique*> techs;
    float nearr;
    float farr;
    float distance_to_eye;
    RenderOption render_option;

    InputLayout* il;
    RState* rs_primary = rs_default;
    RState* rs_override = rs_primary;
    DSState* dss_primary = dss_default;
    DSState* dss_override = dss_primary;
    BState* bs_primary = bs_default;
    BState* bs_override = bs_primary;

    Rotation r;
    Scale s;
    Translation t;
    float4x4 world;
};
