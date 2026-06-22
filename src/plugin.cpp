#include "plugin.hpp"
#include "logger.hpp"
#include "menu.h"
#include "hooks/playercamerahook.h"
#include "utility.h"

void InitHooks();

static void MessageHandler(SKSE::MessagingInterface::Message* msg) {
    switch (msg->type) {
    case SKSE::MessagingInterface::kPostLoad:
    {
        break;
    }
    case SKSE::MessagingInterface::kSaveGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kPreLoadGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kPostLoadGame:
    {
        break;
    }
    case SKSE::MessagingInterface::kNewGame:
    {

        break;
    }
    case SKSE::MessagingInterface::kDataLoaded:
    {

        IniParser::Load();

        break;
    }
    default:
        break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* skse)
{

    SKSE::Init(skse);

    UI::Register();

    SKSE::GetMessagingInterface()->RegisterListener(MessageHandler);

    Hooks::AutoVanityStateHook::Install();
    logger::debug("Everything working... As it should be ?");

    return true;
}