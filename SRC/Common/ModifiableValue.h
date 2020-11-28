#pragma once

namespace ECSEngine
{
namespace ModifierType
{
enum Type
{
    PERCENT,
    FLAT,
    LENGTH
};
const char* GetName(const Type parModifierType);
} // namespace ModifierType

template<typename T>
class ValueModifier
{
public:
    ValueModifier(const T& parValue, const ModifierType::Type parType)
        : FValue(parValue)
        , FType(parType)
    {
    }

    T Value() const { return FValue; }
    ModifierType::Type Type() const { return FType; }

    bool operator==(const ValueModifier<T>& parOther) { return FValue == parOther.FValue && FType == parOther.FType; }

private:
    T FValue;
    ModifierType::Type FType;
};

template<typename T>
class ModifiableValue
{
public:
    ModifiableValue() { }

    void SetInitialValue(const T& parValue)
    {
        FInitialValue = parValue;
        ComputeValue();
    }

    void AddModifier(const ValueModifier<T>& parValueModifier)
    {
        FModifiers.push_back(parValueModifier);
        ComputeValue();
    }

    void RemoveModifer(const ValueModifier<T>& parValueModifier)
    {
        auto itFind = std::find(FModifiers.begin(), FModifiers.end(), parValueModifier);
        AlwaysCheckedAssert(itFind != FModifiers.end());
        if (itFind == FModifiers.end())
            return;
        FModifiers.erase(itFind);
        ComputeValue();
    }

    T InitialValue() const { return FInitialValue; }
    T ComputedValue() const { return FComputedValue; }

    void ComputeValue()
    {
        T flatBonus = T(0);
        T percentBonus = T(0);

        foreachitemconst(modifier, FModifiers)
        {
            switch (modifier.Type())
            {
            case ModifierType::FLAT:
                flatBonus += modifier.Value();
                break;
            case ModifierType::PERCENT:
                percentBonus += modifier.Value();
                break;
            default:
                AssertNotReached();
                break;
            }
        }

        FComputedValue = (FInitialValue + flatBonus) * (T(1) + percentBonus);
    }

    void MakeComputedAsInitialAndRemoveModifiers()
    {
        FInitialValue = FComputedValue;
        FModifiers.clear();
    }


private:
    T FInitialValue = T(0);
    T FComputedValue = T(0);
    std::vector<ValueModifier<T>> FModifiers;
};
} // namespace ECSEngine