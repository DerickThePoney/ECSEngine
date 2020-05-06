#include "stdafx.h"

#include "AssimpMeshDataLoading.h"

#include "MeshFileWriter.h"

#include <assimp/Importer.hpp> // C++ importer interface
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure
namespace AssimpLoading
{

void GenerateMesh(const std::string& parFileName, const char* parMeshFileBuffer, const u32 parSize)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFileFromMemory(
          parMeshFileBuffer, parSize, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_OptimizeMeshes | aiProcess_FindDegenerates | aiProcess_FlipUVs);
    AssertRelease(scene != nullptr);

    ECSEngine::Rendering::MeshFileWriter writer(parFileName + ".gen");
    writer << scene;
}

} // namespace AssimpLoading
