#pragma once
#include "Common/InputManager.h"

#include <RmlUi/Core/Input.h>
#include <RmlUi/Core/SystemInterface.h>

namespace ECSEngine
{
namespace UI
{
class RmlSystemInterface : public Rml::SystemInterface
{
public:
    virtual double GetElapsedTime() override;

    Rml::Input::KeyIdentifier ConvertToRml(InputKeyNames::Type parKey) const;
};
} // namespace UI
} // namespace ECSEngine