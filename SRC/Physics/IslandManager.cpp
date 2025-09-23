#include "stdafx.h"

#include "IslandManager.h"

#include "Contact.h"
#include "Island.h"
#include "PhysicsEngine.h"
#include "RigidBody.h"

namespace ECSEngine
{
namespace Physics
{
void IslandManager::SolveIslands(PhysicsEngine* Engine)
{
    Island IslandToSolve;
    IslandToSolve.Init(Engine->FRigidbodies.size(), Engine->FContactManager.ContactCount());

    for (auto& body : Engine->FRigidbodies)
    {
        body->FFlags.SetBit(ERigidBodyFlag::RB_ISLAND, false);
    }

    std::stack<RigidBody*> stack;
    forrange(i, 0, Engine->FRigidbodies.size())
    {
        RigidBody* seed = Engine->FRigidbodies[i].get();
        if (seed->FFlags.GetValue(ERigidBodyFlag::RB_ISLAND))
        {
            continue;
        }

        // reinit the island
        IslandToSolve.Init(Engine->FRigidbodies.size(), Engine->FContactManager.ContactCount()); //< ???TO OPTIMIZE

        // mark the body as part of the island
        seed->FFlags.SetBit(ERigidBodyFlag::RB_ISLAND, true);

        // push the body on the stack
        stack.push(seed);

        // DFS search on contacts while unwiding the stack
        while (!stack.empty())
        {
            // pop the stack
            RigidBody* body = stack.top();
            stack.pop();

            // add the body to the island
            IslandToSolve.Add(body);

            // TODO awaken + check for static

            // loop through the contacts
            for (ContactEdge* edge = body->FContactList; edge; edge = edge->FNext)
            {
                Contact* c = edge->FContact;
                if (c->FFlags.GetValue(EContactFlag::CT_ISLAND))
                {
                    continue;
                }

                if (!c->FFlags.GetValue(EContactFlag::CT_TOUCHING))
                {
                    continue;
                }

                // sensor ?

                // mark the contact as in the island and add it
                c->FFlags.SetBit(EContactFlag::CT_ISLAND, true);
                IslandToSolve.Add(c);

                // try to add the other body to the list
                RigidBody* other = Engine->GetRigidBody(edge->FOtherBody);
                if (!other || other->FFlags.GetValue(ERigidBodyFlag::RB_ISLAND))
                {
                    continue;
                }
                other->FFlags.SetBit(ERigidBodyFlag::RB_ISLAND, true);
                stack.push(other);
            }
        }

        // solve the island

        // TODO: reset static bodies flags
    }
}
} // namespace Physics
} // namespace ECSEngine
