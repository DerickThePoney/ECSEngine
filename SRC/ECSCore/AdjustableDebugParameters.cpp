#include "stdafx.h"

#include "AdjustableDebugParameters.h"

#ifdef ENABLE_DEBUG_PARAMETERS
#include "Common/InputManager.h"
#include "Common/PoolAllocator.h"
#include "Common/Singleton.h"
namespace ECSEngine
{
//----------------------------------------------------------------
//          FamilyIterator
//----------------------------------------------------------------
class FamilyIterator
{
public:
    FamilyIterator(const char* parFamily);

    void Advance()
    {
        if (FCurrentLevel < (u32)FSubFamilies.size() - 1)
            FCurrentLevel++;
    }
    void Back()
    {
        if (FCurrentLevel > 0)
            FCurrentLevel--;
    }
    bool Done() const { return FCurrentLevel >= (u32)FSubFamilies.size() - 1; }

    u32 size() const { return (u32)FSubFamilies.size(); }

    const std::string& GetFamily(u32 parIndex)
    {
        AssertRelease(parIndex < (u32)FSubFamilies.size());
        return FSubFamilies[parIndex];
    }

    const std::string& GetCurrentFamily()
    {
        AssertRelease(FCurrentLevel < (u32)FSubFamilies.size());
        return FSubFamilies[FCurrentLevel];
    }

private:
    std::vector<std::string> FSubFamilies;
    u32 FCurrentLevel = 0;
};

FamilyIterator::FamilyIterator(const char* parFamily)
{
    std::string temp(parFamily);

    auto p = temp.find_first_of('/');

    while (p != temp.npos)
    {
        std::string newFamily = temp.substr(0, p);
        temp = temp.substr(p + 1);
        p = temp.find_first_of('/');
        FSubFamilies.push_back(newFamily);
    }
    FSubFamilies.push_back(temp);
}

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

    void DrawAdjustableDebug()
    {
        ImGui::PushID(ImGui::GetID(this));
        VirtualDrawAdjustableDebug();
        ImGui::PopID();
    }

protected:
    const char* Name() const { return FName; }

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
    virtual void VirtualDrawAdjustableDebug() override { ImGui::Checkbox(fmt::format("##{}", Name()).c_str(), &FValue); }

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

    float GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override { ImGui::SliderFloat(fmt::format("##{}", Name()).c_str(), &FValue, FMin, FMax); }

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

    double GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override
    {
        ImGui::InputDouble(fmt::format("##{}", Name()).c_str(), &FValue);
        FValue = Clamp(FValue, FMin, FMax);
    }

private:
    double FValue;
    double FMin;
    double FMax;
};

IMPLEMENT_POOL_ALLOCATED(DoubleAdjustableDebug);

//----------------------------------------------------------------
//          DoubleAdjustableDebug
//----------------------------------------------------------------
class ChoiceAdjustableDebug : public IAdjustableDebugParameter
{
    DECLARE_POOL_ALLOCATED(ChoiceAdjustableDebug);

public:
    ChoiceAdjustableDebug(const char* parName, const u32 parDefaultValue, const char* parChoices)
        : FValue(parDefaultValue)
        , FChoices(parChoices)
        , IAdjustableDebugParameter(parName)
    {
    }

    u32 GetValue() const { return FValue; }

protected:
    virtual void VirtualDrawAdjustableDebug() override
    {
        FamilyIterator choiceIt(FChoices);
        AssertRelease(FValue < choiceIt.size());
        if (ImGui::BeginCombo(fmt::format("##{}", Name()).c_str(), choiceIt.GetFamily(FValue).c_str()))
        {
            forrange(i, 0, choiceIt.size())
            {
                if (ImGui::Selectable(choiceIt.GetFamily((u32)i).c_str(), (u32)i == FValue))
                {
                    FValue = (u32)i;
                }
            }

            ImGui::EndCombo();
        }
    }

private:
    u32 FValue;
    const char* FChoices;
};

IMPLEMENT_POOL_ALLOCATED(ChoiceAdjustableDebug);

//----------------------------------------------------------------
//          AdjustableGraph IBaseNode
//----------------------------------------------------------------
class IVisitor;
struct IBaseNode
{
    virtual ~IBaseNode() { }
    virtual bool accept(IVisitor* parVisitor) = 0;
    virtual void cleanup() = 0;
};

//----------------------------------------------------------------
//          IVisitor
//----------------------------------------------------------------

struct FamilyNode;
struct ParameterNode;
class IVisitor
{
public:
    bool visit(IBaseNode* parBaseNode) { return parBaseNode->accept(this); }
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
    ~FamilyNode();
    std::string Family;
    std::vector<std::unique_ptr<IBaseNode>> Children;
    bool accept(IVisitor* parVisitor) override { return parVisitor->visit(this); }
    void cleanup() override
    {
        foreachitem(child, Children)
        {
            child->cleanup();
            child.reset(nullptr);
        }
        Children.clear();
    }
}; // namespace ECSEngine

FamilyNode::~FamilyNode()
{
    cleanup();
}

struct ParameterNode : public IBaseNode
{
    DECLARE_POOL_ALLOCATED(ParameterNode);

public:
    ~ParameterNode();
    const char* Name = nullptr;
    std::unique_ptr<IAdjustableDebugParameter> FNode;
    bool accept(IVisitor* parVisitor) override { return parVisitor->visit(this); };
    void cleanup() override { FNode.reset(nullptr); }
};

ParameterNode::~ParameterNode()
{
    cleanup();
}

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
    FamilyIterator FFamilyIterator;
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
    if (parFamilyNode->Family != FFamilyIterator.GetCurrentFamily())
        return false;

    const bool isDone = FFamilyIterator.Done();
    FFamilyIterator.Advance();
    foreachitem(node, parFamilyNode->Children)
    {
        if (node->accept(this))
            return true;
    }

    bool res = false;
    if (isDone)
    {
        ParameterNode* newParameterNode = new ParameterNode();
        newParameterNode->Name = FName;
        newParameterNode->FNode = std::move(FParameterType);
        parFamilyNode->Children.push_back(std::unique_ptr<IBaseNode>(newParameterNode));
        res = newParameterNode->accept(this);
    }
    else
    {
        FamilyNode* newFamilyNode = new FamilyNode();
        newFamilyNode->Family = FFamilyIterator.GetCurrentFamily();
        parFamilyNode->Children.push_back(std::unique_ptr<IBaseNode>(newFamilyNode));
        res = newFamilyNode->accept(this);
    }

    AlwaysCheckedAssert(res);
    return true;
}

template<class ReturnType, class ParameterType>
GetOrCreateAdjustableVisitor<ReturnType, ParameterType>::GetOrCreateAdjustableVisitor(const char* parName, const char* parFamily, std::unique_ptr<ParameterType>&& parParameterType)
    : FName(parName)
    , FFamily(parFamily)
    , FFamilyIterator(parFamily)
    , FParameterType(std::move(parParameterType))
{
}

//----------------------------------------------------------------
//          DrawAdjustablesVisitor
//----------------------------------------------------------------
class DrawAdjustablesVisitor : public IVisitor
{
public:
    virtual bool visit(FamilyNode* parFamilyNode) override;
    virtual bool visit(ParameterNode* parParameterNode) override;
};

bool DrawAdjustablesVisitor::visit(FamilyNode* parFamilyNode)
{
    if (ImGui::CollapsingHeader(parFamilyNode->Family.c_str()))
    {
        ImGui::Indent();
        foreachitem(child, parFamilyNode->Children) { child->accept(this); }
        ImGui::Unindent();
    }
    return true;
}

bool DrawAdjustablesVisitor::visit(ParameterNode* parParameterNode)
{
    ImGui::Text(parParameterNode->Name);
    ImGui::SameLine();
    parParameterNode->FNode->DrawAdjustableDebug();
    return true;
}

//----------------------------------------------------------------
//          AdjustableDebugParametersManager
//----------------------------------------------------------------
class AdjustableDebugParametersManager : public Singleton<AdjustableDebugParametersManager>
{

public:
    ~AdjustableDebugParametersManager();
    u32 GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, u32 parMinValue, u32 parMaxValue);
    float GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, float parDefaultValue, float parMinValue, float parMaxValue);
    double GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, double parDefaultValue, double parMinValue, double parMaxValue);
    bool GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, bool parDefaultValue);
    u32 GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, const char* parChoices);

    void DrawDebugs();

private:
    void UseGetOrCreateVisitor(const char* parName, const char* parFamily, IVisitor* parVisitor);

private:
    std::vector<std::unique_ptr<IBaseNode>> FAdjustables;
};

AdjustableDebugParametersManager::~AdjustableDebugParametersManager()
{
    foreachitem(node, FAdjustables)
    {
        node->cleanup();
        node.reset(nullptr);
    }
    FAdjustables.clear();
}

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

u32 AdjustableDebugParametersManager::GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, const char* parChoices)
{
    GetOrCreateAdjustableVisitor<u32, ChoiceAdjustableDebug> visitor(parName, parFamily, std::make_unique<ChoiceAdjustableDebug>(parName, parDefaultValue, parChoices));

    UseGetOrCreateVisitor(parName, parFamily, &visitor);

    return visitor.GetValue();
}

void AdjustableDebugParametersManager::DrawDebugs()
{
    DrawAdjustablesVisitor visitor;
    foreachitem(node, FAdjustables) { node->accept(&visitor); }
}

void AdjustableDebugParametersManager::UseGetOrCreateVisitor(const char* parName, const char* parFamily, IVisitor* parVisitor)
{
    foreachitem(node, FAdjustables)
    {
        if (node->accept(parVisitor))
            return;
    }

    FamilyIterator tempIt = FamilyIterator(parFamily);
    FamilyNode* newFamily = new FamilyNode();
    FAdjustables.push_back(std::unique_ptr<IBaseNode>(newFamily));
    newFamily->Family = tempIt.GetCurrentFamily();

    newFamily->accept(parVisitor);
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

void DrawAdjustables()
{
    static bool open = false;
    if (Input::GetButtonDown(InputKeyNames::INPUT_KEY_F1) && Input::GetButtonHasChanged(InputKeyNames::INPUT_KEY_F1))
        open = !open;

    if (open)
    {
        ImGui::Begin("Debug values", NULL, ImGuiWindowFlags_AlwaysAutoResize);

        GetOrCreate().DrawDebugs();

        ImGui::End();
    }
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

u32 GetOrCreateAdjustableDebugParameter(const char* parName, const char* parFamily, u32 parDefaultValue, const char* parChoices)
{
    return GetOrCreate().GetOrCreateAdjustableDebugParameter(parName, parFamily, parDefaultValue, parChoices);
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

void DrawAdjustables()
{
}
} // namespace ECSEngine

#endif // ENABLE_DEBUG_PARAMETERS
