#include "utility.h"
#include "menu.h"

namespace logger = SKSE::log;

static constexpr auto INI_PATH = "Data\\SKSE\\Plugins\\cinematicidlecamera.ini";

void IniParser::Load() {

    if (!std::filesystem::exists(INI_PATH)) {

        logger::info("No config file found, creating default config.");
        IniParser::Save();
        return;

    }

    std::ifstream file(INI_PATH);

    if (!file.is_open()) {

        logger::warn("Failed to open config file !");
        return;

    }

    std::string line;
    while (std::getline(file, line)) {

        if (line.empty() || line.starts_with(";")) {

            continue;

        }

        auto eq = line.find('=');
        if (eq == std::string::npos) {

            continue;

        }

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if (key == "fIdleTimer") {

            UI::g_idleTimer = std::stof(value);
            logger::debug("Loaded fIdleTimer: {}", UI::g_idleTimer);

        } else if (key == "poiDetectionRadius") {

            UI::g_poiDetectionRadius = std::stof(value);
            logger::debug("Loaded poiDetectionRadius: {}", UI::g_poiDetectionRadius);

        } else if (key == "lockDuration") {

            UI::g_lockDuration = std::stof(value);
            logger::debug("Loaded lockDuration: {}", UI::g_lockDuration);

        } else if (key == "blendDuration") {

            UI::g_blendDuration = std::stof(value);
            logger::debug("Loaded blendDuration: {}", UI::g_blendDuration);

        } else if (key == "headTrackFadeSpeed") {

            UI::g_headTrackFadeSpeed = std::stof(value);
            logger::debug("Loaded headTrackFadeSpeed: {}", UI::g_headTrackFadeSpeed);

        } else if (key == "debugRaycasts") {

            UI::g_debugRaycasts = (value == "1" || value == "true");
            logger::debug("Loaded debugRaycasts: {}", UI::g_debugRaycasts);

        } else if (key == "logginglevel") {

            UI::g_loggingLevel = std::clamp(std::stoi(value), 0, 3);
            logger::info("Logging level set to {}", UI::g_loggingLevel);

            spdlog::level::level_enum lvl = spdlog::level::info;
            if (UI::g_loggingLevel == 0) lvl = spdlog::level::critical;
            else if (UI::g_loggingLevel == 1) lvl = spdlog::level::warn;
            else if (UI::g_loggingLevel == 3) lvl = spdlog::level::debug;

            spdlog::set_level(lvl);
            spdlog::flush_on(lvl);
            continue;

        }

    }

    logger::info("User config loaded successfully.");

}

void IniParser::Save() {

    std::ofstream file(INI_PATH);

    if (!file.is_open()) {

        logger::warn("Failed to open config file for writing !");
        return;

    }

    file << "; Camera Idle Timer\n";
    file << "fIdleTimer=" << UI::g_idleTimer << "\n";
    file << "\n";
    file << "; POI Detection Radius (Skyrim units, ~70 units per meter)\n";
    file << "poiDetectionRadius=" << UI::g_poiDetectionRadius << "\n";
    file << "\n";
    file << "; POI Lock Duration (seconds the camera must stay on a POI before it can switch)\n";
    file << "lockDuration=" << UI::g_lockDuration << "\n";
    file << "\n";
    file << "; Camera Blend Duration (seconds for a POI-switch / entry / exit blend)\n";
    file << "blendDuration=" << UI::g_blendDuration << "\n";
    file << "\n";
    file << "; Head-Track Fade Speed (units/sec)\n";
    file << "headTrackFadeSpeed=" << UI::g_headTrackFadeSpeed << "\n";
    file << "\n";
    file << "; Debug Raycast Visualization (0 = off, 1 = on)\n";
    file << "debugRaycasts=" << (UI::g_debugRaycasts ? "1" : "0") << "\n";
    file << "\n";
    file << "; Log Level (0=Quiet/critical, 1=Warnings, 2=Info, 3=Debug)\n";
    file << "logginglevel=" << UI::g_loggingLevel << "\n";

    logger::info("User config saved successfully.");

}