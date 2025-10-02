#pragma once

namespace ECSEngine
{
namespace Physics
{
class Island;
struct RigidBody;
struct VelocityState;
struct Contact;

class ContactSolver
{
public:
    void Initialise(Island* parIsland);

    void PreSolve(float parDeltaTime);
    void Solve();
    void Shutdown();

private:
    Island* FIsland = nullptr;
};
} // namespace Physics
} // namespace ECSEngine