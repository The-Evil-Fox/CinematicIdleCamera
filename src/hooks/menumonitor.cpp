#include "hooks/menumonitor.h"

namespace logger = SKSE::log;

namespace Hooks {

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  The specific menus that Skyrim Souls RE (and similar unpause-everything mods)
    //  patch to no longer pause the game. Auto-vanity is blocked while any of these
    //  is open, exactly recreating the vanilla "camera can't engage while a menu is
    //  open" behavior for just this set.
    // 
    //  Using RE::XMenu::MENU_NAME constants where CommonLib exposes a standard class
    //  for the menu, since that's guaranteed to match the actual registered name.
    // 
    //  NOTE: "HUD Menu" is deliberately excluded, see the header comment for why.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static constexpr std::array<std::string_view, 22> kBlockingMenus{

        RE::BarterMenu::MENU_NAME,
        RE::BookMenu::MENU_NAME,
        "CombatAlertOverlayMenu",
        RE::Console::MENU_NAME,
        RE::ContainerMenu::MENU_NAME,
        "Custom Menu",
        RE::DialogueMenu::MENU_NAME,
        RE::FavoritesMenu::MENU_NAME,
        RE::GiftMenu::MENU_NAME,
        RE::InventoryMenu::MENU_NAME,
        RE::JournalMenu::MENU_NAME,
        RE::LevelUpMenu::MENU_NAME,
        RE::LockpickingMenu::MENU_NAME,
        RE::MagicMenu::MENU_NAME,
        RE::MapMenu::MENU_NAME,
        RE::MessageBoxMenu::MENU_NAME,
        "ModManagerMenu",
        RE::SleepWaitMenu::MENU_NAME,
        RE::StatsMenu::MENU_NAME,
        RE::TrainingMenu::MENU_NAME,
        RE::TutorialMenu::MENU_NAME,
        RE::TweenMenu::MENU_NAME

    };

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Singleton accessor
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    MenuMonitor* MenuMonitor::GetSingleton() {

        static MenuMonitor singleton;
        return &singleton;

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Registers the sink with RE::UI and performs an initial sync.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void MenuMonitor::Register() {

        auto* ui = RE::UI::GetSingleton();

        if (!ui) {

            logger::warn("MenuMonitor::Register: RE::UI singleton not available");
            return;

        }

        ui->AddEventSink<RE::MenuOpenCloseEvent>(GetSingleton());

        logger::info("MenuMonitor: registered as MenuOpenCloseEvent sink");

        // Initial sync: in case one of the blocking menus is already open at the
        // moment we register, e.g. the plugin loading mid-session.

        RefreshAllowAutoVanity();

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Returns true if any menu in kBlockingMenus is currently open.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    bool MenuMonitor::AnyBlockingMenuOpen(RE::UI* a_ui) {

        for (const auto& menuName : kBlockingMenus) {

            if (a_ui->IsMenuOpen(menuName)) {

                logger::debug("MenuMonitor: blocking menu open: {}", menuName);
                return true;

            }

        }

        return false;

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Pushes the current blocking-menu state onto PlayerCamera::allowAutoVanityMode.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void MenuMonitor::RefreshAllowAutoVanity() {

        auto* ui = RE::UI::GetSingleton();

        if (!ui) {

            logger::warn("MenuMonitor::RefreshAllowAutoVanity: RE::UI singleton not available");
            return;

        }

        auto* camera = RE::PlayerCamera::GetSingleton();

        if (!camera) {

            logger::warn("MenuMonitor::RefreshAllowAutoVanity: PlayerCamera singleton not available");
            return;

        }

        const bool blocked = AnyBlockingMenuOpen(ui);
        const bool allow = !blocked;

        camera->GetRuntimeData2().allowAutoVanityMode = allow;

        logger::debug("MenuMonitor: allowAutoVanityMode = {} (blocked = {})", allow, blocked);

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Event handler: fires whenever any menu opens or closes.
    // 
    //  We don't inspect a_event's specific menu name here, we just re-run the full blocking-menu scan on every transition, since the list is small and this only
    //  runs on open/close events, never per-frame.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    RE::BSEventNotifyControl MenuMonitor::ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_source) {

        if (!a_event) {

            return RE::BSEventNotifyControl::kContinue;

        }

        RefreshAllowAutoVanity();

        return RE::BSEventNotifyControl::kContinue;

    }

}