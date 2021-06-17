#pragma once
#include "Common/PoolAllocator.h"
#include "Font.h"
#include "Widget.h"

namespace ECSEngine
{
namespace UI
{
class LabelWidgetDescriptor : public WidgetDescriptor
{
public:
    u32 TextColor() const { return FTextColor; }
    void SetTextColor(const u32 parTextColor) { FTextColor = parTextColor; }

    const std::string& TextToDraw() const { return FTextToDraw; }
    void SetTextToDraw(const std::string& parTextToDraw) { FTextToDraw = parTextToDraw; }

    void SetFont(const std::string& parFont, const float parSize)
    {
        FFontName = parFont;
        FFontSize = parSize;
    }

    SERIALIZE()
    {
        ar(cereal::base_class<WidgetDescriptor>(this));
        PROPERTYFIELD(FontName, "");
        PROPERTYFIELD(TextToDraw, "");
        PROPERTYFIELD(TextColor, 0xFFFFFFFF);
        PROPERTYFIELD(FontSize, 30.f);
    }

protected:
    virtual WidgetPtr CreateThisWidget() const override;
    virtual bool VirtualDrawEditor() override;

private:
    std::string FFontName = "";
    std::string FTextToDraw = "";
    u32 FTextColor = 0xFFFFFFFF;
    float FFontSize = 30.f;
};

class SimpleLabel : public Widget
{
    DECLARE_POOL_ALLOCATED(SimpleLabel);

public:
    SimpleLabel(const std::string& parText, const u32 parColor, const std::string& parFontName, const float parFontSize, const WidgetPlacement& parPlacement);

    void SetText(const std::string& parText) { FText = parText; }

protected:
    virtual void VirtualOnDraw(Rendering::UICommandBuffer& parBuffer);

private:
    Rendering::MaterialInstanceHandle FMaterial;
    std::string FText = "";
    u32 FTextColor = 0xFFFFFFFF;
    const Font* FFont = nullptr;
    float FFontSize = 30.f;
};
} // namespace UI
} // namespace ECSEngine
