#pragma once

namespace Rml
{
class DataModelHandle;
}

namespace ECSEngine
{
namespace UI
{
class IDataModelWrapper
{
public:
    virtual bool IsVariableDirty(const std::string& variable_name) = 0;
    virtual void DirtyVariable(const std::string& variable_name) = 0;

    virtual explicit operator bool() = 0;
};
namespace RmlDataModelWrapperFactory
{
std::unique_ptr<IDataModelWrapper> CreateDataModelWrapper(const Rml::DataModelHandle& parDataModelHandle);
}
} // namespace UI
} // namespace ECSEngine
