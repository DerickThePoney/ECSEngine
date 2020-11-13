#pragma once
#include "Common/Singleton.h"
#include "EntityTemplate.h"

namespace ECSEngine
{
namespace EntityTemplateManagerMethods
{
bool RegisterTemplateFactory(const u32 parId, ModuleTemplate* (*parFactory)());
ModuleTemplate* CreateModuleTemplate(const u32 parId);
const std::map<u32, std::string>& GetModuleList();
void Cleanup();
} // namespace EntityTemplateManagerMethods

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
        foreachitem(temp, FEntityTemplates) { temp->Initialise(); }
    }

private:
    std::vector<std::shared_ptr<EntityTemplate>> FEntityTemplates;
};
} // namespace ECSEngine