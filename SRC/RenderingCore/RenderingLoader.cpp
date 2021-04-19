#include "stdafx.h"

#include "RenderingLoader.h"

#include "BGFXRenderingBackend.h"
#include "Common/Resource.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceHandle.h"
#include "ECSGameplay_Specific/GameplayFeedbackDrawer.h"
#include "GFXKeyHelper.h"
#include "GFXOperator.h"
#include "GFXRepresentationDescriptorManager.h"
#include "GFXRepresentationManager.h"
#include "GLFWDisplayWindowHandler.h"
#include "ImguiRenderer.h"
#include "MaterialManager.h"
#include "MeshManager.h"
#include "SkelettonManager.h"
#include "TexturesManager.h"

namespace ECSEngine
{

bool RenderingLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    Rendering::GLFWDisplayWindowHandler::CreateIFP();
    Rendering::GLFWDisplayWindowHandler::Instance().SetName(FApplicationName);
    Rendering::GLFWDisplayWindowHandler::Instance().Init();

    Rendering::BGFXRenderingBackend::CreateIFP();
    Rendering::BGFXRenderingBackend& rendererInstance = Rendering::BGFXRenderingBackend::Instance();
    rendererInstance.Init();

    Rendering::TextureManager::CreateIFP();
    Rendering::TextureManager::Instance().Initialise();

    Rendering::MaterialManager::Initialise();

    Rendering::ImGUI::Init();

    Rendering::MeshManager::CreateIFP();

    GFXKeyHelper::CreateIFP();
    GFXKeyHelper::Instance().Initialise();

    Rendering::SkelettonManager::CreateIFP();
    Rendering::GFXRepresentationManager::CreateIFP();

    GameplayFeedbackDrawer::CreateIFP();
    GameplayFeedbackDrawer::Instance().Initialise();

    Rendering::GFXRepresentationDescriptorManager::CreateIFP();
    AssertRelease(Rendering::GFXRepresentationDescriptorManager::HasInstance());
    {
        Resource r(FRepresentationDescriptors);
        auto handle = GlobalResourceCache::Instance().FCache->GetResourceHandle(&r);
        AlwaysCheckedAssert(handle != nullptr);
        if (handle != nullptr)
        {
            ResourceBuffer buff = handle->GetResourceBuffer();
            std::istream istr(&buff, std::istream::in);
            cereal::JSONInputArchive archive(istr);
            archive(NAMEDPROPERTY("GFXRepresentationDescriptors", Rendering::GFXRepresentationDescriptorManager::Instance()));
        }
    }

    return true;
}

void RenderingLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    Rendering::GFXRepresentationDescriptorManager::Destroy();

    GameplayFeedbackDrawer::Instance().Shutdown();
    GameplayFeedbackDrawer::Destroy();

    Rendering::GFXRepresentationManager::Destroy();
    Rendering::SkelettonManager::Destroy();

    Rendering::MeshManager::Destroy();

    Rendering::ImGUI::Shutdown();

    Rendering::MaterialManager::Shutdown();

    Rendering::TextureManager::Instance().Shutdown();
    Rendering::TextureManager::Destroy();

    Rendering::BGFXRenderingBackend::Instance().Shutdown();
    Rendering::BGFXRenderingBackend::Destroy();

    Rendering::GLFWDisplayWindowHandler::Instance().Shutdown();
    Rendering::GLFWDisplayWindowHandler::Destroy();

    Rendering::GFXOperatorDescriptorFactory::DestroyManager();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::RenderingLoader);
