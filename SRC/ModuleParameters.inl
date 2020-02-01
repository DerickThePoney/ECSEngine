#ifdef DECLARING_PARAMETERS
namespace ECSEngine
{
namespace ModuleParameters
{
#endif

// template<>
// class ParameterIdentifierTrait<Position> final : public IParameterIdentifierTrait
//{
// public:
//    using InterfaceType = ParameterTypeTrait<glm::vec3>::InterfaceType;
//    using ImplementationType = ParameterTypeTrait<glm::vec3>::ImplementationType;
//    using OriginalType = glm::vec3;
//
// private:
//    constexpr static size_t Size = sizeof(ImplementationType);
//
// public:
//    static_assert(Size <= MaxParameterByteSize, "La taille du paramètre est trop élevée");
//    void Destroy(void* parPtr) const override { ((ImplementationType*)parPtr)->~ImplementationType(); }
//    void Init(const void* parSrc, void* parDst) const override { new (parDst) ImplementationType(*((const ImplementationType*)parSrc)); }
//    const std::type_info& TypeId() const override{ return typeid(ImplementationType) };
//};
DECLARE_MODULE_PARAMETER(Position, glm::vec3)

#ifdef DECLARING_PARAMETERS
}
} // namespace ECSEngine
#endif