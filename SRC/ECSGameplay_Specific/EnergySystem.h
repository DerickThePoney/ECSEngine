#pragma once
#include "Common/Singleton.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class EnergySystem : public ModuleSystem, public Singleton<EnergySystem>
{
public:
    EnergySystem();

    float ConsumedToProducedEnergyRatio() const;

    i32 TotalEnergy() const;

    // Hackos
    void Finalize();
    static void Delete();

protected:
    virtual void VirtualUpdate() override;

private:
    u32 FProducedEnergy = 0;
    u32 FConsumedEnergy = 0;
    float FConsumedToProducedEnergyRatio = 1.f;
};
} // namespace ECSEngine
