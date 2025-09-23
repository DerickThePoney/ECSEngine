#pragma once

namespace ECSEngine
{
namespace Physics
{
class RigidBody;
struct Contact;

class Island
{
public:
    void Init(u32 BodyCount, u32 ContactCount);
    void Reset();

    void Add(RigidBody* BodyToAdd);
    void Add(Contact* ContactToAdd);

private:
    std::vector<RigidBody*> FBodies;
    std::vector<Contact*> FContacts;
};
} // namespace Physics
} // namespace ECSEngine