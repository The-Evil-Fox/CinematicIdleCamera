#include "template/UITemplate.h"

namespace UITemplate {

    // ==================================================================================================================
    //  Setup
    // ==================================================================================================================

    void SaveSettings() {

        IniParser::Save();

    }

    // ==================================================================================================================
    //  Color helper
    //-------------------------------------------------------------------------------------------------------------------
    //  Converts a 24-bit RGB or 32-bit RGBA hex color to ImVec4 with optional alpha override.
    // 
    //  The function extracts RGB from the hex value (ignoring any alpha in the hex itself):
    //    - Bits 16-23: Red channel (0x00 to 0xFF)
    //    - Bits 8-15:  Green channel (0x00 to 0xFF)  
    //    - Bits 0-7:   Blue channel (0x00 to 0xFF)
    //
    //  Alpha is controlled by the a_alpha parameter (default = 1.0f for fully opaque):
    //    0.0f = fully transparent
    //    0.5f = 50% opacity
    //    1.0f = fully opaque
    //
    //  Examples:
    //    HexToImVec4RGBA(Colors::GoldLight)           // Fully opaque gold
    //    HexToImVec4RGBA(0xe74c3c, 0.5f)              // 50% transparent red
    //    HexToImVec4RGBA(Colors::Black, 0.0f)         // Fully transparent black
    // ==================================================================================================================

    ImGuiMCP::ImVec4 HexToImVec4RGBA(uint32_t a_hex, float a_alpha) {

        // Extract RGB from the hex value (works with both 24-bit and 32-bit values, ignoring any alpha in the hex)
        float r = ((a_hex >> 16) & 0xFF) / 255.0f;
        float g = ((a_hex >> 8) & 0xFF) / 255.0f;
        float b = (a_hex & 0xFF) / 255.0f;

        // Alpha is controlled by the a_alpha parameter (the hex's alpha, if any, is ignored)
        return ImGuiMCP::ImVec4{ r, g, b, a_alpha };

    }

    // ==================================================================================================================
    //  Debug log level helper
    // ==================================================================================================================

    spdlog::level::level_enum LoggingLevelToSpdlog(int a_loggingLevel) {

        switch (a_loggingLevel) {

            case 0:  return spdlog::level::critical;
            case 1:  return spdlog::level::warn;
            case 3:  return spdlog::level::debug;
            default: return spdlog::level::info;

        }

    }

    // ==================================================================================================================
    //  Mini-card styling
    // ==================================================================================================================

    void PushMiniCardStyling() {

        ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
        ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, HexToImVec4RGBA(Colors::GrayDark, 0.75f));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, HexToImVec4RGBA(Colors::Gray, 0.75f));

    }

    void PopMiniCardStyling() {

        ImGuiMCP::PopStyleColor(2);
        ImGuiMCP::PopStyleVar(2);

    }

    MiniCardStyleGuard::~MiniCardStyleGuard() {

        if (m_active) {

            PopMiniCardStyling();

        }

    }

    MiniCardStyleGuard::MiniCardStyleGuard(MiniCardStyleGuard&& other) noexcept : m_active(other.m_active) {

        other.m_active = false;

    }

    MiniCardStyleGuard& MiniCardStyleGuard::operator=(MiniCardStyleGuard&& other) noexcept {

        if (this != &other) {

            if (m_active) {

                PopMiniCardStyling();

            }

            m_active = other.m_active;
            other.m_active = false;

        }

        return *this;

    }

    // ==================================================================================================================
    //  Card content line margin helper
    // ==================================================================================================================

    void ApplyCardContentLineMargin(bool a_isSliderLine) {

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentTopMargin));

        if (!a_isSliderLine) {

            ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + k_cardContentLeftMargin);

        }

    }

    // ==================================================================================================================
    //  DrawSettingCard
    // ==================================================================================================================

    static bool HasCardSettingsChanged(const CardContent& a_card) {

        bool changed = false;

        if (a_card.hasSlider && *a_card.sliderValue != a_card.sliderDefault) {

            changed = true;

        }

        if (a_card.hasCheckbox && *a_card.checkboxValue != a_card.checkboxDefault) {

            changed = true;

        }

        if (a_card.hasIntSlider && *a_card.intSliderValue != a_card.intSliderDefault) {

            changed = true;

        }

        return changed;

    }

    static void ResetCardSettings(CardContent& a_card) {

        if (a_card.hasSlider) {

            *a_card.sliderValue = a_card.sliderDefault;

            if (a_card.onSliderChange) {

                a_card.onSliderChange();

            }

        }

        if (a_card.hasCheckbox) {

            *a_card.checkboxValue = a_card.checkboxDefault;

            if (a_card.onCheckboxChange) {

                a_card.onCheckboxChange();

            }

        }

        if (a_card.hasIntSlider) {

            *a_card.intSliderValue = a_card.intSliderDefault;

            if (a_card.onIntSliderChange) {

                a_card.onIntSliderChange();

            }

        }

        if (a_card.onSave) {

            a_card.onSave();

        }

    }

    void DrawSettingCard(const std::string& a_cardId, CardContent& a_card) {

        const bool hasChanges = HasCardSettingsChanged(a_card);

        ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);

        ImGuiMCP::ImVec2 winSize;
        ImGuiMCP::GetWindowSize(&winSize);
        float cardWidth = winSize.x - (k_marginBetweenBordersInHeader * 2);

        ImGuiMCP::BeginChild(a_cardId.c_str(), ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        float startY = ImGuiMCP::GetCursorPosY();
        float startX = ImGuiMCP::GetCursorPosX();

        ImGuiMCP::ImVec2 avail;
        ImGuiMCP::GetContentRegionAvail(&avail);
        float availWidth = avail.x;

        // Header row is offset by the same margins ApplyCardContentLineMargin uses for content lines
        const float headerY = startY + k_cardContentTopMargin;
        const float headerX = startX + k_cardContentLeftMargin;

        ImGuiMCP::SetCursorPosX(headerX);
        ImGuiMCP::SetCursorPosY(headerY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
        ImGuiMCP::Text("%s", a_card.icon);
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(headerX + k_iconColumnWidth + k_spaceAfterIcon);
        ImGuiMCP::Text("%s", a_card.label);

        float currentX = ImGuiMCP::GetCursorPosX();

        auto DisplayValue = [&](float a_raw) {

            return a_card.convertForDisplay ? (a_raw / a_card.displayDivisor) : a_raw;

            };

        std::string currentValueText;

        if (a_card.hasSlider) {

            char buf[64];
            std::snprintf(buf, sizeof(buf), a_card.sliderFormat, DisplayValue(*a_card.sliderValue));
            currentValueText = buf;

        }
        else if (a_card.hasCheckbox) {

            currentValueText = *a_card.checkboxValue ? "Enabled" : "Disabled";

        }
        else if (a_card.hasIntSlider) {

            currentValueText = a_card.intSliderNames[*a_card.intSliderValue];

        }

        ImGuiMCP::ImVec2 valueTextSize;
        ImGuiMCP::CalcTextSize(&valueTextSize, currentValueText.c_str(), nullptr, false, -1.0f);

        float rightEdge = headerX + (availWidth - k_cardContentLeftMargin) - k_cardCurrentValueRightMargin;

        const float valueStartX = rightEdge - valueTextSize.x;
        float minX = currentX + 10.0f;

        if (hasChanges) {

            if (DrawMiniResetButton(a_cardId, valueStartX, headerY, minX, "Reset this card's settings to default values.")) {

                ResetCardSettings(a_card);

            }

        }

        ImGuiMCP::SetCursorPosX(valueStartX);
        ImGuiMCP::SetCursorPosY(headerY);

        if (a_card.hasCheckbox) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, *a_card.checkboxValue ? HexToImVec4RGBA(Colors::GreenBright) : HexToImVec4RGBA(Colors::Red));
            ImGuiMCP::Text("%s", *a_card.checkboxValue ? "Enabled" : "Disabled");
            ImGuiMCP::PopStyleColor();

        }
        else if (a_card.hasIntSlider) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
            ImGuiMCP::Text("%s", a_card.intSliderNames[*a_card.intSliderValue]);
            ImGuiMCP::PopStyleColor();

        }
        else {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
            ImGuiMCP::Text(a_card.sliderFormat, DisplayValue(*a_card.sliderValue));
            ImGuiMCP::PopStyleColor();

        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();

            if (a_card.hasSlider) {

                char curBuf[64];
                std::snprintf(curBuf, sizeof(curBuf), a_card.sliderFormat, DisplayValue(*a_card.sliderValue));
                ImGuiMCP::Text("Current: %s", curBuf);

                ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Blue));

                char defBuf[64];
                std::snprintf(defBuf, sizeof(defBuf), a_card.sliderFormat, DisplayValue(a_card.sliderDefault));
                ImGuiMCP::Text("Default: %s", defBuf);

                ImGuiMCP::PopStyleColor();

            }
            else if (a_card.hasCheckbox) {

                ImGuiMCP::Text("Current: %s", *a_card.checkboxValue ? "Enabled" : "Disabled");
                ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Blue));
                ImGuiMCP::Text("Default: %s", a_card.checkboxDefault ? "Enabled" : "Disabled");
                ImGuiMCP::PopStyleColor();

            }
            else if (a_card.hasIntSlider) {

                ImGuiMCP::Text("Current: %s", a_card.intSliderNames[*a_card.intSliderValue]);
                ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
                ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Blue));
                ImGuiMCP::Text("Default: %s", a_card.intSliderNames[a_card.intSliderDefault]);
                ImGuiMCP::PopStyleColor();

            }

            ImGuiMCP::EndTooltip();

        }

        float frameHeight = ImGuiMCP::GetFrameHeight();
        ImGuiMCP::SetCursorPosY(headerY + frameHeight + 4.0f);

        ImGuiMCP::Separator();

        if (a_card.hasSlider) {

            ApplyCardContentLineMargin(true);

            ImGuiMCP::SetNextItemWidth(-1.0f);

            if (a_card.convertForDisplay) {

                float displayValue = *a_card.sliderValue / a_card.displayDivisor;
                float displayMin = a_card.sliderMin / a_card.displayDivisor;
                float displayMax = a_card.sliderMax / a_card.displayDivisor;

                if (ImGuiMCP::SliderFloat("##slider", &displayValue, displayMin, displayMax, a_card.sliderFormat)) {

                    *a_card.sliderValue = displayValue * a_card.displayDivisor;

                    if (a_card.onSave) {

                        a_card.onSave();

                    }

                    if (a_card.onSliderChange) {

                        a_card.onSliderChange();

                    }

                }

            }
            else {

                if (ImGuiMCP::SliderFloat("##slider", a_card.sliderValue, a_card.sliderMin, a_card.sliderMax, a_card.sliderFormat)) {

                    if (a_card.onSave) {

                        a_card.onSave();

                    }

                    if (a_card.onSliderChange) {

                        a_card.onSliderChange();

                    }

                }

            }

        }

        if (a_card.hasCheckbox) {

            ApplyCardContentLineMargin();

            if (ImGuiMCP::Checkbox("##checkbox", a_card.checkboxValue)) {

                if (a_card.onSave) {

                    a_card.onSave();

                }

                if (a_card.onCheckboxChange) {

                    a_card.onCheckboxChange();

                }

            }

            if (ImGuiMCP::IsItemHovered()) {

                ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

            }

        }

        if (a_card.hasIntSlider) {

            ApplyCardContentLineMargin(true);

            ImGuiMCP::SetNextItemWidth(-1.0f);

            if (ImGuiMCP::SliderInt("##intSlider", a_card.intSliderValue, a_card.intSliderMin, a_card.intSliderMax, a_card.intSliderNames[*a_card.intSliderValue])) {

                *a_card.intSliderValue = std::clamp(*a_card.intSliderValue, a_card.intSliderMin, a_card.intSliderMax);

                if (a_card.onSave) {

                    a_card.onSave();

                }

                if (a_card.onIntSliderChange) {

                    a_card.onIntSliderChange();

                }

            }

        }

        if (a_card.tooltipText) {

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentTopMargin));
            ImGuiMCP::Separator();

            ApplyCardContentLineMargin();

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
            ImGuiMCP::Text("%s", a_card.tooltipText);
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::EndChild();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    }

    // ==================================================================================================================
    //  DrawScoreCard
    // ==================================================================================================================

    static bool HasScoreCardChanged(const ScoreCardData& a_card) {

        return (*a_card.baseScore != a_card.baseDefault) ||
            (*a_card.bonusEnabled != a_card.bonusEnabledDefault) ||
            (*a_card.bonusFactor != a_card.bonusFactorDefault);

    }

    static void ResetScoreCard(const ScoreCardData& a_card) {

        *a_card.baseScore = a_card.baseDefault;
        *a_card.bonusEnabled = a_card.bonusEnabledDefault;
        *a_card.bonusFactor = a_card.bonusFactorDefault;

        if (a_card.onSave) {

            a_card.onSave();

        }

    }

    void DrawScoreCard(const ScoreCardData& a_card) {

        ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);

        ImGuiMCP::ImVec2 winSize;
        ImGuiMCP::GetWindowSize(&winSize);
        float cardWidth = winSize.x - (k_marginBetweenBordersInHeader * 2);

        ImGuiMCP::BeginChild(ImGuiMCP::GetID(a_card.label), ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        float startY = ImGuiMCP::GetCursorPosY();
        float startX = ImGuiMCP::GetCursorPosX();

        ImGuiMCP::ImVec2 avail;
        ImGuiMCP::GetContentRegionAvail(&avail);
        float availWidth = avail.x;

        // Header row is offset by the same margins ApplyCardContentLineMargin uses for content lines
        const float headerY = startY + k_cardContentTopMargin;
        const float headerX = startX + k_cardContentLeftMargin;

        ImGuiMCP::SetCursorPosX(headerX);
        ImGuiMCP::SetCursorPosY(headerY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(a_card.iconColor));
        ImGuiMCP::Text("%s", a_card.icon);
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(headerX + k_iconColumnWidth + k_spaceAfterIcon);
        ImGuiMCP::Text("%s", a_card.label);

        float currentX = ImGuiMCP::GetCursorPosX();

        const bool hasChanges = HasScoreCardChanged(a_card);

        float totalScore = *a_card.baseScore + (*a_card.bonusEnabled ? *a_card.bonusFactor : 0.0f);
        float defaultTotalScore = a_card.baseDefault + (a_card.bonusEnabledDefault ? a_card.bonusFactorDefault : 0.0f);

        std::string scoreText = std::format("{:.0f}", totalScore);
        ImGuiMCP::ImVec2 scoreTextSize;
        ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);

        float rightEdge = headerX + (availWidth - k_cardContentLeftMargin) - k_cardCurrentValueRightMargin;

        const float scoreStartX = rightEdge - scoreTextSize.x;
        float minX = currentX + 10.0f;

        if (hasChanges) {

            if (DrawMiniResetButton(a_card.label, scoreStartX, headerY, minX, "Reset this card's settings to default values.")) {

                ResetScoreCard(a_card);

            }

        }

        ImGuiMCP::SetCursorPosX(scoreStartX);
        ImGuiMCP::SetCursorPosY(headerY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
        ImGuiMCP::Text("%.0f", totalScore);
        ImGuiMCP::PopStyleColor();

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Total Score: %.0f", totalScore);
            ImGuiMCP::Text("Base Score: %.0f", *a_card.baseScore);

            if (*a_card.bonusEnabled) {

                ImGuiMCP::Text("Bonus: +%.0f", *a_card.bonusFactor);

            }
            else {

                ImGuiMCP::Text("Bonus: Disabled");

            }

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Blue));
            ImGuiMCP::Text("Default Total Score: %.0f", defaultTotalScore);
            ImGuiMCP::Text("Default Base Score: %.0f", a_card.baseDefault);

            if (a_card.bonusEnabledDefault) {

                ImGuiMCP::Text("Default Bonus: +%.0f", a_card.bonusFactorDefault);

            }
            else {

                ImGuiMCP::Text("Default Bonus: Disabled");

            }

            ImGuiMCP::PopStyleColor();
            ImGuiMCP::EndTooltip();

        }

        float frameHeight = ImGuiMCP::GetFrameHeight();
        ImGuiMCP::SetCursorPosY(headerY + frameHeight + 4.0f);

        ImGuiMCP::Separator();

        ApplyCardContentLineMargin();
        ImGuiMCP::Text("Base Score");

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("%s", a_card.tooltip);
            ImGuiMCP::EndTooltip();

        }

        ApplyCardContentLineMargin(true);
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, HexToImVec4RGBA(Colors::BlueLight));
        ImGuiMCP::PushID(a_card.label);

        if (ImGuiMCP::SliderFloat("##Base", a_card.baseScore, a_card.baseMin, a_card.baseMax, "%.0f")) {

            if (a_card.onSave) {

                a_card.onSave();

            }

        }

        ImGuiMCP::PopID();
        ImGuiMCP::PopStyleColor();

        ApplyCardContentLineMargin();
        ImGuiMCP::Text("Proximity Bonus");
        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

        ImGuiMCP::PushID(a_card.label);

        if (ImGuiMCP::Checkbox("##BonusToggle", a_card.bonusEnabled)) {

            if (a_card.onSave) {

                a_card.onSave();

            }

        }

        ImGuiMCP::PopID();

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("%s", a_card.bonusTooltip);
            ImGuiMCP::EndTooltip();

        }

        if (*a_card.bonusEnabled) {

            ApplyCardContentLineMargin(true);
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, HexToImVec4RGBA(Colors::GreenLight));
            ImGuiMCP::PushID(a_card.label);

            if (ImGuiMCP::SliderFloat("##BonusFactor", a_card.bonusFactor, a_card.bonusMin, a_card.bonusMax, "+%.0f")) {

                if (a_card.onSave) {

                    a_card.onSave();

                }

            }

            ImGuiMCP::PopID();
            ImGuiMCP::PopStyleColor();

        }
        else {

            ApplyCardContentLineMargin();
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayMedium));
            ImGuiMCP::Text("+0 (disabled)");
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentTopMargin));
        ImGuiMCP::Separator();

        ApplyCardContentLineMargin();
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
        ImGuiMCP::Text("%s", a_card.tooltip);

        if (*a_card.bonusEnabled) {

            ApplyCardContentLineMargin();
            ImGuiMCP::Text("%s", a_card.bonusTooltip);

        }

        ImGuiMCP::PopStyleColor();

        ImGuiMCP::EndChild();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    }

    // ==================================================================================================================
    //  Multi-checkbox card
    // ==================================================================================================================

    static bool HasCheckboxItemsChanged(const std::vector<CheckboxItem>& a_items) {

        for (const auto& item : a_items) {

            if (*item.value != item.defaultValue) {

                return true;

            }

            if (HasCheckboxItemsChanged(item.children)) {

                return true;

            }

        }

        return false;

    }

    static void ResetCheckboxItems(std::vector<CheckboxItem>& a_items) {

        for (auto& item : a_items) {

            *item.value = item.defaultValue;

            if (item.onChange) {

                item.onChange();

            }

            ResetCheckboxItems(item.children);

        }

    }

    static void DrawCheckboxItem(const CheckboxItem& a_item, const std::string& a_idSuffix, const std::function<void()>& a_onSave) {

        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + k_cardContentLeftMargin);

        float rowStartX = ImGuiMCP::GetCursorPosX();

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(a_item.iconColor));
        ImGuiMCP::Text("%s", a_item.icon);
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(rowStartX + k_iconColumnWidth + k_spaceAfterIcon);

        std::string checkboxId = std::format("##checkbox_{}", a_idSuffix);

        if (ImGuiMCP::Checkbox(checkboxId.c_str(), a_item.value)) {

            if (a_onSave) {

                a_onSave();

            }

            if (a_item.onChange) {

                a_item.onChange();

            }

        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

        }

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);
        ImGuiMCP::Text("%s", a_item.label);

        if (!a_item.tooltipLines.empty() && ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();

            for (const auto& line : a_item.tooltipLines) {

                ImGuiMCP::Text("%s", line.c_str());

            }

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Blue));
            ImGuiMCP::Text("Default: %s", a_item.defaultValue ? "Enabled" : "Disabled");
            ImGuiMCP::PopStyleColor();
            ImGuiMCP::EndTooltip();

        }

        if (!a_item.children.empty() && *a_item.value) {

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));
            ImGuiMCP::Indent(40.0f);

            for (size_t i = 0; i < a_item.children.size(); ++i) {

                ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentTopMargin));

                DrawCheckboxItem(a_item.children[i], a_idSuffix + "_" + std::to_string(i), a_onSave);

            }

            ImGuiMCP::Unindent(40.0f);

        }

    }

    void DrawMultiCheckboxCard(const std::string& a_cardId, const std::string& a_title, const char* a_icon,
        std::vector<CheckboxItem> a_items, const std::function<void()>& a_onSave) {

        ImGuiMCP::ImVec2 winSize;
        ImGuiMCP::GetWindowSize(&winSize);
        float cardWidth = winSize.x - (k_marginBetweenBordersInHeader * 2);

        ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);
        ImGuiMCP::BeginChild(a_cardId.c_str(), ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        float startY = ImGuiMCP::GetCursorPosY();
        float startX = ImGuiMCP::GetCursorPosX();

        ImGuiMCP::ImVec2 avail;
        ImGuiMCP::GetContentRegionAvail(&avail);
        float availWidth = avail.x;

        // Header row is offset by the same margins ApplyCardContentLineMargin uses for content lines
        const float headerY = startY + k_cardContentTopMargin;
        const float headerX = startX + k_cardContentLeftMargin;

        ImGuiMCP::SetCursorPosX(headerX);
        ImGuiMCP::SetCursorPosY(headerY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
        ImGuiMCP::Text("%s", a_icon);
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(headerX + k_iconColumnWidth + k_spaceAfterIcon);
        ImGuiMCP::Text("%s", a_title.c_str());

        float currentX = ImGuiMCP::GetCursorPosX();

        const bool hasChanges = HasCheckboxItemsChanged(a_items);

        float rightEdge = headerX + (availWidth - k_cardContentLeftMargin) - k_cardCurrentValueRightMargin;
        float minX = currentX + 10.0f;

        if (hasChanges) {

            if (DrawMiniResetButton(a_cardId, rightEdge, headerY, minX, "Reset this card's settings to default values.")) {

                ResetCheckboxItems(a_items);

                if (a_onSave) {

                    a_onSave();

                }

            }

        }

        float frameHeight = ImGuiMCP::GetFrameHeight();
        ImGuiMCP::SetCursorPosY(headerY + frameHeight + 4.0f);

        ImGuiMCP::Separator();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

        for (size_t i = 0; i < a_items.size(); ++i) {

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentTopMargin));

            DrawCheckboxItem(a_items[i], a_cardId + "_" + std::to_string(i), a_onSave);

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

        ImGuiMCP::EndChild();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    }

    bool DrawMiniResetButton(const std::string& a_id, float a_anchorX, float a_y, float a_minX, const char* a_tooltip) {

        ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
        ImGuiMCP::ImVec2 resetIconSize;
        ImGuiMCP::CalcTextSize(&resetIconSize, Icons::resetIcon.c_str(), nullptr, false, -1.0f);
        float resetButtonWidth = resetIconSize.x + style->FramePadding.x * 2.0f;
        float resetButtonHeight = resetIconSize.y + style->FramePadding.y * 2.0f;

        float visualGap = std::max(0.0f, k_spaceAfterIcon - style->FramePadding.x);
        float resetStartX = std::max(a_minX, a_anchorX - resetButtonWidth - visualGap);

        ImGuiMCP::SetCursorPosX(resetStartX);
        ImGuiMCP::SetCursorPosY(a_y);

        std::string invisId = std::format("##reset_{}", a_id);
        bool clicked = ImGuiMCP::InvisibleButton(invisId.c_str(), ImGuiMCP::ImVec2(resetButtonWidth, resetButtonHeight));
        const bool isHovered = ImGuiMCP::IsItemHovered();

        // Only ever drawn once - color decided before drawing, not after.
        ImGuiMCP::SetCursorPosX(resetStartX + style->FramePadding.x);
        ImGuiMCP::SetCursorPosY(a_y + style->FramePadding.y);
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(isHovered ? Colors::BlueLight : Colors::Blue));
        ImGuiMCP::Text("%s", Icons::resetIcon.c_str());
        ImGuiMCP::PopStyleColor();

        if (isHovered) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("%s", a_tooltip);
            ImGuiMCP::EndTooltip();

        }

        return clicked;

    }

}

namespace UITemplate::Icons {

    const std::string                   cameraIcon                      = FontAwesome::UnicodeToUtf8(0xf03d);
    const std::string                   headTrackIcon                   = FontAwesome::UnicodeToUtf8(0xf06e);
    const std::string                   poiSystemIcon                   = FontAwesome::UnicodeToUtf8(0xf3c5);
    const std::string                   debugIcon                       = FontAwesome::UnicodeToUtf8(0xf7d9);

    const std::string                   arrowUpIcon                     = FontAwesome::UnicodeToUtf8(0xf062);
    const std::string                   arrowLeftAndRightIcon           = FontAwesome::UnicodeToUtf8(0xf07e);
    const std::string                   arrowUpAndDownIcon              = FontAwesome::UnicodeToUtf8(0xf07d);
    const std::string                   zoomIcon                        = FontAwesome::UnicodeToUtf8(0xf002);

    const std::string                   clockIcon                       = FontAwesome::UnicodeToUtf8(0xf017);
    const std::string                   filmIcon                        = FontAwesome::UnicodeToUtf8(0xf008);
    const std::string                   speedIcon                       = FontAwesome::UnicodeToUtf8(0xf625);
    const std::string                   soundIcon                       = FontAwesome::UnicodeToUtf8(0xf028);
    const std::string                   radiusIcon                      = FontAwesome::UnicodeToUtf8(0xf192);
    const std::string                   collisionIcon                   = FontAwesome::UnicodeToUtf8(0xf1b2);

    const std::string                   poiTypesIcon                    = FontAwesome::UnicodeToUtf8(0xf5fd);
    const std::string                   excludeListIcon                 = FontAwesome::UnicodeToUtf8(0xf023);
    const std::string                   poiLockIcon                     = FontAwesome::UnicodeToUtf8(0xe51f);
    const std::string                   followerIcon                    = FontAwesome::UnicodeToUtf8(0xe535);

    const std::string                   dragonScoreIcon                 = FontAwesome::UnicodeToUtf8(0xf6d5);
    const std::string                   inCombatScoreIcon               = FontAwesome::UnicodeToUtf8(0xf0e3);
    const std::string                   movingScoreIcon                 = FontAwesome::UnicodeToUtf8(0xf554);
    const std::string                   inSceneScoreIcon                = FontAwesome::UnicodeToUtf8(0xf630);
    const std::string                   personIcon                      = FontAwesome::UnicodeToUtf8(0xf183);

    const std::string                   flyingCritterIcon               = FontAwesome::UnicodeToUtf8(0xf4ba);
    const std::string                   fishCritterIcon                 = FontAwesome::UnicodeToUtf8(0xf578);

    const std::string                   addIcon                         = FontAwesome::UnicodeToUtf8(0xf067);
    const std::string                   infoIcon                        = FontAwesome::UnicodeToUtf8(0xf05a);

    const std::string                   raycastIcon                     = FontAwesome::UnicodeToUtf8(0xf05b);
    const std::string                   loggingIcon                     = FontAwesome::UnicodeToUtf8(0xf120);

    const std::string                   deniedIcon                      = FontAwesome::UnicodeToUtf8(0xf05e);
    const std::string                   folderOpenIcon                  = FontAwesome::UnicodeToUtf8(0xf07c);

    const std::string                   resetIcon                       = FontAwesome::UnicodeToUtf8(0xf2ea);

}