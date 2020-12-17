#pragma once
#include "Common/BoundingBox.h"
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"
#include "SceneItems.h"
#include "SceneScenario.h"

namespace ECSEngine
{
template<class T>
class PropertyDrawer
{
public:
    PropertyDrawer() { AssertNotReachedMsg("This draw is not implemented yet !") }

    void ShowProperty() { AssertNotReachedMsg("This draw is not implemented yet !") }
};

template<>
class PropertyDrawer<std::string>
{
public:
    PropertyDrawer(const std::string& parPropertyName, std::string* parProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty(bool parIsFile, const char* parPattern = "")
    {
        if (parIsFile)
            ShowPropertyFile(parPattern);
        else
            ShowPropertyStandard();
    }

private:
    void ShowPropertyFile(const char* parPattern)
    {
        std::vector<std::string> fileList;
        GlobalResourceCache::Instance().FCache->GetFileSystem()->ListResourceFiles(parPattern, fileList);

        int selected = -1;
        forrange(i, 0, fileList.size())
        {
            if (*FProperty == fileList[i])
            {
                selected = (int)i;
                break;
            }
        }

        ImGui::Text(FName.c_str());
        ImGui::SameLine();
        if (ImGui::BeginCombo(("##PropertyCombo" + FName).c_str(), (selected == -1) ? "" : fileList[selected].c_str()))
        {
            forrange(i, 0, fileList.size())
            {
                bool is_selected = (selected == (int)i);
                if (ImGui::Selectable(fileList[i].c_str(), is_selected))
                    selected = (int)i;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (selected != -1)
            *FProperty = fileList[selected];
    }

    void ShowPropertyStandard()
    {
        ImGui::Text(FName.c_str());
        ImGui::SameLine();
        char buff[1024];
        sprintf(buff, "%s", FProperty->c_str());
        ImGui::InputText(("##PropertyEdit" + FName).c_str(), buff, 256);
        *FProperty = std::string(buff);
    }

private:
    std::string FName;
    std::string* FProperty = nullptr;
};

template<>
class PropertyDrawer<glm::vec2>
{
public:
    PropertyDrawer(const std::string& parPropertyName, glm::vec2* parProperty, bool parUseLimits, glm::vec2 parMin, glm::vec2 parMax)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(parUseLimits)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty() { ImGui::InputFloat2(FName.c_str(), (float*)FProperty); }

private:
    std::string FName;
    glm::vec2* FProperty = nullptr;
    bool FUseLimits;
    glm::vec2 FMin;
    glm::vec2 FMax;
};

template<>
class PropertyDrawer<glm::vec3>
{
public:
    PropertyDrawer(const std::string& parPropertyName, glm::vec3* parProperty, bool parUseLimits, glm::vec3 parMin, glm::vec3 parMax)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(parUseLimits)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty() { ImGui::InputFloat3(FName.c_str(), (float*)FProperty); }

private:
    std::string FName;
    glm::vec3* FProperty = nullptr;
    bool FUseLimits;
    glm::vec3 FMin;
    glm::vec3 FMax;
};

template<>
class PropertyDrawer<glm::vec4>
{
public:
    PropertyDrawer(const std::string& parPropertyName, glm::vec4* parProperty, bool parUseLimits, glm::vec4 parMin, glm::vec4 parMax)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(parUseLimits)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty() { ImGui::InputFloat4(FName.c_str(), (float*)FProperty); }
    void EditColor() { ImGui::ColorEdit4(FName.c_str(), (float*)FProperty); }

private:
    std::string FName;
    glm::vec4* FProperty = nullptr;
    bool FUseLimits;
    glm::vec4 FMin;
    glm::vec4 FMax;
};

template<>
class PropertyDrawer<glm::quat>
{
public:
    PropertyDrawer(const std::string& parPropertyName, glm::quat* parProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty() { ImGui::InputFloat4(FName.c_str(), (float*)FProperty); }

private:
    std::string FName;
    glm::quat* FProperty = nullptr;
};

template<>
class PropertyDrawer<float>
{
public:
    PropertyDrawer(const std::string& parPropertyName, float* parProperty, bool parUseLimits, float parMin, float parMax)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(parUseLimits)
        , FIsAngle(false)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    PropertyDrawer(const std::string& parPropertyName, float* parProperty, float parMin, float parMax, bool parAngle)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(true)
        , FIsAngle(parAngle)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty()
    {
        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        if (FUseLimits)
        {
            if (FIsAngle)
            {
                ImGui::SliderAngle(FName.c_str(), FProperty, FMin, FMax);
            }
            else
            {
                ImGui::DragFloat(FName.c_str(), FProperty, .5f, FMin, FMax);
            }
        }
        else
        {
            ImGui::InputFloat(FName.c_str(), (float*)FProperty);
        }
    }

private:
    std::string FName;
    float* FProperty = nullptr;
    bool FUseLimits;
    bool FIsAngle;
    float FMin;
    float FMax;
};

template<>
class PropertyDrawer<i32>
{
public:
    PropertyDrawer(const std::string& parPropertyName, i32* parProperty, bool parUseLimits, i32 parMin, i32 parMax)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(parUseLimits)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty()
    {
        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        if (FUseLimits)
            ImGui::DragInt(FName.c_str(), FProperty, .5f, FMin, FMax);
        else
            ImGui::InputInt(FName.c_str(), FProperty);
    }

private:
    std::string FName;
    i32* FProperty = nullptr;
    bool FUseLimits;
    i32 FMin;
    i32 FMax;
};

template<>
class PropertyDrawer<u32>
{
public:
    PropertyDrawer(const std::string& parPropertyName, u32* parProperty, bool parUseLimits, u32 parMin, u32 parMax)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FUseLimits(parUseLimits)
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty()
    {
        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        if (FUseLimits)
            ImGui::DragInt(FName.c_str(), (i32*)FProperty, .5f, (i32)FMin, (i32)FMax);
        else
            ImGui::InputInt(FName.c_str(), (i32*)FProperty);
    }

private:
    std::string FName;
    u32* FProperty = nullptr;
    bool FUseLimits;
    u32 FMin;
    u32 FMax;
};

template<>
class PropertyDrawer<bool>
{
public:
    PropertyDrawer(const std::string& parPropertyName, bool* parProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    {
        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        ImGui::Checkbox(FName.c_str(), FProperty);
    }

private:
    std::string FName;
    bool* FProperty = nullptr;
};

template<>
class PropertyDrawer<AABB2f>
{
public:
    PropertyDrawer(const std::string& parPropertyName, AABB2f* parProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty()
    {
        const glm::vec2 availableSize = ImGui::GetContentRegionAvail();
        ImGui::SetNextItemWidth(availableSize.x * 0.4f);

        glm::vec2 min = FProperty->Min();
        glm::vec2 max = FProperty->Max();

        ImGui::Text(FName.c_str());
        ImGui::Indent();
        ImGui::InputFloat2(fmt::format("Min##{}", (void*)FProperty).c_str(), (float*)&min, 2);
        ImGui::InputFloat2(fmt::format("Max##{}", (void*)FProperty).c_str(), (float*)&max, 2);
        ImGui::Unindent();

        FProperty->SetMin(min);
        FProperty->SetMax(max);
    }

private:
    std::string FName;
    AABB2f* FProperty = nullptr;
};

template<typename T>
void MakeSimpleProperty(const std::string& parName, T* parProperty)
{
    PropertyDrawer<T> drawer(parName, parProperty, false, T(-1), T(-1));
    drawer.ShowProperty();
}

template<typename T>
void MakePropertyWithLimits(const std::string& parName, T* parProperty, const T& parMin, const T& parMax)
{
    PropertyDrawer<T> drawer(parName, parProperty, true, parMin, parMax);
    drawer.ShowProperty();
}

#define EDITOR_PROPERTY_SIMPLE(NAME, PROPERTY) MakeSimpleProperty(NAME, &PROPERTY);
#define EDITOR_PROPERTY_WITH_LIMITS(NAME, PROPERTY, MIN, MAX) MakePropertyWithLimits(NAME, &PROPERTY, MIN, MAX);
#define EDITOR_PROPERTY_ANGLE(NAME, PROPERTY, MIN, MAX)                                                                                                                            \
    {                                                                                                                                                                              \
        PropertyDrawer<float> drawer(NAME, &PROPERTY, MIN, MAX, true);                                                                                                             \
        drawer.ShowProperty();                                                                                                                                                     \
    }

#define EDITOR_PROPERTY_BOOL(NAME, PROPERTY)                                                                                                                                       \
    {                                                                                                                                                                              \
        PropertyDrawer<bool> drawer(NAME, &PROPERTY);                                                                                                                              \
        drawer.ShowProperty();                                                                                                                                                     \
    }

#define EDITOR_PROPERTY_STRING(NAME, PROPERTY, IS_FILE, PATTERN)                                                                                                                   \
    {                                                                                                                                                                              \
        PropertyDrawer<std::string> drawer(NAME, &PROPERTY);                                                                                                                       \
        drawer.ShowProperty(IS_FILE, PATTERN);                                                                                                                                     \
    }

#define EDITOR_PROPERTY_COLOR(NAME, PROPERTY)                                                                                                                                      \
    {                                                                                                                                                                              \
        PropertyDrawer<glm::vec4> drawer(NAME, &PROPERTY, false, glm::vec4(-1), glm::vec4(-1));                                                                                    \
        drawer.EditColor();                                                                                                                                                        \
    }

#define EDITOR_PROPERTY_BOUNDING_BOX_2D(NAME, PROPERTY)                                                                                                                            \
    {                                                                                                                                                                              \
        PropertyDrawer<AABB2f> drawer(NAME, &PROPERTY);                                                                                                                            \
        drawer.ShowProperty();                                                                                                                                                     \
    }

template<>
class PropertyDrawer<const BaseSceneItem*>
{
public:
    PropertyDrawer(const std::string& parPropertyName, const BaseSceneItem** parProperty, u32* parIdProperty, const SceneScenario* parScene)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FIdProperty(parIdProperty)
        , FScene(parScene)
    {
        AssertRelease(FScene != nullptr);
    }

    void ShowProperty()
    {
        const SceneItemsContainer& sceneItems = FScene->GetSceneItems();
        u32 selected = -1;

        auto itFind = sceneItems.find(*FIdProperty);
        if (itFind != sceneItems.end())
        {
            selected = *FIdProperty;
            AssertRelease(itFind->second != nullptr);
        }

        ImGui::Text(FName.c_str());
        ImGui::SameLine();
        if (ImGui::BeginCombo(("##SceneItemCombo" + FName).c_str(), (selected == -1) ? "No associated item" : itFind->second->GetName().c_str()))
        {
            foreachitemconst(sceneItem, sceneItems)
            {
                bool is_selected = (selected == sceneItem.first);
                if (ImGui::Selectable(sceneItem.second->GetName().c_str(), is_selected))
                    selected = (u32)sceneItem.first;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (selected != -1)
        {
            *FProperty = sceneItems.at(selected).get();
            AssertRelease(*FProperty != nullptr);
            *FIdProperty = (*FProperty)->Id();
        }
    }

private:
    std::string FName;
    const BaseSceneItem** FProperty = nullptr;
    u32* FIdProperty = nullptr;
    const SceneScenario* const FScene;
};

#define EDITOR_PROPERTY_SCENE_ITEM(NAME, SCENE_ITEM_PTR, SCENE_ITEM_ID, SCENE)                                                                                                     \
    {                                                                                                                                                                              \
        PropertyDrawer<const BaseSceneItem*> drawer(NAME, &SCENE_ITEM_PTR, &SCENE_ITEM_ID, SCENE);                                                                                 \
        drawer.ShowProperty();                                                                                                                                                     \
    }

template<class T>
class PropertyDrawer<std::vector<T>>
{
public:
    PropertyDrawer(const std::string& parPropertyName, std::vector<T>* parProperty, bool parFixedSize = false)
        : FName(parPropertyName)
        , FProperty(parProperty)
        , FFixedSize(parFixedSize)
    {
    }

    void ShowProperty()
    {
        if (FProperty == nullptr)
            return;

        if (FFixedSize)
            ShowFixedSize();
        else
            ShowVariableSize();
    }

private:
    void ShowFixedSize()
    {
        if (ImGui::CollapsingHeader(fmt::format("{}##VectorPropertyDrawer", FName).c_str()))
        {
            forrange(i, 0, FProperty->size()) { MakeSimpleProperty(fmt::format("Item_{}", i), &(*FProperty)[i]); }
        }
    }

    void ShowVariableSize()
    {
        if (ImGui::CollapsingHeader(fmt::format("{}##VectorPropertyDrawer", FName).c_str()))
        {
            ImGui::Indent();
            std::vector<T>::iterator itToErase = FProperty->end();
            u32 i = 0;
            u32 action = -1; // 0 erase / 1 up / 2 down
            for (auto element = FProperty->begin(); element != FProperty->end(); ++element, ++i)
            {
                ImGui::PushID(i);
                if (ImGui::Button("X"))
                {
                    itToErase = element;
                    action = 0;
                }
                ImGui::SameLine();
                if (i > 0 && ImGui::Button("UP"))
                {
                    itToErase = element;
                    action = 1;
                }
                ImGui::SameLine();
                if (i < ((u32)FProperty->size() - 1) && ImGui::Button("DOWN"))
                {
                    itToErase = element;
                    action = 2;
                }
                ImGui::SameLine();
                MakeSimpleProperty(fmt::format("Item_{}", i), &(*FProperty)[i]);
                ImGui::PopID();
            }
            ImGui::Unindent();

            if (itToErase != FProperty->end())
            {
                switch (action)
                {
                case 0:
                {
                    FProperty->erase(itToErase);
                    break;
                }
                case 1:
                {
                    auto previousIt = itToErase - 1;
                    std::iter_swap(itToErase, previousIt);
                    break;
                }
                case 2:
                {
                    auto nextIt = itToErase + 1;
                    std::iter_swap(itToErase, nextIt);
                    break;
                }
                default:
                    AssertNotReached();
                }
            }

            if (ImGui::Button(fmt::format("Add {}", FName).c_str()))
            {
                T newElement = 0.5f * ((*FProperty)[0] + (*FProperty)[FProperty->size() - 1]);
                FProperty->push_back(newElement);
            }
        }
    }

private:
    std::string FName;
    std::vector<T>* FProperty = nullptr;
    bool FFixedSize;
};

#define EDITOR_PROPERTY_VECTOR(TYPE, NAME, PROPERTY, FIXED_SIZE)                                                                                                                   \
    {                                                                                                                                                                              \
        PropertyDrawer<std::vector<TYPE>> drawer(NAME, &PROPERTY, FIXED_SIZE);                                                                                                     \
        drawer.ShowProperty();                                                                                                                                                     \
    }

} // namespace ECSEngine
