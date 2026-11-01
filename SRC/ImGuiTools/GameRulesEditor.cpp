#include "stdafx.h"

#include "GameRulesEditor.h"

namespace ECSEngine
{
namespace ImGUITools
{
namespace
{
DrawGameRulesEditorFn GDrawGameRulesEditor = nullptr;
}

void SetGameRulesEditorDrawer(DrawGameRulesEditorFn fn)
{
    GDrawGameRulesEditor = fn;
}

bool HasGameRulesEditor()
{
    return GDrawGameRulesEditor != nullptr;
}

void DrawGameRulesEditor(bool* parOpen, float parMenuBarHeight)
{
    if (GDrawGameRulesEditor != nullptr)
        GDrawGameRulesEditor(parOpen, parMenuBarHeight);
}
} // namespace ImGUITools
} // namespace ECSEngine
