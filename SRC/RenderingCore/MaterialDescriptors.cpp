#include "stdafx.h"

#include "MaterialDescriptors.h"
#include "Application/PropertyDrawer.h"
#include "RenderingPropertyDrawers.h"

namespace ECSEngine
{
namespace Rendering
{
//----------------------------------------------------------------
//          ProgramDescriptor
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(ProgramDescriptor);
ProgramDescriptor::ProgramDescriptor()
    : RefCountedObject()
{
}

ProgramDescriptor::~ProgramDescriptor()
{
}

#ifdef PERFORM_SECURITY_CHECKS
bool ProgramDescriptor::UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const
{
    foreachitemconst(uniform, FUniformsAndTypes)
    {
        if (parName == uniform.first)
        {
            return parType == uniform.second;
        }
    }
    return false;
}
#endif

//----------------------------------------------------------------
//          ProgramDescriptorV2
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(ProgramDescriptorV2);
ProgramDescriptorV2::ProgramDescriptorV2()
    : RefCountedObject()
{
}

ProgramDescriptorV2::~ProgramDescriptorV2()
{
}

namespace ProgramEditor
{
const char* UniformTypeToString(const bgfx::UniformType::Enum parCurrentType)
{
    switch (parCurrentType)
    {
    case bgfx::UniformType::Sampler:
        return "Sampler";
    case bgfx::UniformType::Vec4:
        return "Vec4";
    case bgfx::UniformType::Mat3:
        return "Mat3";
    case bgfx::UniformType::Mat4:
        return "Mat4";
    }

    AssertNotReached();
    return "UNKNOWN";
}
bgfx::UniformType::Enum ChooseUniformType(const bgfx::UniformType::Enum parCurrentType)
{
    bgfx::UniformType::Enum res = parCurrentType;
    if (ImGui::BeginCombo("##ChooseUniform", UniformTypeToString(parCurrentType)))
    {
        if (ImGui::Selectable(UniformTypeToString(bgfx::UniformType::Sampler), parCurrentType == bgfx::UniformType::Sampler))
            res = bgfx::UniformType::Sampler;
        if (parCurrentType == bgfx::UniformType::Sampler)
            ImGui::SetItemDefaultFocus();

        if (ImGui::Selectable(UniformTypeToString(bgfx::UniformType::Vec4), parCurrentType == bgfx::UniformType::Vec4))
            res = bgfx::UniformType::Vec4;
        if (parCurrentType == bgfx::UniformType::Vec4)
            ImGui::SetItemDefaultFocus();

        if (ImGui::Selectable(UniformTypeToString(bgfx::UniformType::Mat3), parCurrentType == bgfx::UniformType::Mat3))
            res = bgfx::UniformType::Mat3;
        if (parCurrentType == bgfx::UniformType::Mat3)
            ImGui::SetItemDefaultFocus();

        if (ImGui::Selectable(UniformTypeToString(bgfx::UniformType::Mat4), parCurrentType == bgfx::UniformType::Mat4))
            res = bgfx::UniformType::Mat4;
        if (parCurrentType == bgfx::UniformType::Mat4)
            ImGui::SetItemDefaultFocus();

        ImGui::EndCombo();
    }

    return res;
}
}

void ProgramDescriptorV2::DrawEditor()
{    
    static char buffer[2048];
    ImGui::Text(FFilename.c_str());
    ImGui::SameLine();
    if (ImGui::Button("ChangeName"))
    {
        ImGui::OpenPopup("Change filename window");
        strcpy(buffer, FFilename.c_str());
    }

    bool dummy = true;
    if (ImGui::BeginPopupModal("Change filename window", &dummy))
    {
        ImGui::InputText("Filename", buffer, 2048);
        if (ImGui::Button("Ok"))
        {
            FFilename = std::string(buffer);
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
    }
    

    FDefaultView = RenderPassId::ChooseInList(FDefaultView);
    if (ImGui::CollapsingHeader("Substitutions"))
    {
        static RenderPassId::Type selectedType = FDefaultView;

        ImGui::PushID("Subt");
        selectedType = RenderPassId::ChooseInList(selectedType);
        ImGui::SameLine();
        if (ImGui::Button("Add substitution"))
        {
            auto it = FRenderPassToShaderMap.find(selectedType);
            if (it == FRenderPassToShaderMap.end())
                FRenderPassToShaderMap.insert_or_assign(selectedType, ShaderSubstitutionList());
        }

        u32 idx = 0;
        foreachitem(substitution, FRenderPassToShaderMap)
        {
            ImGui::Indent();
            ImGui::PushID(idx);
            RenderPassId::Type modifyChosenPass = RenderPassId::ChooseInList(substitution.first);

            ImGui::SameLine();
            const bool deleteSubstitution = ImGui::Button("Delete substitution");

            if (modifyChosenPass != substitution.first)
            {
                auto itFind = FRenderPassToShaderMap.find(modifyChosenPass);
                if (itFind == FRenderPassToShaderMap.end())
                {
                    ShaderSubstitutionList shaderList = substitution.second;
                    FRenderPassToShaderMap.erase(itFind);
                    FRenderPassToShaderMap.insert_or_assign(modifyChosenPass, shaderList);
                    ImGui::PopID();
                    break;
                }
            }

            if (deleteSubstitution)
            {
                auto itFind = FRenderPassToShaderMap.find(substitution.first);
                AssertRelease(itFind != FRenderPassToShaderMap.end());
                FRenderPassToShaderMap.erase(itFind);
                ImGui::PopID();
                break;
            }

            // Vertex Shader
            std::string vertexShader = substitution.second[0];
            EDITOR_PROPERTY_STRING(ShaderType::GetName(ShaderType::VERTEX_SHADER), vertexShader, true, "*.sc");
            if (ImGui::Button("Clear vertex shader"))
                vertexShader.clear();

            // Fragment Shader
            std::string fragmentShader = substitution.second[1];
            EDITOR_PROPERTY_STRING(ShaderType::GetName(ShaderType::FRAGMENT_SHADER), fragmentShader, true, "*.sc");
            if (ImGui::Button("Clear fragment shader"))
                fragmentShader.clear();

            substitution.second[0] = vertexShader;
            substitution.second[1] = fragmentShader;

            idx++;
            ImGui::PopID();
            ImGui::Separator();
            ImGui::Unindent();
        }
        ImGui::PopID();
    }

    if (ImGui::CollapsingHeader("Uniforms")) 
    { 
        ImGui::Indent();
        ImGui::PushID("UniformToAdd");
        
        
        static std::string name = "";
        EDITOR_PROPERTY_STRING("Uniform Name", name, false, "");
        
        static bgfx::UniformType::Enum type = bgfx::UniformType::Vec4;
        type = ProgramEditor::ChooseUniformType(type);
        
        if (ImGui::Button("Add Uniform"))
        {
            FUniformsAndTypes.emplace_back(name, type);
        }        

        ImGui::PopID();

        u32 idx = 0;
        auto  idxToErase = FUniformsAndTypes.end();
        for(auto it = FUniformsAndTypes.begin(); it != FUniformsAndTypes.end(); ++it)
        { 
            ImGui::PushID(idx);
            if (ImGui::Button("X"))
            {
                idxToErase = it;
            }
            ImGui::SameLine();
            EDITOR_PROPERTY_STRING("Uniform Name", it->first, false, "");
            ImGui::SameLine();
            it->second = ProgramEditor::ChooseUniformType(it->second);
            ImGui::PopID();
        }
        if (idxToErase != FUniformsAndTypes.end())
            FUniformsAndTypes.erase(idxToErase);
        ImGui::Unindent();
    }

    if (ImGui::CollapsingHeader("Topology"))
    {
        EDITOR_PROPERTY_MESH_LAYOUT_DESCRIPTION("Layout", FLayoutDescription);
    }
}

#ifdef PERFORM_SECURITY_CHECKS
bool ProgramDescriptorV2::UsesUniformOfType(const std::string& parName, bgfx::UniformType::Enum parType) const
{
    foreachitemconst(uniform, FUniformsAndTypes)
    {
        if (parName == uniform.first)
        {
            return parType == uniform.second;
        }
    }
    return false;
}
#endif

//----------------------------------------------------------------
//          MaterialDescriptor
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(MaterialDescriptor);
MaterialDescriptor::MaterialDescriptor()
    : RefCountedObject()
{
}

MaterialDescriptor::~MaterialDescriptor()
{
}

//----------------------------------------------------------------
//          MaterialDescriptorV2
//----------------------------------------------------------------
IMPLEMENT_POOL_ALLOCATED(MaterialDescriptorV2);
MaterialDescriptorV2::MaterialDescriptorV2()
    : RefCountedObject()
{
}

MaterialDescriptorV2::~MaterialDescriptorV2()
{
}
} // namespace Rendering
}

