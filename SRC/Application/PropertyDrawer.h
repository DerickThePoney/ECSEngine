#pragma once
#include "Common/ResourceCache.h"
#include "Common/ResourceFile.h"

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

        if (ImGui::BeginCombo("##PropertyCombo", (selected == -1) ? "" : fileList[selected].c_str()))
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
        ImGui::InputText("##PropertyEdit", buff, 256);
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
    PropertyDrawer(const std::string& parPropertyName, glm::vec3* parProperty)
        : FName(parPropertyName)
        , FProperty(parProperty)
    {
    }

    void ShowProperty() { ImGui::InputFloat3(FName.c_str(), (float*)FProperty); }

private:
    std::string FName;
    glm::vec3* FProperty = nullptr;
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

template<typename T>
void MakeSimpleProperty(const std::string& parName, T* parProperty)
{
    PropertyDrawer<T> drawer(parName, parProperty);
    drawer.ShowProperty();
}

#define EDITOR_PROPERTY_SIMPLE(NAME, PROPERTY) MakeSimpleProperty(NAME, &PROPERTY);

#define EDITOR_PROPERTY_STRING(NAME, PROPERTY, IS_FILE, PATTERN)                                                                                                                   \
    {                                                                                                                                                                              \
        PropertyDrawer<std::string> drawer(NAME, &PROPERTY);                                                                                                                       \
        drawer.ShowProperty(IS_FILE, PATTERN);                                                                                                                                     \
    }
} // namespace ECSEngine
