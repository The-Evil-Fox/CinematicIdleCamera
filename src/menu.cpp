#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"

namespace logger = SKSE::log;

float UI::g_idleTimer = 5.0f;
float UI::g_poiDetectionRadius = 1050.0f;

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) return;
    SKSEMenuFramework::SetSection("Cinematic Idle Camera");
    SKSEMenuFramework::AddSectionItem("Settings", Settings);
    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

void UI::DrawCinematicBars() {

    auto* playerCamera = RE::PlayerCamera::GetSingleton();
    if (!playerCamera || !playerCamera->currentState) return;

    const bool inVanity = playerCamera->currentState->id == RE::CameraState::kAutoVanity;

    static float s_progress = 0.0f;

    auto* io = ImGuiMCP::GetIO();
    const float dt = io->DeltaTime;
    const float slideSpeed = 2.0f;

    if (inVanity) {
        s_progress = std::min(1.0f, s_progress + slideSpeed * dt);
    }
    else {
        s_progress = std::max(0.0f, s_progress - slideSpeed * dt);
    }

    if (s_progress <= 0.0f) return;

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

    const ImGuiMCP::ImU32 barColor = ImGuiMCP::ColorConvertFloat4ToU32(
        ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f }
    );

    // Top bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(
        drawList,
        ImGuiMCP::ImVec2{ 0.0f,   topBarBottom - barHeight },
        ImGuiMCP::ImVec2{ screenW, topBarBottom },
        barColor, 0.0f, 0
    );

    // Bottom bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(
        drawList,
        ImGuiMCP::ImVec2{ 0.0f,   botBarTop },
        ImGuiMCP::ImVec2{ screenW, botBarTop + barHeight },
        barColor, 0.0f, 0
    );

}

void UI::Settings() {

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    FontAwesome::PushSolid();
    auto iconUtf8 = FontAwesome::UnicodeToUtf8(0xf0eb);
    ImGuiMCP::Text("%s Cinematic Idle Camera - Settings", iconUtf8.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    ImGuiMCP::Separator();

    ImGuiMCP::Text("Camera Idle Timer");
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
            }
            else {
                logger::error("Setting not found in INISettingCollection!");
            }
        }

    }

    ImGuiMCP::Separator();

    ImGuiMCP::Text("POI Detection Radius");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetNextItemWidth(200.0f);
    float poiRadiusMeters = g_poiDetectionRadius / 70.0f;
    if (ImGuiMCP::SliderFloat("##poiDetectionRadius", &poiRadiusMeters, 0.0f, 100.0f, "%.1f m")) {
        g_poiDetectionRadius = poiRadiusMeters * 70.0f;
        IniParser::Save();
    }

}