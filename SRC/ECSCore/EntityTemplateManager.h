#pragma once
#include "Common/Singleton.h"

namespace ECSEngine
{
class EntityTemplate;
class EntityTemplateManager final : public Singleton<EntityTemplateManager>
{
public:
    EntityTemplateManager()
        : Singleton<EntityTemplateManager>()
    {
    }

    ~EntityTemplateManager();

    EntityTemplate* CreateNewEntityTemplate();
    const EntityTemplate* GetEntityTemplate(u32 parIndex) const;
    const EntityTemplate* GetEntityTemplate(const std::string& parTemplateName) const;
    EntityTemplate* GetEntityTemplateForWriting(u32 parIndex);
    void DeleteEntityTemplate_IKNOWWHATIMDOING(u32 parIndex);

    const u32 GetEntityTemplatesNumber() { return (u32)FEntityTemplates.size(); }

    template<class Archive>
    void save(Archive& ar) const
    {
        ar(PROPERTY(EntityTemplates));
    }

    template<class Archive>
    void load(Archive& ar)
    {
        ar(PROPERTY(EntityTemplates));
        InitAfterLoad();
    }

private:
    void InitAfterLoad();

private:
    std::vector<std::shared_ptr<EntityTemplate>> FEntityTemplates;
};
} // namespace ECSEngine
