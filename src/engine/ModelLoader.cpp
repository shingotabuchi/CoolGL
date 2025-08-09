#include "ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <stdexcept>

Mesh ModelLoader::loadFirstMeshFromFile(const std::string& path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path,
        aiProcess_Triangulate);
    if (!scene || !scene->HasMeshes())
    {
        throw std::runtime_error("Failed to load mesh from: " + path);
    }
    aiMesh* mesh = scene->mMeshes[0];
    return fromAiMesh(mesh);
}

Mesh ModelLoader::fromAiMesh(aiMesh* mesh)
{
    std::vector<MeshVertex> vertices;
    std::vector<unsigned int> indices;
    vertices.reserve(mesh->mNumVertices);

    for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
    {
        MeshVertex v{};
        v.position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->HasNormals())
        {
            v.normal = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        }
        vertices.push_back(v);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return Mesh(vertices, indices);
}


