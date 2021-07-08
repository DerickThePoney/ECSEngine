#include "stdafx.h"

#include "RmlDataModelWrapper.h"

#ifndef RMLUI_STATIC_LIB
#define RMLUI_STATIC_LIB
#endif
#include <RmlUi/Core/DataModelHandle.h>

namespace ECSEngine
{
namespace UI
{
class RmlDataModelWrapper final : public IDataModelWrapper
{
public:
    RmlDataModelWrapper(Rml::DataModelHandle parHandle)
        : FHandle(parHandle)
    {
    }

    bool IsVariableDirty(const std::string& variable_name) override { return FHandle.IsVariableDirty(variable_name); }

    void DirtyVariable(const std::string& variable_name) override { FHandle.DirtyVariable(variable_name); }
    explicit operator bool() override { return bool(FHandle); }

private:
    Rml::DataModelHandle FHandle;
};

namespace RmlDataModelWrapperFactory
{
std::unique_ptr<IDataModelWrapper> CreateDataModelWrapper(const Rml::DataModelHandle& parDataModelHandle)
{
    return std::make_unique<RmlDataModelWrapper>(parDataModelHandle);
}
} // namespace RmlDataModelWrapperFactory
} // namespace UI
} // namespace ECSEngine