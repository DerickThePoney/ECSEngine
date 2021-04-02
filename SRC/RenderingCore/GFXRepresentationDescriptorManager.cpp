#include "stdafx.h"

#include "GFXRepresentationDescriptorManager.h"

#include "GFXRepresentation.h"

namespace ECSEngine
{
namespace Rendering
{

const GFXRepresentationDescriptor* GFXRepresentationDescriptorManager::Descriptor(const std::string& parDescriptorName) const
{
    foreachitemconst(desc, FGFXRepresentationsDescriptors)
    {
        if (parDescriptorName == desc->Name())
            return desc.get();
    }
    return nullptr;
}

void GFXRepresentationDescriptorManager::AddDescriptor()
{
    FGFXRepresentationsDescriptors.push_back(std::make_unique<GFXRepresentationDescriptor>());
}

void GFXRepresentationDescriptorManager::RemoveDescriptor(const std::string& parDescriptorName)
{
    std::size_t found = -1;
    forrange(i, 0, FGFXRepresentationsDescriptors.size())
    {
        if (parDescriptorName == FGFXRepresentationsDescriptors[i]->Name())
        {
            found = i;
            break;
        }
    }

    if (found != -1)
    {
        FGFXRepresentationsDescriptors.erase(FGFXRepresentationsDescriptors.begin() + found);
    }
}

} // namespace Rendering
} // namespace ECSEngine