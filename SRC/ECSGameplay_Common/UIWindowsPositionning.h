#pragma once
namespace ECSEngine
{
namespace UI
{
namespace SIZE_TYPE
{
enum Type : u8
{
    PIXELS,
    PROPORTION,
    LENGTH
};

const char* GetName(Type parType);
} // namespace SIZE_TYPE

struct WindowSizer
{
    WindowSizer() { }
    WindowSizer(float X, float Y)
        : SizeX(X)
        , SizeY(Y)
    {
    }

    WindowSizer(float X, SIZE_TYPE::Type XType, float Y, SIZE_TYPE::Type YType)
        : SizeX(X)
        , SizeY(Y)
    {
    }

    SIZE_TYPE::Type SizeXType = SIZE_TYPE::PROPORTION;
    float SizeX = 0.f;

    SIZE_TYPE::Type SizeYType = SIZE_TYPE::PROPORTION;
    float SizeY = 0.f;

    void PushSize(glm::uvec2 parWindowSize)
    {
        float x = SizeX;
        float y = SizeY;

        if (SizeXType == SIZE_TYPE::PROPORTION)
            x = parWindowSize.x * x;

        if (SizeYType == SIZE_TYPE::PROPORTION)
            y = parWindowSize.y * y;

        ImGui::SetNextWindowSize(glm::vec2(x, y));
    }
};

struct WindowPosition
{
    WindowPosition() { }
    WindowPosition(float X, float Y)
        : PosX(X)
        , PosY(Y)
    {
    }

    WindowPosition(float X, SIZE_TYPE::Type XType, float Y, SIZE_TYPE::Type YType)
        : PosX(X)
        , PosY(Y)
    {
    }

    SIZE_TYPE::Type PosXType = SIZE_TYPE::PROPORTION;
    float PosX = 0.f;

    SIZE_TYPE::Type PosYType = SIZE_TYPE::PROPORTION;
    float PosY = 0.f;

    glm::vec2 WindowAnchor = glm::vec2(0.f);

    void PushPos(glm::uvec2 parWindowSize)
    {
        float x = PosX;
        float y = PosY;

        if (PosXType == SIZE_TYPE::PROPORTION)
            x = parWindowSize.x * x;

        if (PosYType == SIZE_TYPE::PROPORTION)
            y = parWindowSize.y * y;

        ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Always, WindowAnchor);
    }
};
} // namespace UI
} // namespace ECSEngine
