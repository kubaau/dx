#pragma once

#include <vector>
#include "ContainerTypedefs.hpp"
#include "Lights.hpp"
#include "Material.hpp"

using DirectionalLights = vector<DirectionalLight>;
using Materials = Map<Material>;

struct Mesh;
using Meshes = Map<Mesh>;

class Renderable;
using Renderables = Map<Renderable>;

class Particle;
using Particles = Map<Particle>;
