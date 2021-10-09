#include "Common/Singleton.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class EntityId;
class SelectionManager : public Singleton<SelectionManager>, public ModuleSystem
{
public:
    SelectionManager();
    ~SelectionManager() = default;

    const std::set<EntityId>& SelectedUnits() const { return FSelectedUnits; }
    const std::set<EntityId>& HighlightedUnits() const { return FHighlightedUnits; }

protected:
    void VirtualUpdate() override;

private:
    std::set<EntityId> FSelectedUnits;
    std::set<EntityId> FHighlightedUnits;
};
} // namespace ECSEngine