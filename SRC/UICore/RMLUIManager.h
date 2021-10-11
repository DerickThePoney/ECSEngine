#pragma once
#include "Common/MemoryView.h"
#include "Common/Singleton.h"

namespace Rml
{
class Context;
class ElementDocument;
class DataModelConstructor;
} // namespace Rml

namespace ECSEngine
{
namespace Rendering
{
class RmlRenderer;
}
namespace UI
{
class RmlSystemInterface;
class RmlFileInterface;
class RmlUiManager : public Singleton<RmlUiManager>
{
public:
    void Initialise();
    void NewFrame();
    void Update();
    void Render();
    void Shutdown();

    Rml::ElementDocument* LoadDocument(const std::string& parDocumentFile) const;
    void UnloadDocument(Rml::ElementDocument* parDoc) const;
    bool CreateDataModel(const std::string& parModelName, MemoryView<const std::pair<std::string, u32*>> parData) const;
    Rml::DataModelConstructor CreateDataModel(const std::string& parModelName) const;
    void RemoveDataModel(const std::string& parModelName) const;

private:
    void ProcessInput();
    bool ProcessMouse(u32 parKeyMods);
    bool SetMousePositionHasChanged(glm::vec2 parPos, u32 parKeyMods) const;
    bool SetMouseButtons(u32 parKeyMods) const;
    bool SetKeyboardButtons(u32 parKeyMods) const;
    void SetTextInput() const;
    u32 GetKeyModifiers() const;

private:
    Rml::Context* FContext = nullptr;
    RmlSystemInterface* FSystemInterface = nullptr;
    Rendering::RmlRenderer* FRenderInterface = nullptr;
    RmlFileInterface* FFileInterface = nullptr;

    bool FMouseInput = false;
    bool FKeyboardInput = false;
};
} // namespace UI
} // namespace ECSEngine