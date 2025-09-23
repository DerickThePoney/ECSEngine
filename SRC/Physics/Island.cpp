#include "stdafx.h"

#include "Island.h"

#include "Contact.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{

void Island::Init(u32 BodyCount, u32 ContactCount)
{
    FBodies.reserve(BodyCount);
    FContacts.reserve(ContactCount);
}

void Island::Reset()
{
    FBodies.clear();
    FContacts.clear();
}

void Island::Add(RigidBody* BodyToAdd)
{
    BodyToAdd->FIslandIndex = FBodies.size();
    FBodies.push_back(BodyToAdd);
}

void Island::Add(Contact* ContactToAdd)
{
    FContacts.push_back(ContactToAdd);
}

} // namespace Physics
} // namespace ECSEngine