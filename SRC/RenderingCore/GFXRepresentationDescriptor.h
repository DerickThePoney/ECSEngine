#pragma once
#include "Common/PoolAllocator.h"
#include "GFXOperator.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentationDescriptor
{
    DECLARE_POOL_ALLOCATED(GFXRepresentationDescriptor);

public:
    const std::string& Name() const { return FName; }
    const std::string& MaterialName() const { return FMaterialName; }
    const bool IsMultiPassMaterial() const { return FIsMultpassMaterial; }
    const std::string& MeshFile() const { return FMeshFile; }
    const MemoryView<const std::unique_ptr<AbstractGFXOperatorDescriptor>> OperatorDescriptors() const
    {
        return MemoryView(FOperatorDescriptors.data(), (u32)FOperatorDescriptors.size());
    }

    void DrawInEditor();

    SERIALIZE()
    {
        PROPERTYFIELD(Name, "Default");
        PROPERTYFIELD(MeshFile, "none");
        PROPERTYFIELD(MaterialName, "none");
        PROPERTYFIELD(IsMultpassMaterial, false);
        PROPERTYFIELD(OperatorDescriptors, std::vector<std::unique_ptr<AbstractGFXOperatorDescriptor>>());
    }

private:
    std::string FName = "Default";
    std::string FMeshFile = "none";
    std::string FMaterialName = "none";
    bool FIsMultpassMaterial = false;
    std::vector<std::unique_ptr<AbstractGFXOperatorDescriptor>> FOperatorDescriptors;
};
} // namespace Rendering
} // namespace ECSEngine