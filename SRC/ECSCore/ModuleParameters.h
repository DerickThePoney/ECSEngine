#pragma once

namespace ECSEngine
{
namespace ModuleParameters
{
class IParameterIdentifierTrait;

void AddParameterIdentifierTrait(u32 parIdentifier, IParameterIdentifierTrait* parIdentifierTrait_Steal);
void RemoveParameterIdentifierTrait(u32 parIdentifier);
IParameterIdentifierTrait* GetIdentifierTrait(u32 parId);
} // namespace ModuleParameters
} // namespace ECSEngine

namespace ECSEngine
{
namespace ModuleParameters
{
template<typename T>
struct ParameterTypeTrait
{
    using InterfaceType = const T&;
    using ImplementationType = T;

    static const T& GetInterface(const T& t) { return t; }
    static const T& GetImplementation(const T& t) { return t; }
};

template<typename T>
struct ParameterTypeTrait<T&>
{
    using InterfaceType = T&;
    using ImplementationType = T*;

    static T& GetInterface(T* t) { return *t; }
    static T* GetImplementation(T& t) { return &t; }
};

template<typename T>
struct ParameterTypeTrait<T*>
{
    using InterfaceType = T*;
    using ImplementationType = T*;

    static T* GetInterface(T* t) { return t; }
    static T* GetImplementation(T* t) { return t; }
};

class IParameterIdentifierTrait
{
public:
    virtual ~IParameterIdentifierTrait() {}

    virtual void Init(const void* parSrc, void* parDst) const = 0;
    virtual void Destroy(void* parPtr) const = 0;
    virtual const std::type_info& TypeId() const = 0;
};

template<int ID>
class ParameterIdentifierTrait : public IParameterIdentifierTrait
{
public:
    void Init(const void* parSrc, void* parDst) const override { static_assert(false, "Vous avez oublié de déclarer votre parametre !"); }
    void Destroy(void* parPtr) const override { static_assert(false, "Vous avez oublié de déclarer votre parametre !"); }
    const std::type_info& TypeId() const override
    {
        static_assert(false, "Vous avez oublié de déclarer votre parametre !");
        return typeid(0);
    }
};
} // namespace ModuleParameters
} // namespace ECSEngine

namespace ECSEngine
{
namespace ModuleParameters
{
constexpr size_t MaxParameterByteSize = 512;

class ModuleParameter
{
public:
    template<typename T>
    ModuleParameter(u32 parId, const T& parValue)
        : FId(parId)
    {
        static_assert(sizeof(T) < MaxParameterByteSize, "The parameter is too big in byte size");
        new (FData) T(parValue);
    }

    ModuleParameter(const ModuleParameter& parOther)
    {
        memset(&FData, '\0', MaxParameterByteSize);
        FId = parOther.FId;

        IParameterIdentifierTrait* identifierTrait = GetIdentifierTrait(parOther.FId);
        AssertRelease(identifierTrait != nullptr);
        identifierTrait->Init(parOther.FData, FData);
    }

    ~ModuleParameter()
    {
        IParameterIdentifierTrait* identifierTrait = GetIdentifierTrait(FId);
        AssertRelease(identifierTrait != nullptr);
        identifierTrait->Destroy(FData);
    }

    template<typename T>
    void UpdateValue(const T& parValue)
    {
        *((T*)FData) = parValue;
    }

    template<typename T>
    T& GetValue() const
    {
        return *((T*)FData);
    }

    u32 GetId() const { return FId; }

private:
    u32 FId;
    char FData[MaxParameterByteSize];
};
} // namespace ModuleParameters
} // namespace ECSEngine

namespace ECSEngine
{
namespace ModuleParameters
{
class ParameterContainer
{
public:
    ParameterContainer() {}

    template<int ID>
    void Set(typename ParameterIdentifierTrait<ID>::InterfaceType parValue)
    {
        static_assert(ID != 0, "ParameterContainer::Set : Id ne doit pas être 0, c'est dummy !");

        std::vector<ModuleParameter>::iterator it;
        for (it = FDataList.begin(); it != FDataList.end() && (it->GetId() != ID); ++it)
            ;

        if (it == FDataList.end())
            FDataList.push_back(ModuleParameter(ID, ParameterTypeTrait<typename ParameterIdentifierTrait<ID>::OriginalType>::GetImplementation(parValue)));
        else
            it->UpdateValue(ParameterTypeTrait<typename ParameterIdentifierTrait<ID>::OriginalType>::GetImplementation(parValue));
    }

    template<int ID>
    typename ParameterIdentifierTrait<ID>::InterfaceType Get() const
    {
        static_assert(ID != 0, "ParameterContainer::Set : Id ne doit pas être 0, c'est dummy !");
        std::vector<ModuleParameter>::const_iterator it;
        for (it = FDataList.begin(); it != FDataList.end(); ++it)
        {
            if (it->GetId() == ID)
            {
                break;
            }
        }

        AssertRelease(it != FDataList.end());
        return ParameterTypeTrait<typename ParameterIdentifierTrait<ID>::OriginalType>::GetInterface(it->GetValue<typename ParameterIdentifierTrait<ID>::ImplementationType>());
    }

    template<int ID>
    typename ParameterIdentifierTrait<ID>::InterfaceType Get_IFP(typename ParameterIdentifierTrait<ID>::InterfaceType parDefault) const
    {
        static_assert(ID != 0, "ParameterContainer::Set : Id ne doit pas être 0, c'est dummy !");
        std::vector<ModuleParameter>::const_iterator it;
        for (it = FDataList.begin(); it != FDataList.end(); ++it)
        {
            if (it->GetId() == ID)
            {
                break;
            }
        }

        if (it != FDataList.end())
            return ParameterTypeTrait<typename ParameterIdentifierTrait<ID>::OriginalType>::GetInterface(it->GetValue<typename ParameterIdentifierTrait<ID>::ImplementationType>());
        else
            return parDefault;
    }

    template<int ID>
    bool HasParameter() const
    {
        static_assert(ID != 0, "ParameterContainer::Set : Id ne doit pas être 0, c'est dummy !");
        std::vector<ModuleParameter>::const_iterator it;
        for (it = FDataList.begin(); it != FDataList.end(); ++it)
        {
            if (it->GetId() == ID)
            {
                return true;
            }
        }

        return false;
    }

private:
    std::vector<ModuleParameter> FDataList;
};
} // namespace ModuleParameters
} // namespace ECSEngine

namespace ECSEngine
{
namespace ModuleParameters
{
#define DECLARE_MODULE_PARAMETER(ID, TYPE) ID,
enum Identifiers
{
    __DUMMY_IDENTIFIER__,
#include "ModuleParameters.inl"
    __IDENTIFIER_CORE_SIZE__
};
#undef DECLARE_MODULE_PARAMETER

} // namespace ModuleParameters
} // namespace ECSEngine

#define DECLARE_MODULE_PARAMETER(ID, TYPE)                                                                                                                                         \
    template<>                                                                                                                                                                     \
    class ParameterIdentifierTrait<ID> final : public IParameterIdentifierTrait                                                                                                    \
    {                                                                                                                                                                              \
    public:                                                                                                                                                                        \
        using InterfaceType = ParameterTypeTrait<TYPE>::InterfaceType;                                                                                                             \
        using ImplementationType = ParameterTypeTrait<TYPE>::ImplementationType;                                                                                                   \
        using OriginalType = TYPE;                                                                                                                                                 \
                                                                                                                                                                                   \
    private:                                                                                                                                                                       \
        constexpr static size_t Size = sizeof(ImplementationType);                                                                                                                 \
                                                                                                                                                                                   \
    public:                                                                                                                                                                        \
        static_assert(Size <= MaxParameterByteSize, "La taille du paramètre est trop élevée");                                                                                     \
        void Destroy(void* parPtr) const override { ((ImplementationType*)parPtr)->~ImplementationType(); }                                                                        \
        void Init(const void* parSrc, void* parDst) const override { new (parDst) ImplementationType(*((const ImplementationType*)parSrc)); }                                      \
        const std::type_info& TypeId() const override { return typeid(ImplementationType); }                                                                                       \
    };

#define DECLARING_PARAMETERS

#include "ModuleParameters.inl"
#undef DECLARE_MODULE_PARAMETER
#undef DECLARING_PARAMETERS

namespace ECSEngine
{
namespace ModuleParameters
{
void InitParameterIdentifiersTraits();
void DestroyParameterIdentifiersTraits();
} // namespace ModuleParameters
} // namespace ECSEngine
