#pragma once

namespace ECSEngine
{
namespace ImGUITools
{
using DrawGameRulesEditorFn = void (*)(bool* parOpen, float parMenuBarHeight);

void SetGameRulesEditorDrawer(DrawGameRulesEditorFn fn);
bool HasGameRulesEditor();
void DrawGameRulesEditor(bool* parOpen, float parMenuBarHeight);
} // namespace ImGUITools
} // namespace ECSEngine
