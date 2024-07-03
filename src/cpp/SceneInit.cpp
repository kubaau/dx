#include <cassert>
#include <fstream>
#include <string>
#include "BlendStates.hpp"
#include "D3DEngine.hpp"
#include "DepthStencilStates.hpp"
#include "Effects.hpp"
#include <tools/Endl.hpp>
#include "GraphicsDebugFlags.hpp"
#include "GraphicsDebugUtils.hpp"
#include "InputLayouts.hpp"
#include "MathTypedefs.hpp"
#include <tools/Print.hpp>
#include "RasterizerStates.hpp"
#include "Renderable.hpp"

static D3D_PRIMITIVE_TOPOLOGY toTopology(string t)
{
    if (t == "trianglelist")
        return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    else if (t == "linestrip")
        return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    else if (t == "pointlist")
        return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    else if (t == "3cp")
        return D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST;
    else if (t == "4cp")
        return D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;
    else if (t == "6cp")
        return D3D11_PRIMITIVE_TOPOLOGY_6_CONTROL_POINT_PATCHLIST;
    else if (t == "9cp")
        return D3D11_PRIMITIVE_TOPOLOGY_9_CONTROL_POINT_PATCHLIST;
    else if (t == "10cp")
        return D3D11_PRIMITIVE_TOPOLOGY_10_CONTROL_POINT_PATCHLIST;
    else if (t == "16cp")
        return D3D11_PRIMITIVE_TOPOLOGY_16_CONTROL_POINT_PATCHLIST;
    else
        return D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;
}

void D3DEngine::initScene()
{
    if (debug_scene)
        print("Scene:");

    using namespace std;

    constexpr auto filename = "../../assets/scene.txt";
    ifstream file{filename};
    assert(file && "Cannot find scene");
    string line;

    // skip comments
    streampos g;
    do
    {
        g = file.tellg();
        getline(file, line);
    } while (line.empty() or line.front() == '#');
    file.seekg(g);

    ID renderable, mesh, topology, diffuse, normal, normal_sec, displacement, cube_map, tex_array, material,
        material_shadow, effect, tech_near, tech_mid, tech_far, tech_no_tex_near, tech_no_tex_mid, tech_no_tex_far,
        tech_fog_near, tech_fog_mid, tech_fog_far, tech_face_normals, tech_vertex_normals, input_layout,
        rasterizer_state, ds_state, blend_state;
    unsigned mesh_offset;
    bool has_tex_transforms, reflecting, reflectable, enabled;
    float nearr, farr;
    Rotation r;
    Scale s;
    Translation t;

    while (not file.eof())
    {
        file >> renderable >> mesh >> mesh_offset >> topology >> diffuse >> normal >> normal_sec >> displacement >>
            cube_map >> tex_array >> has_tex_transforms;

        float4x4 diffuse_transform;
        float4x4 normal_transform;
        float4x4 normal_sec_transform;
        float4x4 height_transform;
        float4x4 height_sec_transform;

        if (has_tex_transforms)
        {
            vector<float> transform(16);
            for (auto& v : transform)
                file >> v;
            diffuse_transform = float4x4{transform.data()};
            for (auto& v : transform)
                file >> v;
            normal_transform = float4x4{transform.data()};
            for (auto& v : transform)
                file >> v;
            normal_sec_transform = float4x4{transform.data()};
            for (auto& v : transform)
                file >> v;
            height_transform = float4x4{transform.data()};
            for (auto& v : transform)
                file >> v;
            height_sec_transform = float4x4{transform.data()};
        }
        else
        {
            using namespace DirectX;
            XMStoreFloat4x4(&diffuse_transform, XMMatrixIdentity());
            XMStoreFloat4x4(&normal_transform, XMMatrixIdentity());
            XMStoreFloat4x4(&normal_sec_transform, XMMatrixIdentity());
            XMStoreFloat4x4(&height_transform, XMMatrixIdentity());
            XMStoreFloat4x4(&height_sec_transform, XMMatrixIdentity());
        }

        file >> material >> material_shadow >> reflecting >> reflectable >> enabled >> effect >> nearr >> farr >>
            tech_near >> tech_mid >> tech_far >> tech_no_tex_near >> tech_no_tex_mid >> tech_no_tex_far >>
            tech_fog_near >> tech_fog_mid >> tech_fog_far >> tech_face_normals >> tech_vertex_normals >> input_layout >>
            rasterizer_state >> ds_state >> blend_state >> r.x >> r.y >> r.z >> s.x >> s.y >> s.z >> t.x >> t.y >> t.z;

#if defined(DEBUG) | defined(_DEBUG)
        if (renderable == "waves_cpu")
            continue;
#endif

        r.x *= pi;
        r.y *= pi;
        r.z *= pi;

        auto o = make_unique<Renderable>(*context);
        o->id = renderable;
        o->mesh = &meshes.at(mesh);
        o->mesh_offset = mesh_offset;
        o->topology = toTopology(topology);
        if (diffuse != "0")
            o->diffuse = textures[diffuse].Get();
        if (normal != "0")
            o->normal = textures[normal].Get();
        if (normal_sec != "0")
            o->normal_sec = textures[normal_sec].Get();
        if (displacement != "0")
            o->displacement = textures[displacement].Get();
        if (cube_map != "0")
            o->cube_map = textures[cube_map].Get();
        if (tex_array != "0")
            o->tex_array = textures[tex_array].Get();
        o->diffuse_transform = diffuse_transform;
        o->normal_transform = normal_transform;
        o->normal_sec_transform = normal_sec_transform;
        o->height_transform = height_transform;
        o->height_sec_transform = height_sec_transform;
        if (material != "none")
            o->mat = &materials[material];
        if (material_shadow != "none")
            o->mat_shadow = &materials[material_shadow];
        o->reflecting = reflecting;
        o->reflectable = reflectable;
        o->enabled = enabled;
        o->effect = &effects.at(effect);
        o->techs["near"] = o->effect->technique(tech_near);
        o->techs["mid"] = o->effect->technique(tech_mid);
        o->techs["far"] = o->effect->technique(tech_far);
        o->techs["no_tex_near"] = o->effect->technique(tech_no_tex_near);
        o->techs["no_tex_mid"] = o->effect->technique(tech_no_tex_mid);
        o->techs["no_tex_far"] = o->effect->technique(tech_no_tex_far);
        o->techs["fog_near"] = o->effect->technique(tech_fog_near);
        o->techs["fog_mid"] = o->effect->technique(tech_fog_mid);
        o->techs["fog_far"] = o->effect->technique(tech_fog_far);
        if (tech_face_normals != "none")
            o->techs["tech_face_normals"] = o->effect->technique(tech_face_normals);
        if (tech_vertex_normals != "none")
            o->techs["tech_vertex_normals"] = o->effect->technique(tech_vertex_normals);
        o->nearr = nearr;
        o->farr = farr;
        o->il = input_layouts[input_layout].Get();
        if (rasterizer_state != "none")
            o->rs_primary = rs[rasterizer_state].Get();
        if (ds_state != "none")
            o->dss_primary = dss[ds_state].Get();
        if (blend_state != "none")
            o->bs_primary = bs[blend_state].Get();
        o->r = r;
        o->s = s;
        o->t = t;
        o->updateWorld();
        renderables.emplace(renderable, *(o));

        if (debug_scene)
        {
            endl();
            print("Loaded object", renderable);
            print("mesh:", mesh, "offset:", mesh_offset, "topology:", topology);
            print("diffuse:",
                  diffuse,
                  "normal:",
                  normal,
                  "normal_sec:",
                  normal_sec,
                  "displacement:",
                  displacement,
                  "cube_map:",
                  cube_map,
                  "tex array:",
                  tex_array);
            print("dt:",
                  diffuse_transform,
                  "nt:",
                  normal_transform,
                  "nst:",
                  normal_sec_transform,
                  "ht:",
                  height_transform,
                  "hst:",
                  height_sec_transform);
            print("material:",
                  material,
                  "mat_shadow:",
                  material_shadow,
                  "reflecting:",
                  reflecting,
                  "reflectable:",
                  reflectable,
                  "enabled:",
                  enabled);
            print("effect:",
                  effect,
                  "tech_near:",
                  tech_near,
                  "tech_mid:",
                  tech_mid,
                  "tech_far:",
                  tech_far,
                  "tech_no_tex_near:",
                  tech_no_tex_near,
                  "tech_no_tex_mid:",
                  tech_no_tex_mid,
                  "tech_no_tex_far:",
                  tech_no_tex_far,
                  "tech_fog_near:",
                  tech_fog_near,
                  "tech_fog_mid:",
                  tech_fog_mid,
                  "tech_fog_far:",
                  tech_fog_far,
                  "techfn:",
                  tech_face_normals,
                  "techvn:",
                  tech_vertex_normals);
            print("near:", nearr, "far:", farr);
            print("il:", input_layout, "rs:", rasterizer_state, "dss:", ds_state, "bs:", blend_state);
            print("rotation:", r);
            print("scale:", s);
            print("translation", t);
            endl();
        }
    }
}
