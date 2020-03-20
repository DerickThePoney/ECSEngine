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
                selected = i;
                break;
            }
        }

        if (ImGui::BeginCombo("##PropertyCombo", (selected == -1) ? "" : fileList[selected].c_str()))
        {
            forrange(i, 0, fileList.size())
            {
                bool is_selected = (selected == i);
                if (ImGui::Selectable(fileList[i].c_str(), is_selected))
                    selected = i;
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

#define PROPERTY_STRING(NAME, PROPERTY, IS_FILE, PATTERN)                                                                                                                          \
    {                                                                                                                                                                              \
        PropertyDrawer<std::string> drawer(NAME, &PROPERTY);                                                                                                                       \
        drawer.ShowProperty(IS_FILE, PATTERN);                                                                                                                                     \
    }
} // namespace ECSEngine
