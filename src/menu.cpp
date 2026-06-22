#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"

namespace logger = SKSE::log;

float UI::g_idleTimer = 30.0f;

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) return;
    SKSEMenuFramework::SetSection("Cinematic Idle Camera");
    SKSEMenuFramework::AddSectionItem("Settings", Settings);

}

void UI::Settings() {

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    FontAwesome::PushSolid();
    auto iconUtf8 = FontAwesome::UnicodeToUtf8(0xf0eb);
    ImGuiMCP::Text("%s Cinematic Idle Camera - Settings", iconUtf8.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // To go idle whenever I want

    bool ActivateIdleCam = ImGuiMCP::Button("Go Idle Camera Mode");

    if (ActivateIdleCam) {

        auto* playerCamera = RE::PlayerCamera::GetSingleton();

        if (playerCamera) {

            playerCamera->PushCameraState(RE::CameraState::kAutoVanity);

        }

    }

    ImGuiMCP::Separator();

    // Camera Idle Timer
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
}