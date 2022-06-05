#include "stdafx.h"

#include "RmlUiManager.h"

#ifndef RMLUI_STATIC_LIB
#define RMLUI_STATIC_LIB
#endif
#include "Common/InputManager.h"
#include "Common/Logger.h"
#include "Common/ResourceCache.h"
#include "Rendering/RmlRenderer.h"
#include "RenderingCore/GLFWDisplayWindowHandler.h"
#include "RmlFileInterface.h"
#include "RmlSystemInterface.h"

#include <RmlUi/Core.h>
#if !defined(COMPILE_FINAL) and !defined(COMPILE_PROFILE)
#include <RmlUi/Debugger.h>
#endif

namespace ECSEngine
{
namespace UI
{

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
#if !defined(COMPILE_FINAL) and !defined(COMPILE_PROFILE)
    Rml::Debugger::Initialise(FContext);
#endif

    // Tell RmlUi to load the given fonts.
    success = Rml::LoadFontFace("fonts\\LatoLatin-Regular.ttf");
    AssertRelease(success);
    // Fonts can be registered as fallback fonts, as in this case to display emojis.
    success = Rml::LoadFontFace("fonts\\NotoEmoji-Regular.ttf", true);
    AssertRelease(success);
}

void RmlUiManager::NewFrame()
{
    SCOPED_PROFILE_CLASS(RmlUiManager, NewFrame);
    ProcessInput();

#if !defined(COMPILE_FINAL) and !defined(COMPILE_PROFILE)
    if (Input::GetButtonDown(InputKeyNames::INPUT_KEY_F8) && Input::GetButtonHasChanged(InputKeyNames::INPUT_KEY_F8))
    {
        Rml::Debugger::SetVisible(!Rml::Debugger::IsVisible());
    }
#endif
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

void RmlUiManager::ProcessInput()
{
    // TODO Check if used

    bool mouse, keys;
    Input::GetInputsAlreadyUsed(keys, mouse);

    if (mouse && keys)
        return;

    u32 keymods = GetKeyModifiers();

    if (!mouse)
        mouse = ProcessMouse(keymods);

    if (!keys)
    {
        bool keys = SetKeyboardButtons(keymods);
        SetTextInput();
    }

    Input::SetInputsAlreadyUsed(keys, mouse);
}

bool RmlUiManager::ProcessMouse(u32 parKeyMods)
{
    bool res = true;

    // mouse position
    if (glm::length2(Input::GetMousePositionDelta()) > 0.f)
    {
        FMouseInput = SetMousePositionHasChanged(Input::GetMousePosition(), parKeyMods);
    }
    res = res && FMouseInput;

    // mouse buttons
    bool thisRes = SetMouseButtons(parKeyMods);
    res = res && thisRes;

    // mouse wheel
    const glm::vec2 delta = Input::GetMouseScrollDelta();
    if (glm::length2(delta) > 0.f)
    {
        bool mw = !FContext->ProcessMouseWheel(-delta.y, parKeyMods);
        res = res && mw;
    }
    return res;
}

bool RmlUiManager::SetMousePositionHasChanged(glm::vec2 parPos, u32 parKeyMods) const
{
    if (FContext == nullptr)
        return true;

    return !FContext->ProcessMouseMove((int)parPos.x, (int)parPos.y, parKeyMods);
}

bool RmlUiManager::SetMouseButtons(u32 parKeyMods) const
{
    bool res = true;
    forrange(i, 0, MouseButtons::MOUSE_BUTTON_LAST)
    {
        if (Input::GetMouseButtonHasChanged(i))
        {
            bool button = Input::GetMouseButtonState(i);
            bool thisRes = false;
            if (button)
                thisRes = FContext->ProcessMouseButtonDown(i, parKeyMods);
            else
                thisRes = FContext->ProcessMouseButtonUp(i, parKeyMods);
            res = res && thisRes;
        }
    }
    return !res;
}

bool RmlUiManager::SetKeyboardButtons(u32 parKeyMods) const
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
                thisRes = FContext->ProcessKeyDown(id, parKeyMods);
            else
                thisRes = FContext->ProcessKeyUp(id, parKeyMods);
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

u32 RmlUiManager::GetKeyModifiers() const
{
    u32 res = 0;

    if (Input::IsCtrlDown())
        res |= Rml::Input::KeyModifier::KM_CTRL;
    if (Input::IsAltDown())
        res |= Rml::Input::KeyModifier::KM_ALT;
    if (Input::IsShiftDown())
        res |= Rml::Input::KeyModifier::KM_SHIFT;
    if (Input::IsCapsLock())
        res |= Rml::Input::KeyModifier::KM_CAPSLOCK;

    return res;
}

Rml::ElementDocument* RmlUiManager::LoadDocument(const std::string& parDocumentFile) const
{
    AssertRelease(FContext != nullptr);
    Rml::ElementDocument* doc = FContext->LoadDocument(parDocumentFile);
    return doc;
}

void RmlUiManager::UnloadDocument(Rml::ElementDocument* parDoc) const
{
    AssertRelease(FContext != nullptr);
    FContext->UnloadDocument(parDoc);
}

bool RmlUiManager::CreateDataModel(const std::string& parModelName, MemoryView<const std::pair<std::string, u32*>> parData) const
{
    AssertRelease(FContext != nullptr);
    Rml::DataModelConstructor ctr = FContext->CreateDataModel(parModelName);

    if (!ctr)
        return false;

    foreachitemconst(data, parData) ctr.Bind(data.first, data.second);
    return true;
}

Rml::DataModelConstructor RmlUiManager::CreateDataModel(const std::string& parModelName) const
{
    AssertRelease(FContext != nullptr);
    return FContext->CreateDataModel(parModelName);
}

void RmlUiManager::RemoveDataModel(const std::string& parModelName) const
{
    AssertRelease(FContext != nullptr);
    FContext->RemoveDataModel(parModelName);
}

} // namespace UI
} // namespace ECSEngine