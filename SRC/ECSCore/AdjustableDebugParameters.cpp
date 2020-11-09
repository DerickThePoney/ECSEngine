#include "stdafx.h"

#include "AdjustableDebugParameters.h"

#ifdef ENABLE_DEBUG_PARAMETERS
#include "Common/PoolAllocator.h"
#include "Common/Singleton.h"
namespace ECSEngine
{

//----------------------------------------------------------------
//          IAdjustableDebugParameter
//----------------------------------------------------------------
class IAdjustableDebugParameter
{
public:
    IAdjustableDebugParameter(const char* parName)
        : FName(parName)
    {
    }
    virtual ~IAdjustableDebugParameter() { }

protected:
    const char* Name() const { return FName; }

    void DrawAdjustableDebug() { VirtualDrawAdjustableDebug(); }

protected:
    virtual void VirtualDrawAdjustableDebug() = 0;

private:
    const char* FName = nullptr;
};

class AdjustableDebugParametersManager;

//----------------------------------------------------------------
//          UnsignedAdjustableDebug
//----------------------------------------------------------------
class UnsignedAdjustableDebug : public IAdjustableDebugParameter
{
    DECLARE_POOL_ALLOCATED(UnsignedAdjustableDebug);

public:
    UnsignedAdjustableDebug(const char* parName, u32 parDefaultValue, u32 parMinValue, u32 parMaxValue)
        : FValue(parDefaultValue)
        , FMin(parMinValue)
        , FMax(parMaxValue)
        , IAdjustableDebugParameter(parName)
    {
    }
    ~UnsignedAdjustableDebug() { }

    u32 GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override { ImGui::SliderInt(fmt::format("#{}", Name()).c_str(), (int*)&FValue, FMin, FMax); }

private:
    u32 FValue = 0;
    u32 FMin = 0;
    u32 FMax = 0;
};
IMPLEMENT_POOL_ALLOCATED(UnsignedAdjustableDebug);

//----------------------------------------------------------------
//          BooleanAdjustableDebug
//----------------------------------------------------------------
class BooleanAdjustableDebug : public IAdjustableDebugParameter
{
    DECLARE_POOL_ALLOCATED(BooleanAdjustableDebug);

public:
    BooleanAdjustableDebug(const char* parName, bool parDefaultValue)
        : FValue(parDefaultValue)
        , IAdjustableDebugParameter(parName)
    {
    }

    bool GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override { ImGui::Checkbox(fmt::format("#{}", Name()).c_str(), &FValue); }

private:
    bool FValue;
};

IMPLEMENT_POOL_ALLOCATED(BooleanAdjustableDebug);

//----------------------------------------------------------------
//          FloatAdjustableDebug
//----------------------------------------------------------------
class FloatAdjustableDebug : public IAdjustableDebugParameter
{
    DECLARE_POOL_ALLOCATED(FloatAdjustableDebug);

public:
    FloatAdjustableDebug(const char* parName, float parDefaultValue, float parMinValue, float parMaxValue)
        : FValue(parDefaultValue)
        , FMin(parMinValue)
        , FMax(parMaxValue)
        , IAdjustableDebugParameter(parName)
    {
    }

    bool GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override { ImGui::SliderFloat(fmt::format("#{}", Name()).c_str(), &FValue, FMin, FMax); }

private:
    float FValue;
    float FMin;
    float FMax;
};

IMPLEMENT_POOL_ALLOCATED(FloatAdjustableDebug);

//----------------------------------------------------------------
//          DoubleAdjustableDebug
//----------------------------------------------------------------
class DoubleAdjustableDebug : public IAdjustableDebugParameter
{
    DECLARE_POOL_ALLOCATED(DoubleAdjustableDebug);

public:
    DoubleAdjustableDebug(const char* parName, double parDefaultValue, double parMinValue, double parMaxValue)
        : FValue(parDefaultValue)
        , FMin(parMinValue)
        , FMax(parMaxValue)
        , IAdjustableDebugParameter(parName)
    {
    }

    bool GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override
    {
        ImGui::InputDouble(fmt::format("#{}", Name()).c_str(), &FValue);
        FValue = glm::clamp(FValue, FMin, FMax);
    }

private:
    double FValue;
    double FMin;
    double FMax;
};

IMPLEMENT_POOL_ALLOCATED(DoubleAdjustableDebug);

//----------------------------------------------------------------
//          AdjustableGraph IBaseNode
//----------------------------------------------------------------
class IVisitor;
struct IBaseNode
{
    virtual bool visit(IVisitor* parVisitor) = 0;
};

//----------------------------------------------------------------
//          IVisitor
//----------------------------------------------------------------

struct FamilyNode;
struct ParameterNode;
class IVisitor
{
public:
    bool visit(IBaseNode* parBaseNode) { return parBaseNode->visit(this); }
    virtual bool visit(FamilyNode* parFamilyNode) = 0;
    virtual bool visit(ParameterNode* parParameterNode) = 0;
};

//----------------------------------------------------------------
//          Nodes implementations
//----------------------------------------------------------------
struct FamilyNode : public IBaseNode
{
    DECLARE_POOL_ALLOCATED(FamilyNode);

public:
    const char* Family = nullptr;
    std::vector<std::unique_ptr<IBaseNode>> Children;
    bool visit(IVisitor* parVisitor) override { return parVisitor->visit(this); }
};

struct ParameterNode : public IBaseNode
{
    DECLARE_POOL_ALLOCATED(ParameterNode);

public:
    const char* Name = nullptr;
    std::unique_ptr<IAdjustableDebugParameter> FNode;
    bool visit(IVisitor* parVisitor) override { return parVisitor->visit(this); };
};

IMPLEMENT_POOL_ALLOCATED(FamilyNode);
IMPLEMENT_POOL_ALLOCATED(ParameterNode);

//----------------------------------------------------------------
//          GetOrCreateAdjustableVisitor
//----------------------------------------------------------------
template<class ReturnType, class ParameterType>
class GetOrCreateAdjustableVisitor : public IVisitor
{
public:
    GetOrCreateAdjustableVisitor(const char* parName, const char* parFamily, std::unique_ptr<ParameterType>&& parParameterType);

    virtual bool visit(FamilyNode* parFamilyNode) override;
    virtual bool visit(ParameterNode* parParameterNode) override;

    ReturnType GetValue() const { return FResult; }

private:
    ReturnType FResult;
    const char* FName;
    const char* FFamily;
    std::unique_ptr<ParameterType> FParameterType;
};

template<class ReturnType, class ParameterType>
bool GetOrCreateAdjustableVisitor<ReturnType, ParameterType>::visit(ParameterNode* parParameterNode)
{
    if (parParameterNode->Name != FName)
        return false;

    ParameterType* parameter = dynamic_cast<ParameterType*>(parParameterNode->FNode.get());

    if (parameter == nullptr)
        return false;

    FResult = parameter->GetValue();
    return true;
}

template<class ReturnType, class ParameterType>
bool GetOrCreateAdjustableVisitor<ReturnType, ParameterType>::visit(FamilyNode* parFamilyNode)
{
    if (parFamilyNode->Family != FFamily)
        return false;

    foreachitem(node, parFamilyNode->Children)
    {
        if (node->visit(this))
            return true;
    }

    ParameterNode* newParameterNode = new ParameterNode();
    newParameterNode->Name = FName;
    newParameterNode->FNode = std::move(FParameterType);

    parFamilyNode->Children.push_back(std::unique_ptr<ParameterNode>(newParameterNode));

    bool res = newParameterNode->visit(this);
    AlwaysCheckedAssert(res);
    return true;
}

template<class ReturnType, class ParameterType>
GetOrCreateAdjustableVisitor<ReturnType, ParameterType>::GetOrCreateAdjustableVisitor(const char* parName, const char* parFamily, std::unique_ptr<ParameterType>&& parParameterType)
    : FName(parName)
    , FFamily(parFamily)
    , FParameterType(std::move(parParameterType))
{
}

//----------------------------------------------------------------
//          AdjustableDebugParametersManager
//----------------------------------------------------------------
class AdjustableDebugParametersManager : public Singleton<AdjustableDebugParametersManager>
{

public:
    u32 GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, u32 parMinValue, u32 parMaxValue);
    float GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, float parDefaultValue, float parMinValue, float parMaxValue);
    double GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, double parDefaultValue, double parMinValue, double parMaxValue);
    bool GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, bool parDefaultValue);

private:
    void UseGetOrCreateVisitor(const char* parName, const char* parFamily, IVisitor* parVisitor);

private:
    std::vector<std::unique_ptr<IBaseNode>> FAdjustables;
};

// TODO TEMPLATE THIS
u32 AdjustableDebugParametersManager::GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, u32 parMinValue, u32 parMaxValue)
{
    GetOrCreateAdjustableVisitor<u32, UnsignedAdjustableDebug> visitor(
          parName, parFamily, std::unique_ptr<UnsignedAdjustableDebug>(new UnsignedAdjustableDebug(parName, parDefaultValue, parMinValue, parMaxValue)));

    UseGetOrCreateVisitor(parName, parFamily, &visitor);

    return visitor.GetValue();
}

bool AdjustableDebugParametersManager::GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, bool parDefaultValue)
{
    GetOrCreateAdjustableVisitor<bool, BooleanAdjustableDebug> visitor(
          parName, parFamily, std::unique_ptr<BooleanAdjustableDebug>(new BooleanAdjustableDebug(parName, parDefaultValue)));

    UseGetOrCreateVisitor(parName, parFamily, &visitor);

    return visitor.GetValue();
}

float AdjustableDebugParametersManager::GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, float parDefaultValue, float parMinValue, float parMaxValue)
{
    GetOrCreateAdjustableVisitor<float, FloatAdjustableDebug> visitor(
          parName, parFamily, std::unique_ptr<FloatAdjustableDebug>(new FloatAdjustableDebug(parName, parDefaultValue, parMinValue, parMaxValue)));

    UseGetOrCreateVisitor(parName, parFamily, &visitor);

    return visitor.GetValue();
}

double AdjustableDebugParametersManager::GetOrCreateAdjustableDebugParameter(const char* parName,
      const char* parFamily,
      double parDefaultValue,
      double parMinValue,
      double parMaxValue)
{
    GetOrCreateAdjustableVisitor<double, DoubleAdjustableDebug> visitor(
          parName, parFamily, std::unique_ptr<DoubleAdjustableDebug>(new DoubleAdjustableDebug(parName, parDefaultValue, parMinValue, parMaxValue)));

    UseGetOrCreateVisitor(parName, parFamily, &visitor);

    return visitor.GetValue();
}

void AdjustableDebugParametersManager::UseGetOrCreateVisitor(const char* parName, const char* parFamily, IVisitor* parVisitor)
{
    foreachitem(node, FAdjustables)
    {
        if (node->visit(parVisitor))
            return;
    }

    FamilyNode* newFamily = new FamilyNode();
    FAdjustables.push_back(std::unique_ptr<IBaseNode>(newFamily));
    newFamily->Family = parFamily;

    newFamily->visit(parVisitor);
}

//----------------------------------------------------------------
//          Access methods
//----------------------------------------------------------------

AdjustableDebugParametersManager& GetOrCreate()
{
    if (!AdjustableDebugParametersManager::HasInstance())
        AdjustableDebugParametersManager::CreateIFP();

    return AdjustableDebugParametersManager::Instance();
}

void CreateAdjustables()
{
    GetOrCreate();
}

void DestroyAdjustables()
{
    if (AdjustableDebugParametersManager::HasInstance())
        AdjustableDebugParametersManager::Destroy();
}

u32 GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, u32 parMinValue, u32 parMaxValue)
{
    return GetOrCreate().GetOrCreateAdjustableDebugParameter(parName, parFamily, parDefaultValue, parMinValue, parMaxValue);
}

bool GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, bool parDefaultValue)
{
    return GetOrCreate().GetOrCreateAdjustableDebugParameter(parName, parFamily, parDefaultValue);
}

float GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, float parDefaultValue, float parMinValue, float parMaxValue)
{
    return GetOrCreate().GetOrCreateAdjustableDebugParameter(parName, parFamily, parDefaultValue, parMinValue, parMaxValue);
}

double GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, double parDefaultValue, double parMinValue, double parMaxValue)
{
    return GetOrCreate().GetOrCreateAdjustableDebugParameter(parName, parFamily, parDefaultValue, parMinValue, parMaxValue);
}

} // namespace ECSEngine
#else

namespace ECSEngine
{
void CreateAdjustables()
{
}

void DestroyAdjustables()
{
}
} // namespace ECSEngine

#endif // ENABLE_DEBUG_PARAMETERS
