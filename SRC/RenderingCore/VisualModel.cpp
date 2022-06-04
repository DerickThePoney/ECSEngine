#include "stdafx.h"

#include "VisualModel.h"

#include "MaterialManager.h"
#include "MeshManager.h"

namespace ECSEngine
{
namespace Rendering
{
IMPLEMENT_POOL_ALLOCATED(VisualModel);

void VisualModel::Init(const std::string& parMaterialFile, bool parIsMutipass, const std::string& parMeshFile)
{
    FMeshHandle = Rendering::MeshManager::Instance().CreateMesh(parMeshFile);

    if (parIsMutipass)
        FMultiPassMaterialHandle = Rendering::MaterialManager::CreateMultiPassMaterialInstanceIFN(parMaterialFile);
    else
        FMaterialHandle = Rendering::MaterialManager::CreateMaterialInstanceIFN(parMaterialFile);

    AssertRelease(FMeshHandle.IsValid());
    AssertRelease(FMaterialHandle.IsValid() || FMultiPassMaterialHandle.IsValid());
}

bool VisualModel::HasMultipassMaterial() const
{
    return FMultiPassMaterialHandle.IsValid();
}

} // namespace Rendering
} // namespace ECSEngine
