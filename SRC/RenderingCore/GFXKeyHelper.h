#pragma once
#include "Common/Singleton.h"
namespace ECSEngine
{
class GFXKeyHelper : public Singleton<GFXKeyHelper>
{
public:
    void Initialise();

    u32 Position;
    u32 Orientation;
    u32 Visible;
    u32 Selectable;
};
} // namespace ECSEngine
