#include "AssetsInit.hpp"
#include <cassert>
#include <tools/Print.hpp>
#include <fstream>
#include "Effects.hpp"
#include "GraphicsDebugFlags.hpp"
#include "GraphicsDebugUtils.hpp"
#include "Mesh.hpp"
#include "Particle.hpp"
#include "TextureLoaders.hpp"

static auto removeExtension(Filename filename)
{
    using namespace std;
    const auto lastdot = filename.find_last_of(".");
    if (lastdot == string::npos)
        return filename;
    return filename.substr(0, lastdot);
}

Meshes loadFileMeshes(Device& device)
{
    using namespace std;

    ifstream file{"../../assets/models.txt"};
    assert(file && "Cannot find models");
    string line;
    Meshes meshes;
    while (getline(file, line))
        meshes.emplace(removeExtension(line), Mesh{device, line});
    return meshes;
}

DirectionalLights loadLights()
{
    using namespace std;

    ifstream file{"../../assets/lights.txt"};
    assert(file && "Cannot find lights");
    string line;
    getline(file, line); // skip first
    DirectionalLights dir_lights;
    while (not file.eof())
    {
        DirectionalLight l{};
        file >> l.ambient.x >> l.ambient.y >> l.ambient.z >> l.ambient.w;
        file >> l.diffuse.x >> l.diffuse.y >> l.diffuse.z >> l.diffuse.w;
        file >> l.specular.x >> l.specular.y >> l.specular.z >> l.specular.w;
        file >> l.direction.x >> l.direction.y >> l.direction.z;
        dir_lights.push_back(l);
    }
    if (debug_lights)
        print("Lights:", dir_lights);
    return dir_lights;
}

Textures loadTextures(Device& device, Context& context)
{
    using namespace std;

    const string base_filename = "../../assets/textures/";
    ifstream file{"../../assets/textures.txt"};
    assert(file && "Cannot find textures");
    string tex_file, tex_extension, tex_name;
    unsigned tex_array_count;
    Textures textures;
    while (not file.eof())
    {
        file >> tex_file >> tex_extension >> tex_name >> tex_array_count;
        if (tex_name.find("array") != string::npos)
        {
            FilePaths tex_paths;
            for (auto i = 0u; i < tex_array_count; ++i)
                tex_paths.push_back(base_filename + tex_file + to_string(i) + "." + tex_extension);
            if (debug_texture_load)
                print("Loading tex array", tex_name, tex_paths);
            SRVPtr tex;
            tex = create2DTextureArray(device, context, tex_paths);
            textures.emplace(tex_name, tex);
        }
        else
        {
            if (not tex_array_count)
            {
                const auto tex_file_full = tex_file + "." + tex_extension;
                if (debug_texture_load)
                    print("Loading", tex_file_full, "as", tex_name);
                const auto texture_filename = base_filename + tex_file_full;
                if (tex_extension == "dds")
                    textures.emplace(tex_name, loadDDSTexture(device, context, texture_filename));
                else if (tex_extension == "bmp")
                    textures.emplace(tex_name, loadBMPTexture(device, context, texture_filename));
            }
            else
            {
                for (auto i = 0u; i < tex_array_count; ++i)
                {
                    stringstream n;
                    n.width(3);
                    n.fill('0');
                    n << i + 1;
                    const auto tex_file_full = tex_file + n.str() + "." + tex_extension;
                    const auto tex_key = tex_name + n.str();
                    const auto texture_filename = base_filename + tex_file_full;
                    if (debug_texture_load)
                        print("Loading", tex_file_full, "as", tex_key);
                    if (tex_extension == "dds")
                        textures.emplace(tex_key, loadDDSTexture(device, context, texture_filename));
                    else if (tex_extension == "bmp")
                        textures.emplace(tex_key, loadBMPTexture(device, context, texture_filename));
                }
            }
        }
    }
    return textures;
}

Textures initRandomTextures(Device& device)
{
    Textures textures;
    textures.emplace("random_fire", createRandomTexture(device));
    return textures;
}

Materials loadMaterials()
{
    using namespace std;

    constexpr auto filename = "../../assets/materials.txt";
    ifstream file{filename};
    assert(file && "Cannot find materials");
    string line;
    getline(file, line); // skip first
    Materials materials;
    while (not file.eof())
    {
        Material m{};
        string id;
        file >> id;
        file >> m.ambient.x >> m.ambient.y >> m.ambient.z >> m.ambient.w;
        file >> m.diffuse.x >> m.diffuse.y >> m.diffuse.z >> m.diffuse.w;
        file >> m.specular.x >> m.specular.y >> m.specular.z >> m.specular.w;
        file >> m.reflect.x >> m.reflect.y >> m.reflect.z >> m.reflect.w;
        materials.emplace(id, m);
    }
    if (debug_materials)
        print("Materials:", materials);
    return materials;
}

Particles loadParticles(Textures& textures)
{
    using namespace std;

    constexpr auto filename = "../../assets/particles.txt";
    ifstream file{filename};
    assert(file && "Cannot find particles");
    string line;
    getline(file, line); // skip first
    Particles particles;
    while (not file.eof())
    {
        string id, effect, diffuse, random;
        unsigned max_particle_count;
        float3 position;
        file >> id >> effect >> diffuse >> random >> max_particle_count >> position.x >> position.y >> position.z;

        particles.emplace(id,
                          Particle{id,
                                   &effects.at(effect),
                                   textures.at(diffuse).Get(),
                                   textures.at(random).Get(),
                                   max_particle_count,
                                   position});

        if (debug_particles)
            print("Particle:", id, effect, diffuse, random, max_particle_count, position);
    }
    return particles;
}
