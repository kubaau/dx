#pragma once

#include "DXTypedefs.hpp"
#include "EffectTypedefs.hpp"
#include "GraphicsDebugFlags.hpp"
#include "IOTypedefs.hpp"
#include "MathTypedefs.hpp"
#include <tools/Print.hpp>

#define DEBUG_EFFECT_SETTER   \
    if (debug_effect_setters) \
        print("Setting", __FUNCTION__, name, "in", id);
#define DEBUG_EFFECT_SETTER_VALUE \
    if (debug_effect_setters)     \
        print("Setting", __FUNCTION__, name, "in", id, "to", v);

using Name = string;
using Names = const vector<Name>&;

class Effect
{

public:
    Effect(Device&,
           Filename,
           Names techniques,
           Names matrix_names,
           Names vector_names,
           Names scalar_names,
           Names variable_names,
           Names srv_names,
           Names uav_names);

    auto technique(Name name) { return techniques.at(name).Get(); }

    void matrix(Name, const Matrix&);
    void vector(Name, const float3&);
    void vector(Name, const Vector&);
    void scalar(Name, bool);
    void scalar(Name, unsigned);
    void scalar(Name, float);
    void scalar(Name, std::vector<float>);
    template <typename T>
    void variable(Name, T&);
    template <typename T>
    void variable(Name, T&, unsigned count);
    void srv(Name, SRV&);
    void uav(Name, UAV&);

private:
    bool hasMatrix(Name name) const;
    bool hasVector(Name name) const;
    bool hasScalar(Name name) const;
    bool hasVariable(Name name) const;
    bool hasSRV(Name name) const;
    bool hasUAV(Name name) const;

    void loadEffectFile(Device&, FilePath);

    EffectPtr effect;
    Techniques techniques;

    MatrixVariables matrices;
    VectorVariables vectors;
    ScalarVariables scalars;
    EffectVariables variables;
    EffectSRVs srvs;
    EffectUAVs uavs;

    ID id;
};

template <typename T>
void Effect::variable(Name name, T& val)
{
    if (not hasVariable(name))
        return;
    DEBUG_EFFECT_SETTER;
    variables[name]->SetRawValue(&val, 0, sizeof(T));
}

template <typename T>
void Effect::variable(Name name, T& val, unsigned count)
{
    if (not hasVariable(name))
        return;
    DEBUG_EFFECT_SETTER;
    variables[name]->SetRawValue(&val, 0, sizeof(T) * count);
}
