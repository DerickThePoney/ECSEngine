#include "stdafx.h"

#include "RenderingLoader.h"

#include "BGFXRenderer.h"
#include "GLFWDisplayWindowHandler.h"
#include "ImguiRenderer.h"
#include "MaterialManager.h"
#include "MeshManager.h"

namespace ECSEngine
{

bool RenderingLoader::VirtualInitialise()
{
    ILoader::VirtualInitialise();

    Rendering::GLFWDisplayWindowHandler::CreateIFP();
    Rendering::GLFWDisplayWindowHandler::Instance().SetName(FApplicationName);
    Rendering::GLFWDisplayWindowHandler::Instance().Init();

    Rendering::BGFXRenderer::CreateIFP();
    Rendering::BGFXRenderer& rendererInstance = Rendering::BGFXRenderer::Instance();
    rendererInstance.Init();

    Rendering::MaterialManager::Initialise();

    Rendering::ImGUI::Init();

    Rendering::MeshManager::CreateIFP();

    return true;
}

void RenderingLoader::VirtualShutdown()
{
    ILoader::VirtualShutdown();

    Rendering::MeshManager::Destroy();

    Rendering::ImGUI::Shutdown();

    Rendering::MaterialManager::Shutdown();

    Rendering::BGFXRenderer::Instance().Shutdown();
    Rendering::BGFXRenderer::Destroy();

    Rendering::GLFWDisplayWindowHandler::Instance().Shutdown();
    Rendering::GLFWDisplayWindowHandler::Destroy();
}

} // namespace ECSEngine

CEREAL_REGISTER_POLYMORPHIC_RELATION(ECSEngine::ILoader, ECSEngine::RenderingLoader);