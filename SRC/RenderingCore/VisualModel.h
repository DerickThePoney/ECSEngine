#pragma once
#include "Common/PoolAllocator.h"
#include "Common/RenderingHandles.h"

namespace ECSEngine
{
namespace Rendering
{
class VisualModel
{
    DECLARE_POOL_ALLOCATED(VisualModel);

public:
    void Init(const std::string& parMaterialFile, bool parIsMutipass, const std::string& parMeshFile);

    const MeshHandle GetMeshHandle() const { return FMeshHandle; }
    const MaterialInstanceHandle GetMaterialInstanceHandle() const { return FMaterialHandle; }
    const MultiPassMaterialInstanceHandle GetMultiPassMaterialInstanceHandle() const { return FMultiPassMaterialHandle; }
    bool HasMultipassMaterial() const;
    const bool Visible() const { return FVisible; }

    void SetVisible(bool parValue) { FVisible = parValue; }

private:
    MeshHandle FMeshHandle;
    MaterialInstanceHandle FMaterialHandle;
    MultiPassMaterialInstanceHandle FMultiPassMaterialHandle;
    bool FVisible = true;
};
} // namespace Rendering
} // namespace ECSEngine
