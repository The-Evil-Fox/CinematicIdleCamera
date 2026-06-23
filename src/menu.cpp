#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"

namespace logger = SKSE::log;

float UI::g_idleTimer = 30.0f;
float UI::g_poiDetectionRadius = 1400.0f;

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) return;
    SKSEMenuFramework::SetSection("Cinematic Idle Camera");
    SKSEMenuFramework::AddSectionItem("Settings", Settings);
    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

void UI::DrawCinematicBars() {

    auto* playerCamera = RE::PlayerCamera::GetSingleton();
    if (!playerCamera || !playerCamera->currentState) return;
    if (playerCamera->currentState->id != RE::CameraState::kAutoVanity) return;

    auto* drawList = ImGuiMCP::GetForegroundDrawList();
    auto* io = ImGuiMCP::GetIO();
    float screenW = io->DisplaySize.x;
    float screenH = io->DisplaySize.y;

    const float barHeight = screenH * 0.08f;
    const ImGuiMCP::ImU32 barColor = ImGuiMCP::ColorConvertFloat4ToU32(
        ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f }
    );

    ImGuiMCP::ImDrawListManager::AddRectFilled(
        drawList,
        ImGuiMCP::ImVec2{ 0.0f, 0.0f },
        ImGuiMCP::ImVec2{ screenW, barHeight },
        barColor, 0.0f, 0
    );

    ImGuiMCP::ImDrawListManager::AddRectFilled(
        drawList,
        ImGuiMCP::ImVec2{ 0.0f, screenH - barHeight },
        ImGuiMCP::ImVec2{ screenW, screenH },
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

    bool ActivateIdleCam = ImGuiMCP::Button("Go Idle Camera Mode");

    if (ActivateIdleCam) {

        auto* playerCamera = RE::PlayerCamera::GetSingleton();

        if (playerCamera) {

            playerCamera->PushCameraState(RE::CameraState::kAutoVanity);

        }

    }

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