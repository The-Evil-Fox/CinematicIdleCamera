#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"

namespace logger = SKSE::log;

// ---------------------------------------------------------------------------
// Default settings used when the ini doesn't exist when the game is started
// ---------------------------------------------------------------------------

static constexpr float          k_defaultIdleTimer = 5.0f;
static constexpr float          k_defaultPoiDetectionRadius = 1050.0f;
static constexpr float          k_defaultLockDuration = 3.0f;
static constexpr float          k_defaultBlendDuration = 3.0f;
static constexpr float          k_defaultHeadTrackFadeSpeed = 2.0f;
static constexpr bool           k_defaultDebugRaycasts = false;
static constexpr int            k_defaultLoggingLevel = 2; // 0 = critical, 1 = warn, 2 = info, 3 = debug

float                           UI::g_idleTimer = k_defaultIdleTimer;
float                           UI::g_poiDetectionRadius = k_defaultPoiDetectionRadius;
float                           UI::g_lockDuration = k_defaultLockDuration;
float                           UI::g_blendDuration = k_defaultBlendDuration;
float                           UI::g_headTrackFadeSpeed = k_defaultHeadTrackFadeSpeed;
bool                            UI::g_debugRaycasts = k_defaultDebugRaycasts;
int                             UI::g_loggingLevel = k_defaultLoggingLevel;

// ---------------------------------------------------------------------------
// Logging level names, indexed 0-3 to match the simplified scheme:
// 0 = Quiet (critical only), 1 = Warnings, 2 = Info (default), 3 = Debug
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Font Awesome Icons
// ---------------------------------------------------------------------------

auto settingsIcon       =       FontAwesome::UnicodeToUtf8(0xf013);
auto poiSystemIcon      =       FontAwesome::UnicodeToUtf8(0xf3c5);
auto cameraIcon         =       FontAwesome::UnicodeToUtf8(0xf03d);
auto playerIcon         =       FontAwesome::UnicodeToUtf8(0xf183);
auto debugIcon          =       FontAwesome::UnicodeToUtf8(0xf7d9);

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {

        return;

    }

    SKSEMenuFramework::SetSection("Cinematic Idle Camera");
    SKSEMenuFramework::AddSectionItem(std::string("Settings"), Settings);
    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

// ---------------------------------------------------------------------------
// Black bars used to make the vanity mode more cinematic
// ---------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------
// Settings menu
// ---------------------------------------------------------------------------

void UI::Settings() {

    FontAwesome::PushSolid();

    // ---------------------------------------------------------------------------
    // POI System Settings
    // ---------------------------------------------------------------------------

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s POI System", poiSystemIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Maximum Detection Radius");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);
    float poiRadiusMeters = g_poiDetectionRadius / 70.0f;

    if (ImGuiMCP::SliderFloat("##poiDetectionRadius", &poiRadiusMeters, 0.0f, 100.0f, "%.1f m")) {

        g_poiDetectionRadius = poiRadiusMeters * 70.0f;
        IniParser::Save();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Minimum Lock Duration");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##lockDuration", &g_lockDuration, 0.0f, 30.0f, "%.1f sec")) {

        IniParser::Save();

    }

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------
    // Camera settings
    // ---------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 30.0f));
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Camera Settings", cameraIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Timer");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##idleTimer", &g_idleTimer, 1.0f, 300.0f, "%.0f sec")) {

        IniParser::Save();

        auto* iniSettings = RE::INISettingCollection::GetSingleton();

        if (iniSettings) {

            auto* setting = iniSettings->GetSetting("fAutoVanityModeDelay:Camera");

            if (setting) {

                setting->data.f = g_idleTimer;
                logger::info("Camera Idle Timer Setting manually set to {} second(s)", g_idleTimer);

            } else {

                logger::error("Setting not found in INISettingCollection!");

            }

        }

    }

    ImGuiMCP::Separator();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Blend Duration");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##blendDuration", &g_blendDuration, 0.1f, 5.0f, "%.2f sec")) {

        IniParser::Save();

    }

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------
    // Player settings
    // ---------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 30.0f));
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Player Settings", playerIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Head-Tracking Fade Speed");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderFloat("##headTrackFadeSpeed", &g_headTrackFadeSpeed, 0.1f, 10.0f, "%.1f units/s")) {

        IniParser::Save();

    }

    ImGuiMCP::Separator();

    // ---------------------------------------------------------------------------
    // Debug settings
    // ---------------------------------------------------------------------------

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 30.0f));
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Debug Settings", debugIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Separator();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Debug Raycast Visualization");
    ImGuiMCP::SameLine();

    if (ImGuiMCP::Checkbox("##debugRaycasts", &g_debugRaycasts)) {

        IniParser::Save();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Logging Level");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);

    if (ImGuiMCP::SliderInt("##loggingLevel", &g_loggingLevel, 0, k_loggingLevelCount - 1, k_loggingLevelNames[g_loggingLevel])) {

        g_loggingLevel = std::clamp(g_loggingLevel, 0, k_loggingLevelCount - 1);

        auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
        spdlog::set_level(lvl);
        spdlog::flush_on(lvl);
        logger::info("Logging level manually set to '{}'", k_loggingLevelNames[g_loggingLevel]);
        IniParser::Save();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    ImGuiMCP::Text("Reset All Settings To Default");
    ImGuiMCP::SameLine();

    if (ImGuiMCP::Button("Reset##resetAll")) {

        g_idleTimer = k_defaultIdleTimer;
        g_poiDetectionRadius = k_defaultPoiDetectionRadius;
        g_lockDuration = k_defaultLockDuration;
        g_blendDuration = k_defaultBlendDuration;
        g_headTrackFadeSpeed = k_defaultHeadTrackFadeSpeed;
        g_debugRaycasts = k_defaultDebugRaycasts;
        g_loggingLevel = k_defaultLoggingLevel;

        auto* iniSettings = RE::INISettingCollection::GetSingleton();
        if (iniSettings) {

            auto* setting = iniSettings->GetSetting("fAutoVanityModeDelay:Camera");

            if (setting) {

                setting->data.f = g_idleTimer;

            }

        }

        auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
        spdlog::set_level(lvl);
        spdlog::flush_on(lvl);

        IniParser::Save();

    }

}