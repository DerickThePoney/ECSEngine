#pragma once

#include "Joints.h"
#include "PhysicsJointHandle.h"

namespace ECSEngine
{
namespace Physics
{
class Island;

class IJointBatch
{
public:
    virtual ~IJointBatch() = default;

    virtual EJointType GetType() const = 0;
    virtual void Clear() = 0;
    virtual void Add(JointBase* joint) = 0;
    virtual void Initialize(Island* island) = 0;
    virtual void PreSolve(Island* island, float dt) = 0;
    virtual void Solve(Island* island) = 0;
    virtual void Shutdown() = 0;
};

template<class TJoint, class TState, EJointType TypeValue>
class JointBatch : public IJointBatch
{
public:
    EJointType GetType() const override { return TypeValue; }

    void Clear() override
    {
        FJoints.clear();
        FStates.clear();
    }

    void Add(JointBase* joint) override
    {
        AssertRelease(joint != nullptr);
        FJoints.push_back(static_cast<TJoint*>(joint));
    }

protected:
    std::vector<TJoint*> FJoints;
    std::vector<TState> FStates;
};

} // namespace Physics
} // namespace ECSEngine
