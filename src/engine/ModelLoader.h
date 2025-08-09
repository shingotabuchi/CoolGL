#pragma once

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "Mesh.h"

class ModelLoader
{
public:
    static Mesh loadFirstMeshFromFile(const std::string& path);

private:
    static Mesh fromAiMesh(aiMesh* mesh);
};


