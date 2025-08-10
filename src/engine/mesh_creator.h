#pragma once

#include "mesh.h"

// Utility to procedurally create basic meshes
class MeshCreator
{
public:
    // General cube builder with side length in object units
    static Mesh CreateCube(float side_length);

    // Axis-aligned unit cube centered at origin, size 1 (extends -0.5..0.5 on each axis)
    // Per-face normals and UVs [0,1] per face
    static Mesh CreateUnitCube();

    // Unit plane on XZ at y=0, centered at origin, size 1x1 (x,z in [-0.5,0.5])
    // Upward normal (0,1,0) and UVs spanning [0,1]
    static Mesh CreateUnitPlane();
};


