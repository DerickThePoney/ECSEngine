#include "stdafx.h"

#include "RMLUIManager.h"

#define RMLUI_STATIC_LIB
#include "Common/ResourceCache.h"
#include "Rendering/RmlRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RmlSystemInterface.h"

#include <RmlUi/Core.h>

namespace ECSEngine
{
namespace UI
{

struct ApplicationData
{
    bool show_text = true;
    Rml::String animal = "dog";
} my_data;

Rml::Context* context = nullptr;

void RmlUiManager::Initialise()
{
    FSystemInterface = new RmlSystemInterface();

    FRenderInterface = new Rendering::RmlRenderer();
    FRenderInterface->Initialise();

    Rml::SetSystemInterface(FSystemInterface);
    Rml::SetRenderInterface(FRenderInterface);

    bool success = Rml::Initialise();
    AssertRelease(success);

    // Create a context to display documents within.
    auto size = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    context = Rml::CreateContext("main", Rml::Vector2i(size.x, size.y));

    // Tell RmlUi to load the given fonts.
    success = Rml::LoadFontFace(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\fonts\\LatoLatin-Regular.ttf");
    AssertRelease(success);
    // Fonts can be registered as fallback fonts, as in this case to display emojis.
    success = Rml::LoadFontFace(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\fonts\\NotoEmoji-Regular.ttf", true);
    AssertRelease(success);

    // Set up data bindings to synchronize application data.
    if (Rml::DataModelConstructor constructor = context->CreateDataModel("animals"))
    {
        constructor.Bind("show_text", &my_data.show_text);
        constructor.Bind("animal", &my_data.animal);
    }

    Rml::ElementDocument* document = context->LoadDocument(GlobalResourceCache::Instance().FCache->GetBasePath() + "\\UI\\HelloWorld\\helloworld.rml");
    document->Show();

    // Replace and style some text in the loaded document.
    Rml::Element* element = document->GetElementById("world");
    element->SetInnerRML(reinterpret_cast<const char*>(u8"🌍"));
    element->SetProperty("font-size", "1.5em");
}

void RmlUiManager::Update()
{
    context->Update();

    FRenderInterface->OnPreUpdate();
    // Render the user interface. All geometry and other rendering commands are now
    // submitted through the render interface.
    context->Render();
}

void RmlUiManager::Shutdown()
{
    Rml::Shutdown();

    FRenderInterface->Shutdown();
    delete FRenderInterface;
    FRenderInterface = nullptr;

    delete FSystemInterface;
    FSystemInterface = nullptr;
}

} // namespace UI
} // namespace ECSEngine