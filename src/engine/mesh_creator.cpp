#include "mesh_creator.h"

#include <vector>

static inline void AppendVertex(std::vector<MeshVertex>& destination,
                                const glm::vec3& position,
                                const glm::vec3& normal,
                                const glm::vec2& uv)
{
    MeshVertex vertex{};
    vertex.position = position;
    vertex.normal = normal;
    vertex.uv = uv;
    destination.push_back(vertex);
}

Mesh MeshCreator::CreateCube(float side_length)
{
    // Build a cube with 24 unique vertices (4 per face) for correct face normals and UVs
    std::vector<MeshVertex> vertices;
    vertices.reserve(24);
    std::vector<unsigned int> triangleIndices;
    triangleIndices.reserve(36);

    const float halfExtent = side_length * 0.5f;

    // +X face
    {
        const glm::vec3 faceNormal = {1,0,0};
        AppendVertex(vertices, { halfExtent,-halfExtent,-halfExtent}, faceNormal, {0,0}); // 0
        AppendVertex(vertices, { halfExtent, halfExtent,-halfExtent}, faceNormal, {0,1}); // 1
        AppendVertex(vertices, { halfExtent, halfExtent, halfExtent}, faceNormal, {1,1}); // 2
        AppendVertex(vertices, { halfExtent,-halfExtent, halfExtent}, faceNormal, {1,0}); // 3
        const unsigned int base = 0;
        const unsigned int tris[] = { base+0, base+1, base+2, base+2, base+3, base+0 };
        triangleIndices.insert(triangleIndices.end(), std::begin(tris), std::end(tris));
    }

    // -X face
    {
        const glm::vec3 faceNormal = {-1,0,0};
        const unsigned int base = static_cast<unsigned int>(vertices.size());
        AppendVertex(vertices, {-halfExtent,-halfExtent, halfExtent}, faceNormal, {0,0});
        AppendVertex(vertices, {-halfExtent, halfExtent, halfExtent}, faceNormal, {0,1});
        AppendVertex(vertices, {-halfExtent, halfExtent,-halfExtent}, faceNormal, {1,1});
        AppendVertex(vertices, {-halfExtent,-halfExtent,-halfExtent}, faceNormal, {1,0});
        const unsigned int tris[] = { base+0, base+1, base+2, base+2, base+3, base+0 };
        triangleIndices.insert(triangleIndices.end(), std::begin(tris), std::end(tris));
    }

    // +Y face
    {
        const glm::vec3 faceNormal = {0,1,0};
        const unsigned int base = static_cast<unsigned int>(vertices.size());
        AppendVertex(vertices, {-halfExtent, halfExtent,-halfExtent}, faceNormal, {0,0});
        AppendVertex(vertices, {-halfExtent, halfExtent, halfExtent}, faceNormal, {0,1});
        AppendVertex(vertices, { halfExtent, halfExtent, halfExtent}, faceNormal, {1,1});
        AppendVertex(vertices, { halfExtent, halfExtent,-halfExtent}, faceNormal, {1,0});
        const unsigned int tris[] = { base+0, base+1, base+2, base+2, base+3, base+0 };
        triangleIndices.insert(triangleIndices.end(), std::begin(tris), std::end(tris));
    }

    // -Y face
    {
        const glm::vec3 faceNormal = {0,-1,0};
        const unsigned int base = static_cast<unsigned int>(vertices.size());
        AppendVertex(vertices, {-halfExtent,-halfExtent, halfExtent}, faceNormal, {0,0});
        AppendVertex(vertices, {-halfExtent,-halfExtent,-halfExtent}, faceNormal, {0,1});
        AppendVertex(vertices, { halfExtent,-halfExtent,-halfExtent}, faceNormal, {1,1});
        AppendVertex(vertices, { halfExtent,-halfExtent, halfExtent}, faceNormal, {1,0});
        const unsigned int tris[] = { base+0, base+1, base+2, base+2, base+3, base+0 };
        triangleIndices.insert(triangleIndices.end(), std::begin(tris), std::end(tris));
    }

    // +Z face
    {
        const glm::vec3 faceNormal = {0,0,1};
        const unsigned int base = static_cast<unsigned int>(vertices.size());
        AppendVertex(vertices, {-halfExtent,-halfExtent, halfExtent}, faceNormal, {0,0});
        AppendVertex(vertices, { halfExtent,-halfExtent, halfExtent}, faceNormal, {1,0});
        AppendVertex(vertices, { halfExtent, halfExtent, halfExtent}, faceNormal, {1,1});
        AppendVertex(vertices, {-halfExtent, halfExtent, halfExtent}, faceNormal, {0,1});
        const unsigned int tris[] = { base+0, base+1, base+2, base+2, base+3, base+0 };
        triangleIndices.insert(triangleIndices.end(), std::begin(tris), std::end(tris));
    }

    // -Z face
    {
        const glm::vec3 faceNormal = {0,0,-1};
        const unsigned int base = static_cast<unsigned int>(vertices.size());
        AppendVertex(vertices, { halfExtent,-halfExtent,-halfExtent}, faceNormal, {0,0});
        AppendVertex(vertices, {-halfExtent,-halfExtent,-halfExtent}, faceNormal, {1,0});
        AppendVertex(vertices, {-halfExtent, halfExtent,-halfExtent}, faceNormal, {1,1});
        AppendVertex(vertices, { halfExtent, halfExtent,-halfExtent}, faceNormal, {0,1});
        const unsigned int tris[] = { base+0, base+1, base+2, base+2, base+3, base+0 };
        triangleIndices.insert(triangleIndices.end(), std::begin(tris), std::end(tris));
    }

    return Mesh(vertices, triangleIndices);
}

Mesh MeshCreator::CreateUnitCube()
{
    return CreateCube(1.0f);
}

Mesh MeshCreator::CreateUnitPlane()
{
    // Plane on XZ at y=0, centered at origin, size 1x1, CCW winding for +Y facing
    std::vector<MeshVertex> vertices;
    vertices.reserve(4);
    // Indices arranged CCW when viewed from +Y so front face points upward
    std::vector<unsigned int> triangleIndices = { 0,2,1, 0,3,2 };

    const float halfExtent = 0.5f;
    const glm::vec3 upNormal = {0,1,0};

    // Quad corners in CCW when looking from +Y
    AppendVertex(vertices, {-halfExtent, 0.0f, -halfExtent}, upNormal, {0.0f, 0.0f}); // 0: bottom-left
    AppendVertex(vertices, { halfExtent, 0.0f, -halfExtent}, upNormal, {1.0f, 0.0f}); // 1: bottom-right
    AppendVertex(vertices, { halfExtent, 0.0f,  halfExtent}, upNormal, {1.0f, 1.0f}); // 2: top-right
    AppendVertex(vertices, {-halfExtent, 0.0f,  halfExtent}, upNormal, {0.0f, 1.0f}); // 3: top-left

    return Mesh(vertices, triangleIndices);
}


