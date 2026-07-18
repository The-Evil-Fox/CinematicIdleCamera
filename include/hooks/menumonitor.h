#pragma once

#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "logger.hpp"

#include <array>
#include <string_view>

namespace Hooks {

    // ===================================================================================================================================================================================
    //  MenuMonitor
    // 
    //  Listens for RE::MenuOpenCloseEvent and, on every open/close, re-syncs
    //  PlayerCamera::allowAutoVanityMode against an explicit list of "blocking" menus.
    // 
    //  Context: in vanilla Skyrim, the vanity camera never engages while a menu is
    //  open because the game itself is paused. Mods like Skyrim Souls RE patch a
    //  specific set of vanilla menus (BarterMenu, InventoryMenu, MapMenu, etc.) to no
    //  longer pause the game, which also means the engine no longer blocks auto-vanity
    //  while they're open. This class recreates that vanilla blocking behavior only for
    //  the menus those unpause patches touch.
    // 
    //  Deliberately a DENYLIST, not an allow-list: earlier versions of this tried
    //  allow-listing "safe" menus (HUD Menu, Cursor Menu) and treating everything
    //  else as blocking, which broke as soon as any HUD/overlay mod (TrueHUD, BTPS,
    //  screen-fade effects, loading spinners, debug overlays, ...) registered its own
    //  menu, since those aren't and can't be enumerated in advance. A denylist of the
    //  fixed, known set of menus that Skyrim Souls-style mods unpause doesn't have
    //  that problem.
    // 
    //  NOTE: "HUD Menu" is intentionally NOT included below even though SkyrimSoulsRE
    //  ships a HUDMenuEx. That extension exists for visual/compat reasons (crosshair,
    //  compass behavior), not because HUD Menu should count as blocking. HUD Menu is
    //  open essentially all the time during normal gameplay, so including it here
    //  would prevent auto-vanity from ever engaging.
    // 
    //  This is fully decoupled from AutoVanityState: it does not hook Begin, Update,
    //  or EndState, and does not require a PlayerCamera::Update vtable hook. It runs
    //  purely off the UI event source, so it works whether or not vanity mode is
    //  currently active.
    // ===================================================================================================================================================================================

    class MenuMonitor final : public RE::BSTEventSink<RE::MenuOpenCloseEvent> {

        public:

            static MenuMonitor* GetSingleton();

            // Registers the sink with RE::UI and performs an initial sync in case
            // one of the blocking menus below is already open at the time of
            // registration (e.g. plugin loads mid-game via a save with a menu
            // already open).

            static void                                                     Register();

            RE::BSEventNotifyControl                                        ProcessEvent(const RE::MenuOpenCloseEvent* a_event, RE::BSTEventSource<RE::MenuOpenCloseEvent>* a_source) override;

        private:

            MenuMonitor() = default;
            ~MenuMonitor() override = default;
            MenuMonitor(const MenuMonitor&) = delete;
            MenuMonitor(MenuMonitor&&) = delete;
            MenuMonitor& operator=(const MenuMonitor&) = delete;
            MenuMonitor& operator=(MenuMonitor&&) = delete;

            static bool                                                     AnyBlockingMenuOpen(RE::UI* a_ui);

            static void                                                     RefreshAllowAutoVanity();

    };

}