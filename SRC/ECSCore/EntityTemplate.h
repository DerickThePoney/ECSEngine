#pragma once
#include "EntityModuleKey.h"
#include "WorldIds_fwd.h"

namespace ECSEngine
{
class ModuleTemplate;
class EntityTemplate
{
public:
    EntityTemplate();

    EntityTemplate(const EntityTemplate& other) = delete;
    EntityTemplate& operator=(const EntityTemplate& other) = delete;
    ~EntityTemplate();

    template<typename T>
    const bool HasModule() const
    {
        return FKey.HasModule<T>();
    }

    const bool HasModule(const u32 parModuleId) const { return FKey.HasModule(parModuleId); }

    template<typename Module>
    void SetHasModule();

    const void SetHasModule(const u32 parModuleId)
    {
        FKey.SetHasModule(parModuleId);
        AddModule(parModuleId);
    }

    void Initialise();

    void AddModule(const u32 parId);

    const EEntityWorlds& GetWorldId() const { return FWorld; }
    void SetWorldId_IKnowWhatImDoing(EEntityWorlds parWorld) { FWorld = parWorld; }

    template<typename Module>
    const ModuleTemplate* GetModuleTemplate() const;

    const ModuleTemplate* GetModuleTemplate(const u32 parId) const;

    const std::string& GetName() const { return FName; }

#ifdef PERFORM_SECURITY_CHECKS
    bool IsInitialised() const { return FHasBeenInit; }
#endif

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(PROPERTY(Name), PROPERTY(World), PROPERTY(Key), NAMEDPROPERTY("ModuleTemplatesList", FModuleTemplates));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(PROPERTY(Name), PROPERTY(World), PROPERTY(Key), NAMEDPROPERTY("ModuleTemplatesList", FModuleTemplates));

        UpdateKey();
    }

    void DrawEditor();

private:
    template<typename T>
    void RemoveModule()
    {
        FKey.RemoveModule<T>();
    }

    const void RemoveModule(const u32 parModuleId) { return FKey.RemoveModule(parModuleId); }

    void UpdateKey();

private:
    EEntityWorlds FWorld;
    EntityModuleKey FKey;
    std::map<u32, std::unique_ptr<ModuleTemplate>> FModuleTemplates;

    std::string FName;
#ifdef PERFORM_SECURITY_CHECKS
    bool FHasBeenInit;
#endif
};

} // namespace ECSEngine
