#pragma once
#include "ContactState.h"
namespace ECSEngine
{
namespace Physics
{
class RigidBody;
struct Contact;

struct VelocityState
{
    vec3 FLinearVelocity;
    vec3 FRotationVelocity;
};

class Island
{
    friend struct ContactSolver;
    friend class IslandManager;

public:
    void Initialise();
    void Reserve(u32 BodyCount, u32 ContactCount);
    void Reset();

    void Add(RigidBody* BodyToAdd);
    void Add(Contact* ContactToAdd);

    void Solve(float parDeltaTime);

private:
    std::vector<RigidBody*> FBodies;
    std::vector<VelocityState> FVelocities;
    std::vector<Contact*> FContacts;
    std::vector<ContactState> FContactStates;
};
} // namespace Physics
} // namespace ECSEngine