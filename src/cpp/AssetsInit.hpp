#pragma once

#include "DXTypedefs.hpp"
#include "GraphicsTypedefs.hpp"

Meshes loadFileMeshes(Device&);
DirectionalLights loadLights();
Textures loadTextures(Device&, Context&);
Textures initRandomTextures(Device&);
Materials loadMaterials();
Particles loadParticles(Textures&);