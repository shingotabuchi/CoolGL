#pragma once

#include <vector>
#include <string>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include "mesh.h"

class ModelLoader
{
public:
    static Mesh LoadFirstMeshFromFile(const std::string& path);

private:
    static Mesh FromAiMesh(aiMesh* mesh);
};


