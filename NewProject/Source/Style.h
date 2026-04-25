#pragma once
#include <JuceHeader.h>

class Style : public juce::LookAndFeel_V4
{
public:
    static constexpr uint32_t COL_BG = 0xFF0A0F0A; 
    static constexpr uint32_t COL_PANEL = 0xFF0D130D; 
    static constexpr uint32_t COL_BORDER = 0xFF1A2E1A; 
    static constexpr uint32_t COL_KNOB_BG = 0xFF0B100B; 
    static constexpr uint32_t COL_KNOB_RING = 0xFF1E3020; 
    static constexpr uint32_t COL_TEXT = 0xFF4A6B4A; 
    static constexpr uint32_t COL_TEAL = 0xFF4DDB6E;
    static constexpr uint32_t COL_RED = 0xFF2EB870; 
    static constexpr uint32_t COL_AMBER = 0xFF89D98A; 

    Style()
    {
        // background
        setColour(juce::ResizableWindow::backgroundColourId,
            juce::Colour(COL_BG));
        setColour(juce::Slider::backgroundColourId,
            juce::Colour(COL_KNOB_BG));

        // box
        setColour(juce::ComboBox::backgroundColourId,
            juce::Colour(0xFF0A0A0E));
        setColour(juce::ComboBox::outlineColourId,
            juce::Colour(COL_BORDER));
        setColour(juce::ComboBox::textColourId,
            juce::Colour(0xFF888888));
        setColour(juce::ComboBox::arrowColourId,
            juce::Colour(COL_TEXT));

        // menu
        setColour(juce::PopupMenu::backgroundColourId,
            juce::Colour(0xFF0D0D12));
        setColour(juce::PopupMenu::textColourId,
            juce::Colour(0xFF888888));
        setColour(juce::PopupMenu::highlightedBackgroundColourId,
            juce::Colour(0xFF1A1A24));
        setColour(juce::PopupMenu::highlightedTextColourId,
            juce::Colours::white);

        // labels
        setColour(juce::Label::textColourId,
            juce::Colour(COL_TEXT));

        // sliders
        setColour(juce::Slider::textBoxTextColourId,
            juce::Colour(0xFF666680));
        setColour(juce::Slider::textBoxBackgroundColourId,
            juce::Colour(0xFF0A0A0E));
        setColour(juce::Slider::textBoxOutlineColourId,
            juce::Colour(COL_BORDER));
    }

    // =========================================================
    // Rotary knob
    // =========================================================
    void drawRotarySlider(juce::Graphics& g,
        int x, int y, int width, int height,
        float sliderPosProportional,
        float rotaryStartAngle,
        float rotaryEndAngle,
        juce::Slider& slider) override
    {
        auto bounds = juce::Rectangle<float>(x, y, width, height).reduced(4.0f);
        auto centre = bounds.getCentre();
        auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) * 0.5f;
        auto angle = rotaryStartAngle
            + sliderPosProportional * (rotaryEndAngle - rotaryStartAngle);

        // accent
        auto accent = getAccentForSlider(slider);

        // background circle
        g.setColour(juce::Colour(COL_KNOB_BG));
        g.fillEllipse(centre.x - radius, centre.y - radius,
            radius * 2.0f, radius * 2.0f);

        // border ring
        g.setColour(juce::Colour(COL_BORDER));
        g.drawEllipse(centre.x - radius, centre.y - radius,
            radius * 2.0f, radius * 2.0f, 1.0f);

        // arc
        {
            juce::Path track;
            track.addArc(centre.x - radius + 3, centre.y - radius + 3,
                (radius - 3) * 2.0f, (radius - 3) * 2.0f,
                rotaryStartAngle, rotaryEndAngle, true);
            g.setColour(juce::Colour(COL_BORDER));
            g.strokePath(track, juce::PathStrokeType(1.5f,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded));
        }

        // value
        {
            juce::Path arc;
            arc.addArc(centre.x - radius + 3, centre.y - radius + 3,
                (radius - 3) * 2.0f, (radius - 3) * 2.0f,
                rotaryStartAngle, angle, true);
            g.setColour(accent);
            g.strokePath(arc, juce::PathStrokeType(2.0f,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded));
        }

        // circle
        auto innerR = radius * 0.6f;
        g.setColour(juce::Colour(0xFF13131A));
        g.fillEllipse(centre.x - innerR, centre.y - innerR,
            innerR * 2.0f, innerR * 2.0f);
        g.setColour(juce::Colour(COL_KNOB_RING));
        g.drawEllipse(centre.x - innerR, centre.y - innerR,
            innerR * 2.0f, innerR * 2.0f, 0.8f);

        // pointer
        {
            juce::Path pointer;
            float pointerLen = innerR * 0.75f;
            float pointerW = 2.0f;
            pointer.addRectangle(-pointerW * 0.5f, -(innerR - 2.0f),
                pointerW, pointerLen);
            pointer.applyTransform(
                juce::AffineTransform::rotation(angle)
                .translated(centre.x, centre.y));
            g.setColour(juce::Colours::white.withAlpha(0.85f));
            g.fillPath(pointer);
        }

        // pointer tip dot
        {
            float dotX = centre.x + (innerR - 4.0f) * std::sin(angle);
            float dotY = centre.y - (innerR - 4.0f) * std::cos(angle);
            g.setColour(accent.withAlpha(0.9f));
            g.fillEllipse(dotX - 2.0f, dotY - 2.0f, 4.0f, 4.0f);
        }
    }

    // =========================================================
    // Combo Box
    // =========================================================
    void drawComboBox(juce::Graphics& g,
        int width, int height,
        bool ,
        int buttonX, int buttonY,
        int buttonW, int buttonH,
        juce::ComboBox& ) override
    {
        // background
        g.setColour(juce::Colour(0xFF0A0A0E));
        g.fillRoundedRectangle(0.0f, 0.0f, width, height, 3.0f);

        // border
        g.setColour(juce::Colour(COL_BORDER));
        g.drawRoundedRectangle(0.5f, 0.5f, width - 1.0f, height - 1.0f, 3.0f, 0.8f);

        // arrow
        auto arrowX = buttonX + buttonW * 0.5f;
        auto arrowY = buttonY + buttonH * 0.5f;
        juce::Path arrow;
        arrow.addTriangle(arrowX - 4.0f, arrowY - 2.0f,
            arrowX + 4.0f, arrowY - 2.0f,
            arrowX, arrowY + 3.0f);
        g.setColour(juce::Colour(COL_TEXT));
        g.fillPath(arrow);
    }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return juce::Font(juce::Font::getDefaultMonospacedFontName(), 10.0f,
            juce::Font::plain);
    }

    void positionComboBoxText(juce::ComboBox& box,
        juce::Label& label) override
    {
        label.setBounds(6, 0, box.getWidth() - 24, box.getHeight());
        label.setFont(getComboBoxFont(box));
    }

    // =========================================================
    // Label
    // =========================================================
    juce::Font getLabelFont(juce::Label&) override
    {
        return juce::Font(juce::Font::getDefaultMonospacedFontName(), 9.0f,
            juce::Font::plain);
    }

    // =========================================================
    // Slider text box
    // =========================================================
    juce::Font getSliderPopupFont(juce::Slider&) override
    {
        return juce::Font(juce::Font::getDefaultMonospacedFontName(), 9.0f,
            juce::Font::plain);
    }

    int getSliderPopupPlacement(juce::Slider&) override
    {
        return juce::BubbleComponent::below;
    }

private:
    // Returns accent colour 
    juce::Colour getAccentForSlider(juce::Slider& slider)
    {
        auto name = slider.getName();
        if (name.startsWith("bit") || name.startsWith("down"))
            return juce::Colour(COL_TEAL);
        if (name.startsWith("dist") || name.startsWith("drive") || name.startsWith("mix"))
            return juce::Colour(COL_RED);
        if (name.startsWith("morph") || name.startsWith("freq"))
            return juce::Colour(COL_AMBER);
        return juce::Colour(COL_TEAL); 
    }
};