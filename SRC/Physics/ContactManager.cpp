#include "stdafx.h"

#include "ContactManager.h"

#include "AABBTree.h"
#include "BroadPhase.h"
#include "Contact.h"
#include "PhysicsEngine.h"

#ifdef PERFORM_SECURITY_CHECKS
#include "Common/CameraHelpers.h"
#include "Common/CameraManager.h"
#include "Common/ColorUtils.h"
#include "Common/InputManager.h"
#include "ECSCore/AdjustableDebugParameters.h"
#include "RenderingCore/BGFXRenderingBackend.h"
#include "RenderingCore/DrawCommands.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RenderingCore/MaterialManager.h"
#endif

namespace ECSEngine
{
namespace Physics
{
ContactManager::~ContactManager()
{
    Shutdown();
}

void ContactManager::RemoveBody(const PhysicsBodyHandle& Handle)
{
    Contact* Current = FContactList;
    while (Current != nullptr)
    {
        Contact* CurrentToLookIn = Current;
        Current = Current->FNext;
        if (CurrentToLookIn->FFirstBody == Handle || CurrentToLookIn->FSecondBody == Handle)
        {
            DestroyContact(CurrentToLookIn);
        }
    }
}

void ContactManager::Shutdown()
{
    Contact* Current = FContactList;
    while (Current != nullptr)
    {
        Contact* ThisContact = Current;
        Current = ThisContact->FNext;
        delete ThisContact;
    }
}

void ContactManager::FindNewContacts(BroadPhase& parBroadPhase)
{
    FOverlapingPairDelegate Del = DELEGATE(&ContactManager::AddPotentialContactPair, *this);
    parBroadPhase.UpdatePairs(Del);
}

void ContactManager::CollideContacts(PhysicsEngine* Engine, BroadPhase& parBroadPhase)
{
    Contact* Current = FContactList;
    while (Current != nullptr)
    {
        // TODO: Check if filtering is still valid, bodies are awake and all
        Current->FFlags.SetBit(EContactFlag::CT_ISLAND, false);

        if (!Current->CheckCanStillCollide())
        {
            // The bodies can't collide with each other.
            Contact* ToDestroy = Current;
            Current = Current->FNext;
            DestroyContact(ToDestroy);
            continue;
        }

        bool bOverlap = parBroadPhase.TestOverlap(Current->FFirstBody, Current->FSecondBody);
        if (!bOverlap)
        {
            Contact* ToDestroy = Current;
            Current = Current->FNext;
            DestroyContact(ToDestroy);
            continue;
        }

        // TODO EVALUATE CONTACT POINTS
        ContactManifold OldManifold = Current->FManifold;
        vec3 ot0 = Current->FContactTangents[0];
        vec3 ot1 = Current->FContactTangents[1];
        Current->Evaluate();
        ComputeBasis(Current);

        foreachitem(CP, Current->FManifold.FContactPoints)
        {
            CP.FNormalImpulse = CP.FTangentImpulse[0] = CP.FTangentImpulse[1] = 0.f;

            foreachitem(OCP, OldManifold.FContactPoints)
            {
                if (CP.FP.key == OCP.FP.key)
                {
                    CP.FNormalImpulse = OCP.FNormalImpulse;

                    // Attempt to re-project old friction solutions
                    vec3 friction = ot0 * OCP.FTangentImpulse[0] + ot1 * OCP.FTangentImpulse[1];
                    CP.FTangentImpulse[0] = Dot(friction, Current->FContactTangents[0]);
                    CP.FTangentImpulse[1] = Dot(friction, Current->FContactTangents[1]);
                    break;
                }
            }
        }

        // TODO SENSORS

        Current = Current->FNext;
    }
}

void ContactManager::AddPotentialContactPair(const PhysicsBodyHandle& first, const PhysicsBodyHandle& second)
{
    // 1. check if contact doesn't exist or shouldn't happen
    // - bodies can't collide together
    // - shapes can't collide together
    // - contact filters
    // - contact is already in contact list

    RigidBody* a = PhysicsEngine::Instance().GetRigidBody(first);
    RigidBody* b = PhysicsEngine::Instance().GetRigidBody(second);

    if (a == b)
    {
        return;
    }

    if ((a->FCollisionMask & b->GetCollisionCategoryBit()) == 0 || (b->FCollisionMask & a->GetCollisionCategoryBit()) == 0)
    {
        // The bodies can't collide with each other.
        return;
    }

    ContactEdge* Current = b->FContactList;
    while (Current != nullptr)
    {
        if (Current->FOtherBody == first)
        {
            // Needs updating if several shapes per body
            return;
        }

        Current = Current->FNext;
    }

    // 2. Create a Contact with info -- CLASS EXISTS IN HEADER
    Contact* c = new Contact();
    c->FFirstBody = first;
    c->FSecondBody = second;

    c->FFriction = sqrtf(a->FFriction * b->FFriction);
    c->FRestitution = Max(a->FRestitution, b->FRestitution);

    // 3. Insert it in a contact list
    c->FNext = FContactList;
    if (FContactList != nullptr)
    {
        FContactList->FPrev = c;
    }
    FContactList = c;

    // 4. Connect the edges to:
    // first body
    c->FFirstBodyEdge.FOtherBody = second;
    c->FFirstBodyEdge.FContact = c;
    c->FFirstBodyEdge.FNext = a->FContactList;
    if (a->FContactList != nullptr)
    {
        a->FContactList->FPrev = &c->FFirstBodyEdge;
    }
    a->FContactList = &c->FFirstBodyEdge;

    // second body
    c->FSecondBodyEdge.FOtherBody = first;
    c->FSecondBodyEdge.FContact = c;
    c->FSecondBodyEdge.FNext = b->FContactList;
    if (b->FContactList != nullptr)
    {
        b->FContactList->FPrev = &c->FSecondBodyEdge;
    }
    b->FContactList = &c->FSecondBodyEdge;

    a->SetAwake(true);
    b->SetAwake(true);
    FContactCount++;
}

void ContactManager::DestroyContact(Contact* c)
{
    RigidBody* a = PhysicsEngine::Instance().GetRigidBody(c->FFirstBody);
    RigidBody* b = PhysicsEngine::Instance().GetRigidBody(c->FSecondBody);

    // Remove from contacts list
    if (c->FPrev != nullptr)
    {
        c->FPrev->FNext = c->FNext;
    }

    if (c->FNext != nullptr)
    {
        c->FNext->FPrev = c->FPrev;
    }

    if (c == FContactList)
    {
        FContactList = c->FNext;
    }

    // remove from first body
    if (c->FFirstBodyEdge.FPrev != nullptr)
    {
        c->FFirstBodyEdge.FPrev->FNext = c->FFirstBodyEdge.FNext;
    }
    if (c->FFirstBodyEdge.FNext != nullptr)
    {
        c->FFirstBodyEdge.FNext->FPrev = c->FFirstBodyEdge.FPrev;
    }
    if (a->FContactList == &c->FFirstBodyEdge)
    {
        a->FContactList = c->FFirstBodyEdge.FNext;
    }

    // remove from second body
    if (c->FSecondBodyEdge.FPrev != nullptr)
    {
        c->FSecondBodyEdge.FPrev->FNext = c->FSecondBodyEdge.FNext;
    }
    if (c->FSecondBodyEdge.FNext != nullptr)
    {
        c->FSecondBodyEdge.FNext->FPrev = c->FSecondBodyEdge.FPrev;
    }
    if (b->FContactList == &c->FSecondBodyEdge)
    {
        b->FContactList = c->FSecondBodyEdge.FNext;
    }

    a->SetAwake(true);
    b->SetAwake(true);

    delete c;
    FContactCount--;
}

// http://box2d.org/2014/02/computing-a-basis/
void ContactManager::ComputeBasis(Contact* C)
{
    // Suppose vector a has all equal components and is a unit vector: a = (s, s, s)
    // Then 3*s*s = 1, s = sqrt(1/3) = 0.57735027. This means that at least one component of a
    // unit vector must be greater or equal to 0.57735027. Can use SIMD select operation.
    vec3& a = C->FContactNormal;
    vec3& b = C->FContactTangents[0];
    vec3& c = C->FContactTangents[1];

    if (fabsf(a.x) >= 0.57735027f)
        b = vec3(a.y, -a.x, 0.f);
    else
        b = vec3(0.f, a.z, -a.y);

    b = Normalize(b);
    c = Cross(a, b);
}

#ifdef PERFORM_SECURITY_CHECKS
void ContactManager::DebugDrawContacts(BroadPhase& parBroadPhase)
{
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bShowPotentialContacts, false, "Show potential contacts", "Physics/Contacts");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDrawContactInformation, false, "Draw Contact Information", "Physics/Contacts");
    ADJUSTABLE_DEBUG_PARAMETER_BOOLEAN(bDebugBoxContact, false, "Debug Box Contact", "Physics/Contacts");
    if (bShowPotentialContacts)
    {
        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        static const u32 PotentialContactColor = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 0.f, 1.f));
        static const u32 TouchingContactColor = ColorUtils::ConvertToU32(vec4(0.f, 1.f, 0.f, 1.f));

        Contact* Current = FContactList;
        while (Current != nullptr)
        {
            AABB3f aabb1 = parBroadPhase.GetFatAABB3f(Current->FFirstBody);
            AABB3f aabb2 = parBroadPhase.GetFatAABB3f(Current->FSecondBody);

            u32 Color = PotentialContactColor;
            const bool bTouching = Current->FFlags.GetValue(EContactFlag::CT_TOUCHING);
            if (bTouching)
            {
                Color = TouchingContactColor;
            }

            buffer->DrawAABB(handle, aabb1.Min(), aabb1.Max(), Color);
            buffer->DrawAABB(handle, aabb2.Min(), aabb2.Max(), Color);

            Current = Current->FNext;
        }
        buffer->Submit();
        Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(buffer);
    }

    if (bDrawContactInformation)
    {
        Rendering::DrawCommandBuffer* buffer = Rendering::BGFXRenderingBackend::Instance().CreateCommandBuffer(Rendering::RenderPassId::DEBUG_PASS);
        u32 camId = CameraManager::Instance().CreateCameraIFN("GameplayCamera");
        Camera* camera = CameraManager::Instance().GetCamera(camId);
        buffer->SetViewTranform(camera->GetWorldViewMatrix(), camera->GetProjectionMatrix(Rendering::GLFWDisplayWindowHandler::Instance().AspectRatio()));
        Rendering::MaterialInstanceHandle handle = Rendering::MaterialManager::CreateMaterialInstanceIFN("materials\\vertexcolormaterial.material");

        static const u32 AwakeContactColor = ColorUtils::ConvertToU32(vec4(0.f, 1.f, 0.f, 1.f));
        static const u32 AsleepContactColor = ColorUtils::ConvertToU32(vec4(1.f, 0.f, 0.f, 1.f));

        Contact* Current = FContactList;
        while (Current != nullptr)
        {
            if (Current->FFlags.GetValue(EContactFlag::CT_TOUCHING))
            {
                RigidBody* firstBody = PhysicsEngine::Instance().GetRigidBody(Current->FFirstBody);
                RigidBody* secondBody = PhysicsEngine::Instance().GetRigidBody(Current->FSecondBody);

                const u32 Color = (firstBody->IsAwake()) ? AwakeContactColor : AsleepContactColor;
                for (const ContactPoint& CP : Current->FManifold.FContactPoints)
                {
                    constexpr float Size = 0.05f;
                    buffer->DrawAABB(handle, CP.FPosition - Size, CP.FPosition + Size, Color);
                    buffer->DrawDebugArrow(handle, CP.FPosition, Current->FContactNormal, 0.5f, Color);
                }
            }

            Current = Current->FNext;
        }
        buffer->Submit();
        Rendering::BGFXRenderingBackend::Instance().ReleaseCommandBuffer(buffer);
    }
}
#endif

} // namespace Physics
} // namespace ECSEngine
