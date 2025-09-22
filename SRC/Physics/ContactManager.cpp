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

        bool bOverlap = parBroadPhase.TestOverlap(Current->FFirstBody, Current->FSecondBody);
        if (!bOverlap)
        {
            Contact* ToDestroy = Current;
            Current = Current->FNext;
            DestroyContact(ToDestroy);
            continue;
        }

        // TODO EVALUATE CONTACT POINTS
        Current->Evaluate();

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
        c->FFirstBodyEdge.FPrev = c->FFirstBodyEdge.FNext;
    }
    if (c->FFirstBodyEdge.FNext != nullptr)
    {
        c->FFirstBodyEdge.FNext = c->FFirstBodyEdge.FPrev;
    }
    if (a->FContactList == &c->FFirstBodyEdge)
    {
        a->FContactList = c->FFirstBodyEdge.FNext;
    }

    // remove from second body
    if (c->FSecondBodyEdge.FPrev != nullptr)
    {
        c->FSecondBodyEdge.FPrev = c->FSecondBodyEdge.FNext;
    }
    if (c->FSecondBodyEdge.FNext != nullptr)
    {
        c->FSecondBodyEdge.FNext = c->FSecondBodyEdge.FPrev;
    }
    if (b->FContactList == &c->FSecondBodyEdge)
    {
        b->FContactList = c->FSecondBodyEdge.FNext;
    }

    delete c;
    FContactCount--;
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
            if (Current->FFlags.GetValue(EContactFlag::CT_TOUCHING))
            {
                Color = TouchingContactColor;
            }

            buffer->DrawAABB(handle, aabb1.Min(), aabb1.Max(), Color);
            buffer->DrawAABB(handle, aabb2.Min(), aabb2.Max(), Color);

            if (bDrawContactInformation && bTouching && Current->FFlags.GetValue(EContactFlag::CT_CONTACT_INFO))
            {
                for (const vec3& ContactPoint : Current->FManifold.FPositions)
                {
                    constexpr float Size = 0.05f;
                    buffer->DrawAABB(handle, ContactPoint - Size, ContactPoint + Size, 0xFFFFFFFF);
                    buffer->DrawDebugArrow(handle, ContactPoint, Current->FContactNormal, 0.5f, 0xFFFFFFFF);
                }

                RigidBody* firstBody = PhysicsEngine::Instance().GetRigidBody(Current->FFirstBody);
                RigidBody* secondBody = PhysicsEngine::Instance().GetRigidBody(Current->FSecondBody);
                mat4 tr1 = firstBody->GetTransform();
                mat4 tr2 = secondBody->GetTransform();
                vec4 toCenter = tr2.Column(3) - tr1.Column(3);

                buffer->DrawDebugArrow(handle, tr1.Column(3).xyz(), Normalize(toCenter.xyz()), Length(toCenter), 0xFF00FFFF);

                if (bDebugBoxContact)
                {
                    buffer->DrawDebugArrow(handle, tr1.Column(3).xyz(), Current->SeparationVector, Length(Current->SeparationVector), 0xFF0000FF);
                    buffer->DrawDebugArrow(handle, tr1.Column(3).xyz(), Current->SeparatingAxis, 0.5f, 0x0000FFFF);
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
