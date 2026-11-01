#pragma once
#include "GameResources.h"
#pragma once

#ifndef RMLUI_STATIC_LIB
#define RMLUI_STATIC_LIB
#endif

#include "RmlUi/Core/Traits.h"

namespace ECSEngine
{
namespace UI
{
struct UIResourceView
{
    GameResource::Type Resource = GameResource::LENGTH;
    std::string ResourceName;
    int Quantity = 0;
};
using UIResourceArray = std::vector<UIResourceView>;
} // namespace UI
} // namespace ECSEngine

template struct Rml::Family<ECSEngine::UI::UIResourceView>;
template class std::vector<ECSEngine::UI::UIResourceView>;
template struct Rml::Family<ECSEngine::UI::UIResourceArray>;
