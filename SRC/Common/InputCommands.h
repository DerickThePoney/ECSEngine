#pragma once
#include "InputManager.h"

namespace ECSEngine
{
struct IInputCommand
{
    virtual bool Evaluate() const = 0;
};

namespace EInputType
{
enum Type
{
    PRESSED,
    RELEASED,
    REPEATED,
    LENGTH
};

const std::string ToString(const Type parInputType);

} // namespace EInputType

struct KeyboardCommand : public IInputCommand
{
    bool Evaluate() const override;

    InputKeyNames::Type FKeyboardKey = InputKeyNames::INPUT_KEY_A;
    EInputType::Type FInputType = EInputType::PRESSED;
    bool FControl = false;
    bool FShift = false;
    bool FAlt = false;

    template<typename Archive>
    void serialize(Archive& ar)
    {
        ar(PROPERTY(KeyboardKey), NAMEDPROPERTY("Control pressed", FControl), NAMEDPROPERTY("Shift pressed", FShift), NAMEDPROPERTY("Alt pressed", FAlt));
    }
};
} // namespace ECSEngine