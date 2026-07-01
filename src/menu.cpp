#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"

namespace logger = SKSE::log;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Default settings used when the ini doesn't exist when the game is started
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static constexpr float          k_defaultIdleTimer = 5.0f;
static constexpr float          k_defaultPoiDetectionRadius = 1050.0f;
static constexpr float          k_defaultLockDuration = 3.0f;
static constexpr float          k_defaultBlendDuration = 3.0f;
static constexpr float          k_defaultVanityCamOffsetX = 100.0f;
static constexpr float          k_defaultVanityCamOffsetY = 130.0f;
static constexpr float          k_defaultVanityCamOffsetZ = 0.0f;
static constexpr float          k_defaultHeadTrackFadeSpeed = 2.0f;
static constexpr bool           k_defaultDebugRaycasts = false;
static constexpr int            k_defaultLoggingLevel = 2; // 0 = critical, 1 = warn, 2 = info, 3 = debug

float                           UI::g_idleTimer = k_defaultIdleTimer;
float                           UI::g_poiDetectionRadius = k_defaultPoiDetectionRadius;
float                           UI::g_lockDuration = k_defaultLockDuration;
float                           UI::g_blendDuration = k_defaultBlendDuration;
float                           UI::g_IdleCamOffsetX = k_defaultVanityCamOffsetX;
float                           UI::g_IdleCamOffsetY = k_defaultVanityCamOffsetY;
float                           UI::g_IdleCamOffsetZ = k_defaultVanityCamOffsetZ;
float                           UI::g_headTrackFadeSpeed = k_defaultHeadTrackFadeSpeed;
bool                            UI::g_debugRaycasts = k_defaultDebugRaycasts;
int                             UI::g_loggingLevel = k_defaultLoggingLevel;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Logging level names, indexed 0-3 to match the simplified scheme:
//  0 = Quiet (critical only), 1 = Warnings, 2 = Info (default), 3 = Debug
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Used to force apply some new settings to the virtual copy of the ini user side. Only used for the idle timer for now.
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Font Awesome Icons
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

auto settingsIcon = FontAwesome::UnicodeToUtf8(0xf013);
auto poiSystemIcon = FontAwesome::UnicodeToUtf8(0xf3c5);
auto cameraIcon = FontAwesome::UnicodeToUtf8(0xf03d);
auto playerIcon = FontAwesome::UnicodeToUtf8(0xf183);
auto debugIcon = FontAwesome::UnicodeToUtf8(0xf7d9);
auto resetIcon = FontAwesome::UnicodeToUtf8(0xf2ea);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Shows a hovering tooltip describing the parameter. Call this right after the widget (slider/checkbox) it
//  belongs to, while the widget is still the "last item" (ImGui tracks hover state per last-submitted item).
//
//  The wrap width is computed dynamically from the longest line in `text` (splitting on '\n'), so the
//  tooltip box is always exactly as wide as its widest line and nothing wraps unexpectedly.
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HelpTooltip(const char* text) {

    if (ImGuiMCP::IsItemHovered()) {

        // ---------------------------------------------------------------------------------------------------------------
        //  Walk the string, splitting on '\n', and measure each line's pixel width via ImGui's own text
        //  measurement (CalcTextSize) so the result accounts for the actual font metrics rather than raw
        //  character counts (which would be wrong for proportional fonts). SKSEMenuFramework's CalcTextSize
        //  writes its result through an out-param (ImVec2*) rather than returning by value, and has no
        //  default arguments, so all 5 parameters must be supplied explicitly.
        // ---------------------------------------------------------------------------------------------------------------

        float       longestLineWidth = 0.0f;
        const char* lineStart = text;

        for (const char* p = text; ; ++p) {

            if (*p == '\n' || *p == '\0') {

                ImGuiMCP::ImVec2 lineSize{};
                ImGuiMCP::CalcTextSize(&lineSize, lineStart, p, false, -1.0f);

                if (lineSize.x > longestLineWidth) {

                    longestLineWidth = lineSize.x;

                }

                if (*p == '\0') {

                    break;

                }

                lineStart = p + 1;

            }

        }

        // Pad slightly so the wrap boundary sits just past the widest line's actual pixel width,
        // preventing any edge-case wrapping caused by rounding.
        const float wrapWidth = longestLineWidth + 1.0f;

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::PushTextWrapPos(ImGuiMCP::GetCursorPosX() + wrapWidth);
        ImGuiMCP::TextUnformatted(text);
        ImGuiMCP::PopTextWrapPos();
        ImGuiMCP::EndTooltip();

    }

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Register all the different setting sections in the SKSE menu
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {

        return;

    }

    SKSEMenuFramework::SetSection("Cinematic Idle Camera");
    SKSEMenuFramework::AddSectionItem(std::string("Camera"), CameraSettings);
    SKSEMenuFramework::AddSectionItem(std::string("Head Tracking"), HeadTrackingSettings);
    SKSEMenuFramework::AddSectionItem(std::string("POI System"), POISystemSettings);
    SKSEMenuFramework::AddSectionItem(std::string("Debug"), DebugSettings);
    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Black bars used to make the vanity mode more cinematic
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::DrawCinematicBars() {

    auto* playerCamera = RE::PlayerCamera::GetSingleton();

    if (!playerCamera || !playerCamera->currentState) {

        return;

    }

    const bool inVanity = playerCamera->currentState->id == RE::CameraState::kAutoVanity;

    static float s_progress = 0.0f;

    auto* io = ImGuiMCP::GetIO();
    const float dt = io->DeltaTime;
    const float slideSpeed = 2.0f;

    if (inVanity) {

        s_progress = std::min(1.0f, s_progress + slideSpeed * dt);

    } else {

        s_progress = std::max(0.0f, s_progress - slideSpeed * dt);

    }

    if (s_progress <= 0.0f) {

        return;

    }

    auto* drawList = ImGuiMCP::GetForegroundDrawList();
    float screenW = io->DisplaySize.x;
    float screenH = io->DisplaySize.y;

    const float barHeight = screenH * 0.08f;

    // Smoothstep easing
    const float t = s_progress * s_progress * (3.0f - 2.0f * s_progress);

    // At t=0: bars are fully off-screen. At t=1: bars are fully on-screen.
    // Top bar slides down: at t=0 bottom edge is at 0 (hidden above), at t=1 bottom edge is at barHeight
    const float topBarBottom = barHeight * t;

    // Bottom bar slides up: at t=0 top edge is at screenH (hidden below), at t=1 top edge is at screenH - barHeight
    const float botBarTop = screenH - barHeight * t;

    const ImGuiMCP::ImU32 barColor = ImGuiMCP::ColorConvertFloat4ToU32(ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });

    // Top bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, topBarBottom - barHeight }, ImGuiMCP::ImVec2{ screenW, topBarBottom }, barColor, 0.0f, 0);

    // Bottom bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, botBarTop }, ImGuiMCP::ImVec2{ screenW, botBarTop + barHeight }, barColor, 0.0f, 0);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Camera Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::CameraSettings() {

    FontAwesome::PushSolid();

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Camera Settings", cameraIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Idle timer
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##idleTimer", &g_idleTimer, 1.0f, 300.0f, "%.0f sec")) {

        IniParser::Save();
        ApplyIdleTimerToIniSettings("fAutoVanityModeDelay:Camera", g_idleTimer);
        logger::info("Camera Idle Timer Setting manually set to {} second(s)", g_idleTimer);

    }

    HelpTooltip("How many seconds of player inactivity before the camera switches into idle mode.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Idle timer");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Camera blend duration
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));
    
    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##blendDuration", &g_blendDuration, 0.1f, 5.0f, "%.2f sec")) {

        IniParser::Save();

    }

    HelpTooltip("How long the camera takes to rotate to focus on a POI.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Blend duration");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Idle camera offset X
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##idleCameraOffsetx", &g_IdleCamOffsetX, -500.0f, 500.0f, "%.0f")) {

        g_IdleCamOffsetX = std::round(g_IdleCamOffsetX);
        IniParser::Save();

    }

    HelpTooltip("Horizontal (X-axis) offset of the idle vanity camera relative to the player.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Idle camera offset X");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Idle camera offset Y
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##idleCameraOffsety", &g_IdleCamOffsetY, -500.0f, 500.0f, "%.0f")) {

        g_IdleCamOffsetY = std::round(g_IdleCamOffsetY);
        IniParser::Save();

    }

    HelpTooltip("Depth (Y-axis) offset of the idle camera relative to the player.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Idle camera offset Y");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Idle camera offset Y
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##idleCameraOffsetz", &g_IdleCamOffsetZ, -500.0f, 500.0f, "%.0f")) {

        g_IdleCamOffsetZ = std::round(g_IdleCamOffsetZ);
        IniParser::Save();

    }

    HelpTooltip("Vertical (Z-axis) offset of the idle camera relative to the player.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Idle camera offset Z");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset the camera related settings back to default
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    if (ImGuiMCP::Button(std::format("{} Reset To Default##resetCamera", resetIcon).c_str())) {

        g_idleTimer = k_defaultIdleTimer;
        g_blendDuration = k_defaultBlendDuration;
        g_IdleCamOffsetX = k_defaultVanityCamOffsetX;
        g_IdleCamOffsetY = k_defaultVanityCamOffsetY;
        g_IdleCamOffsetZ = k_defaultVanityCamOffsetZ;

        ApplyIdleTimerToIniSettings("fAutoVanityModeDelay:Camera", g_idleTimer);
        IniParser::Save();

    }

    HelpTooltip("Restores all camera settings on this page to their default values.");

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Head Tracking Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::HeadTrackingSettings() {

    FontAwesome::PushSolid();

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Head Tracking Settings", playerIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Head Tracking Fade Speed
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##headTrackFadeSpeed", &g_headTrackFadeSpeed, 0.1f, 10.0f, "%.1f units/s")) {

        IniParser::Save();

    }

    HelpTooltip("How quickly the player's head-tracking rotates toward a focused POI");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Head-tracking fade speed");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset the head tracking related settings back to default
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    if (ImGuiMCP::Button(std::format("{} Reset To Default##resetHeadTracking", resetIcon).c_str())) {

        g_headTrackFadeSpeed = k_defaultHeadTrackFadeSpeed;

        IniParser::Save();

    }

    HelpTooltip("Restores all head tracking settings on this page to their default values.");

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  POI System Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::POISystemSettings() {

    FontAwesome::PushSolid();

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s POI System", poiSystemIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Maximum detection radius
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);
    float poiRadiusMeters = g_poiDetectionRadius / 70.0f;

    if (ImGuiMCP::SliderFloat("##poiDetectionRadius", &poiRadiusMeters, 0.0f, 100.0f, "%.1f m")) {

        g_poiDetectionRadius = poiRadiusMeters * 70.0f;
        IniParser::Save();

    }

    HelpTooltip("Maximum POI detection range from the player.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Maximum Detection Radius");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Minimum lock duration
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##lockDuration", &g_lockDuration, 0.0f, 30.0f, "%.1f sec")) {

        IniParser::Save();

    }

    HelpTooltip("Minimum time the camera stays locked onto a point of interest before it can switch to another.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Minimum Lock Duration");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset the POI system related settings back to default
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    if (ImGuiMCP::Button(std::format("{} Reset To Default##resetPoi", resetIcon).c_str())) {

        g_poiDetectionRadius = k_defaultPoiDetectionRadius;
        g_lockDuration = k_defaultLockDuration;

        IniParser::Save();

    }

    HelpTooltip("Restores all POI system settings on this page to their default values.");

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Debug Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::DebugSettings() {

    FontAwesome::PushSolid();

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Debug Settings", debugIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Debug raycast
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    if (ImGuiMCP::Checkbox("##debugRaycasts", &g_debugRaycasts)) {

        IniParser::Save();

    }

    HelpTooltip("Draws debug raycast lines used by the POI detection system, for troubleshooting purposes.");
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Debug Raycast Visualization");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Logging level
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderInt("##loggingLevel", &g_loggingLevel, 0, k_loggingLevelCount - 1, k_loggingLevelNames[g_loggingLevel])) {

        g_loggingLevel = std::clamp(g_loggingLevel, 0, k_loggingLevelCount - 1);

        auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
        spdlog::set_level(lvl);
        spdlog::flush_on(lvl);
        logger::info("Logging level manually set to '{}'", k_loggingLevelNames[g_loggingLevel]);
        IniParser::Save();

    }

    HelpTooltip(
        "Controls how much detail is written to the log file:\n"
        "\n"
        "Quiet - only critical errors that prevent the mod from working.\n"
        "\n"
        "Warnings - critical errors plus non-fatal issues worth noticing.\n"
        "\n"
        "Info - warnings plus general status messages.\n"
        "\n"
        "Debug - everything, including detailed internal values for troubleshooting."
    );
    ImGuiMCP::SameLine();
    ImGuiMCP::Text("Logging Level");

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset the debug related settings back to default
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    if (ImGuiMCP::Button(std::format("{} Reset To Default##resetDebug", resetIcon).c_str())) {

        g_debugRaycasts = k_defaultDebugRaycasts;
        g_loggingLevel = k_defaultLoggingLevel;

        auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
        spdlog::set_level(lvl);
        spdlog::flush_on(lvl);
        logger::info("Logging level reset to default ('{}')", k_loggingLevelNames[g_loggingLevel]);

        IniParser::Save();

    }

    HelpTooltip("Restores all debug settings on this page to their default values.");

}