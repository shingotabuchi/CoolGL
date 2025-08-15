#pragma once

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "mesh.h"

class ModelLoader
{
public:
    // Optional Assimp post-process flags. If 0, sensible defaults are used internally.
    static Mesh LoadFirstMeshFromFile(const std::string& path, bool pre_transform_vertices = false);
    static std::vector<Mesh> LoadAllMeshesFromFile(const std::string& path, bool pre_transform_vertices = false);

private:
    static Mesh FromAiMesh(aiMesh* mesh);
};


