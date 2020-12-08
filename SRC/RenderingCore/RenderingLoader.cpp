#include "stdafx.h"

#include "RenderingLoader.h"

#include "BGFXRenderingBackend.h"
#include "GFXKeyHelper.h"
#include "GFXRepresentationManager.h"
#include "GLFWDisplayWindowHandler.h"
#include "ImguiRenderer.h"
#include "MaterialManager.h"
#include "MeshManager.h"
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

    Rendering::GFXRepresentationManager::CreateIFP();

    return true;
}

void RenderingLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    Rendering::GFXRepresentationManager::Destroy();

    Rendering::MeshManager::Destroy();

    Rendering::ImGUI::Shutdown();

    Rendering::MaterialManager::Shutdown();

    Rendering::TextureManager::Instance().Shutdown();
    Rendering::TextureManager::Destroy();

    Rendering::BGFXRenderingBackend::Instance().Shutdown();
    Rendering::BGFXRenderingBackend::Destroy();

    Rendering::GLFWDisplayWindowHandler::Instance().Shutdown();
    Rendering::GLFWDisplayWindowHandler::Destroy();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::RenderingLoader);