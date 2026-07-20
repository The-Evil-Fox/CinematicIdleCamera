#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace logger = SKSE::log;

// ==================================================================================================================================================================================
//  Skyrim Constants
// ==================================================================================================================================================================================

static constexpr float              SKYRIM_UNITS_TO_METERS                                      = 70.0f;  // 1 meter = 70 game units

// ==================================================================================================================================================================================
//  UI Layout constants
// ==================================================================================================================================================================================

static constexpr float              k_iconColumnWidth                                           = 50.0f;
static constexpr float              k_spaceAfterIcon                                            = 15.0f;
static constexpr float              k_marginBetweenBordersInMenuHeader                          = 30.0f;

// ==================================================================================================================================================================================
//  Default settings used when the ini doesn't exist when the game is started (ordered by menus)
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Camera
// =====================================================================================================================

// Main Settings

static constexpr float              k_defaultIdleTimer                                          = 5.0f;
static constexpr bool               k_defaultBlackBarsEnabled                                   = true;
static constexpr float              k_defaultBlackBarsSpeed                                     = 1.0f;
static constexpr bool               k_defaultBlackBarsSoundEnabled                              = true;

// Position/Transition

static constexpr float              k_defaultVanityCamOffsetX                                   = 75.0f;
static constexpr float              k_defaultVanityCamOffsetY                                   = 130.0f;
static constexpr float              k_defaultVanityCamOffsetZ                                   = 0.0f;
static constexpr float              k_defaultBlendDuration                                      = 5.0f;

// Zoom/Dezoom

static constexpr float              k_defaultDezoomTriggerRadius                                = 350.0f;
static constexpr float              k_defaultDezoomTriggerHeight                                = 210.0f;
static constexpr float              k_defaultDezoomAmount                                       = 250.0f;
static constexpr float              k_defaultDezoomBlendSpeed                                   = 0.7f;

// =====================================================================================================================
//  Head Tracking
// =====================================================================================================================

static constexpr float              k_defaultHeadTrackFadeSpeed = 0.3f;

// =====================================================================================================================
//  POI System
// =====================================================================================================================

// Main settings

static constexpr bool               k_defaultPoiSystemEnabled                                   = true;
static constexpr bool               k_defaultActorPoiEnabled                                    = true;
static constexpr bool               k_defaultPreventFollowers                                   = true;
static constexpr bool               k_defaultFlyingCritterPoiEnabled                            = true;
static constexpr bool               k_defaultFishCritterPoiEnabled                              = true;
static constexpr float              k_defaultPoiDetectionRadius                                 = 1050.0f;
static constexpr float              k_defaultLockDuration                                       = 5.0f;

// Actors score system

static constexpr float              k_defaultDragonScore                                        = 2000.0f;
static constexpr bool               k_defaultDragonProximityEnabled                             = true;
static constexpr float              k_defaultDragonProximityFactor                              = 1000.0f;

static constexpr float              k_defaultActorCombatScore                                   = 600.0f;
static constexpr bool               k_defaultActorCombatProximityEnabled                        = true;
static constexpr float              k_defaultActorCombatProximityFactor                         = 200.0f;

static constexpr float              k_defaultActorMovingScore                                   = 400.0f;
static constexpr bool               k_defaultActorMovingProximityEnabled                        = true;
static constexpr float              k_defaultActorMovingProximityFactor                         = 150.0f;

static constexpr float              k_defaultActorInSceneScore                                  = 300.0f;
static constexpr bool               k_defaultActorInSceneProximityEnabled                       = true;
static constexpr float              k_defaultActorInSceneProximityFactor                        = 50.0f;

static constexpr float              k_defaultActorIdleScore                                     = 10.0f;
static constexpr bool               k_defaultActorIdleProximityEnabled                          = true;
static constexpr float              k_defaultActorIdleProximityFactor                           = 50.0f;

// Flying critters score system

static constexpr float              k_defaultFlyingCritterScore                                 = 400.0f;
static constexpr bool               k_defaultFlyingCritterProximityEnabled                      = true;
static constexpr float              k_defaultFlyingCritterProximityFactor                       = 150.0f;

// Fish critters score system

static constexpr float              k_defaultFishCritterScore                                          = 300.0f;
static constexpr bool               k_defaultFishCritterProximityEnabled                               = true;
static constexpr float              k_defaultFishCritterProximityFactor                                = 150.0f;

// =====================================================================================================================
//  Debug
// =====================================================================================================================

static constexpr bool               k_defaultDebugRaycasts                                      = false;
static constexpr int                k_defaultLoggingLevel                                       = 2; // 0 = critical, 1 = warn, 2 = info, 3 = debug

// ==================================================================================================================================================================================
//  Initalize the sliders in the ui with the default values (used only when the INI doesn't exist or a param in it was completely removed)
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Camera
// =====================================================================================================================

// Main settings

float                               UI::g_idleTimer                                             = k_defaultIdleTimer;
bool                                UI::g_blackBarsEnabled                                      = k_defaultBlackBarsEnabled;
float                               UI::g_blackBarsSpeed                                        = k_defaultBlackBarsSpeed;
bool                                UI::g_blackBarsSoundEnabled                                 = k_defaultBlackBarsSoundEnabled;

// Camera position

float                               UI::g_IdleCamOffsetX                                        = k_defaultVanityCamOffsetX;
float                               UI::g_IdleCamOffsetY                                        = k_defaultVanityCamOffsetY;
float                               UI::g_IdleCamOffsetZ                                        = k_defaultVanityCamOffsetZ;
float                               UI::g_blendDuration                                         = k_defaultBlendDuration;

// Zoom/Dezoom

float                               UI::g_dezoomTriggerRadius                                   = k_defaultDezoomTriggerRadius;
float                               UI::g_dezoomTriggerHeight                                   = k_defaultDezoomTriggerHeight;
float                               UI::g_dezoomAmount                                          = k_defaultDezoomAmount;
float                               UI::g_dezoomBlendSpeed                                      = k_defaultDezoomBlendSpeed;

// =====================================================================================================================
//  Head Tracking
// =====================================================================================================================

float                               UI::g_headTrackFadeSpeed                                    = k_defaultHeadTrackFadeSpeed;

// =====================================================================================================================
//  POI System
// =====================================================================================================================

// Main settings

bool                                UI::g_poiSystemEnabled                                      = k_defaultPoiSystemEnabled;
bool                                UI::g_actorPoiEnabled                                       = k_defaultActorPoiEnabled;
bool                                UI::g_preventFollowers                                      = k_defaultPreventFollowers;
bool                                UI::g_flyingCritterPoiEnabled                               = k_defaultFlyingCritterPoiEnabled;
bool                                UI::g_fishCritterPoiEnabled                                 = k_defaultFishCritterPoiEnabled;
float                               UI::g_poiDetectionRadius                                    = k_defaultPoiDetectionRadius;
float                               UI::g_lockDuration                                          = k_defaultLockDuration;

// Actors scores

float                               UI::g_dragonScore                                           = k_defaultDragonScore;
bool                                UI::g_dragonProximityEnabled                                = k_defaultDragonProximityEnabled;
float                               UI::g_dragonProximityFactor                                 = k_defaultDragonProximityFactor;

float                               UI::g_actorCombatScore                                      = k_defaultActorCombatScore;
bool                                UI::g_actorCombatProximityEnabled                           = k_defaultActorCombatProximityEnabled;
float                               UI::g_actorCombatProximityFactor                            = k_defaultActorCombatProximityFactor;

float                               UI::g_actorMovingScore                                      = k_defaultActorMovingScore;
bool                                UI::g_actorMovingProximityEnabled                           = k_defaultActorMovingProximityEnabled;
float                               UI::g_actorMovingProximityFactor                            = k_defaultActorMovingProximityFactor;

float                               UI::g_actorInSceneScore                                     = k_defaultActorInSceneScore;
bool                                UI::g_actorInSceneProximityEnabled                          = k_defaultActorInSceneProximityEnabled;
float                               UI::g_actorInSceneProximityFactor                           = k_defaultActorInSceneProximityFactor;

float                               UI::g_actorIdleScore                                        = k_defaultActorIdleScore;
bool                                UI::g_actorIdleProximityEnabled                             = k_defaultActorIdleProximityEnabled;
float                               UI::g_actorIdleProximityFactor                              = k_defaultActorIdleProximityFactor;

// Flying critters scores

float                               UI::g_flyingCritterScore                                    = k_defaultFlyingCritterScore;
bool                                UI::g_flyingCritterProximityEnabled                         = k_defaultFlyingCritterProximityEnabled;
float                               UI::g_flyingCritterProximityFactor                          = k_defaultFlyingCritterProximityFactor;

// Fish critters scores

float                               UI::g_fishCritterScore                                      = k_defaultFishCritterScore;
bool                                UI::g_fishCritterProximityEnabled                           = k_defaultFishCritterProximityEnabled;
float                               UI::g_fishCritterProximityFactor                            = k_defaultFishCritterProximityFactor;

// Exclusion list

std::vector<UI::ActorExclusionEntry> UI::g_actorExclusionList;

namespace Hooks {

    extern RE::TESObjectREFR* g_currentPOI;

}

// =====================================================================================================================
//  Debug
// =====================================================================================================================

bool                                UI::g_debugRaycasts                                         = k_defaultDebugRaycasts;
int                                 UI::g_loggingLevel                                          = k_defaultLoggingLevel;

// ==================================================================================================================================================================================
//  Logging level names, indexed 0-3 to match the simplified scheme:
//  0 = Quiet (critical only), 1 = Warnings, 2 = Info (default), 3 = Debug
// ==================================================================================================================================================================================

static constexpr const char* k_loggingLevelNames[] = {

    "Quiet",
    "Warnings",
    "Info",
    "Debug"

};

static constexpr int k_loggingLevelCount = static_cast<int>(std::size(k_loggingLevelNames));

// Mirrors the mapping used in IniParser::Load() / utility.cpp, so the slider's
// live preview matches what gets applied and saved.
static spdlog::level::level_enum LoggingLevelToSpdlog(int loggingLevel) {

    switch (loggingLevel) {

        case 0:  return spdlog::level::critical;

        case 1:  return spdlog::level::warn;

        case 3:  return spdlog::level::debug;

        default: return spdlog::level::info;

    }

}

// ==================================================================================================================================================================================
//  Used to force apply some new settings to the virtual copy of the ini user side. Only used for the idle timer for now.
// ==================================================================================================================================================================================

static void ApplyIdleTimerToIniSettings(std::string settingName, float value) {

    auto* iniSettings = RE::INISettingCollection::GetSingleton();

    if (iniSettings) {

        auto* setting = iniSettings->GetSetting(settingName);

        if (setting) {

            setting->data.f = value;

        } else {

            logger::error("Setting not found in INISettingCollection!");

        }

    }

}

// ==================================================================================================================================================================================
//  Font Awesome Icons
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Section Icons (Headers)
// =====================================================================================================================

auto cameraIcon = FontAwesome::UnicodeToUtf8(0xf03d);
auto headTrackIcon = FontAwesome::UnicodeToUtf8(0xf06e);
auto poiSystemIcon = FontAwesome::UnicodeToUtf8(0xf3c5);
auto debugIcon = FontAwesome::UnicodeToUtf8(0xf7d9);

// =====================================================================================================================
//  Arrows & Direction Icons
// =====================================================================================================================

auto arrowUpIcon = FontAwesome::UnicodeToUtf8(0xf062);
auto anglesRightIcon = FontAwesome::UnicodeToUtf8(0xf101);
auto arrowLeftAndRightIcon = FontAwesome::UnicodeToUtf8(0xf07e);
auto arrowUpAndDownIcon = FontAwesome::UnicodeToUtf8(0xf07d);
auto zoomIcon = FontAwesome::UnicodeToUtf8(0xf002);

// =====================================================================================================================
//  Camera Settings Icons
// =====================================================================================================================

auto clockIcon = FontAwesome::UnicodeToUtf8(0xf017);
auto filmIcon = FontAwesome::UnicodeToUtf8(0xf008);
auto speedIcon = FontAwesome::UnicodeToUtf8(0xf625);
auto soundIcon = FontAwesome::UnicodeToUtf8(0xf028);
auto radiusIcon = FontAwesome::UnicodeToUtf8(0xf192);

// =====================================================================================================================
//  POI System Icons
// =====================================================================================================================

auto poiTypesIcon = FontAwesome::UnicodeToUtf8(0xf5fd);
auto ExcludeListIcon = FontAwesome::UnicodeToUtf8(0xf023);
auto poiLockIcon = FontAwesome::UnicodeToUtf8(0xe51f);
auto followerIcon = FontAwesome::UnicodeToUtf8(0xe535);

// =====================================================================================================================
//  Actor POI Icons
// =====================================================================================================================

auto dragonScoreIcon = FontAwesome::UnicodeToUtf8(0xf6d5);
auto inCombatScoreIcon = FontAwesome::UnicodeToUtf8(0xf0e3);
auto movingScoreIcon = FontAwesome::UnicodeToUtf8(0xf554);
auto inSceneScoreIcon = FontAwesome::UnicodeToUtf8(0xf630);
auto personIcon = FontAwesome::UnicodeToUtf8(0xf183);

// =====================================================================================================================
//  Critter POI Icons
// =====================================================================================================================

auto flyingCritterIcon = FontAwesome::UnicodeToUtf8(0xf4ba);
auto fishCritterIcon = FontAwesome::UnicodeToUtf8(0xf578);

// =====================================================================================================================
//  Exclusion List Icons
// =====================================================================================================================

auto trashIcon = FontAwesome::UnicodeToUtf8(0xf2ed);
auto addIcon = FontAwesome::UnicodeToUtf8(0xf067);
auto infoIcon = FontAwesome::UnicodeToUtf8(0xf05a);

// =====================================================================================================================
//  Debug Icons
// =====================================================================================================================

auto raycastIcon = FontAwesome::UnicodeToUtf8(0xf05b);
auto loggingIcon = FontAwesome::UnicodeToUtf8(0xf120);

// =====================================================================================================================
//  General UI Icons
// =====================================================================================================================

auto resetIcon = FontAwesome::UnicodeToUtf8(0xf2ea);
auto deniedIcon = FontAwesome::UnicodeToUtf8(0xf05e);
auto folderOpenIcon = FontAwesome::UnicodeToUtf8(0xf07c);

// =====================================================================================================================
//  Hexadecimal colors
// =====================================================================================================================

static const uint32_t               k_hexRed                            = 0xe74c3c;
static const uint32_t               k_hexGreen                          = 0x7ef566;
static const uint32_t               k_hexGoldLight                      = 0xF5D966;
static const uint32_t               k_hexBlue                           = 0x036ffc;
static const uint32_t               k_hexDarkRed                        = 0xc0392b;
static const uint32_t               k_hexDarkerRed                      = 0x922b21;
static const uint32_t               k_hexBrightGreen                    = 0x2ecc71;
static const uint32_t               k_hexDarkGreen                      = 0x27ae60;
static const uint32_t               k_hexDarkerGreen                    = 0x1e8449;
static const uint32_t               k_hexWarning                        = 0xf39c12;
static const uint32_t               k_hexCritterPink                    = 0xf566dd;
static const uint32_t               k_hexFishCyan                       = 0x1ff0ff;

// ==================================================================================================================================================================================
//  UI Helper function to convert hex color values to ImVec4
// ==================================================================================================================================================================================

inline ImGuiMCP::ImVec4 HexToImVec4(uint32_t hex) {

    // Pass "0x"+hex code of the color as a param

    float r = ((hex >> 16) & 0xFF) / 255.0f;
    float g = ((hex >> 8) & 0xFF) / 255.0f;
    float b = (hex & 0xFF) / 255.0f;

    return ImGuiMCP::ImVec4{ r, g, b, 1.0f };

}

// ==================================================================================================================================================================================
//  Reset Button Helper with separator and spacing
// ==================================================================================================================================================================================

// Structure to hold a setting value and its default, with optional custom reset logic
template<typename T>

struct SettingWithDefault {
    
    T* currentValue;
    T defaultValue;
    std::function<void()> customResetAction;  // Optional custom action

    SettingWithDefault(T* value, T defaultVal, std::function<void()> customAction = nullptr) : currentValue(value), defaultValue(defaultVal), customResetAction(customAction) {}

};

// Helper to check if any settings have changed from their defaults
template<typename... Args>

static bool HasSettingsChanged(const Args&... settings) {

    bool changed = false;
    ([&]() {

        if (*(settings.currentValue) != settings.defaultValue) {

            changed = true;

        }

    }(), ...);

    return changed;

}

struct ExclusionListReset {

    std::vector<UI::ActorExclusionEntry>* list;

    ExclusionListReset(std::vector<UI::ActorExclusionEntry>* l) : list(l) {}

    bool HasChanged() const {

        return !list->empty();

    }

    void Reset() const {

        list->clear();
        IniParser::Save();
        logger::info("POI Exclusion List cleared.");

    }

};

// Helper to reset multiple settings to their defaults
template<typename... Args>

static void ResetSettings(const std::string& sectionName, const Args&... settings) {

    ([&]() {

        *(settings.currentValue) = settings.defaultValue;

        if (settings.customResetAction) {

            settings.customResetAction();

        }

    }(), ...);

    IniParser::Save();
    logger::info("{} reset to default values.", sectionName);

}

// =====================================================================================================================
//  Draw Reset Button with separator and spacing:
// 
//  Shows only when any setting in the list has changed & always reserves space to prevent layout shifts
// =====================================================================================================================

template<typename... Args>

static void DrawResetButtonWithSeparator(const std::string& sectionName, const std::string& buttonId, const Args&... settings) {

    // Check if any settings have changed
    const bool hasChanges = HasSettingsChanged(settings...);

    // Get the available width for positioning
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    // Calculate the full button text
    std::string fullButtonText = std::format("{} Reset To Default##{}", resetIcon, buttonId);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, fullButtonText.c_str(), nullptr, true, -1.0f);

    // Calculate button width (add padding for button margins)
    float buttonWidth = resetTextSize.x + 20.0f;

    // Define margin from edges (30px on each side)
    const float kEdgeMargin = 30.0f;

    // Calculate the position for the button then align to the right with margin
    float buttonPosX = ImGuiMCP::GetCursorPosX() + availWidth - buttonWidth - kEdgeMargin;
    ImGuiMCP::SetCursorPosX(buttonPosX);

    // Store the current cursor Y position
    float cursorY = ImGuiMCP::GetCursorPosY();

    // Get the height of the current text line
    float textHeight = ImGuiMCP::GetTextLineHeight();
    float frameHeight = ImGuiMCP::GetFrameHeight();

    // Calculate vertical offset to align button with text
    float verticalOffset = (frameHeight - textHeight) / 2.0f;

    if (hasChanges) {

        // Adjust cursor Y to align button with text
        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);

        // Show the actual button
        if (ImGuiMCP::Button(fullButtonText.c_str())) {

            ResetSettings(sectionName, settings...);

        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Restores all %s settings on this page to their default values.", sectionName.c_str());
            ImGuiMCP::EndTooltip();

        }

    } else {

        // Reserve space with a dummy that matches button height
        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(buttonWidth, frameHeight));

    }

    // Restore cursor Y for the separator
    ImGuiMCP::SetCursorPosY(cursorY + frameHeight + 5.0f);
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

}

// OVERLOAD for ExclusionListReset
// Always reserves space to prevent layout shifts
static inline void DrawResetButtonWithSeparator(const std::string& sectionName, const std::string& buttonId, const ExclusionListReset& exclusionSettings) {

    const bool hasChanges = exclusionSettings.HasChanged();

    // Get the available width for positioning
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    // Calculate the full button text
    std::string fullButtonText = std::format("{} Clear All Exclusions##{}", resetIcon, buttonId);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, fullButtonText.c_str(), nullptr, true, -1.0f);

    // Calculate button width (add padding for button margins)
    float buttonWidth = resetTextSize.x + 20.0f;

    // Define margin from edges (30px on each side)
    const float kEdgeMargin = 30.0f;

    // Calculate the position for the button then align to the right with margin
    float buttonPosX = ImGuiMCP::GetCursorPosX() + availWidth - buttonWidth - kEdgeMargin;
    ImGuiMCP::SetCursorPosX(buttonPosX);

    // Store the current cursor Y position
    float cursorY = ImGuiMCP::GetCursorPosY();

    // Get the height of the current text line
    float textHeight = ImGuiMCP::GetTextLineHeight();
    float frameHeight = ImGuiMCP::GetFrameHeight();

    // Calculate vertical offset to align button with text
    float verticalOffset = (frameHeight - textHeight) / 2.0f;

    if (hasChanges) {

        // Adjust cursor Y to align button with text
        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);

        // Show the actual button
        if (ImGuiMCP::Button(fullButtonText.c_str())) {

            exclusionSettings.Reset();

        }

        if (ImGuiMCP::IsItemHovered()) {
        
            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Removes all actors from the %s list.", sectionName.c_str());
            ImGuiMCP::EndTooltip();
        
        }

    } else {

        // Reserve space with a dummy that matches button height
        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(buttonWidth, frameHeight));

    }

    // Restore cursor Y for the separator
    ImGuiMCP::SetCursorPosY(cursorY + frameHeight + 5.0f);
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

}

// =====================================================================================================================
//  Draw UI Header with Reset Button - Complete header with margins and separator
// =====================================================================================================================

template<typename... Args>

static void DrawUIHeaderWithReset(const std::string& title, const std::string& icon, const std::string& sectionName, const std::string& buttonId, const Args&... settings) {

    // Add top padding
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);

    // Pull the title toward the left border, leaving a small margin
    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);

    float startY = ImGuiMCP::GetCursorPosY();

    // Draw the title with gold color
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
    ImGuiMCP::Text("%s", icon.c_str());
    ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
    ImGuiMCP::Text("%s", title.c_str());
    ImGuiMCP::PopStyleColor();

    // Check if any settings have changed
    const bool hasChanges = HasSettingsChanged(settings...);

    // Calculate the full button text
    std::string fullButtonText = std::format("{} Reset To Default##{}", resetIcon, buttonId);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, fullButtonText.c_str(), nullptr, true, -1.0f);

    // Match ImGui's actual button sizing formula: label size + FramePadding.x * 2
    ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
    float buttonWidth = resetTextSize.x + style->FramePadding.x * 2.0f;

    // Push the button toward the right border, leaving a small margin
    float buttonPosX = winSize.x - k_marginBetweenBordersInMenuHeader - buttonWidth;
    ImGuiMCP::SetCursorPosX(buttonPosX);

    float cursorY = startY;

    // Get the height of the current text line
    float textHeight = ImGuiMCP::GetTextLineHeight();
    float frameHeight = ImGuiMCP::GetFrameHeight();

    // Calculate vertical offset to align button with text
    float verticalOffset = (frameHeight - textHeight) / 2.0f;

    if (hasChanges) {

        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);

        if (ImGuiMCP::Button(fullButtonText.c_str())) {

            ResetSettings(sectionName, settings...);

        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Restores all %s settings on this page to their default values.", sectionName.c_str());
            ImGuiMCP::EndTooltip();

        }

    } else {

        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(buttonWidth, frameHeight));

    }

    // Restore cursor Y for the separator
    ImGuiMCP::SetCursorPosY(cursorY + frameHeight + 5.0f);
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

}

// Overload for ExclusionListReset
static void DrawUIHeaderWithReset(const std::string& title, const std::string& icon, const std::string& sectionName, const std::string& buttonId, const ExclusionListReset& exclusionSettings) {

    // Add top padding
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);

    // Pull the title toward the left border, leaving a small margin
    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);

    float startY = ImGuiMCP::GetCursorPosY();

    // Draw the title with gold color
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
    ImGuiMCP::Text("%s", icon.c_str());
    ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
    ImGuiMCP::Text("%s", title.c_str());
    ImGuiMCP::PopStyleColor();

    // Check if any settings have changed
    const bool hasChanges = exclusionSettings.HasChanged();

    // Calculate the full button text
    std::string fullButtonText = std::format("{} Clear All Exclusions##{}", resetIcon, buttonId);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, fullButtonText.c_str(), nullptr, true, -1.0f);

    // Match ImGui's actual button sizing formula: label size + FramePadding.x * 2
    ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
    float buttonWidth = resetTextSize.x + style->FramePadding.x * 2.0f;

    // Push the button toward the right border, leaving a small margin
    float buttonPosX = winSize.x - k_marginBetweenBordersInMenuHeader - buttonWidth;
    ImGuiMCP::SetCursorPosX(buttonPosX);

    float cursorY = startY;

    // Get the height of the current text line
    float textHeight = ImGuiMCP::GetTextLineHeight();
    float frameHeight = ImGuiMCP::GetFrameHeight();

    // Calculate vertical offset to align button with text
    float verticalOffset = (frameHeight - textHeight) / 2.0f;

    if (hasChanges) {

        // Adjust cursor Y to align button with text
        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);

        // Show the actual button
        if (ImGuiMCP::Button(fullButtonText.c_str())) {

            exclusionSettings.Reset();
        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Removes all actors from the %s list.", sectionName.c_str());
            ImGuiMCP::EndTooltip();

        }

    } else {

        // Reserve space with a dummy that matches button height
        ImGuiMCP::SetCursorPosY(cursorY - verticalOffset);
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(buttonWidth, frameHeight));

    }

    // Restore cursor Y for the separator
    ImGuiMCP::SetCursorPosY(cursorY + frameHeight + 5.0f);
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

}

// =====================================================================================================================
//  Draw a standardized setting card with reset button
// =====================================================================================================================

// Structure to hold card content
struct CardContent {

    const char* icon;           // FontAwesome icon
    const char* label;          // Display name
    const char* tooltipText;    // Helper text shown at bottom
    bool hasSlider;             // Whether this card has a float slider
    float* sliderValue;         // Pointer to the value (if hasSlider)
    float sliderMin;            // Min value
    float sliderMax;            // Max value
    const char* sliderFormat;   // Format string for slider
    float sliderDefault;        // Default value for the slider
    bool hasCheckbox;           // Whether this card has a checkbox
    bool* checkboxValue;        // Pointer to checkbox value (if hasCheckbox)
    bool checkboxDefault;       // Default value for checkbox
    std::function<void()> onSliderChange;  // Optional callback when slider changes
    std::function<void()> onCheckboxChange; // Optional callback when checkbox changes
    bool convertToMeters = false; // Can be set to true when creating a new setting card for some kind of distance setting that needs to be displayed in meters.

    // Enum-style int slider (named steps, e.g. logging level) mutually exclusive with hasSlider/hasCheckbox
    bool hasIntSlider = false;
    int* intSliderValue = nullptr;
    int intSliderMin = 0;
    int intSliderMax = 0;
    int intSliderDefault = 0;
    const char* const* intSliderNames = nullptr;   // array of display names indexed by value
    std::function<void()> onIntSliderChange;

};

// Helper to check if card settings have changed
static bool HasCardSettingsChanged(const CardContent& card) {

    bool changed = false;

    if (card.hasSlider && *card.sliderValue != card.sliderDefault) {

        changed = true;

    }

    if (card.hasCheckbox && *card.checkboxValue != card.checkboxDefault) {

        changed = true;

    }

    if (card.hasIntSlider && *card.intSliderValue != card.intSliderDefault) {

        changed = true;

    }

    return changed;

}

// Helper to reset card settings to defaults
static void ResetCardSettings(CardContent& card) {

    if (card.hasSlider) {

        *card.sliderValue = card.sliderDefault;

        if (card.onSliderChange) {

            card.onSliderChange();

        }

    }

    if (card.hasCheckbox) {

        *card.checkboxValue = card.checkboxDefault;

        if (card.onCheckboxChange) {

            card.onCheckboxChange();

        }

    }

    if (card.hasIntSlider) {

        *card.intSliderValue = card.intSliderDefault;

        if (card.onIntSliderChange) {

            card.onIntSliderChange();

        }

    }

    IniParser::Save();

}

// Draw a standardized setting card
static void DrawSettingCard(const std::string& cardId, CardContent& card) {

    const bool hasChanges = HasCardSettingsChanged(card);

    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);

    // Calculate card width to match header's right margin
    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);
    float cardWidth = winSize.x - (k_marginBetweenBordersInMenuHeader * 2);

    // Begin child with explicit width
    ImGuiMCP::BeginChild(cardId.c_str(), ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    float startY = ImGuiMCP::GetCursorPosY();
    float startX = ImGuiMCP::GetCursorPosX();

    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
    ImGuiMCP::Text("%s", card.icon);
    ImGuiMCP::PopStyleColor();

    // Label is placed at a fixed column so labels align across cards,
    // regardless of each icon glyph's own width.
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(startX + k_iconColumnWidth + k_spaceAfterIcon);
    ImGuiMCP::Text("%s", card.label);

    float currentX = ImGuiMCP::GetCursorPosX();

    // Helper: value actually shown to the user (converted to meters if needed)
    auto DisplayValue = [&](float raw) {

        return card.convertToMeters ? (raw / SKYRIM_UNITS_TO_METERS) : raw;

    };

    // Determine the current value text for width calculation
    std::string currentValueText = "";

    if (card.hasSlider) {

        char buf[64];
        std::snprintf(buf, sizeof(buf), card.sliderFormat, DisplayValue(*card.sliderValue));
        currentValueText = buf;

    } else if (card.hasCheckbox) {

        currentValueText = *card.checkboxValue ? "Enabled" : "Disabled";

    } else if (card.hasIntSlider) {

        currentValueText = card.intSliderNames[*card.intSliderValue];

    }

    ImGuiMCP::ImVec2 valueTextSize;
    ImGuiMCP::CalcTextSize(&valueTextSize, currentValueText.c_str(), nullptr, false, -1.0f);

    const float margin = 10.0f;
    float rightEdge = startX + availWidth - margin;

    // Value position is FIXED — it never depends on the reset button.
    const float valueStartX = rightEdge - valueTextSize.x;
    float minX = currentX + 10.0f;

    float resetStartX = valueStartX; // unused when there's no reset button

    if (hasChanges) {

        // Real button width, computed once, up front (no more +4.0f guess).
        ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
        ImGuiMCP::ImVec2 resetIconSize;
        ImGuiMCP::CalcTextSize(&resetIconSize, resetIcon.c_str(), nullptr, false, -1.0f);
        float resetButtonWidth = resetIconSize.x + style->FramePadding.x * 2.0f;

        float visualGap = std::max(0.0f, k_spaceAfterIcon - style->FramePadding.x);

        resetStartX = valueStartX - resetButtonWidth - visualGap;

        // Only the button gets clamped if the row is too tight —
        // the value text never moves.
        if (resetStartX < minX) {

            resetStartX = minX;

        }

        std::string resetButtonText = std::format("{}##reset_{}", resetIcon, cardId);

        ImGuiMCP::SetCursorPosX(resetStartX);
        ImGuiMCP::SetCursorPosY(startY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_BorderShadow, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

        if (ImGuiMCP::Button(resetButtonText.c_str())) {

            ResetCardSettings(card);

        }

        ImGuiMCP::PopStyleColor(6);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Reset this card's settings to default values.");
            ImGuiMCP::EndTooltip();

        }

        // Use the modified valueStartX
        ImGuiMCP::SetCursorPosX(valueStartX);

    }

    ImGuiMCP::SetCursorPosX(valueStartX);
    ImGuiMCP::SetCursorPosY(startY);

    if (card.hasCheckbox) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, *card.checkboxValue ? HexToImVec4(k_hexBrightGreen) : HexToImVec4(k_hexRed));
        ImGuiMCP::Text("%s", *card.checkboxValue ? "Enabled" : "Disabled");
        ImGuiMCP::PopStyleColor();

    } else if (card.hasIntSlider) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
        ImGuiMCP::Text("%s", card.intSliderNames[*card.intSliderValue]);
        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
        ImGuiMCP::Text(card.sliderFormat, DisplayValue(*card.sliderValue));
        ImGuiMCP::PopStyleColor();

    }

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();

        if (card.hasSlider) {


            char curBuf[64];
            std::snprintf(curBuf, sizeof(curBuf), card.sliderFormat, DisplayValue(*card.sliderValue));
            ImGuiMCP::Text("Current: %s", curBuf);

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));

            char defBuf[64];
            std::snprintf(defBuf, sizeof(defBuf), card.sliderFormat, DisplayValue(card.sliderDefault));
            ImGuiMCP::Text("Default: %s", defBuf);

            ImGuiMCP::PopStyleColor();

        } else if (card.hasCheckbox) {

            ImGuiMCP::Text("Current: %s", *card.checkboxValue ? "Enabled" : "Disabled");
            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
            ImGuiMCP::Text("Default: %s", card.checkboxDefault ? "Enabled" : "Disabled");
            ImGuiMCP::PopStyleColor();

        } else if (card.hasIntSlider) {

            ImGuiMCP::Text("Current: %s", card.intSliderNames[*card.intSliderValue]);
            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
            ImGuiMCP::Text("Default: %s", card.intSliderNames[card.intSliderDefault]);
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::EndTooltip();
    
    }

    float frameHeight = ImGuiMCP::GetFrameHeight();
    ImGuiMCP::SetCursorPosY(startY + frameHeight + 4.0f);

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // =================================================================================================================
    //  Content (Slider or Checkbox)
    // =================================================================================================================

    if (card.hasSlider) {

        ImGuiMCP::SetNextItemWidth(-1.0f);

        if (card.convertToMeters) {

            // Slider operates on a temp value in meters; convert back to game units on change
            float metersValue = *card.sliderValue / SKYRIM_UNITS_TO_METERS;
            float metersMin = card.sliderMin / SKYRIM_UNITS_TO_METERS;
            float metersMax = card.sliderMax / SKYRIM_UNITS_TO_METERS;

            if (ImGuiMCP::SliderFloat("##slider", &metersValue, metersMin, metersMax, card.sliderFormat)) {
                *card.sliderValue = metersValue * SKYRIM_UNITS_TO_METERS;
                IniParser::Save();
                
                if (card.onSliderChange) {

                    card.onSliderChange();

                }

            }

        } else {

            if (ImGuiMCP::SliderFloat("##slider", card.sliderValue, card.sliderMin, card.sliderMax, card.sliderFormat)) {
                
                IniParser::Save();
                
                if (card.onSliderChange) {
                    
                    card.onSliderChange();
                
                }
            
            }

        }
    }

    if (card.hasCheckbox) {

        if (ImGuiMCP::Checkbox("##checkbox", card.checkboxValue)) {

            IniParser::Save();

            if (card.onCheckboxChange) {

                card.onCheckboxChange();

            }

        }

    }

    if (card.hasIntSlider) {

        ImGuiMCP::SetNextItemWidth(-1.0f);

        if (ImGuiMCP::SliderInt("##intSlider", card.intSliderValue, card.intSliderMin, card.intSliderMax, card.intSliderNames[*card.intSliderValue])) {

            *card.intSliderValue = std::clamp(*card.intSliderValue, card.intSliderMin, card.intSliderMax);

            IniParser::Save();

            if (card.onIntSliderChange) {

                card.onIntSliderChange();

            }

        }

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    if (card.tooltipText) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
        ImGuiMCP::Text("%s", card.tooltipText);
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

}

// =====================================================================================================================
//  Draw POI score cards (actors & critters)
// =====================================================================================================================

struct ScoreCardData {

    // Display
    const char* icon;                   // FontAwesome icon code
    const char* label;                  // Display name
    uint32_t iconColor;                 // Hex color for the icon

    // Settings pointers
    float* baseScore;                   // Points to g_dragonScore, g_actorCombatScore, etc.
    float baseDefault;                  // Default value for reset

    bool* proximityEnabled;             // Points to g_dragonProximityEnabled, etc.
    bool proximityEnabledDefault;       // Default value for reset

    float* proximityFactor;             // Points to g_dragonProximityFactor, etc.
    float proximityFactorDefault;       // Default value for reset

    // Help text
    const char* tooltip;                // Description shown in tooltip
    const char* proximityTooltip;       // Description for the proximity bonus

};

// =====================================================================================================================
//  Actor Score Cards Data
// =====================================================================================================================

static const std::vector<ScoreCardData> actorCards = {

    {

        dragonScoreIcon.c_str(),
        "Dragon",
        k_hexGreen,
        &UI::g_dragonScore,
        k_defaultDragonScore,
        &UI::g_dragonProximityEnabled,
        k_defaultDragonProximityEnabled,
        &UI::g_dragonProximityFactor,
        k_defaultDragonProximityFactor,
        "Base score awarded to a dragon.",
        "Bonus increases as dragon gets closer."

    },
    {

        inCombatScoreIcon.c_str(),
        "In Combat",
        k_hexGreen,
        &UI::g_actorCombatScore,
        k_defaultActorCombatScore,
        &UI::g_actorCombatProximityEnabled,
        k_defaultActorCombatProximityEnabled,
        &UI::g_actorCombatProximityFactor,
        k_defaultActorCombatProximityFactor,
        "Base score awarded to an actor who is currently in combat.",
        "Bonus increases as actor gets closer."

    },
    {

        movingScoreIcon.c_str(),
        "Moving",
        k_hexGreen,
        &UI::g_actorMovingScore,
        k_defaultActorMovingScore,
        &UI::g_actorMovingProximityEnabled,
        k_defaultActorMovingProximityEnabled,
        &UI::g_actorMovingProximityFactor,
        k_defaultActorMovingProximityFactor,
        "Base score awarded to an actor who is currently moving.",
        "Bonus increases as actor gets closer."

    },
    {

        inSceneScoreIcon.c_str(),
        "In Scene",
        k_hexGreen,
        &UI::g_actorInSceneScore,
        k_defaultActorInSceneScore,
        &UI::g_actorInSceneProximityEnabled,
        k_defaultActorInSceneProximityEnabled,
        &UI::g_actorInSceneProximityFactor,
        k_defaultActorInSceneProximityFactor,
        "Base score awarded to an actor who is actively engaged in a scripted sequence (dialogue, cinematic, or quest scene).",
        "Bonus increases as actor gets closer."

    },
    {

        personIcon.c_str(),
        "Idle",
        k_hexGreen,
        &UI::g_actorIdleScore,
        k_defaultActorIdleScore,
        &UI::g_actorIdleProximityEnabled,
        k_defaultActorIdleProximityEnabled,
        &UI::g_actorIdleProximityFactor,
        k_defaultActorIdleProximityFactor,
        "Base score awarded to an actor who is in an idle animation (not moving, not in combat and not in a scene).",
        "Bonus increases as actor gets closer."

    }

};

// =====================================================================================================================
//  Critter Score Cards Data
// =====================================================================================================================

static const std::vector<ScoreCardData> critterCards = {

    {

        flyingCritterIcon.c_str(),
        "Flying Critter",
        k_hexCritterPink,
        &UI::g_flyingCritterScore,
        k_defaultFlyingCritterScore,
        &UI::g_flyingCritterProximityEnabled,
        k_defaultFlyingCritterProximityEnabled,
        &UI::g_flyingCritterProximityFactor,
        k_defaultFlyingCritterProximityFactor,
        "Base score awarded to a flying critter (butterflies, moths, dragonflies, fireflies, bees, etc).",
        "Bonus increases as critter gets closer."

    },
    {

        fishCritterIcon.c_str(),
        "Fish Critter",
        k_hexFishCyan,
        &UI::g_fishCritterScore,
        k_defaultFishCritterScore,
        &UI::g_fishCritterProximityEnabled,
        k_defaultFishCritterProximityEnabled,
        &UI::g_fishCritterProximityFactor,
        k_defaultFishCritterProximityFactor,
        "Base score awarded to a fish critter (perches, salmon, pond fish, and other aquatic critters).",
        "Bonus increases as critter gets closer."

    }

};

// =====================================================================================================================
//  Score card reset helpers
// =====================================================================================================================

static bool HasScoreCardChanged(const ScoreCardData& card) {

    return (*card.baseScore != card.baseDefault) || (*card.proximityEnabled != card.proximityEnabledDefault) || (*card.proximityFactor != card.proximityFactorDefault);

}

static void ResetScoreCard(const ScoreCardData& card) {

    *card.baseScore = card.baseDefault;
    *card.proximityEnabled = card.proximityEnabledDefault;
    *card.proximityFactor = card.proximityFactorDefault;

    IniParser::Save();
    logger::info("{} score settings reset to default values.", card.label);

}

// =====================================================================================================================
//  Draw a single score card
// =====================================================================================================================

static void DrawScoreCard(const ScoreCardData& card) {

    // Set X position to match the header
    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);

    // Calculate card width to match header's right margin
    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);
    float cardWidth = winSize.x - (k_marginBetweenBordersInMenuHeader * 2);

    // Card container with explicit width
    ImGuiMCP::BeginChild(ImGuiMCP::GetID(card.label), ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    float startY = ImGuiMCP::GetCursorPosY();
    float startX = ImGuiMCP::GetCursorPosX();

    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    // Icon + label
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(card.iconColor));
    ImGuiMCP::Text("%s", card.icon);
    ImGuiMCP::PopStyleColor();

    // Same fixed icon-column alignment as DrawSettingCard
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(startX + k_iconColumnWidth + k_spaceAfterIcon);
    ImGuiMCP::Text("%s", card.label);

    float currentX = ImGuiMCP::GetCursorPosX();

    // Check if any of this card's settings differ from their defaults
    const bool hasChanges = HasScoreCardChanged(card);

    float totalScore = *card.baseScore + (*card.proximityEnabled ? *card.proximityFactor : 0.0f);
    float defaultTotalScore = card.baseDefault + (card.proximityEnabledDefault ? card.proximityFactorDefault : 0.0f);

    std::string scoreText = std::format("{:.0f}", totalScore);
    ImGuiMCP::ImVec2 scoreTextSize;
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);

    float margin = 10.0f;
    float rightEdge = startX + availWidth - margin;

    // Score position is FIXED — it never depends on the reset button.
    const float scoreStartX = rightEdge - scoreTextSize.x;
    float minX = currentX + 10.0f;

    float resetStartX = scoreStartX; // unused when there's no reset button

    if (hasChanges) {

        // Real button width, computed once, up front (matches DrawSettingCard).
        ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
        ImGuiMCP::ImVec2 resetIconSize;
        ImGuiMCP::CalcTextSize(&resetIconSize, resetIcon.c_str(), nullptr, false, -1.0f);
        float resetButtonWidth = resetIconSize.x + style->FramePadding.x * 2.0f;

        // Compensate for the button's own FramePadding so the visible gap
        // matches k_spaceAfterIcon, same as the icon-to-label gap.
        float visualGap = std::max(0.0f, k_spaceAfterIcon - style->FramePadding.x);

        resetStartX = scoreStartX - resetButtonWidth - visualGap;

        // Only the button gets clamped if the row is too tight —
        // the score text never moves.
        if (resetStartX < minX) {

            resetStartX = minX;

        }

    }

    // Draw the reset button if visible (pinned to startY so it never affects row height)
    if (hasChanges) {

        std::string resetButtonText = std::format("{}##reset_{}", resetIcon, card.label);

        ImGuiMCP::SetCursorPosX(resetStartX);
        ImGuiMCP::SetCursorPosY(startY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_BorderShadow, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

        if (ImGuiMCP::Button(resetButtonText.c_str())) {

            ResetScoreCard(card);

        }

        ImGuiMCP::PopStyleColor(6);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Reset this card's settings to default values.");
            ImGuiMCP::EndTooltip();

        }

    }

    // Draw the total score (also pinned to startY)
    ImGuiMCP::SetCursorPosX(scoreStartX);
    ImGuiMCP::SetCursorPosY(startY);

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", totalScore);
    ImGuiMCP::PopStyleColor();

    // Total score tooltip
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", totalScore);
        ImGuiMCP::Text("Base Score: %.0f", *card.baseScore);

        if (*card.proximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", *card.proximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
        ImGuiMCP::Text("Default Total Score: %.0f", defaultTotalScore);
        ImGuiMCP::Text("Default Base Score: %.0f", card.baseDefault);

        if (card.proximityEnabledDefault) {

            ImGuiMCP::Text("Default Proximity Bonus: +%.0f", card.proximityFactorDefault);

        } else {

            ImGuiMCP::Text("Default Proximity Bonus: Disabled");

        }

        ImGuiMCP::PopStyleColor();
        ImGuiMCP::EndTooltip();

    }

    // Always advance by the same fixed amount, regardless of whether the button was drawn,
    // so the header row height (and therefore the card's AutoResizeY height) never changes.
    float frameHeight = ImGuiMCP::GetFrameHeight();
    ImGuiMCP::SetCursorPosY(startY + frameHeight + 4.0f);

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("%s", card.tooltip);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    ImGuiMCP::PushID(card.label);

    if (ImGuiMCP::SliderFloat("##Base", card.baseScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopID();

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    ImGuiMCP::PushID(card.label);

    if (ImGuiMCP::Checkbox("##ProxToggle", card.proximityEnabled)) {

        IniParser::Save();

    }

    ImGuiMCP::PopID();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("%s", card.proximityTooltip);
        ImGuiMCP::EndTooltip();

    }

    if (*card.proximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        ImGuiMCP::PushID(card.label);

        if (ImGuiMCP::SliderFloat("##ProxFactor", card.proximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopID();

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // --- Helper text ---
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("%s", card.tooltip);

    if (*card.proximityEnabled) {

        ImGuiMCP::Text("%s", card.proximityTooltip);

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

}

// =====================================================================================================================
//  Draw all actor score cards
// =====================================================================================================================

static void DrawActorScoreCards() {

    for (const auto& card : actorCards) {

        DrawScoreCard(card);

    }

}

// =====================================================================================================================
//  Draw all critter score cards
// =====================================================================================================================

static void DrawCritterScoreCards() {

    for (const auto& card : critterCards) {

        DrawScoreCard(card);

    }

}

// =====================================================================================================================
//  Draw a standardized multi-checkbox card (several related toggles sharing one reset button)
// =====================================================================================================================

// A single checkbox row, with optional nested children shown only while it's enabled
struct CheckboxItem {

    const char* icon;
    uint32_t iconColor;
    bool* value;
    bool defaultValue;
    const char* label;
    std::vector<std::string> tooltipLines;   // description shown in the tooltip
    std::function<void()> onChange;          // optional callback when toggled
    std::vector<CheckboxItem> children;      // drawn indented, only while *value is true

};

// Recursively check if any item (or nested child) differs from its default
static bool HasCheckboxItemsChanged(const std::vector<CheckboxItem>& items) {

    for (const auto& item : items) {

        if (*item.value != item.defaultValue) {

            return true;

        }

        if (HasCheckboxItemsChanged(item.children)) {

            return true;

        }

    }

    return false;

}

// Recursively reset every item (and nested children) to its default
static void ResetCheckboxItems(std::vector<CheckboxItem>& items) {

    for (auto& item : items) {

        *item.value = item.defaultValue;

        if (item.onChange) {

            item.onChange();

        }

        ResetCheckboxItems(item.children);

    }

}

// Draw a single checkbox row (icon, toggle, label, tooltip), then its children if enabled
static void DrawCheckboxItem(const CheckboxItem& item, const std::string& idSuffix) {

    // Capture the row's own left edge BEFORE drawing the icon, so the checkbox
    // (and label after it) can be placed at fixed offsets regardless of how
    // wide this particular icon glyph is. This also naturally accounts for
    // Indent()/Unindent() on nested children, since GetCursorPosX() already
    // reflects the current indent level.
    float rowStartX = ImGuiMCP::GetCursorPosX();

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(item.iconColor));
    ImGuiMCP::Text("%s", item.icon);
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(rowStartX + k_iconColumnWidth + k_spaceAfterIcon);

    std::string checkboxId = std::format("##checkbox_{}", idSuffix);

    if (ImGuiMCP::Checkbox(checkboxId.c_str(), item.value)) {

        IniParser::Save();

        if (item.onChange) {

            item.onChange();

        }

    }

    // Checkbox width is fixed (ImGui's own frame size), so the label's offset
    // from here is already consistent — no column needed for this part.
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);
    ImGuiMCP::Text("%s", item.label);

    if (!item.tooltipLines.empty() && ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();

        for (const auto& line : item.tooltipLines) {

            ImGuiMCP::Text("%s", line.c_str());

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
        ImGuiMCP::Text("Default: %s", item.defaultValue ? "Enabled" : "Disabled");
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::EndTooltip();

    }

    if (!item.children.empty() && *item.value) {

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));
        ImGuiMCP::Indent(40.0f);

        for (size_t i = 0; i < item.children.size(); ++i) {

            DrawCheckboxItem(item.children[i], idSuffix + "_" + std::to_string(i));

        }

        ImGuiMCP::Unindent(40.0f);

    }

}

// Draw the standardized multi-checkbox card
static void DrawMultiCheckboxCard(const std::string& cardId, const std::string& title, const char* icon, std::vector<CheckboxItem> items) {

    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);
    float cardWidth = winSize.x - (k_marginBetweenBordersInMenuHeader * 2);

    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);
    ImGuiMCP::BeginChild(cardId.c_str(), ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    float startY = ImGuiMCP::GetCursorPosY();
    float startX = ImGuiMCP::GetCursorPosX();

    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    // Icon + title
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
    ImGuiMCP::Text("%s", icon);
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(startX + k_iconColumnWidth + k_spaceAfterIcon);
    ImGuiMCP::Text("%s", title.c_str());

    float currentX = ImGuiMCP::GetCursorPosX();

    const bool hasChanges = HasCheckboxItemsChanged(items);

    const float margin = 10.0f;
    float rightEdge = startX + availWidth - margin;
    float minX = currentX + 10.0f;

    // Header row for this card has no fixed value text to its right, so the
    // reset button is simply pinned to the right edge (same math as DrawUIHeaderWithReset's button).
    if (hasChanges) {

        ImGuiMCP::ImGuiStyle* style = ImGuiMCP::GetStyle();
        ImGuiMCP::ImVec2 resetIconSize;
        ImGuiMCP::CalcTextSize(&resetIconSize, resetIcon.c_str(), nullptr, false, -1.0f);
        float resetButtonWidth = resetIconSize.x + style->FramePadding.x * 2.0f;

        float resetStartX = rightEdge - resetButtonWidth;

        if (resetStartX < minX) {

            resetStartX = minX;

        }

        ImGuiMCP::SetCursorPosX(resetStartX);
        ImGuiMCP::SetCursorPosY(startY);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexBlue));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_BorderShadow, ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 0.0f });

        if (ImGuiMCP::Button(std::format("{}##reset_{}", resetIcon, cardId).c_str())) {

            ResetCheckboxItems(items);
            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor(6);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Reset this card's settings to default values.");
            ImGuiMCP::EndTooltip();

        }

    }

    float frameHeight = ImGuiMCP::GetFrameHeight();
    ImGuiMCP::SetCursorPosY(startY + frameHeight + 4.0f);

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

    for (size_t i = 0; i < items.size(); ++i) {

        DrawCheckboxItem(items[i], cardId + "_" + std::to_string(i));

        if (i + 1 < items.size()) {

            ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

        }

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

}

// ==================================================================================================================================================================================
//  Exclusion list specific functions
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Helper: Get actor name from Form ID at runtime (language-independent)
// =====================================================================================================================

static std::string GetActorNameFromFormID(RE::FormID a_formID) {

    auto* form = RE::TESForm::LookupByID(a_formID);

    if (!form) {

        logger::debug("GetActorNameFromFormID: Form 0x{:08X} not found", a_formID);
        return "Unknown";

    }

    // First, search for a loaded reference with this Base ID
    auto* tes = RE::TES::GetSingleton();

    if (tes) {

        std::string foundName;
        int refCount = 0;

        logger::debug("GetActorNameFromFormID: Searching for references with Base ID 0x{:08X}", a_formID);

        tes->ForEachReference([&](RE::TESObjectREFR* a_ref) -> RE::BSContainer::ForEachResult {

            if (!a_ref) return RE::BSContainer::ForEachResult::kContinue;

            refCount++;

            auto* refBase = a_ref->GetBaseObject();

            if (refBase && refBase->GetFormID() == a_formID) {

                const char* refName = a_ref->GetName();
                logger::debug("GetActorNameFromFormID: Found reference with matching Base ID, Name: '{}'", refName ? refName : "null");

                if (refName && refName[0] != '\0') {

                    foundName = refName;
                    return RE::BSContainer::ForEachResult::kStop;

                }

            }

            return RE::BSContainer::ForEachResult::kContinue;
        });

        logger::debug("GetActorNameFromFormID: Checked {} references, found name: '{}'", refCount, foundName.empty() ? "none" : foundName);

        if (!foundName.empty()) {

            return foundName;

        }

    }

    // If no reference found, try as NPC (base form)
    auto* npc = form->As<RE::TESNPC>();

    if (npc) {

        const char* name = npc->GetName();
        logger::debug("GetActorNameFromFormID: Base Form name: '{}'", name ? name : "null");

        if (name && name[0] != '\0') {

            return std::string(name);

        }

    }

    // Fallback to EditorID
    const char* editorID = form->GetFormEditorID();

    if (editorID && editorID[0] != '\0') {

        logger::debug("GetActorNameFromFormID: EditorID: '{}'", editorID);
        return std::string(editorID);

    }

    return "Unknown";

}

// =====================================================================================================================
//  Add the console selected actor to the exclusion list
// =====================================================================================================================

void UI::AddActorToExclusionList(RE::Actor* a_actor) {

    if (!a_actor) {

        logger::warn("AddActorToExclusionList: a_actor is null!");
        return;

    }

    auto* actorBase = a_actor->GetActorBase();

    if (!actorBase) {

        logger::warn("AddActorToExclusionList: actorBase is null!");
        return;

    }

    // Get the actor base ID
    RE::FormID actorBaseID = actorBase->GetFormID();
    logger::info("ActorBase Form ID: 0x{:08X}", actorBaseID);

    RE::FormID finalFormID = actorBaseID;

    // Check if this is a dynamic/temporary ID (starts with FF)
    if ((finalFormID & 0xFF000000) == 0xFF000000) {

        // Try to get the root template
        auto* rootTemplate = actorBase->GetRootFaceNPC();

        if (rootTemplate && rootTemplate != actorBase) {

            RE::FormID rootID = rootTemplate->GetFormID();
            logger::info("Root Template Form ID: 0x{:08X}", rootID);

            // If root ID is not dynamic, use it
            if ((rootID & 0xFF000000) != 0xFF000000) {

                finalFormID = rootID;
                logger::info("Using Root Template Form ID: 0x{:08X}", finalFormID);

            }

        }

        // If still dynamic, try faceNPC
        if ((finalFormID & 0xFF000000) == 0xFF000000) {

            auto* templateNPC = actorBase->faceNPC;

            if (templateNPC) {

                RE::FormID faceID = templateNPC->GetFormID();
                logger::info("faceNPC Form ID: 0x{:08X}", faceID);

                if ((faceID & 0xFF000000) != 0xFF000000) {

                    finalFormID = faceID;
                    logger::info("Using faceNPC Form ID: 0x{:08X}", finalFormID);

                }

            }

        }

    }

    // If we STILL have a dynamic ID (FF), use the Reference ID as last resort
    if ((finalFormID & 0xFF000000) == 0xFF000000) {

        finalFormID = a_actor->GetFormID();
        logger::warn("Using Reference ID as last resort: 0x{:08X}", finalFormID);

    }

    const char* actorName = a_actor->GetName();
    logger::info("=== Adding actor to exclusion list ===");
    logger::info("Actor name: {}", actorName ? actorName : "Unnamed");
    logger::info("Final Form ID: 0x{:08X}", finalFormID);

    // Check if already in list
    for (auto& entry : g_actorExclusionList) {

        if (entry.formID == finalFormID) {

            logger::info("Actor already in exclusion list: {}", actorName ? actorName : "Unnamed");
            return;

        }

    }

    // Only store the Form ID (no name)
    g_actorExclusionList.push_back({ finalFormID });

    logger::info("Added actor (Form ID: 0x{:08X}) to exclusion list", finalFormID);
    IniParser::Save();

    // If this actor is currently the focused POI, clear it
    if (Hooks::g_currentPOI == a_actor) {

        Hooks::g_currentPOI = nullptr;
        logger::debug("Cleared current targeted POI because it was added to exclusion list");

    }

}

// =====================================================================================================================
//  Remove an actor from the exclusion list when pressing the button on its left in the exclusion list
// =====================================================================================================================

void UI::RemoveFromActorExclusionList(size_t index) {

    if (index < g_actorExclusionList.size()) {

        // Get name at runtime for logging
        std::string name = GetActorNameFromFormID(g_actorExclusionList[index].formID);
        logger::info("Removed {} from exclusion list", name);
        g_actorExclusionList.erase(g_actorExclusionList.begin() + index);
        IniParser::Save();

    }

}

// =====================================================================================================================
//  Checks if an actor is excluded. 
//  Also used inside playercamerahook.cpp to check if a potential poi is excluded.
// =====================================================================================================================

bool UI::IsActorExcluded(RE::Actor* a_actor) {

    if (!a_actor) return false;

    auto* actorBase = a_actor->GetActorBase();

    if (!actorBase) return false;

    RE::FormID formID = actorBase->GetFormID();

    // If dynamic, try to get the root template
    if ((formID & 0xFF000000) == 0xFF000000) {

        auto* rootTemplate = actorBase->GetRootFaceNPC();

        if (rootTemplate && rootTemplate != actorBase) {

            RE::FormID rootID = rootTemplate->GetFormID();

            if ((rootID & 0xFF000000) != 0xFF000000) {

                formID = rootID;
            
            }
        
        }

        // If still dynamic, try faceNPC
        if ((formID & 0xFF000000) == 0xFF000000) {

            auto* templateNPC = actorBase->faceNPC;

            if (templateNPC) {

                RE::FormID faceID = templateNPC->GetFormID();
                
                if ((faceID & 0xFF000000) != 0xFF000000) {
                    
                    formID = faceID;
                
                }
            
            }
        
        }
    
    }

    for (auto& entry : g_actorExclusionList) {

        if (entry.formID == formID) {

            return true;

        }
    
    }
    
    return false;

}

// ==================================================================================================================================================================================
//  Register all the different setting sections in the SKSE menu
// ==================================================================================================================================================================================

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {

        return;

    }

    SKSEMenuFramework::SetSection("Cinematic Idle Camera");

    SKSEMenuFramework::AddSectionItem(std::string("Camera/Main Settings"), CameraMainSettings);
    SKSEMenuFramework::AddSectionItem(std::string("Camera/Position"), CameraPositionSettings);
    SKSEMenuFramework::AddSectionItem(std::string("Camera/Zoom & Dezoom"), CameraZoomSettings);

    SKSEMenuFramework::AddSectionItem(std::string("Head Tracking"), HeadTrackingSettings);

    SKSEMenuFramework::AddSectionItem(std::string("POI System/Main Settings"), POISystemMainSettings);
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Exclusion List"), POISystemExclusionListSettings);
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Actor Scores"), POISystemActorScores);
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Critter Scores"), POISystemCritterScores);

    SKSEMenuFramework::AddSectionItem(std::string("Debug"), DebugSettings);

    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

// ==================================================================================================================================================================================
//  Sound effect player
// ==================================================================================================================================================================================

static void playSoundEffect(const std::string& a_filePath) {

    std::string fullPath = "Data\\" + a_filePath;
    std::replace(fullPath.begin(), fullPath.end(), '/', '\\');
    PlaySoundA(fullPath.c_str(), NULL, SND_ASYNC | SND_FILENAME);

}

// ==================================================================================================================================================================================
//  Black bars used to make the vanity mode more cinematic
// ==================================================================================================================================================================================

void UI::DrawCinematicBars() {

    auto* playerCamera = RE::PlayerCamera::GetSingleton();

    if (!playerCamera || !playerCamera->currentState || !g_blackBarsEnabled) {

        return;

    }

    const bool inVanity = playerCamera->currentState->id == RE::CameraState::kAutoVanity;

    // Track previous vanity state
    static bool s_wasInVanity = false;
    static float s_progress = 0.0f;

    auto* io = ImGuiMCP::GetIO();
    const float dt = io->DeltaTime;

    if (inVanity) {

        s_progress = std::min(1.0f, s_progress + g_blackBarsSpeed * dt);

    } else {

        s_progress = std::max(0.0f, s_progress - g_blackBarsSpeed * dt);

    }

    // Only plays the sound effects when sound effects are enabled
    if (g_blackBarsSoundEnabled) {

        if (inVanity != s_wasInVanity) {

            if (inVanity) {

                logger::debug("Cinematic black bars drawing -> Playing entering vanity mode sound effect");

                playSoundEffect("SKSE\\Plugins\\cinematicidlecamera\\FX\\entervanitymode.wav");

            } else {

                logger::debug("Cinematic black bars removing -> Playing exiting vanity mode sound effect");

                playSoundEffect("SKSE\\Plugins\\cinematicidlecamera\\FX\\exitvanitymode.wav");

            }

        }

    }

    s_wasInVanity = inVanity;

    // Early return if bars are fully hidden
    if (s_progress <= 0.0f) {

        return;

    }

    auto* drawList = ImGuiMCP::GetForegroundDrawList();
    float screenW = io->DisplaySize.x;
    float screenH = io->DisplaySize.y;

    const float barHeight = screenH * 0.08f;

    // Smoothstep easing
    const float t = s_progress * s_progress * (3.0f - 2.0f * s_progress);

    const float topBarBottom = barHeight * t;
    const float botBarTop = screenH - barHeight * t;

    const ImGuiMCP::ImU32 barColor = ImGuiMCP::ColorConvertFloat4ToU32(ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });

    // Top bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, topBarBottom - barHeight }, ImGuiMCP::ImVec2{ screenW, topBarBottom }, barColor, 0.0f, 0);

    // Bottom bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, botBarTop }, ImGuiMCP::ImVec2{ screenW, botBarTop + barHeight }, barColor, 0.0f, 0);

}

// ==================================================================================================================================================================================
//  Camera Settings - Main Settings
// ==================================================================================================================================================================================

void UI::CameraMainSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("Camera Settings - Main Settings", cameraIcon, "Camera Main Settings", "resetCameraMain",
        SettingWithDefault(&g_idleTimer, k_defaultIdleTimer, []() {

            ApplyIdleTimerToIniSettings("fAutoVanityModeDelay:Camera", g_idleTimer);

        }),
        SettingWithDefault(&g_blackBarsEnabled, k_defaultBlackBarsEnabled),
        SettingWithDefault(&g_blackBarsSpeed, k_defaultBlackBarsSpeed),
        SettingWithDefault(&g_blackBarsSoundEnabled, k_defaultBlackBarsSoundEnabled)
    );

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Idle Timer Card
    // =====================================================================================================================

    CardContent idleTimerCard = {

        .icon = clockIcon.c_str(),
        .label = "Idle Timer",
        .tooltipText = "How many seconds of player inactivity before the camera switches into idle mode.",
        .hasSlider = true,
        .sliderValue = &g_idleTimer,
        .sliderMin = 1.0f,
        .sliderMax = 300.0f,
        .sliderFormat = "%.0f sec",
        .sliderDefault = k_defaultIdleTimer,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = []() {
            ApplyIdleTimerToIniSettings("fAutoVanityModeDelay:Camera", g_idleTimer);
            logger::info("Camera Idle Timer Setting manually set to {} second(s)", g_idleTimer);
        },
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("idleTimerCard", idleTimerCard);

    // =====================================================================================================================
    //  Cinematic Black Bars Enabled Card
    // =====================================================================================================================

    CardContent blackBarsCard = {

        .icon = filmIcon.c_str(),
        .label = "Cinematic Black Bars",
        .tooltipText = "Enable or disable the cinematic black bars that appear when entering vanity mode.",
        .hasSlider = false,
        .sliderValue = nullptr,
        .sliderMin = 0.0f,
        .sliderMax = 0.0f,
        .sliderFormat = "",
        .sliderDefault = 0.0f,
        .hasCheckbox = true,
        .checkboxValue = &g_blackBarsEnabled,
        .checkboxDefault = k_defaultBlackBarsEnabled,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("blackBarsCard", blackBarsCard);

    // =====================================================================================================================
    //  Black Bars Slide Speed Card (Only shown if enabled)
    // =====================================================================================================================

    if (g_blackBarsEnabled) {

        CardContent speedCard = {

            .icon = speedIcon.c_str(),
            .label = "Black Bars Slide Speed",
            .tooltipText = "How fast the cinematic bars slide in and out when entering/exiting vanity mode.",
            .hasSlider = true,
            .sliderValue = &g_blackBarsSpeed,
            .sliderMin = 0.1f,
            .sliderMax = 5.0f,
            .sliderFormat = "%.1f",
            .sliderDefault = k_defaultBlackBarsSpeed,
            .hasCheckbox = false,
            .checkboxValue = nullptr,
            .checkboxDefault = false,
            .onSliderChange = nullptr,
            .onCheckboxChange = nullptr

        };

        DrawSettingCard("speedCard", speedCard);

        // =====================================================================================================================
        //  Black Bars Sound Effects Card
        // =====================================================================================================================

        CardContent soundCard = {

            .icon = soundIcon.c_str(),
            .label = "Black Bars Sound Effects",
            .tooltipText = "Enable or disable the sound effects that play when the cinematic black bars appear or disappear.",
            .hasSlider = false,
            .sliderValue = nullptr,
            .sliderMin = 0.0f,
            .sliderMax = 0.0f,
            .sliderFormat = "",
            .sliderDefault = 0.0f,
            .hasCheckbox = true,
            .checkboxValue = &g_blackBarsSoundEnabled,
            .checkboxDefault = k_defaultBlackBarsSoundEnabled,
            .onSliderChange = nullptr,
            .onCheckboxChange = nullptr
        };

        DrawSettingCard("soundCard", soundCard);

    }

    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

/// ==================================================================================================================================================================================
//  Camera Settings - Position
// ==================================================================================================================================================================================

void UI::CameraPositionSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("Camera Settings - Position", cameraIcon, "Camera Position", "resetCameraPosition",
        SettingWithDefault(&g_IdleCamOffsetX, k_defaultVanityCamOffsetX),
        SettingWithDefault(&g_IdleCamOffsetY, k_defaultVanityCamOffsetY),
        SettingWithDefault(&g_IdleCamOffsetZ, k_defaultVanityCamOffsetZ),
        SettingWithDefault(&g_blendDuration, k_defaultBlendDuration)
    );

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Idle Camera Offset X Card
    // =====================================================================================================================

    CardContent offsetXCard = {

        .icon = arrowLeftAndRightIcon.c_str(),
        .label = "Idle Camera Offset X",
        .tooltipText = "Horizontal (X-axis) offset of the idle camera relative to the player.",
        .hasSlider = true,
        .sliderValue = &g_IdleCamOffsetX,
        .sliderMin = -500.0f,
        .sliderMax = 500.0f,
        .sliderFormat = "%.0f",
        .sliderDefault = k_defaultVanityCamOffsetX,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = []() {
            g_IdleCamOffsetX = std::round(g_IdleCamOffsetX);
        },
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("offsetXCard", offsetXCard);

    // =====================================================================================================================
    //  Idle Camera Zoom Card (Offset Y)
    // =====================================================================================================================

    CardContent zoomCard = {

        .icon = zoomIcon.c_str(),
        .label = "Idle Camera Zoom",
        .tooltipText = "Zoom (Y-axis) offset of the idle camera relative to the player.",
        .hasSlider = true,
        .sliderValue = &g_IdleCamOffsetY,
        .sliderMin = -500.0f,
        .sliderMax = 500.0f,
        .sliderFormat = "%.0f",
        .sliderDefault = k_defaultVanityCamOffsetY,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = []() {
            g_IdleCamOffsetY = std::round(g_IdleCamOffsetY);
        },
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("zoomCard", zoomCard);

    // =====================================================================================================================
    //  Idle Camera Offset Z Card
    // =====================================================================================================================

    CardContent offsetZCard = {

        .icon = arrowUpAndDownIcon.c_str(),
        .label = "Idle Camera Offset Z",
        .tooltipText = "Vertical (Z-axis) offset of the idle camera relative to the player.",
        .hasSlider = true,
        .sliderValue = &g_IdleCamOffsetZ,
        .sliderMin = -500.0f,
        .sliderMax = 500.0f,
        .sliderFormat = "%.0f",
        .sliderDefault = k_defaultVanityCamOffsetZ,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = []() {
            g_IdleCamOffsetZ = std::round(g_IdleCamOffsetZ);
        },
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("offsetZCard", offsetZCard);

    // =====================================================================================================================
    //  Blend Duration Card
    // =====================================================================================================================

    CardContent blendCard = {

        .icon = clockIcon.c_str(),
        .label = "Blend Duration",
        .tooltipText = "How long the camera takes to rotate to focus on a POI.",
        .hasSlider = true,
        .sliderValue = &g_blendDuration,
        .sliderMin = 0.1f,
        .sliderMax = 5.0f,
        .sliderFormat = "%.2f sec",
        .sliderDefault = k_defaultBlendDuration,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("blendCard", blendCard);

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  Camera Settings - Zoom/Dezoom
// ==================================================================================================================================================================================

void UI::CameraZoomSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("Camera Settings - Zoom/Dezoom", cameraIcon, "Zoom/Dezoom", "resetCameraZoom",
        SettingWithDefault(&g_dezoomTriggerRadius, k_defaultDezoomTriggerRadius),
        SettingWithDefault(&g_dezoomTriggerHeight, k_defaultDezoomTriggerHeight),
        SettingWithDefault(&g_dezoomAmount, k_defaultDezoomAmount),
        SettingWithDefault(&g_dezoomBlendSpeed, k_defaultDezoomBlendSpeed)
    );

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Dezoom Trigger Radius Card
    // =====================================================================================================================

    CardContent radiusCard = {

        .icon = radiusIcon.c_str(),
        .label = "Dezoom Trigger Radius",
        .tooltipText = "Horizontal distance from the player within which an overhead POI can trigger the dezoom.",
        .hasSlider = true,
        .sliderValue = &g_dezoomTriggerRadius,
        .sliderMin = 0.0f,
        .sliderMax = 1400.0f, // 20 meters * 70
        .sliderFormat = "%.1f m",
        .sliderDefault = k_defaultDezoomTriggerRadius,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr,
        .convertToMeters = true

    };

    DrawSettingCard("radiusCard", radiusCard);

    // =====================================================================================================================
    //  Dezoom Trigger Height Card
    // =====================================================================================================================

    CardContent heightCard = {

        .icon = arrowUpIcon.c_str(),
        .label = "Dezoom Trigger Height",
        .tooltipText = "How far above the player a POI must be, while inside the trigger radius, before the dezoom kicks in.",
        .hasSlider = true,
        .sliderValue = &g_dezoomTriggerHeight,
        .sliderMin = 0.0f,
        .sliderMax = 1400.0f, // 20 meters * 70
        .sliderFormat = "%.1f m",
        .sliderDefault = k_defaultDezoomTriggerHeight,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr,
        .convertToMeters = true

    };

    DrawSettingCard("heightCard", heightCard);

    // =====================================================================================================================
    //  Dezoom Amount Card
    // =====================================================================================================================

    CardContent amountCard = {

        .icon = zoomIcon.c_str(),
        .label = "Dezoom Amount",
        .tooltipText = "How far the camera pulls back once the dezoom is fully active (added on top of the Idle camera offset Y).",
        .hasSlider = true,
        .sliderValue = &g_dezoomAmount,
        .sliderMin = 0.0f,
        .sliderMax = 500.0f,
        .sliderFormat = "%.0f",
        .sliderDefault = k_defaultDezoomAmount,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = []() {
            g_dezoomAmount = std::round(g_dezoomAmount);
        },
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("amountCard", amountCard);

    // =====================================================================================================================
    //  Dezoom Blend Speed Card
    // =====================================================================================================================

    CardContent blendSpeedCard = {

        .icon = clockIcon.c_str(),
        .label = "Dezoom Blend Speed",
        .tooltipText = "How quickly the dezoom fades in and out as a POI enters or leaves the trigger zone.",
        .hasSlider = true,
        .sliderValue = &g_dezoomBlendSpeed,
        .sliderMin = 0.1f,
        .sliderMax = 10.0f,
        .sliderFormat = "%.1f units/s",
        .sliderDefault = k_defaultDezoomBlendSpeed,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("blendSpeedCard", blendSpeedCard);

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  Head Tracking Settings
// ==================================================================================================================================================================================

void UI::HeadTrackingSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("Head Tracking Settings", headTrackIcon, "Head Tracking", "resetHeadTracking",
        SettingWithDefault(&g_headTrackFadeSpeed, k_defaultHeadTrackFadeSpeed)
    );

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Head Tracking Fade Speed Card
    // =====================================================================================================================

    CardContent fadeSpeedCard = {

        .icon = speedIcon.c_str(),
        .label = "Head Tracking Fade Speed",
        .tooltipText = "How quickly the player's head-tracking rotates toward a focused POI.",
        .hasSlider = true,
        .sliderValue = &g_headTrackFadeSpeed,
        .sliderMin = 0.1f,
        .sliderMax = 1.0f,
        .sliderFormat = "%.2f units/s",
        .sliderDefault = k_defaultHeadTrackFadeSpeed,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("fadeSpeedCard", fadeSpeedCard);

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  POI System - Main Settings
// ==================================================================================================================================================================================

void UI::POISystemMainSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("POI System - Main Settings", poiSystemIcon, "POI System Main Settings", "resetPoiGeneral",
        SettingWithDefault(&g_poiSystemEnabled, k_defaultPoiSystemEnabled),
        SettingWithDefault(&g_actorPoiEnabled, k_defaultActorPoiEnabled),
        SettingWithDefault(&g_preventFollowers, k_defaultPreventFollowers),
        SettingWithDefault(&g_flyingCritterPoiEnabled, k_defaultFlyingCritterPoiEnabled),
        SettingWithDefault(&g_fishCritterPoiEnabled, k_defaultFishCritterPoiEnabled),
        SettingWithDefault(&g_poiDetectionRadius, k_defaultPoiDetectionRadius),
        SettingWithDefault(&g_lockDuration, k_defaultLockDuration)
    );

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  POI System Master Toggle Card
    // =====================================================================================================================

    // POI System Master Toggle
    CardContent masterToggleCard = {

        .icon = poiSystemIcon.c_str(),
        .label = "POI System Master Toggle",
        .tooltipText = "Master toggle for the entire POI (Point of Interest) system. When disabled, the camera will not track any POIs automatically.",
        .hasSlider = false,
        .sliderValue = nullptr,
        .sliderMin = 0.0f,
        .sliderMax = 0.0f,
        .sliderFormat = "",
        .sliderDefault = 0.0f,
        .hasCheckbox = true,
        .checkboxValue = &g_poiSystemEnabled,
        .checkboxDefault = k_defaultPoiSystemEnabled,
        .onSliderChange = nullptr,
        .onCheckboxChange = []() {
            logger::info("POI System master toggle set to: {}", g_poiSystemEnabled);
        }

    };

    DrawSettingCard("masterToggleCard", masterToggleCard);

    // If master toggle is disabled, hide everything else below
    if (!g_poiSystemEnabled) {

        // Pop styling
        ImGuiMCP::PopStyleColor(2);
        ImGuiMCP::PopStyleVar(2);
        return;

    }

    // =====================================================================================================================
    //  Enabled POI Types Section
    // =====================================================================================================================

    DrawMultiCheckboxCard("poiTypesCard", "Enabled POI Types", poiTypesIcon.c_str(), {

        {
            personIcon.c_str(),
            k_hexGreen,
            &g_actorPoiEnabled,
            k_defaultActorPoiEnabled,
            "Actors (NPCs, creatures)",
            {
                "When enabled, the system will detect and track actors (NPCs, creatures)",
                "as points of interest based on their current action state (combat, moving, etc)."
            },
            []() {
                logger::debug("Actor POI toggle set to: {}", g_actorPoiEnabled);
            },
            {
                {
                    followerIcon.c_str(),
                    k_hexGreen,
                    &g_preventFollowers,
                    k_defaultPreventFollowers,
                    "Prevent Followers",
                    {
                        "When enabled, followers (NPCs that are in the follower faction)",
                        "will NOT be targeted by the POI system."
                    },
                    []() {
                        logger::debug("Prevent Followers toggle set to: {}", g_preventFollowers);
                    },
                    {}
                }
            }
        },
        {
            flyingCritterIcon.c_str(),
            k_hexCritterPink,
            &g_flyingCritterPoiEnabled,
            k_defaultFlyingCritterPoiEnabled,
            "Flying Critters (butterflies, moths, dragonflies, etc)",
            {
                "When enabled, the system will detect and track flying critters",
                "(butterflies, moths, dragonflies, fireflies, bees, etc)."
            },
            []() {
                logger::debug("Flying Critter POI toggle set to: {}", g_flyingCritterPoiEnabled);
            },
            {}
        },
        {
            fishCritterIcon.c_str(),
            k_hexFishCyan,
            &g_fishCritterPoiEnabled,
            k_defaultFishCritterPoiEnabled,
            "Fish Critters (perches, salmon, pond fish, etc)",
            {
                "When enabled, the system will detect and track fish critters",
                "(perches, salmon, pond fish, and other aquatic critters)."
            },
            []() {
                logger::debug("Fish Critter POI toggle set to: {}", g_fishCritterPoiEnabled);
            },
            {}
        }
    });

    // =====================================================================================================================
    //  Maximum Detection Radius Card
    // =====================================================================================================================

    CardContent radiusCard = {

        .icon = radiusIcon.c_str(),
        .label = "Maximum Detection Radius",
        .tooltipText = "Maximum POI detection range from the player.",
        .hasSlider = true,
        .sliderValue = &g_poiDetectionRadius,
        .sliderMin = 0.0f,
        .sliderMax = 7000.0f, // 100 meters * 70
        .sliderFormat = "%.1f m",
        .sliderDefault = k_defaultPoiDetectionRadius,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr,
        .convertToMeters = true

    };

    DrawSettingCard("poiRadiusCard", radiusCard);

    // =====================================================================================================================
    //  Minimum Lock Duration Card
    // =====================================================================================================================

    CardContent lockCard = {

        .icon = poiLockIcon.c_str(),
        .label = "Minimum Lock Duration",
        .tooltipText = "Minimum time the camera stays locked onto a point of interest before it can switch to another.",
        .hasSlider = true,
        .sliderValue = &g_lockDuration,
        .sliderMin = 0.0f,
        .sliderMax = 30.0f,
        .sliderFormat = "%.1f sec",
        .sliderDefault = k_defaultLockDuration,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("lockCard", lockCard);

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  POI System - Exclusion List
// ==================================================================================================================================================================================

void UI::POISystemExclusionListSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("POI System - Exclusion List", ExcludeListIcon, "Exclusion List", "resetPoiExclusion",
        ExclusionListReset(&g_actorExclusionList)
    );

    // =====================================================================================================================
    //  Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Card 1: Add Actor to Exclusion List
    // =====================================================================================================================

    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);
    float cardWidth = winSize.x - (k_marginBetweenBordersInMenuHeader * 2);

    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);
    ImGuiMCP::BeginChild("##addActorCard", ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
    ImGuiMCP::Text("%s", addIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
    ImGuiMCP::Text("Add Actor to Exclusion List");

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // =====================================================================================================================
    //  Add selected actor logic
    // =====================================================================================================================

    auto* player = RE::PlayerCharacter::GetSingleton();
    RE::Actor* selectedActor = nullptr;
    bool hasSelectedActor = false;
    bool isPlayer = false;
    bool isExcluded = false;
    bool isNotActor = false;

    if (player) {

        auto selectedRef = RE::Console::GetSelectedRef();

        if (selectedRef) {

            selectedActor = selectedRef->As<RE::Actor>();

            if (selectedActor) {

                hasSelectedActor = true;

                if (selectedActor->IsPlayerRef()) {

                    isPlayer = true;

                } else {

                    isExcluded = IsActorExcluded(selectedActor);

                }

            } else {

                // Selected object is not an actor
                isNotActor = true;
                hasSelectedActor = false;

            }

        }

    }

    // =====================================================================================================================
    //  Display appropriate message based on selection state
    // =====================================================================================================================

    if (!hasSelectedActor && !selectedActor && !isNotActor) {

        // No actor selected
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
        ImGuiMCP::Text("%s To exclude an actor, open the console, click on the actor, then use the button below to add them.", infoIcon.c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Button("Add Selected Actor: [No actor selected]");
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Select an actor with the console first.");
            ImGuiMCP::EndTooltip();

        }

    } else if (isNotActor) {

        // Selected object is not an actor
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexRed));
        ImGuiMCP::Text("%s The currently selected object is not an actor.", deniedIcon.c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Button("Add Selected Actor: [No actor selected]");
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("The selected object with the console is not an actor.");
            ImGuiMCP::EndTooltip();

        }

    } else if (isPlayer) {

        // Player cannot be excluded
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexRed));
        ImGuiMCP::Text("%s The player cannot be excluded.", deniedIcon.c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        const char* playerName = selectedActor->GetName();
        std::string buttonText = std::string("Add Selected Actor: ") + (playerName ? playerName : "Player");
        ImGuiMCP::Button(buttonText.c_str());
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("The player cannot be excluded from the POI System.");
            ImGuiMCP::EndTooltip();

        }

    } else if (isExcluded) {

        // Already excluded - show warning message and gray button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexWarning));
        ImGuiMCP::Text("%s Already Excluded: %s", deniedIcon.c_str(), selectedActor->GetName() ? selectedActor->GetName() : "Unnamed");
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Grayed out button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        const char* actorName = selectedActor->GetName();
        std::string buttonText = std::string("Already Excluded: ") + (actorName ? actorName : "Unnamed");
        ImGuiMCP::Button(buttonText.c_str());
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("The selected actor is already excluded.");
            ImGuiMCP::EndTooltip();

        }

    } else {

        // Valid actor to add
        const char* actorName = selectedActor->GetName();
        std::string buttonText = std::string("Add Selected Actor: ") + (actorName ? actorName : "Unnamed");

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4(k_hexBrightGreen));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, HexToImVec4(k_hexDarkGreen));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, HexToImVec4(k_hexDarkerGreen));

        if (ImGuiMCP::Button(buttonText.c_str())) {

            AddActorToExclusionList(selectedActor);

        }

        ImGuiMCP::PopStyleColor(3);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Adds the currently selected actor to the exclusion list.");
            ImGuiMCP::EndTooltip();

        }

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Card 2: Excluded Actors List
    // =====================================================================================================================

    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInMenuHeader);
    ImGuiMCP::BeginChild("##excludedListCard", ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and count
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGreen));
    ImGuiMCP::Text("%s", personIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
    ImGuiMCP::Text("Excluded Actors (%zu)", g_actorExclusionList.size());

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // =====================================================================================================================
    //  Display exclusion list
    // =====================================================================================================================

    if (g_actorExclusionList.empty()) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Text("%s No actors in exclusion list.", folderOpenIcon.c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::EndChild();
        ImGuiMCP::PopStyleColor(2);
        ImGuiMCP::PopStyleVar(2);
        return;

    }

    // =====================================================================================================================
    //  Build a sorted copy of the exclusion list
    // =====================================================================================================================

    std::vector<ActorExclusionEntry> sortedList = g_actorExclusionList;

    std::sort(sortedList.begin(), sortedList.end(), [](const ActorExclusionEntry& a, const ActorExclusionEntry& b) {

        std::string aName = GetActorNameFromFormID(a.formID);
        std::string bName = GetActorNameFromFormID(b.formID);
        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);
        return aName < bName;

    });

    // Find the selected actor in the sorted list
    size_t selectedIndex = SIZE_MAX;

    if (selectedActor && !selectedActor->IsPlayerRef()) {

        RE::FormID selectedFormID = 0;
        auto* actorBase = selectedActor->GetActorBase();

        if (actorBase) {

            selectedFormID = actorBase->GetFormID();

            if ((selectedFormID & 0xFF000000) == 0xFF000000) {

                auto* rootTemplate = actorBase->GetRootFaceNPC();

                if (rootTemplate && rootTemplate != actorBase) {

                    RE::FormID rootID = rootTemplate->GetFormID();

                    if ((rootID & 0xFF000000) != 0xFF000000) {

                        selectedFormID = rootID;

                    }

                }

                if ((selectedFormID & 0xFF000000) == 0xFF000000) {

                    auto* templateNPC = actorBase->faceNPC;

                    if (templateNPC) {

                        RE::FormID faceID = templateNPC->GetFormID();

                        if ((faceID & 0xFF000000) != 0xFF000000) {

                            selectedFormID = faceID;

                        }

                    }

                }

            }

        }

        for (size_t i = 0; i < sortedList.size(); ++i) {

            if (sortedList[i].formID == selectedFormID) {

                selectedIndex = i;
                break;

            }

        }

    }

    // Move selected actor to the top
    if (selectedIndex != SIZE_MAX && selectedIndex > 0) {

        ActorExclusionEntry selectedEntry = sortedList[selectedIndex];
        sortedList.erase(sortedList.begin() + selectedIndex);
        sortedList.insert(sortedList.begin(), selectedEntry);

    }

    // =====================================================================================================================
    //  Scrollable list
    // =====================================================================================================================

    ImGuiMCP::BeginChild("ExcludedActorsList", ImGuiMCP::ImVec2(0.0f, 600.0f), ImGuiMCP::ImGuiChildFlags_Border);

    for (size_t i = 0; i < sortedList.size(); ++i) {

        auto& entry = sortedList[i];
        std::string actorName = GetActorNameFromFormID(entry.formID);
        bool isSelected = (selectedIndex != SIZE_MAX && i == 0 && selectedActor != nullptr);

        ImGuiMCP::PushID(static_cast<int>(i));

        // Entry background with rounded corners
        if (isSelected) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.2f, 0.4f, 0.8f, 0.3f });
            ImGuiMCP::BeginChild(ImGuiMCP::GetID("Highlight"), ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

        // Delete button (first thing on the left)
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4(k_hexRed));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, HexToImVec4(k_hexDarkRed));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, HexToImVec4(k_hexDarkerRed));

        // Find original index
        size_t originalIndex = SIZE_MAX;

        for (size_t j = 0; j < g_actorExclusionList.size(); ++j) {

            if (g_actorExclusionList[j].formID == entry.formID) {

                originalIndex = j;
                break;

            }

        }

        if (ImGuiMCP::Button("Delete")) {

            if (originalIndex != SIZE_MAX) {

                RemoveFromActorExclusionList(originalIndex);
                ImGuiMCP::PopStyleColor(3);
                ImGuiMCP::PopID();

                if (isSelected) {

                    ImGuiMCP::EndChild();
                    ImGuiMCP::PopStyleColor();

                }

                ImGuiMCP::EndChild();
                ImGuiMCP::PopStyleColor(2);
                ImGuiMCP::PopStyleVar(2);
                return;

            }

        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Remove this actor from the exclusion list.");
            ImGuiMCP::EndTooltip();

        }

        ImGuiMCP::PopStyleColor(3);

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        // Icon
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGreen));
        ImGuiMCP::Text("%s", personIcon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();

        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        // Actor name (bold if selected)
        if (isSelected) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
            ImGuiMCP::Text("%s", actorName.c_str());
            ImGuiMCP::PopStyleColor();

        } else {

            ImGuiMCP::Text("%s", actorName.c_str());

        }

        ImGuiMCP::SameLine();

        // Form ID (muted)
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Text("(%08X)", entry.formID);
        ImGuiMCP::PopStyleColor();

        // "Currently selected in console" in brackets
        if (isSelected) {

            ImGuiMCP::SameLine();
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(k_hexGoldLight));
            ImGuiMCP::Text("[Currently selected with the console]");
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

        if (isSelected) {

            ImGuiMCP::EndChild();
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::PopID();

    }

    ImGuiMCP::EndChild();  // End scrollable list

    ImGuiMCP::EndChild();  // End Excluded Actors card

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  POI System - Actor Scores
// ==================================================================================================================================================================================

void UI::POISystemActorScores() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("POI System - Actor Scores", poiTypesIcon, "Actor Score Settings", "resetPoiActor",
        SettingWithDefault(&g_dragonScore, k_defaultDragonScore),
        SettingWithDefault(&g_dragonProximityEnabled, k_defaultDragonProximityEnabled),
        SettingWithDefault(&g_dragonProximityFactor, k_defaultDragonProximityFactor),
        SettingWithDefault(&g_actorCombatScore, k_defaultActorCombatScore),
        SettingWithDefault(&g_actorCombatProximityEnabled, k_defaultActorCombatProximityEnabled),
        SettingWithDefault(&g_actorCombatProximityFactor, k_defaultActorCombatProximityFactor),
        SettingWithDefault(&g_actorMovingScore, k_defaultActorMovingScore),
        SettingWithDefault(&g_actorMovingProximityEnabled, k_defaultActorMovingProximityEnabled),
        SettingWithDefault(&g_actorMovingProximityFactor, k_defaultActorMovingProximityFactor),
        SettingWithDefault(&g_actorInSceneScore, k_defaultActorInSceneScore),
        SettingWithDefault(&g_actorInSceneProximityEnabled, k_defaultActorInSceneProximityEnabled),
        SettingWithDefault(&g_actorInSceneProximityFactor, k_defaultActorInSceneProximityFactor),
        SettingWithDefault(&g_actorIdleScore, k_defaultActorIdleScore),
        SettingWithDefault(&g_actorIdleProximityEnabled, k_defaultActorIdleProximityEnabled),
        SettingWithDefault(&g_actorIdleProximityFactor, k_defaultActorIdleProximityFactor)
    );

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.5f));

    DrawActorScoreCards();

    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  POI System - Critter Scores
// ==================================================================================================================================================================================

void UI::POISystemCritterScores() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("POI System - Critter Scores", poiTypesIcon, "Critter Score Settings", "resetPoiCritter",
        SettingWithDefault(&g_flyingCritterScore, k_defaultFlyingCritterScore),
        SettingWithDefault(&g_flyingCritterProximityEnabled, k_defaultFlyingCritterProximityEnabled),
        SettingWithDefault(&g_flyingCritterProximityFactor, k_defaultFlyingCritterProximityFactor),
        SettingWithDefault(&g_fishCritterScore, k_defaultFishCritterScore),
        SettingWithDefault(&g_fishCritterProximityEnabled, k_defaultFishCritterProximityEnabled),
        SettingWithDefault(&g_fishCritterProximityFactor, k_defaultFishCritterProximityFactor)
    );

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.5f));

    DrawCritterScoreCards();

    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ==================================================================================================================================================================================
//  Debug Settings
// ==================================================================================================================================================================================

void UI::DebugSettings() {

    FontAwesome::PushSolid();

    DrawUIHeaderWithReset("Debug Settings", debugIcon, "Debug", "resetDebug",
        SettingWithDefault(&g_debugRaycasts, k_defaultDebugRaycasts),
        SettingWithDefault(&g_loggingLevel, k_defaultLoggingLevel, []() {

            auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
            spdlog::set_level(lvl);
            spdlog::flush_on(lvl);
            logger::info("Logging level reset to default ('{}')", k_loggingLevelNames[g_loggingLevel]);

        })

    );

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Debug Raycast Visualization Card
    // =====================================================================================================================

    CardContent raycastCard = {

        .icon = raycastIcon.c_str(),
        .label = "Debug Raycast Visualization",
        .tooltipText = "Draws debug raycast lines used by the POI detection system, for troubleshooting purposes.",
        .hasSlider = false,
        .sliderValue = nullptr,
        .sliderMin = 0.0f,
        .sliderMax = 0.0f,
        .sliderFormat = "",
        .sliderDefault = 0.0f,
        .hasCheckbox = true,
        .checkboxValue = &g_debugRaycasts,
        .checkboxDefault = k_defaultDebugRaycasts,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr

    };

    DrawSettingCard("raycastCard", raycastCard);

    // =====================================================================================================================
    //  Logging Level Card
    // =====================================================================================================================

    CardContent loggingCard = {

        .icon = loggingIcon.c_str(),
        .label = "Logging Level",
        .tooltipText =
            "Controls how much detail is written to the log file:\n"
            "\n"
            "Quiet - only critical errors that prevent the mod from working.\n"
            "\n"
            "Warnings - critical errors plus non-fatal issues worth noticing.\n"
            "\n"
            "Info - warnings plus general status messages.\n"
            "\n"
            "Debug - everything, including detailed internal values for troubleshooting.",
        .hasSlider = false,
        .sliderValue = nullptr,
        .sliderMin = 0.0f,
        .sliderMax = 0.0f,
        .sliderFormat = "",
        .sliderDefault = 0.0f,
        .hasCheckbox = false,
        .checkboxValue = nullptr,
        .checkboxDefault = false,
        .onSliderChange = nullptr,
        .onCheckboxChange = nullptr,
        .hasIntSlider = true,
        .intSliderValue = &g_loggingLevel,
        .intSliderMin = 0,
        .intSliderMax = k_loggingLevelCount - 1,
        .intSliderDefault = k_defaultLoggingLevel,
        .intSliderNames = k_loggingLevelNames,
        .onIntSliderChange = []() {

            auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
            spdlog::set_level(lvl);
            spdlog::flush_on(lvl);
            logger::info("Logging level manually set to '{}'", k_loggingLevelNames[g_loggingLevel]);

        }

    };

    DrawSettingCard("loggingCard", loggingCard);

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}