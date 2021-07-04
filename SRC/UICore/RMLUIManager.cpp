#include "stdafx.h"

#include "RMLUIManager.h"

#define RMLUI_STATIC_LIB
#include "Common/InputManager.h"
#include "Common/Logger.h"
#include "Common/ResourceCache.h"
#include "Rendering/RmlRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RmlSystemInterface.h"

#include <RmlUi/Core.h>
#if !defined(COMPILE_FINAL) and !defined(ENABLE_PROFILING)
#include <RmlUi/Debugger.h>
#endif
#include "RmlFileInterface.h"

namespace ECSEngine
{
namespace UI
{

struct ApplicationData
{
    bool show_text = true;
    Rml::String animal = "dog";
} my_data;

void RmlUiManager::Initialise()
{
    FSystemInterface = new RmlSystemInterface();

    FRenderInterface = new Rendering::RmlRenderer();
    FRenderInterface->Initialise();

    FFileInterface = new RmlFileInterface();

    Rml::SetSystemInterface(FSystemInterface);
    Rml::SetRenderInterface(FRenderInterface);
    Rml::SetFileInterface(FFileInterface);

    // TODO FILE INTERFACE

    bool success = Rml::Initialise();
    AssertRelease(success);

    // Create a context to display documents within.
    auto size = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    FContext = Rml::CreateContext("main", Rml::Vector2i(size.x, size.y));
#if !defined(COMPILE_FINAL) and !defined(ENABLE_PROFILING)
    Rml::Debugger::Initialise(FContext);
    Rml::Debugger::SetVisible(true);
#endif

    // Tell RmlUi to load the given fonts.
    success = Rml::LoadFontFace("fonts\\LatoLatin-Regular.ttf");
    AssertRelease(success);
    // Fonts can be registered as fallback fonts, as in this case to display emojis.
    success = Rml::LoadFontFace("fonts\\NotoEmoji-Regular.ttf", true);
    AssertRelease(success);

    // Set up data bindings to synchronize application data.
    if (Rml::DataModelConstructor constructor = FContext->CreateDataModel("animals"))
    {
        constructor.Bind("show_text", &my_data.show_text);
        constructor.Bind("animal", &my_data.animal);
    }

    Rml::ElementDocument* document = FContext->LoadDocument("UI\\HelloWorld\\helloworld.rml");
    document->Show();

    // Replace and style some text in the loaded document.
    Rml::Element* element = document->GetElementById("world");
    element->SetInnerRML(reinterpret_cast<const char*>(u8"🌍"));
    element->SetProperty("font-size", "1.5em");
}

void RmlUiManager::NewFrame()
{
    SCOPED_PROFILE_CLASS(RmlUiManager, NewFrame);
    ProcessInput();
}

void RmlUiManager::Update()
{
    SCOPED_PROFILE_CLASS(RmlUiManager, Update);
    NewFrame();
    Rml::Vector2i d = FContext->GetDimensions();
    auto size = Rendering::GLFWDisplayWindowHandler::Instance().GetSize();
    if (size.x != d.x || size.y != d.y)
        FContext->SetDimensions(Rml::Vector2i(size.x, size.y));
    FContext->Update();
}

void RmlUiManager::Render()
{
    SCOPED_PROFILE_CLASS(RmlUiManager, Render);
    FRenderInterface->OnPreUpdate();
    // Render the user interface. All geometry and other rendering commands are now
    // submitted through the render interface.
    FContext->Render();
}

void RmlUiManager::Shutdown()
{
    Rml::Shutdown();

    delete FFileInterface;
    FFileInterface = nullptr;

    FRenderInterface->Shutdown();
    delete FRenderInterface;
    FRenderInterface = nullptr;

    delete FSystemInterface;
    FSystemInterface = nullptr;
}

void RmlUiManager::ProcessInput() const
{
    // TODO Check if used
    bool mouse = ProcessMouse();
    bool keys = SetKeyboardButtons();
    SetTextInput();

    Input::SetInputsAlreadyUsed(keys, mouse);
}

bool RmlUiManager::ProcessMouse() const
{
    bool res = true;
    if (glm::length2(Input::GetMousePositionDelta()) > 0.f)
    {
        bool thisRes = SetMousePositionHasChanged(Input::GetMousePosition());
        res = res && thisRes;
    }

    bool thisRes = SetMouseButtons();
    res = res && thisRes;

    const glm::vec2 delta = Input::GetMouseScrollDelta();
    if (glm::length2(delta) > 0.f)
    {
        bool mw = FContext->ProcessMouseWheel(-delta.y, 0);
        res = res && mw;
    }
    return !res;
}

bool RmlUiManager::SetMousePositionHasChanged(glm::vec2 parPos) const
{
    if (FContext == nullptr)
        return true;

    return !FContext->ProcessMouseMove((int)parPos.x, (int)parPos.y, 0);
}

bool RmlUiManager::SetMouseButtons() const
{
    bool res = true;
    forrange(i, 0, MouseButtons::MOUSE_BUTTON_LAST)
    {
        if (Input::GetMouseButtonHasChanged(i))
        {
            bool button = Input::GetMouseButtonState(i);
            bool thisRes = false;
            if (button)
                thisRes = FContext->ProcessMouseButtonDown(i, 0);
            else
                thisRes = FContext->ProcessMouseButtonUp(i, 0);
            res = res && thisRes;
        }
    }
    return !res;
}

bool RmlUiManager::SetKeyboardButtons() const
{
    bool res = true;
    forrange(i, 0, InputKeyNames::INPUT_KEY_LAST)
    {
        if (Input::GetButtonHasChanged(i))
        {
            bool button = Input::GetButtonDown(i);
            Rml::Input::KeyIdentifier id = FSystemInterface->ConvertToRml((InputKeyNames::Type)i);
            if (id == Rml::Input::KeyIdentifier::KI_UNKNOWN)
                continue;
            bool thisRes = false;
            if (button)
                thisRes = FContext->ProcessKeyDown(id, 0);
            else
                thisRes = FContext->ProcessKeyUp(id, 0);
            res = res && thisRes;
        }
    }
    return res;
}

void RmlUiManager::SetTextInput() const
{
    if (Input::TextInputHasChanged())
    {
        foreachitemconst(c, Input::TextInput())
        {
            Rml::Character cr = (c <= 0xFFFF) ? (Rml::Character)c : Rml::Character::Replacement;
            FContext->ProcessTextInput(cr);
        }
    }
}

} // namespace UI
} // namespace ECSEngine