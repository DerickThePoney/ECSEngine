#include "stdafx.h"

#include "LoggerGUI.h"

#include <ctime>

namespace ECSEngine
{
namespace ImGUITools
{

void DrawLogger(const std::vector<MessageRecord>& parRecords, bool drawOwnWindow, bool* open)
{
    static bool showMessages[ELoggingCategory::LENGTH] = { true, true, true, true, true, true, true, true };
    static glm::vec4 colors[ELoggingCategory::LENGTH] = {

        glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
        glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.7f, 0.0f, 1.0f)

    };

    ImGui::SetNextWindowSize(ImVec2(600.f, 400.f), ImGuiCond_Once);
    if (drawOwnWindow)
        ImGui::Begin("Logger window", open);

    const glm::vec2 currentWindowSize = ImGui::GetWindowSize();
    const glm::vec2 loggerSize = currentWindowSize - 50.0f;
    const float messagePlaceProportion = 0.75f;
    const glm::vec2 messagePlace = loggerSize * glm::vec2(messagePlaceProportion, 1.0f);
    const glm::vec2 checkboxesPlace = loggerSize * glm::vec2(1 - messagePlaceProportion, 1.0f);
    ImGui::SetCursorPosX(((currentWindowSize - loggerSize) * 0.5f).x);

    ImGui::BeginChildFrame(ImGui::GetID("Test"), messagePlace);
    ImGui::Columns(2, "LoggerColumns", true);
    ImGui::Separator();
    ImGui::Text("Message");
    ImGui::NextColumn();
    ImGui::Text("Date");
    ImGui::NextColumn();
    ImGui::Separator();

    reverseforrange(i, 0, parRecords.size())
    {
        if (!showMessages[parRecords[i].Category])
            continue;

        ImGui::PushID((int)i);
        ImGui::TextColored(colors[parRecords[i].Category], parRecords[i].Message.c_str());
        ImGui::NextColumn();
        std::time_t today_time = std::chrono::system_clock::to_time_t(parRecords[i].LogTime);
        std::stringstream sstr;
        sstr << std::ctime(&today_time);
        ImGui::TextColored(colors[parRecords[i].Category], sstr.str().c_str());
        ImGui::NextColumn();
        ImGui::PopID();
    }

    ImGui::Columns(1);
    ImGui::Separator();
    ImGui::EndChildFrame();

    ImGui::SameLine();

    ImGui::BeginChild("MessgageCheckboxes", checkboxesPlace, true);

    if (ImGui::Button("Select all"))
    {
        forrange(i, 0, ELoggingCategory::LENGTH) { showMessages[i] = true; }
    }
    ImGui::SameLine(0.f, 20.f);
    if (ImGui::Button("Select None"))
    {
        forrange(i, 0, ELoggingCategory::LENGTH) { showMessages[i] = false; }
    }

    ImGui::Separator();

    forrange(i, 0, ELoggingCategory::LENGTH)
    {
        ImGui::PushID((int)i);
        ImGui::Checkbox("", &showMessages[i]);
        ImGui::PopID();
        ImGui::SameLine();
        ImGui::TextColored(colors[i], ELoggingCategory::GetName((ELoggingCategory::Type)i));
    }

    ImGui::EndChild();

    if (drawOwnWindow)
        ImGui::End();
}

} // namespace ImGUITools
} // namespace ECSEngine