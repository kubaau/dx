#pragma once

#include "DXTypedefs.hpp"
#include "GraphicsTypedefs.hpp"

class IWavesCPU;
class IWavesGPU;

Meshes buildCustomMeshes(Device&, IWavesCPU&, IWavesGPU&);
Meshes buildSkullRoom(Device&);
Meshes buildCircle(Device&, float radius, unsigned v_count);
Meshes buildIcosahedron(Device&);
Meshes buildLand(Device&);
Meshes buildWavesCPU(Device&, IWavesCPU&);
Meshes buildWavesGPU(Device&, IWavesGPU&);
Meshes buildBox(Device&);
Meshes buildCylinder(Device&);
Meshes buildTreeSprites(Device&, unsigned count);
Meshes buildScreenQuad(Device&);
Meshes buildQuadPatch(Device&);
Meshes buildTrianglePatch(Device&);
Meshes buildIcosahedronPatch(Device&);
Meshes buildCubicBezierQuadPatch(Device&);
Meshes buildCubicBezierTrianglePatch(Device&);
Meshes buildQuadraticBezierQuadPatch(Device&);
Meshes buildQuadraticBezierTrianglePatch(Device&);
Meshes buildSky(Device&, float sky_sphere_radius);
