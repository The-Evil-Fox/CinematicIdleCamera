#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <functional>
#include <format>
#include <algorithm>
#include "SKSEMenuFramework.h"
#include "utility.h"
#include "logger.hpp"

namespace UITemplate {

    // ==================================================================================================================================================================================
    //  Layout constants
    // ==================================================================================================================================================================================

    // Headers

    inline constexpr float              k_marginBetweenBordersInHeader              = 30.0f;

    // Icons & labels

    inline constexpr float              k_iconColumnWidth                           = 30.0f;
    inline constexpr float              k_spaceAfterIcon                            = 20.0f;

    // Cards

    inline constexpr float              k_cardCurrentValueRightMargin               = 10.0f;
    inline constexpr float              k_cardContentLeftMargin                     = 15.0f;
    inline constexpr float              k_cardContentTopMargin                      = 15.0f;
    inline constexpr float              k_cardContentBottomMargin                   = 15.0f;
    inline constexpr float              k_cardSpaceAfter                            = 10.0f;

    // ==================================================================================================================================================================================
    //  Skyrim Constants
    // ==================================================================================================================================================================================

    inline constexpr float              SKYRIM_UNITS_TO_METERS                      = 70.0f;  // 1 meter = 70 game units

    // ==================================================================================================================================================================================
    //  Color Palette
    // ==================================================================================================================================================================================

    namespace Colors {

        // ==================================================================================================================
        //  Reds
        // ==================================================================================================================

        inline constexpr uint32_t       Red                             = 0xe74c3c;
        inline constexpr uint32_t       RedLight                        = 0xff6b6b;
        inline constexpr uint32_t       RedDark                         = 0xc0392b;
        inline constexpr uint32_t       RedDarker                       = 0x922b21;
        inline constexpr uint32_t       RedCrimson                      = 0xdc143c;
        inline constexpr uint32_t       RedMaroon                       = 0x800000;
        inline constexpr uint32_t       RedRose                         = 0xff007f;
        inline constexpr uint32_t       RedRust                         = 0xb7410e;

        // ==================================================================================================================
        //  Oranges
        // ==================================================================================================================

        inline constexpr uint32_t       Orange                          = 0xf39c12;
        inline constexpr uint32_t       OrangeLight                     = 0xffa500;
        inline constexpr uint32_t       OrangeDark                      = 0xe67e22;
        inline constexpr uint32_t       OrangeDarker                    = 0xca6f1e;
        inline constexpr uint32_t       OrangePeach                     = 0xffdab9;
        inline constexpr uint32_t       OrangeCoral                     = 0xff7f50;
        inline constexpr uint32_t       OrangeTangerine                 = 0xf28500;

        // ==================================================================================================================
        //  Yellows / Golds
        // ==================================================================================================================

        inline constexpr uint32_t       GoldLight                       = 0xF5D966;
        inline constexpr uint32_t       Gold                            = 0xffd700;
        inline constexpr uint32_t       GoldDark                        = 0xb8860b;
        inline constexpr uint32_t       Yellow                          = 0xffff00;
        inline constexpr uint32_t       YellowLight                     = 0xfff9c4;
        inline constexpr uint32_t       YellowDark                      = 0xf1c40f;
        inline constexpr uint32_t       YellowLemon                     = 0xfff44f;
        inline constexpr uint32_t       YellowMustard                   = 0xe1ad01;

        // ==================================================================================================================
        //  Greens
        // ==================================================================================================================

        inline constexpr uint32_t       Green                           = 0x7ef566;
        inline constexpr uint32_t       GreenLight                      = 0x8eff8e;
        inline constexpr uint32_t       GreenBright                     = 0x2ecc71;
        inline constexpr uint32_t       GreenDark                       = 0x27ae60;
        inline constexpr uint32_t       GreenDarker                     = 0x1e8449;
        inline constexpr uint32_t       GreenLime                       = 0x00ff00;
        inline constexpr uint32_t       GreenForest                     = 0x228b22;
        inline constexpr uint32_t       GreenOlive                      = 0x808000;
        inline constexpr uint32_t       GreenTeal                       = 0x008080;
        inline constexpr uint32_t       GreenMint                       = 0x98ff98;
        inline constexpr uint32_t       GreenEmerald                    = 0x50c878;
        inline constexpr uint32_t       GreenSpring                     = 0x00ff7f;
        inline constexpr uint32_t       GreenPale                       = 0x98fb98;

        // ==================================================================================================================
        //  Blues
        // ==================================================================================================================

        inline constexpr uint32_t       Blue                            = 0x036ffc;
        inline constexpr uint32_t       BlueLight                       = 0x5b9aff;
        inline constexpr uint32_t       BlueDark                        = 0x16439e;
        inline constexpr uint32_t       BlueNavy                        = 0x000080;
        inline constexpr uint32_t       BlueSky                         = 0x87ceeb;
        inline constexpr uint32_t       BlueSteel                       = 0x4682b4;
        inline constexpr uint32_t       BlueRoyal                       = 0x4169e1;
        inline constexpr uint32_t       BlueCornflower                  = 0x6495ed;
        inline constexpr uint32_t       BlueDeep                        = 0x00008b;
        inline constexpr uint32_t       BlueMidnight                    = 0x191970;
        inline constexpr uint32_t       BlueAzure                       = 0x007fff;
        inline constexpr uint32_t       BlueCerulean                    = 0x007ba7;

        // ==================================================================================================================
        //  Cyans / Turquoises
        // ==================================================================================================================

        inline constexpr uint32_t       Cyan                            = 0x1ff0ff;
        inline constexpr uint32_t       CyanLight                       = 0x80ffff;
        inline constexpr uint32_t       CyanDark                        = 0x008b8b;
        inline constexpr uint32_t       Turquoise                       = 0x40e0d0;
        inline constexpr uint32_t       TurquoiseDark                   = 0x00ced1;
        inline constexpr uint32_t       Aqua                            = 0x00ffff;
        inline constexpr uint32_t       Aquamarine                      = 0x7fffd4;

        // ==================================================================================================================
        //  Purples / Violets
        // ==================================================================================================================

        inline constexpr uint32_t       Pink                            = 0xf566dd;
        inline constexpr uint32_t       PinkLight                       = 0xffb6c1;
        inline constexpr uint32_t       PinkHot                         = 0xff69b4;
        inline constexpr uint32_t       PinkDeep                        = 0xff1493;
        inline constexpr uint32_t       Purple                          = 0x800080;
        inline constexpr uint32_t       PurpleLight                     = 0x9b59b6;
        inline constexpr uint32_t       PurpleDark                      = 0x4a235a;
        inline constexpr uint32_t       Violet                          = 0x8f00ff;
        inline constexpr uint32_t       VioletLight                     = 0xee82ee;
        inline constexpr uint32_t       Lavender                        = 0xe6e6fa;
        inline constexpr uint32_t       Magenta                         = 0xff00ff;
        inline constexpr uint32_t       Indigo                          = 0x4b0082;
        inline constexpr uint32_t       Orchid                          = 0xda70d6;
        inline constexpr uint32_t       Plum                            = 0xdda0dd;

        // ==================================================================================================================
        //  Browns / Tans
        // ==================================================================================================================

        inline constexpr uint32_t       Brown                           = 0x8b4513;
        inline constexpr uint32_t       BrownLight                      = 0xd2b48c;
        inline constexpr uint32_t       BrownDark                       = 0x5c3a21;
        inline constexpr uint32_t       Beige                           = 0xf5f5dc;
        inline constexpr uint32_t       Chocolate                       = 0xd2691e;
        inline constexpr uint32_t       Copper                          = 0xb87333;
        inline constexpr uint32_t       Bronze                          = 0xcd7f32;

        // ==================================================================================================================
        //  Grays
        // ==================================================================================================================

        inline constexpr uint32_t       Black                           = 0x000000;
        inline constexpr uint32_t       BlackLight                      = 0x1a1a1a;
        inline constexpr uint32_t       GrayDark                        = 0x26262E;
        inline constexpr uint32_t       Gray                            = 0x40404D;
        inline constexpr uint32_t       GrayMedium                      = 0x6b6b7b;
        inline constexpr uint32_t       GrayLight                       = 0x96969e;
        inline constexpr uint32_t       GrayLighter                     = 0xc0c0c0;
        inline constexpr uint32_t       GrayPale                        = 0xe0e0e0;
        inline constexpr uint32_t       White                           = 0xffffff;

    }

    ImGuiMCP::ImVec4 HexToImVec4RGBA(uint32_t a_hex, float a_alpha = 1.0f);

    // ==================================================================================================================================================================================
    //  Icons
    // ==================================================================================================================================================================================

    namespace Icons {

        extern const std::string        addIcon;
        extern const std::string        arrowLeftAndRightIcon;
        extern const std::string        arrowUpAndDownIcon;
        extern const std::string        arrowUpIcon;

        extern const std::string        banIcon;

        extern const std::string        circleDotIcon;
        extern const std::string        circleInfoIcon;
        extern const std::string        clockIcon;
        extern const std::string        crosshairsIcon;
        extern const std::string        cubeIcon;

        extern const std::string        doveIcon;
        extern const std::string        dragonIcon;

        extern const std::string        eyeIcon;

        extern const std::string        filmIcon;
        extern const std::string        fishIcon;
        extern const std::string        folderOpenIcon;

        extern const std::string        gaugeHighIcon;
        extern const std::string        gavelIcon;

        extern const std::string        layerGroupIcon;
        extern const std::string        locationDotIcon;
        extern const std::string        locationPinLockIcon;
        extern const std::string        lockIcon;

        extern const std::string        magnifyingGlassIcon;
        extern const std::string        masksTheaterIcon;

        extern const std::string        peoplePullingIcon;
        extern const std::string        personIcon;
        extern const std::string        personWalkingIcon;

        extern const std::string        rotateLeftIcon;

        extern const std::string        screwdriverWrenchIcon;

        extern const std::string        terminalIcon;

        extern const std::string        videoIcon;
        extern const std::string        volumeHighIcon;

    }

    // ==================================================================================================================================================================================
    //  Logging level names
    // ==================================================================================================================================================================================

    inline constexpr const char* k_loggingLevelNames[] = {

        "Quiet",
        "Warnings",
        "Info",
        "Debug"

    };

    inline constexpr int k_loggingLevelCount = static_cast<int>(std::size(k_loggingLevelNames));

    spdlog::level::level_enum LoggingLevelToSpdlog(int a_loggingLevel);

    // ==================================================================================================================
    //  RAII guard for the mini-card style push done by DrawHeaderWithReset() /
    //  DrawHeaderWithListReset(). Holding one means "2 style vars + 2 style
    //  colors are currently pushed"; its destructor pops them automatically on
    //  any scope exit (normal return, early return, etc.), so a mismatched
    //  push/pop pair can't be reintroduced by accident.
    // ==================================================================================================================

    class MiniCardStyleGuard {

        public:

            MiniCardStyleGuard() = default;
            ~MiniCardStyleGuard();

            MiniCardStyleGuard(const MiniCardStyleGuard&) = delete;
            MiniCardStyleGuard& operator=(const MiniCardStyleGuard&) = delete;

            MiniCardStyleGuard(MiniCardStyleGuard&& other) noexcept;
            MiniCardStyleGuard& operator=(MiniCardStyleGuard&& other) noexcept;

        private:

            bool m_active = true;

    };

    // Exposed in case you need to pop early / push your own mini-card manually.
    void PushMiniCardStyling();
    void PopMiniCardStyling();

    // ==================================================================================================================
    //  Generic "setting + default (+ optional custom reset action)" wrapper.
    //  Pass a list of these to DrawHeaderWithReset() to get an auto-hiding
    //  "Reset to Default" button for an entire page.
    // ==================================================================================================================

    template<typename T>
    struct SettingWithDefault {

        T* currentValue;
        T defaultValue;
        std::function<void()> customResetAction; // optional, e.g. re-apply to a live engine setting

        SettingWithDefault(T* a_value, T a_defaultVal, std::function<void()> a_customAction = nullptr)
            : currentValue(a_value), defaultValue(a_defaultVal), customResetAction(a_customAction) {}

    };

    template<typename... Args>
    bool HasSettingsChanged(const Args&... a_settings) {

        bool changed = false;

        ([&]() {

            if (*(a_settings.currentValue) != a_settings.defaultValue) {

                changed = true;

            }

        }(), ...);

        return changed;

    }

    // a_onReset is called once after every value has been reset (hook up your
    // own IniParser::Save() / logging here).
    template<typename... Args>
    void ResetSettings(const std::function<void()>& a_onReset, const Args&... a_settings) {

        ([&]() {

            *(a_settings.currentValue) = a_settings.defaultValue;

            if (a_settings.customResetAction) {

                a_settings.customResetAction();

            }

        }(), ...);

        if (a_onReset) {

            a_onReset();

        }

    }

    bool DrawMiniResetButton(const std::string& a_id, float a_anchorX, float a_y, float a_minX, const char* a_tooltip);

    void SaveSettings();

    // ==================================================================================================================
    //  Builds a "reset + notify" callback: runs a_onSave, then logs that the
    //  given section was reset. Pass the result as a_onReset to
    //  DrawHeaderWithReset()/ResetSettings().
    // ==================================================================================================================

    inline std::function<void()> MakeResetLogger(const std::string& a_sectionName) {

        return [a_sectionName]() {

            SaveSettings();
            SKSE::log::info("{} reset to default values.", a_sectionName);

        };

    }

    // ==================================================================================================================
    //  Draws a page header: gold title + icon on the left, an auto-hiding
    //  "Reset to Default" button on the right (only shown once any setting in
    //  the list differs from its default), a separator, and then pushes the
    //  standard "mini-card" child-window styling used by DrawSettingCard() /
    //  DrawScoreCard() / DrawMultiCheckboxCard().
    //
    //  Returns a MiniCardStyleGuard — keep it alive for the duration of the
    //  page (e.g. `auto guard = DrawHeaderWithReset(...)`) so the styling is
    //  popped automatically when the function returns, including early
    //  returns.
    // ==================================================================================================================

    template<typename... Args>
    MiniCardStyleGuard DrawHeaderWithReset(const std::string& a_title, const std::string& a_icon,
        const std::string& a_sectionName, const std::string& a_buttonId,
        const std::function<void()>& a_onReset, const Args&... a_settings) {

        FontAwesome::PushSolid();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

        ImGuiMCP::ImVec2 winSize;
        ImGuiMCP::GetWindowSize(&winSize);

        ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);

        float startY = ImGuiMCP::GetCursorPosY();

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
        ImGuiMCP::Text("%s", a_icon.c_str());
        ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
        ImGuiMCP::Text("%s", a_title.c_str());
        ImGuiMCP::PopStyleColor();

        const bool hasChanges = HasSettingsChanged(a_settings...);

        std::string fullButtonText = std::format("{} Reset To Default##{}", Icons::rotateLeftIcon, a_buttonId);
        ImGuiMCP::ImVec2 resetTextSize;
        ImGuiMCP::CalcTextSize(&resetTextSize, fullButtonText.c_str(), nullptr, true, -1.0f);

        ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
        float buttonWidth = resetTextSize.x + style->FramePadding.x * 2.0f;

        float buttonPosX = winSize.x - k_marginBetweenBordersInHeader - buttonWidth;
        ImGuiMCP::SetCursorPosX(buttonPosX);

        float cursorY = startY;
        float textHeight = ImGuiMCP::GetTextLineHeight();
        float frameHeight = ImGuiMCP::GetFrameHeight();
        float verticalOffset = (frameHeight - textHeight) / 2.0f;

        if (hasChanges) {

            ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);

            if (ImGuiMCP::Button(fullButtonText.c_str())) {

                ResetSettings(a_onReset, a_settings...);

            }

            if (ImGuiMCP::IsItemHovered()) {

                ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

                ImGuiMCP::BeginTooltip();
                ImGuiMCP::Text("Restores all %s settings on this page to their default values.", a_sectionName.c_str());
                ImGuiMCP::EndTooltip();

            }

        } else {

            ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);
            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(buttonWidth, frameHeight));

        }

        ImGuiMCP::SetCursorPosY(cursorY + frameHeight + 5.0f);
        ImGuiMCP::Separator();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

        PushMiniCardStyling();

        return MiniCardStyleGuard{};

    }

    // ==================================================================================================================
    // Overload for any "list-like" reset target that exposes:
    //   bool HasChanged() const;
    //   void Reset() const;
    //   should perform its own save/log if needed (e.g. an exclusion list, a favorites list, a saved-preset list...)
    // ==================================================================================================================
    template<typename ListResetType>
    MiniCardStyleGuard DrawHeaderWithListReset(const std::string& a_title, const std::string& a_icon,
        const std::string& a_buttonId, const std::string& a_buttonLabel, const std::string& a_tooltipText,
        const ListResetType& a_listReset) {

        FontAwesome::PushSolid();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

        ImGuiMCP::ImVec2 winSize;
        ImGuiMCP::GetWindowSize(&winSize);

        ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);

        float startY = ImGuiMCP::GetCursorPosY();

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
        ImGuiMCP::Text("%s", a_icon.c_str());
        ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
        ImGuiMCP::Text("%s", a_title.c_str());
        ImGuiMCP::PopStyleColor();

        const bool hasChanges = a_listReset.HasChanged();

        std::string fullButtonText = std::format("{} {}##{}", Icons::rotateLeftIcon, a_buttonLabel, a_buttonId);
        ImGuiMCP::ImVec2 resetTextSize;
        ImGuiMCP::CalcTextSize(&resetTextSize, fullButtonText.c_str(), nullptr, true, -1.0f);

        ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
        float buttonWidth = resetTextSize.x + style->FramePadding.x * 2.0f;

        float buttonPosX = winSize.x - k_marginBetweenBordersInHeader - buttonWidth;
        ImGuiMCP::SetCursorPosX(buttonPosX);

        float cursorY = startY;
        float textHeight = ImGuiMCP::GetTextLineHeight();
        float frameHeight = ImGuiMCP::GetFrameHeight();
        float verticalOffset = (frameHeight - textHeight) / 2.0f;

        if (hasChanges) {

            ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);

            if (ImGuiMCP::Button(fullButtonText.c_str())) {

                a_listReset.Reset();

            }

            if (ImGuiMCP::IsItemHovered()) {

                ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

                ImGuiMCP::BeginTooltip();
                ImGuiMCP::Text("%s", a_tooltipText.c_str());
                ImGuiMCP::EndTooltip();

            }

        } else {

            ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);
            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(buttonWidth, frameHeight));

        }

        ImGuiMCP::SetCursorPosY(cursorY + frameHeight + 5.0f);
        ImGuiMCP::Separator();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

        PushMiniCardStyling();

        return MiniCardStyleGuard{};

    }

    // ==================================================================================================================
    //  Standardized single setting card: icon + label on the left, current
    //  value (+ optional per-card reset button) on the right, a separator,
    //  then the actual control (slider / checkbox / named int-slider), then
    //  optional helper text.
    // ==================================================================================================================

    struct CardContent {

        const char*                     icon;
        const char*                     label;
        const char*                     tooltipText                     = nullptr;

        // Float slider
        bool                            hasSlider                       = false;
        float*                          sliderValue                     = nullptr;
        float                           sliderMin                       = 0.0f;
        float                           sliderMax                       = 0.0f;
        const char*                     sliderFormat                    = "%.0f";
        float                           sliderDefault                   = 0.0f;

        // Checkbox
        bool                            hasCheckbox                     = false;
        bool*                           checkboxValue                   = nullptr;
        bool                            checkboxDefault                 = false;

        // Callbacks
        std::function<void()> onSliderChange;
        std::function<void()> onCheckboxChange;
        std::function<void()> onIntSliderChange;
        std::function<void()> onSave; // called any time a value is edited or reset (hook your save/ini logic here)

        // Optional unit conversion for display only (e.g. game units -> meters)
        bool                            convertForDisplay               = false;
        float                           displayDivisor                  = 1.0f;

        // Named-step int slider (mutually exclusive with hasSlider/hasCheckbox)
        bool                             hasIntSlider                   = false;
        int*                            intSliderValue                  = nullptr;
        int                             intSliderMin                    = 0;
        int                             intSliderMax                    = 0;
        int                             intSliderDefault                = 0;
        const char* const*              intSliderNames                  = nullptr; // indexed by value

        static CardContent Slider(const char* a_icon, const char* a_label, const char* a_tooltipText,
            float* a_value, float a_min, float a_max, const char* a_format, float a_default,
            std::function<void()> a_onSave, std::function<void()> a_onChange = nullptr,
            bool a_convertForDisplay = false, float a_displayDivisor = 1.0f) {

            CardContent card{};
            card.icon = a_icon;
            card.label = a_label;
            card.tooltipText = a_tooltipText;
            card.hasSlider = true;
            card.sliderValue = a_value;
            card.sliderMin = a_min;
            card.sliderMax = a_max;
            card.sliderFormat = a_format;
            card.sliderDefault = a_default;
            card.onSave = a_onSave;
            card.onSliderChange = a_onChange;
            card.convertForDisplay = a_convertForDisplay;
            card.displayDivisor = a_displayDivisor;
            return card;

        }

        static CardContent Checkbox(const char* a_icon, const char* a_label, const char* a_tooltipText,
            bool* a_value, bool a_default, std::function<void()> a_onSave, std::function<void()> a_onChange = nullptr) {

            CardContent card{};
            card.icon = a_icon;
            card.label = a_label;
            card.tooltipText = a_tooltipText;
            card.hasCheckbox = true;
            card.checkboxValue = a_value;
            card.checkboxDefault = a_default;
            card.onSave = a_onSave;
            card.onCheckboxChange = a_onChange;
            return card;

        }

        static CardContent IntSlider(const char* a_icon, const char* a_label, const char* a_tooltipText,
            int* a_value, int a_min, int a_max, int a_default, const char* const* a_names,
            std::function<void()> a_onSave, std::function<void()> a_onChange = nullptr) {

            CardContent card{};
            card.icon = a_icon;
            card.label = a_label;
            card.tooltipText = a_tooltipText;
            card.hasIntSlider = true;
            card.intSliderValue = a_value;
            card.intSliderMin = a_min;
            card.intSliderMax = a_max;
            card.intSliderDefault = a_default;
            card.intSliderNames = a_names;
            card.onSave = a_onSave;
            card.onIntSliderChange = a_onChange;
            return card;

        }

    };

    void DrawSettingCard(const std::string& a_cardId, CardContent& a_card);

    // ==================================================================================================================
    //  Weighted score card: a base value plus an optional toggleable bonus,
    //  displayed as icon + label + total on top, then a base slider and a
    //  bonus checkbox+slider below. Useful for any "priority/weight" system
    //  (e.g. this mod's POI scoring), not just Skyrim actors.
    // ==================================================================================================================

    struct ScoreCardData {

        const char* icon;
        const char* label;
        uint32_t                                iconColor;

        float* baseScore;
        float                                   baseDefault;
        float                                   baseMin = 0.0f;
        float                                   baseMax = 2000.0f;

        bool* bonusEnabled;
        bool                                    bonusEnabledDefault;

        float* bonusFactor;
        float                                   bonusFactorDefault;
        float                                   bonusMin = 0.0f;
        float                                   bonusMax = 1000.0f;

        const char* tooltip;
        const char* bonusTooltip;

        std::function<void()>                   onSave;

        static ScoreCardData Make(const char* a_icon, const char* a_label, uint32_t a_iconColor,
            float* a_baseScore, float a_baseDefault, bool* a_bonusEnabled, bool a_bonusEnabledDefault,
            float* a_bonusFactor, float a_bonusFactorDefault, const char* a_tooltip, const char* a_bonusTooltip,
            std::function<void()> a_onSave, float a_baseMin = 0.0f, float a_baseMax = 2000.0f,
            float a_bonusMin = 0.0f, float a_bonusMax = 1000.0f) {

            ScoreCardData card{};
            card.icon = a_icon;
            card.label = a_label;
            card.iconColor = a_iconColor;
            card.baseScore = a_baseScore;
            card.baseDefault = a_baseDefault;
            card.baseMin = a_baseMin;
            card.baseMax = a_baseMax;
            card.bonusEnabled = a_bonusEnabled;
            card.bonusEnabledDefault = a_bonusEnabledDefault;
            card.bonusFactor = a_bonusFactor;
            card.bonusFactorDefault = a_bonusFactorDefault;
            card.bonusMin = a_bonusMin;
            card.bonusMax = a_bonusMax;
            card.tooltip = a_tooltip;
            card.bonusTooltip = a_bonusTooltip;
            card.onSave = a_onSave;
            return card;

        }

    };

    void DrawScoreCard(const ScoreCardData& a_card);

    // ==================================================================================================================
    //  Multi-checkbox card: several related toggles sharing one reset button,
    //  with optional nested children shown only while their parent is enabled.
    // ==================================================================================================================

    struct CheckboxItem {

        const char*                             icon;
        uint32_t                                iconColor;
        bool*                                   value;
        bool                                    defaultValue;
        const char*                             label;

        std::vector<std::string>                tooltipLines;
        std::function<void()>                   onChange; // optional, called when this box is toggled
        std::vector<CheckboxItem>               children; // drawn indented, only while *value is true

    };

    // a_onSave is called any time any checkbox (top-level or nested) is toggled
    // or reset — hook your save/ini logic here.
    void DrawMultiCheckboxCard(const std::string& a_cardId, const std::string& a_title, const char* a_icon, std::vector<CheckboxItem> a_items, const std::function<void()>& a_onSave);

    void ApplyCardContentLineMargin(bool a_isSliderLine = false);

}