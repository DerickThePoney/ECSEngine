#pragma once
#include "Common/PoolAllocator.h"
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

    virtual const std::string GetName() const = 0;

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

    bool DrawEditor();

#ifdef PERFORM_SECURITY_CHECKS
    void VerifyTemplate();
#endif

protected:
    virtual void VirtualDrawEditor() = 0;

#ifdef PERFORM_SECURITY_CHECKS
    virtual void VirtualVerifyTemplate() const { }

    bool IsInitialised() const { return FHasBeenInit; }
#endif

private:
    const EntityTemplate* FTemplate;

#ifdef PERFORM_SECURITY_CHECKS
    bool FHasBeenInit;
#endif
};

#define DECLARE_MODULE_TEMPLATE(TYPE, TEMPLATE)                                                                                                                                    \
    DECLARE_POOL_ALLOCATED(TEMPLATE);                                                                                                                                              \
                                                                                                                                                                                   \
public:                                                                                                                                                                            \
    using parent_type = ModuleTemplate;                                                                                                                                            \
    const std::string GetName() const override { return #TYPE; }                                                                                                                   \
    static ModuleTemplate* CreateTemplate() { return new TEMPLATE; }                                                                                                               \
    static const std::string StaticGetName() { return #TYPE; }                                                                                                                     \
    static const u32 GetId() { return ECSEngine::ModuleTraits<TYPE>::GetModuleId(); }

#define IMPLEMENT_MODULE_TEMPLATE(TYPE, TEMPLATE)                                                                                                                                  \
    IMPLEMENT_POOL_ALLOCATED(TEMPLATE);                                                                                                                                            \
    ModuleTemplate* CreateTemplate##TEMPLATE() { return TEMPLATE::CreateTemplate(); }                                                                                              \
    static bool gRegister_##TEMPLATE = EntityTemplateManagerMethods::RegisterTemplateFactory(ECSEngine::ModuleTraits<TYPE>::GetModuleId(), &CreateTemplate##TEMPLATE);

} // namespace ECSEngine