#include "stdafx.h"

#include "GFXOperator.h"

#include "Common/Singleton.h"

namespace ECSEngine
{
namespace Rendering
{

void AbstractGFXOperatorDescriptor::DrawInEditor()
{
    if (ImGui::CollapsingHeader(Name()))
    {
        ImGui::Indent();
        VirtualDrawInEditor();
        ImGui::Unindent();
    }
}

namespace GFXOperatorDescriptorFactory
{
class GFXOperatorsManager : public Singleton<GFXOperatorsManager>
{
public:
    std::map<const char*, AbstractGFXOperatorDescriptor* (*)()> FactoryMap;
    std::vector<const char*> OperatorsList;
};

void DestroyManager()
{
    GFXOperatorsManager::Destroy();
}

bool RegisterOperatorDescriptor(const char* parOperatorName, AbstractGFXOperatorDescriptor* (*parFactory)())
{
    if (!GFXOperatorsManager::HasInstance())
        GFXOperatorsManager::CreateIFP();

    AssertRelease(GFXOperatorsManager::Instance().FactoryMap.find(parOperatorName) == GFXOperatorsManager::Instance().FactoryMap.end());
    GFXOperatorsManager::Instance().FactoryMap.insert_or_assign(parOperatorName, parFactory);
    GFXOperatorsManager::Instance().OperatorsList.push_back(parOperatorName);
    return true;
}

std::unique_ptr<AbstractGFXOperatorDescriptor> CreateOperator(const char* parOperatorName)
{
    AssertRelease(GFXOperatorsManager::HasInstance());
    auto itFind = GFXOperatorsManager::Instance().FactoryMap.find(parOperatorName);
    AssertRelease(itFind != GFXOperatorsManager::Instance().FactoryMap.end());
    return std::unique_ptr<AbstractGFXOperatorDescriptor>(itFind->second());
}

MemoryView<const char*> GetOperatorsList()
{
    AssertRelease(GFXOperatorsManager::HasInstance());
    return MemoryView<const char*>(GFXOperatorsManager::Instance().OperatorsList.data(), (u32)GFXOperatorsManager::Instance().OperatorsList.size());
}

} // namespace GFXOperatorDescriptorFactory

} // namespace Rendering
} // namespace ECSEngine
