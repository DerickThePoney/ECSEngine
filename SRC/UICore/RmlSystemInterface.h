#pragma once
#include <RmlUi/Core/SystemInterface.h>

namespace ECSEngine
{
namespace UI
{
class RmlSystemInterface : public Rml::SystemInterface
{
public:
    virtual double GetElapsedTime() override;
};
} // namespace UI
} // namespace ECSEngine