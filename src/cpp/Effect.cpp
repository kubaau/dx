#include "Effect.hpp"
#include <cassert>
#include <fstream>
#include <tools/Contains.hpp>
#include <tools/Endl.hpp>
#include "GraphicsDebugUtils.hpp"
#include <tools/Typedefs.hpp>

Effect::Effect(Device& device,
               Filename filename,
               Names technique_names,
               Names matrix_names,
               Names vector_names,
               Names scalar_names,
               Names variable_names,
               Names srv_names,
               Names uav_names)
{
    if (debug_effect_setters)        
        print("Loading effect file:", filename);

    loadEffectFile(device, filename);

    for (auto n : technique_names) techniques.emplace(n, effect->GetTechniqueByName(n.c_str()));
    for (auto n : matrix_names) matrices[n] = effect->GetVariableByName(n.c_str())->AsMatrix();
    for (auto n : vector_names) vectors[n] = effect->GetVariableByName(n.c_str())->AsVector();
    for (auto n : scalar_names) scalars[n] = effect->GetVariableByName(n.c_str())->AsScalar();
    for (auto n : variable_names) variables[n] = effect->GetVariableByName(n.c_str());
    for (auto n : srv_names) srvs[n] = effect->GetVariableByName(n.c_str())->AsShaderResource();
    for (auto n : uav_names) uavs[n] = effect->GetVariableByName(n.c_str())->AsUnorderedAccessView();

    id = filename;

    if (debug_effect_setters)
    {
        print("technique_names:", technique_names);
        print("matrix_names:", matrix_names);
        print("vector_names:", vector_names);
        print("scalar_names:", scalar_names);
        print("variable_names:", variable_names);
        print("srv_names:", srv_names);
        print("uav_names:", uav_names);
        endl();
    }
}

template <typename T>
auto asCFloatPtr(T& val)
{
    return reinterpret_cast<const float*>(&val);
}

void Effect::matrix(Name name, const Matrix& m)
{
    if (not hasMatrix(name)) return;
    DEBUG_EFFECT_SETTER;
    matrices[name]->SetMatrix(asCFloatPtr(m));
}

void Effect::vector(Name name, const float3& v)
{
    if (not hasVector(name)) return;
    DEBUG_EFFECT_SETTER_VALUE;
    vectors[name]->SetRawValue(&v, 0, sizeof(v));
}

void Effect::vector(Name name, const Vector& v)
{
    if (not hasVector(name)) return;
    DEBUG_EFFECT_SETTER;
    vectors[name]->SetFloatVector(asCFloatPtr(v));
}

void Effect::scalar(Name name, bool v)
{
    if (not hasScalar(name)) return;
    DEBUG_EFFECT_SETTER_VALUE;
    scalars[name]->SetBool(v);
}

void Effect::scalar(Name name, unsigned v)
{
    if (not hasScalar(name)) return;
    DEBUG_EFFECT_SETTER_VALUE;
    scalars[name]->SetInt(v);
}

void Effect::scalar(Name name, float v)
{
    if (not hasScalar(name)) return;
    DEBUG_EFFECT_SETTER_VALUE;
    scalars[name]->SetFloat(v);
}

void Effect::scalar(Name name, std::vector<float> v)
{
    if (not hasScalar(name)) return;
    DEBUG_EFFECT_SETTER_VALUE;
    scalars[name]->SetFloatArray(v.data(), 0, static_cast<unsigned>(v.size()));
}

void Effect::srv(Name name, SRV& v)
{
    if (not hasSRV(name)) return;
    DEBUG_EFFECT_SETTER;
    srvs[name]->SetResource(&v);
}

void Effect::uav(Name name, UAV& v)
{
    if (not hasUAV(name)) return;
    DEBUG_EFFECT_SETTER;
    uavs[name]->SetUnorderedAccessView(&v);
}

bool Effect::hasMatrix(Name name) const { return contains(matrices, name); }
bool Effect::hasVector(Name name) const { return contains(vectors, name); }
bool Effect::hasScalar(Name name) const { return contains(scalars, name); }
bool Effect::hasVariable(Name name) const { return contains(variables, name); }
bool Effect::hasSRV(Name name) const { return contains(srvs, name); }
bool Effect::hasUAV(Name name) const { return contains(uavs, name); }

void Effect::loadEffectFile(Device& device, FilePath path)
{
    using namespace std;
    ifstream effect_file{path, ios::binary};
    assert(effect_file && "Count not load effect file");
    effect_file.seekg(0, ios_base::end);
    const auto size = static_cast<size_t>(effect_file.tellg());
    effect_file.seekg(0, ios_base::beg);
    IOBuffer compiled_shader(size);
    effect_file.read(compiled_shader.data(), size);
    HR(D3DX11CreateEffectFromMemory(compiled_shader.data(), size, 0, &device, &effect));
}
