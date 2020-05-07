#pragma once
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
        , FMin(parMin)
        , FMax(parMax)
    {
    }

    void ShowProperty()
    {
        if (FUseLimits)
            ImGui::DragFloat(FName.c_str(), FProperty, .5f, FMin, FMax);
        else
            ImGui::InputFloat(FName.c_str(), (float*)FProperty);
    }

private:
    std::string FName;
    float* FProperty = nullptr;
    bool FUseLimits;
    float FMin;
    float FMax;
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

#define EDITOR_PROPERTY_STRING(NAME, PROPERTY, IS_FILE, PATTERN)                                                                                                                   \
    {                                                                                                                                                                              \
        PropertyDrawer<std::string> drawer(NAME, &PROPERTY);                                                                                                                       \
        drawer.ShowProperty(IS_FILE, PATTERN);                                                                                                                                     \
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
        const std::vector<std::shared_ptr<BaseSceneItem>>& sceneItems = FScene->GetSceneItems();
        u32 selected = -1;

        if (*FIdProperty < sceneItems.size())
        {
            forrange(i, 0, sceneItems.size())
            {
                AssertRelease(sceneItems[i] != nullptr);
                if (sceneItems[i]->Id() == (*FIdProperty))
                {
                    selected = (u32)i;
                    break;
                }
            }
        }

        ImGui::Text(FName.c_str());
        ImGui::SameLine();
        if (ImGui::BeginCombo(("##SceneItemCombo" + FName).c_str(), (selected == -1) ? "No associated item" : sceneItems[selected]->GetName().c_str()))
        {
            forrange(i, 0, sceneItems.size())
            {
                bool is_selected = (selected == (u32)i);
                if (ImGui::Selectable(sceneItems[i]->GetName().c_str(), is_selected))
                    selected = (u32)i;
                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (selected != -1)
        {
            *FProperty = sceneItems[selected].get();
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

#define EDITOR_PROPERTY_SCENE_ITEM(NAME, PROPERTY, NAME_PROPERTY, SCENE)                                                                                                           \
    {                                                                                                                                                                              \
        PropertyDrawer<const BaseSceneItem*> drawer(NAME, &PROPERTY, &NAME_PROPERTY, SCENE);                                                                                       \
        drawer.ShowProperty();                                                                                                                                                     \
    }

} // namespace ECSEngine
