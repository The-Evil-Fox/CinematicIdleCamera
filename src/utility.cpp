#include "utility.h"
#include "menu.h"

namespace logger = SKSE::log;

static constexpr auto INI_PATH = "Data\\SKSE\\Plugins\\cinematicidlecamera.ini";

void IniParser::Load() {

    if (!std::filesystem::exists(INI_PATH)) {
        logger::info("No config file found, creating default config");
        IniParser::Save();  // creates the file with current defaults
        return;
    }

    std::ifstream file(INI_PATH);
    if (!file.is_open()) {
        logger::warn("Failed to open config file");
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line.starts_with(";")) continue;

        auto eq = line.find('=');
        if (eq == std::string::npos) continue;

        std::string key = line.substr(0, eq);
        std::string value = line.substr(eq + 1);

        if (key == "fIdleTimer") {
            UI::g_idleTimer = std::stof(value);
            logger::info("Loaded fIdleTimer: {}", UI::g_idleTimer);
        }
    }

    logger::info("Config loaded successfully");
}

void IniParser::Save() {
    std::ofstream file(INI_PATH);
    if (!file.is_open()) {
        logger::warn("Failed to open config file for writing");
        return;
    }

    file << "; Camera Idle Timer\n";
    file << "fIdleTimer=" << UI::g_idleTimer << "\n";

    logger::info("Config saved successfully");
}