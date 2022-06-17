#pragma once
#include "Common/MemoryView.h"
#include "Common/Singleton.h"
#include "GFXRepresentationDescriptor.h"

namespace ECSEngine
{
namespace Rendering
{
class GFXRepresentationDescriptorManager : public Singleton<GFXRepresentationDescriptorManager>
{
public:
    MemoryView<std::unique_ptr<GFXRepresentationDescriptor>> Decriptors() { return { FGFXRepresentationsDescriptors.data(), (u32)FGFXRepresentationsDescriptors.size() }; }

    const GFXRepresentationDescriptor* Descriptor(const std::string& parDescriptorName) const;

    void AddDescriptor();
    void RemoveDescriptor(const std::string& parDescriptorName);

    SERIALIZE() { PROPERTYFIELD(GFXRepresentationsDescriptors, std::vector<std::unique_ptr<GFXRepresentationDescriptor>>()); }

private:
    std::vector<std::unique_ptr<GFXRepresentationDescriptor>> FGFXRepresentationsDescriptors;
};
} // namespace Rendering
} // namespace ECSEngine