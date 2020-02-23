#pragma once

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer;
}
class EntityId;
class EntityTemplate;
class Module;
class ModuleTemplate
{
    friend class EntityTemplate;

protected:
    ModuleTemplate();
    ~ModuleTemplate();

    void Init(const EntityTemplate* parTemplate);

    const EntityTemplate* GetTemplate() const
    {
        AssertRelease(IsInitialised());
        return FTemplate;
    }

public:
    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const = 0;

protected:
#ifdef PERFORM_SECURITY_CHECKS
    bool IsInitialised() const { return FHasBeenInit; }
#endif

private:
    const EntityTemplate* FTemplate;

#ifdef PERFORM_SECURITY_CHECKS
    bool FHasBeenInit;
#endif
};

#define DECLARE_MODULE_TEMPLATE(TYPE)                                                                                                                                              \
public:                                                                                                                                                                            \
    using parent_type = ModuleTemplate;

#define IMPLEMENT_MODULE_TEMPLATE(TYPE, TEMPLATE)                                                                                                                                  \
    ModuleTemplate* CreateTemplate##TEMPLATE() { return new TEMPLATE; }                                                                                                            \
    static bool registered##TEMPLATE = EntityTemplateManagerMethods::RegisterTemplateFactory(ModuleTraits<TYPE>::GetModuleId(), &CreateTemplate##TEMPLATE);
} // namespace ECSEngine