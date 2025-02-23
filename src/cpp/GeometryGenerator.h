//***************************************************************************************
// GeometryGenerator.h by Frank Luna (C) 2011 All Rights Reserved.
//   
// Defines a static class for procedurally generating the geometry of 
// common mathematical objects.
//
// All triangles are generated "outward" facing.  If you want "inward" 
// facing triangles (for example, if you want to place the camera inside
// a sphere to simulate a sky), you will need to:
//   1. Change the Direct3D cull mode or manually reverse the winding order.
//   2. Invert the normal.
//   3. Update the texture coordinates and tangent vectors.
//***************************************************************************************

#ifndef GEOMETRYGENERATOR_H
#define GEOMETRYGENERATOR_H

#include <vector>

#include "MathTypedefs.hpp"

class GeometryGenerator
{
public:
    struct Vertex
    {
        Vertex() {}
        Vertex(const float3& p, const float3& n, const float3& t, const float2& uv)
            : Position(p), Normal(n), TangentU(t), TexC(uv)
        {
        }
        Vertex(
            float px, float py, float pz, float nx, float ny, float nz, float tx, float ty, float tz, float u, float v)
            : Position(px, py, pz), Normal(nx, ny, nz), TangentU(tx, ty, tz), TexC(u, v)
        {
        }

        float3 Position;
        float3 Normal;
        float3 TangentU;
        float2 TexC;
    };

    struct MeshData
    {
        std::vector<Vertex> Vertices;
        std::vector<unsigned> Indices;
    };

    ///<summary>
    /// Creates a box centered at the origin with the given dimensions.
    ///</summary>
    void CreateBox(float width, float height, float depth, MeshData& meshData);

    ///<summary>
    /// Creates a sphere centered at the origin with the given radius.  The
    /// slices and stacks parameters control the degree of tessellation.
    ///</summary>
    void CreateSphere(float radius, unsigned sliceCount, unsigned stackCount, MeshData& meshData);

    ///<summary>
    /// Creates a geosphere centered at the origin with the given radius.  The
    /// depth controls the level of tessellation.
    ///</summary>
    void CreateGeosphere(float radius, unsigned numSubdivisions, MeshData& meshData);

    ///<summary>
    /// Creates a cylinder parallel to the y-axis, and centered about the origin.  
    /// The bottom and top radius can vary to form various cone shapes rather than true
    // cylinders.  The slices and stacks parameters control the degree of tessellation.
    ///</summary>
    void CreateCylinder(float bottomRadius, float topRadius, float height, unsigned sliceCount, unsigned stackCount, MeshData& meshData);

    ///<summary>
    /// Creates an mxn grid in the xz-plane with m rows and n columns, centered
    /// at the origin with the specified width and depth.
    ///</summary>
    void CreateGrid(float width, float depth, unsigned m, unsigned n, MeshData& meshData);

    ///<summary>
    /// Creates a quad covering the screen in NDC coordinates.  This is useful for
    /// postprocessing effects.
    ///</summary>
    void CreateFullscreenQuad(MeshData& meshData);

private:
    void Subdivide(MeshData& meshData);
    void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, unsigned sliceCount, unsigned stackCount, MeshData& meshData);
    void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, unsigned sliceCount, unsigned stackCount, MeshData& meshData);
};

#endif // GEOMETRYGENERATOR_H