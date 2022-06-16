#pragma once
#include "Common/Singleton.h"
#include "ECSCore/ModuleSystem.h"

namespace ECSEngine
{
class EnergySystem : public ModuleSystem, public Singleton<EnergySystem>
{
public:
    EnergySystem();

    float ConsumedToProducedEnergyRatio() const { return FConsumedToProducedEnergyRatio; }
    float EnergyEfficiency() const { return FEnergyEfficiency; }

    i32 TotalAvailableEnergy() const;

    // Hackos
    void Finalize();
    static void Delete();

protected:
    virtual void VirtualUpdate() override;

private:
    u32 FProducedEnergy = 0;
    u32 FConsumedEnergy = 0;
    float FConsumedToProducedEnergyRatio = 1.f;
    float FEnergyEfficiency = 1.f;
};
} // namespace ECSEngine
