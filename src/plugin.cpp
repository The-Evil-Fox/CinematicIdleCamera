#include "plugin.hpp"
#include "logger.hpp"
#include "menu.h"
#include "hooks/playercamerahook.h"
#include "hooks/menumonitor.h"
#include "utility.h"
#include "DrawDebug.hpp"

void InitHooks();

static void MessageHandler(SKSE::MessagingInterface::Message* msg) {

    switch (msg->type) {

    case SKSE::MessagingInterface::kPostLoad: {

        Hooks::SmoothCamCompat::RegisterListener();
        break;

    }

    case SKSE::MessagingInterface::kPostPostLoad: {

        Hooks::SmoothCamCompat::RequestInterface();
        break;

    }

    case SKSE::MessagingInterface::kSaveGame: {

        break;

    }

    case SKSE::MessagingInterface::kPreLoadGame: {

        break;

    }

    case SKSE::MessagingInterface::kPostLoadGame: {

        Hooks::KillMoveCameraStateHook::Install();

        break;

    }

    case SKSE::MessagingInterface::kNewGame: {

        static bool registered = false;

        if (!registered) {

            DebugAPI_IMPL::DebugOverlayMenu::Register();
            registered = true;

        }

        Hooks::KillMoveCameraStateHook::Install();

        break;

    }

    case SKSE::MessagingInterface::kDataLoaded: {

        IniParser::Load();

        // ===================================================================================================================================================================================
        // Used to manually set the idle timer for the vanity cam in the virtual copy of the skyrimprefs everytime the game is loaded.
        // So it doesn't overwrite user's existing params in skyrimprefs since this copy is not saved once the game is shut down.
        // ===================================================================================================================================================================================

        auto* iniSettings = RE::INISettingCollection::GetSingleton();

        if (iniSettings) {

            auto* vanityModeDelaySetting = iniSettings->GetSetting("fAutoVanityModeDelay:Camera");

            if (vanityModeDelaySetting) {

                vanityModeDelaySetting->data.f = UI::g_idleTimer;

            } else {

                logger::error("Setting not found in INISettingCollection !");

            }

        }

        Hooks::MenuMonitor::Register();
        Hooks::KillMoveCameraStateHook::Install();

        break;

    }

    default:

        break;

    }

}

SKSEPluginLoad(const SKSE::LoadInterface* skse) {

    SKSE::Init(skse);

    SKSE::AllocTrampoline(256);

    UI::Register();

    SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);

    Hooks::AutoVanityStateHook::Install();
    Hooks::AutoVanityState_GetTranslationHelper::Install();
    logger::info("Cinematic Camera Idle has fully loaded successfully!");

    return true;

}