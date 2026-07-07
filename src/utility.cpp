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

        } else if (key == "actorCombatScore") {

            UI::g_actorCombatScore = std::stof(value);
            logger::debug("Loaded actorCombatScore: {}", UI::g_actorCombatScore);

        } else if (key == "actorCombatProximityEnabled") {

            UI::g_actorCombatProximityEnabled = (value == "1" || value == "true");
            logger::debug("Loaded actorCombatProximityEnabled: {}", UI::g_actorCombatProximityEnabled);

        } else if (key == "actorCombatProximityFactor") {

            UI::g_actorCombatProximityFactor = std::stof(value);
            logger::debug("Loaded actorCombatProximityFactor: {}", UI::g_actorCombatProximityFactor);

        } else if (key == "actorMovingScore") {

            UI::g_actorMovingScore = std::stof(value);
            logger::debug("Loaded actorMovingScore: {}", UI::g_actorMovingScore);

        } else if (key == "actorMovingProximityEnabled") {

            UI::g_actorMovingProximityEnabled = (value == "1" || value == "true");
            logger::debug("Loaded actorMovingProximityEnabled: {}", UI::g_actorMovingProximityEnabled);

        } else if (key == "actorMovingProximityFactor") {

            UI::g_actorMovingProximityFactor = std::stof(value);
            logger::debug("Loaded actorMovingProximityFactor: {}", UI::g_actorMovingProximityFactor);

        } else if (key == "actorInSceneScore") {

            UI::g_actorInSceneScore = std::stof(value);
            logger::debug("Loaded actorInSceneScore: {}", UI::g_actorInSceneScore);

        } else if (key == "actorInSceneProximityEnabled") {

            UI::g_actorInSceneProximityEnabled = (value == "1" || value == "true");
            logger::debug("Loaded actorInSceneProximityEnabled: {}", UI::g_actorInSceneProximityEnabled);

        } else if (key == "actorInSceneProximityFactor") {

            UI::g_actorInSceneProximityFactor = std::stof(value);
            logger::debug("Loaded actorInSceneProximityFactor: {}", UI::g_actorInSceneProximityFactor);

        } else if (key == "actorIdleScore") {

            UI::g_actorIdleScore = std::stof(value);
            logger::debug("Loaded actorIdleScore: {}", UI::g_actorIdleScore);

        } else if (key == "actorIdleProximityEnabled") {

            UI::g_actorIdleProximityEnabled = (value == "1" || value == "true");
            logger::debug("Loaded actorIdleProximityEnabled: {}", UI::g_actorIdleProximityEnabled);

        } else if (key == "actorIdleProximityFactor") {

            UI::g_actorIdleProximityFactor = std::stof(value);
            logger::debug("Loaded actorIdleProximityFactor: {}", UI::g_actorIdleProximityFactor);

        } else if (key == "actorFlyingCritterScore") {

            UI::g_flyingCritterScore = std::stof(value);
            logger::debug("Loaded actorFlyingCritterScore: {}", UI::g_flyingCritterScore);

        } else if (key == "actorFlyingCritterProximityEnabled") {

            UI::g_flyingCritterProximityEnabled = (value == "1" || value == "true");
            logger::debug("Loaded actorFlyingCritterProximityEnabled: {}", UI::g_flyingCritterProximityEnabled);

        } else if (key == "actorFlyingCritterProximityFactor") {

            UI::g_flyingCritterProximityFactor = std::stof(value);
            logger::debug("Loaded actorFlyingCritterProximityFactor: {}", UI::g_flyingCritterProximityFactor);

        } else if (key == "blendDuration") {

            UI::g_blendDuration = std::stof(value);
            logger::debug("Loaded blendDuration: {}", UI::g_blendDuration);

        } else if (key == "idleCameraOffsetX") {

            UI::g_IdleCamOffsetX = std::stof(value);
            logger::debug("Loaded idleCameraOffsetX: {}", UI::g_IdleCamOffsetX);

        } else if (key == "idleCameraOffsetY") {

            UI::g_IdleCamOffsetY = std::stof(value);
            logger::debug("Loaded idleCameraOffsetY: {}", UI::g_IdleCamOffsetY);

        } else if (key == "idleCameraOffsetZ") {

            UI::g_IdleCamOffsetZ = std::stof(value);
            logger::debug("Loaded idleCameraOffsetZ: {}", UI::g_IdleCamOffsetZ);

        } else if (key == "headTrackFadeSpeed") {

            UI::g_headTrackFadeSpeed = std::stof(value);
            logger::debug("Loaded headTrackFadeSpeed: {}", UI::g_headTrackFadeSpeed);

        } else if (key == "dezoomTriggerRadius") {

            UI::g_dezoomTriggerRadius = std::stof(value);
            logger::debug("Loaded dezoomTriggerRadius: {}", UI::g_dezoomTriggerRadius);

        } else if (key == "dezoomTriggerHeight") {

            UI::g_dezoomTriggerHeight = std::stof(value);
            logger::debug("Loaded dezoomTriggerHeight: {}", UI::g_dezoomTriggerHeight);

        } else if (key == "dezoomAmount") {

            UI::g_dezoomAmount = std::stof(value);
            logger::debug("Loaded dezoomAmount: {}", UI::g_dezoomAmount);

        } else if (key == "dezoomBlendSpeed") {

            UI::g_dezoomBlendSpeed = std::stof(value);
            logger::debug("Loaded dezoomBlendSpeed: {}", UI::g_dezoomBlendSpeed);

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

    file << ";=====================CAMERA SETTINGS============================\n";
    file << "\n";
    file << "; Camera Idle Timer (seconds without any player input before Cinematic Idle Mode activates)\n";
    file << "fIdleTimer=" << UI::g_idleTimer << "\n";
    file << "\n";
    file << "; Camera Blend Duration (seconds for a POI-switch / entry / exit blend)\n";
    file << "blendDuration=" << UI::g_blendDuration << "\n";
    file << "\n";
    file << "; Idle Camera Offsets (Skyrim units)\n";
    file << "idleCameraOffsetX=" << UI::g_IdleCamOffsetX << "\n";
    file << "idleCameraOffsetY=" << UI::g_IdleCamOffsetY << "\n";
    file << "idleCameraOffsetZ=" << UI::g_IdleCamOffsetZ << "\n";
    file << "\n";
    file << "; Dynamic Dezoom: pulls the camera back when a POI is both close to the player and well\n";
    file << "; above them, since this system only rotates yaw and can't tilt to keep it in frame.\n";
    file << "; Trigger radius/height are in Skyrim units (~70 units per meter). Amount is added to\n";
    file << "; idleCameraOffsetY at full weight (signed - flip if it zooms the wrong way). Blend speed\n";
    file << "; is in units/sec, matching headTrackFadeSpeed.\n";
    file << "dezoomTriggerRadius=" << UI::g_dezoomTriggerRadius << "\n";
    file << "dezoomTriggerHeight=" << UI::g_dezoomTriggerHeight << "\n";
    file << "dezoomAmount=" << UI::g_dezoomAmount << "\n";
    file << "dezoomBlendSpeed=" << UI::g_dezoomBlendSpeed << "\n";
    file << "\n";
    file << ";=====================HEAD TRACKING SETTINGS=====================\n";
    file << "\n";
    file << "; Head-Track Fade Speed (units/sec)\n";
    file << "headTrackFadeSpeed=" << UI::g_headTrackFadeSpeed << "\n";
    file << "\n";
    file << ";=====================POI SYSTEM SETTINGS========================\n";
    file << "\n";
    file << "; POI Detection Radius (Skyrim units, ~70 units per meter)\n";
    file << "poiDetectionRadius=" << UI::g_poiDetectionRadius << "\n";
    file << "\n";
    file << "; POI Lock Duration (seconds the camera must stay on a POI before it can switch)\n";
    file << "lockDuration=" << UI::g_lockDuration << "\n";
    file << "\n";
    file << "; Base score awarded to an actor per action state, plus an optional proximity bonus\n";
    file << "; (added on top, scaled 0-1 by how close the POI is relative to poiDetectionRadius)\n";
    file << "actorCombatScore=" << UI::g_actorCombatScore << "\n";
    file << "actorCombatProximityEnabled=" << (UI::g_actorCombatProximityEnabled ? "1" : "0") << "\n";
    file << "actorCombatProximityFactor=" << UI::g_actorCombatProximityFactor << "\n";
    file << "\n";
    file << "actorMovingScore=" << UI::g_actorMovingScore << "\n";
    file << "actorMovingProximityEnabled=" << (UI::g_actorMovingProximityEnabled ? "1" : "0") << "\n";
    file << "actorMovingProximityFactor=" << UI::g_actorMovingProximityFactor << "\n";
    file << "\n";
    file << "actorInSceneScore=" << UI::g_actorInSceneScore << "\n";
    file << "actorInSceneProximityEnabled=" << (UI::g_actorInSceneProximityEnabled ? "1" : "0") << "\n";
    file << "actorInSceneProximityFactor=" << UI::g_actorInSceneProximityFactor << "\n";
    file << "\n";
    file << "actorIdleScore=" << UI::g_actorIdleScore << "\n";
    file << "actorIdleProximityEnabled=" << (UI::g_actorIdleProximityEnabled ? "1" : "0") << "\n";
    file << "actorIdleProximityFactor=" << UI::g_actorIdleProximityFactor << "\n";
    file << "\n";
    file << "; Flying critters (butterflies, moths, dragonflies, etc) use their own score, since they\n";
    file << "; aren't Actors and don't have an action state.\n";
    file << "actorFlyingCritterScore=" << UI::g_flyingCritterScore << "\n";
    file << "actorFlyingCritterProximityEnabled=" << (UI::g_flyingCritterProximityEnabled ? "1" : "0") << "\n";
    file << "actorFlyingCritterProximityFactor=" << UI::g_flyingCritterProximityFactor << "\n";
    file << "\n";
    file << ";=====================DEBUG SETTINGS=============================\n";
    file << "\n";
    file << "; Debug Raycast Visualization (0 = off, 1 = on)\n";
    file << "debugRaycasts=" << (UI::g_debugRaycasts ? "1" : "0") << "\n";
    file << "\n";
    file << "; Log Level (0=Quiet/critical, 1=Warnings, 2=Info, 3=Debug)\n";
    file << "logginglevel=" << UI::g_loggingLevel << "\n";

    logger::info("User config saved successfully.");

}