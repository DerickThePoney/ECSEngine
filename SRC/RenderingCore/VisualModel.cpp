#include "stdafx.h"

#include "VisualModel.h"

#include "MaterialManager.h"
#include "MeshManager.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(VisualModel);

void VisualModel::Init(const std::string& parMaterialFile, const std::string& parMeshFile)
{
    FMeshHandle = Rendering::MeshManager::Instance().CreateMesh(parMeshFile);
    FMaterialHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(parMaterialFile);

    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialHandle.IsValid());
}

} // namespace Rendering
} // namespace ECSEngine