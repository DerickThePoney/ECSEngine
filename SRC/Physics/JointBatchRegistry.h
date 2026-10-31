#pragma once

#include "Common/Singleton.h"
#include "IJointBatch.h"
#include "PhysicsJointHandle.h"

namespace ECSEngine
{
namespace Physics
{

class JointBatchRegistry : public Singleton<JointBatchRegistry>
{
public:
    JointBatchRegistry() = default;

    void Initialize();
    void Shutdown();

    IJointBatch* Get(EJointType type) const;
    IJointBatch* const* GetBatches() const { return FBatches; }

private:
    IJointBatch* FBatches[(u8)EJointType::LENGTH] = {};
};

} // namespace Physics
} // namespace ECSEngine
