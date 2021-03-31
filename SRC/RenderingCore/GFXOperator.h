#pragma once
#include "Common/MemoryView.h"

namespace ECSEngine
{
namespace Rendering
{
enum OperatorMask
{
    NONE = 0,
    APPLY_ON_MESH = 1,
};

class VisualModel;
class Carrier;
class AbstractGFXOperator;
class AbstractGFXOperatorDescriptor
{
public:
    AbstractGFXOperatorDescriptor(const char* parName = "DEFAULT")
        : FName(parName)
    {
    }
    virtual ~AbstractGFXOperatorDescriptor() = default;

    void DrawInEditor();

    const char* Name() const { return FName; }

    virtual AbstractGFXOperator* CreateOperator() const = 0;

    SERIALIZE() { PROPERTYFIELD(Name, ""); }

protected:
    virtual void VirtualDrawInEditor(){};

protected:
    const char* FName = nullptr;
};

class AbstractGFXOperator
{
public:
    virtual void ApplyChangesOnMesh(VisualModel& parModel, const Carrier& parCarrier){};

    virtual OperatorMask GetMask() const = 0;
};

namespace GFXOperatorDescriptorFactory
{
void DestroyManager();
bool RegisterOperatorDescriptor(const char* parOperatorName, AbstractGFXOperatorDescriptor* (*parFactory)());
std::unique_ptr<AbstractGFXOperatorDescriptor> CreateOperator(const char* parOperatorName);
MemoryView<const char*> GetOperatorsList();
} // namespace GFXOperatorDescriptorFactory

#define REGISTER_OPERATOR_FACTORY(TYPE)                                                                                                                                            \
    AbstractGFXOperatorDescriptor* Create##TYPE() { return new TYPE(); }                                                                                                           \
    static const bool s_Registered_##TYPE = GFXOperatorDescriptorFactory::RegisterOperatorDescriptor(#TYPE, &Create##TYPE);

} // namespace Rendering
} // namespace ECSEngine
