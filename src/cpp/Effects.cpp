#include "Effects.hpp"
#include "GraphicsDebugUtils.hpp"

Effects effects;

void initEffects(Device& device)
{
    using namespace FX;
    initBasic(device);
    initTreeSprite(device);
    initCircleToCylinder(device);
    initIcosahedron(device);
    initComputeMath(device);
    initBlur(device);
    initWaves(device);
    initTessellation(device);
    initBasicInstanced(device);
    initSky(device);
    initNormalMap(device);
    initDisplacementMap(device);
    initWater(device);
    initTerrain(device);
    initParticle(device);
    initShadowMap(device);
    initDebugTexture(device);
}

namespace FX
{
    void initBasic(Device& device)
    {
        const auto filename = "Basic.fxo";
        const Names tn = {"light0",
                          "light1",
                          "light2",
                          "light3",
                          "light0_tex",
                          "light1_tex",
                          "light2_tex",
                          "light3_tex",
                          "light0_tex_alphaClip",
                          "light1_tex_alphaClip",
                          "light2_tex_alphaClip",
                          "light3_tex_alphaClip",
                          "light1_fog",
                          "light2_fog",
                          "light3_fog",
                          "light0_tex_fog",
                          "light1_tex_fog",
                          "light2_tex_fog",
                          "light3_tex_fog",
                          "light0_tex_alphaClip_fog",
                          "light1_tex_alphaClip_fog",
                          "light2_tex_alphaClip_fog",
                          "light3_tex_alphaClip_fog",
                          "blend_depth_visualizer",
                          "displacement0",
                          "displacement3_tex",
                          "displacement3_tex_fog",
                          "light3_reflect",
                          "light3_tex_reflect",
                          "light3_refract",
                          "light3_tex_refract"};
        const Names mn = {"wvp", "world", "wit", "diffuse_transform", "shadow_transform"};
        const Names vecn = {"eye_pos", "fog_color"};
        const Names scn = {"fog_start", "fog_range", "displacement_map_texel_size", "spatial_step"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {"diffuse_map", "displacement_map", "cube_map", "shadow_map"};
        const Names uavn = {};

        effects.emplace("basic", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initTreeSprite(Device& device)
    {
        const auto filename = "TreeSprite.fxo";
        const Names tn = {"light3", "light3_tex_alphaClip", "light3_tex_alphaClip_fog"};
        const Names mn = {"vp"};
        const Names vecn = {"eye_pos", "fog_color"};
        const Names scn = {"fog_start", "fog_range"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {"tex_array"};
        const Names uavn = {};

        effects.emplace("tree_sprite", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initCircleToCylinder(Device& device)
    {
        const auto filename = "CircleToCylinder.fxo";
        const Names tn = {"light3"};
        const Names mn = {"wvp", "world", "wit"};
        const Names vecn = {"eye_pos"};
        const Names scn = {};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {};
        const Names uavn = {};

        effects.emplace("circle_to_cylinder", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initIcosahedron(Device& device)
    {
        const auto filename = "Icosahedron.fxo";
        const Names tn = {"icosahedron_subdivision0",
                          "icosahedron_subdivision1",
                          "icosahedron_subdivision2",
                          "icosahedron_explosion",
                          "icosahedron_vertex_normals",
                          "icosahedron_face_normals"};
        const Names mn = {"wvp", "world", "wit"};
        const Names vecn = {"eye_pos"};
        const Names scn = {"elapsed"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {};
        const Names uavn = {};

        effects.emplace("icosahedron", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initComputeMath(Device& device)
    {
        const auto filename = "ComputeMath.fxo";
        const Names tn = {"vec_add", "vec_length"};
        const Names mn = {};
        const Names vecn = {};
        const Names scn = {};
        const Names varn = {};
        const Names srvn = {"input_vec_add1", "input_vec_add2"};
        const Names uavn = {"output_vec_add", "input_vec_length", "output_vec_length"};

        effects.emplace("compute_math", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initBlur(Device& device)
    {
        const auto filename = "Blur.fxo";
        const Names tn = {"horizontal_blur", "vertical_blur"};
        const Names mn = {};
        const Names vecn = {};
        const Names scn = {"gaussian_weights", "color_sigma", "bilateral"};
        const Names varn = {};
        const Names srvn = {"input"};
        const Names uavn = {"output"};

        effects.emplace("blur", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initWaves(Device& device)
    {
        const auto filename = "Waves.fxo";
        const Names tn = {"waves_update", "waves_disturb"};
        const Names mn = {};
        const Names vecn = {};
        const Names scn = {"disturbance_magnitude", "x", "y", "k1", "k2", "k3"};
        const Names varn = {};
        const Names srvn = {"prev_srv", "curr_srv"};
        const Names uavn = {"curr_uav", "next_uav"};

        effects.emplace("waves", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initTessellation(Device& device)
    {
        const auto filename = "Tessellation.fxo";
        const Names tn = {"tessellation_quad",
                          "tessellation_triangle",
                          "tessellation_icosahedron",
                          "tessellation_cubic_bezier_quad",
                          "tessellation_cubic_bezier_triangle",
                          "tessellation_quadratic_bezier_quad",
                          "tessellation_quadratic_bezier_quad_light3",
                          "tessellation_quadratic_bezier_triangle",
                          "tessellation_quadratic_bezier_triangle_light3"};
        const Names mn = {"wvp", "world", "wit"};
        const Names vecn = {"eye_pos"};
        const Names scn = {};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {};
        const Names uavn = {};

        effects.emplace("tessellation", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initBasicInstanced(Device& device)
    {
        const auto filename = "BasicInstanced.fxo";
        const Names tn = {"light3"};
        const Names mn = {"vp"};
        const Names vecn = {"eye_pos"};
        const Names scn = {};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {};
        const Names uavn = {};

        effects.emplace("basic_instanced", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initSky(Device& device)
    {
        const auto filename = "Sky.fxo";
        const Names tn = {"sky"};
        const Names mn = {"wvp"};
        const Names vecn = {};
        const Names scn = {};
        const Names varn = {};
        const Names srvn = {"cube_map"};
        const Names uavn = {};

        effects.emplace("sky", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initNormalMap(Device& device)
    {
        const auto filename = "NormalMap.fxo";
        const Names tn = {"light3_tex_normal"};
        const Names mn = {"wvp", "world", "wit", "diffuse_transform", "shadow_transform"};
        const Names vecn = {"eye_pos", "fog_color"};
        const Names scn = {"fog_start", "fog_range"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {"diffuse_map", "cube_map", "normal_map", "shadow_map"};
        const Names uavn = {};

        effects.emplace("normal_map", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initDisplacementMap(Device& device)
    {
        const auto filename = "DisplacementMap.fxo";
        const Names tn = {"light3_tex_displacement"};
        const Names mn = {"vp", "wvp", "world", "wit", "diffuse_transform", "shadow_transform"};
        const Names vecn = {"eye_pos", "fog_color"};
        const Names scn = {"fog_start",
                           "fog_range",
                           "height_scale",
                           "max_tess_distance",
                           "min_tess_distance",
                           "min_tess",
                           "max_tess"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {"diffuse_map", "cube_map", "normal_map", "shadow_map"};
        const Names uavn = {};

        effects.emplace("displacement_map", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initWater(Device& device)
    {
        const auto filename = "Water.fxo";
        const Names tn = {"light3_water"};
        const Names mn = {"vp",
                          "wvp",
                          "world",
                          "wit",
                          "diffuse_transform",
                          "normal_transform",
                          "normal_transform_sec",
                          "height_transform",
                          "height_transform_sec"};
        const Names vecn = {"eye_pos", "fog_color"};
        const Names scn = {"fog_start",
                           "fog_range",
                           "height_scale",
                           "height_scale_sec",
                           "max_tess_distance",
                           "min_tess_distance",
                           "min_tess",
                           "max_tess"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {"diffuse_map", "cube_map", "normal_map", "normal_map_sec"};
        const Names uavn = {};

        effects.emplace("water", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initTerrain(Device& device)
    {
        const auto filename = "Terrain.fxo";
        const Names tn = {"light3"};
        const Names mn = {"vp"};
        const Names vecn = {"eye_pos", "fog_color", "world_frustum_planes"};
        const Names scn = {"fog_start",
                           "fog_range",
                           "texel_cell_space_u",
                           "texel_cell_space_v",
                           "world_cell_space",
                           "max_tess_distance",
                           "min_tess_distance",
                           "min_tess",
                           "max_tess"};
        const Names varn = {"dir_lights", "mat"};
        const Names srvn = {"diffuse_maps", "blend_map", "height_map", "density_map"};
        const Names uavn = {};

        effects.emplace("terrain", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initParticle(Device& device)
    {
        const Names tn = {"stream_out", "draw"};
        const Names mn = {"vp"};
        const Names vecn = {"eye_pos", "emit_pos", "emit_dir"};
        const Names scn = {"game_time", "time_step"};
        const Names varn = {};
        const Names srvn = {"tex_array", "random_tex"};
        const Names uavn = {};

        effects.emplace("fire", Effect{device, "Fire.fxo", tn, mn, vecn, scn, varn, srvn, uavn});
        effects.emplace("rain", Effect{device, "Rain.fxo", tn, mn, vecn, scn, varn, srvn, uavn});
        effects.emplace("smoke", Effect{device, "Smoke.fxo", tn, mn, vecn, scn, varn, srvn, uavn});
        effects.emplace("fountain", Effect{device, "Fountain.fxo", tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initShadowMap(Device& device)
    {
        const auto filename = "ShadowMap.fxo";
        const Names tn = {"build_shadow_map",
                          "build_shadow_map_alpha_clip",
                          "build_shadow_map_tess",
                          "build_shadow_map_tess_alpha_clip"};
        const Names mn = {"vp", "wvp", "world", "wit", "diffuse_transform"};
        const Names vecn = {"eye_pos"};
        const Names scn = {
            "height_scale", "max_tess_distance", "min_tess_distance", "min_tess", "max_tess"};
        const Names varn = {};
        const Names srvn = {"diffuse_map", "normal_map"};
        const Names uavn = {};

        effects.emplace("shadow_map", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }

    void initDebugTexture(Device& device)
    {
        const auto filename = "DebugTexture.fxo";
        const Names tn = {"rgba", "red", "green", "blue", "alpha"};
        const Names mn = {"wvp"};
        const Names vecn = {};
        const Names scn = {};
        const Names varn = {};
        const Names srvn = {"tex"};
        const Names uavn = {};

        effects.emplace("debug_texture", Effect{device, filename, tn, mn, vecn, scn, varn, srvn, uavn});
    }
}
