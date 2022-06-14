#pragma once
#include "GameResources.h"

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
} // namespace UI
} // namespace ECSEngine