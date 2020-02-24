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

public:
    ModuleTemplate();
    ModuleTemplate(const ModuleTemplate& other) = default;
    ModuleTemplate(ModuleTemplate&& other) = default;

    ModuleTemplate& operator=(const ModuleTemplate& other) = default;
    ModuleTemplate& operator=(ModuleTemplate&& other) = default;

    virtual ~ModuleTemplate();

    void Init(const EntityTemplate* parTemplate);

    const EntityTemplate* GetTemplate() const
    {
        AssertRelease(IsInitialised());
        return FTemplate;
    }
    virtual Module* CreateInstance(const EntityId& parUnitId, const ModuleParameters::ParameterContainer& parParameters) const
    {
        AssertNotReached();
        return nullptr;
    };

    template<class Archive>
    void serialize(Archive& ar)
    {
    }

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