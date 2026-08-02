#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "logger.hpp"
#define SMOOTHCAM_API_COMMONLIB
#include "api/SmoothCamAPI.h"

namespace Hooks {

    // ==================================================================================================================================================================================
    //  POI actions that can be processed by the FindBestPOI system
    // ==================================================================================================================================================================================

    enum class POIAction {

        None,
        Dragon,
        Fish,
        Idle,
        InCombat,
        Insect,
        InScene,
        Moving

    };

    // ==================================================================================================================================================================================
    // SmoothCam compatibility hook
    // ==================================================================================================================================================================================

    class SmoothCamCompat {

        public:

            static void                                     RegisterListener() noexcept;
            static void                                     RequestInterface() noexcept;
            static bool                                     Acquire() noexcept; // now returns whether control is held
            static void                                     Release() noexcept;

            // True once another mod has been observed holding SmoothCam camera
            // control. Once set, this mod stops trying entirely for the rest of
            // the session rather than repeatedly contesting it.
            static bool                                     IsDisabledByConflict() noexcept { return s_disabledByConflict; }

        private:

            static inline                                   SmoothCamAPI::IVSmoothCam3* s_api = nullptr;
            static inline bool                              s_holding = false;
            static inline bool                              s_disabledByConflict = false;

    };

    // ==================================================================================================================================================================================
    // Auto Vanity hook
    // ==================================================================================================================================================================================

    class AutoVanityStateHook {

        public:

            static void                                     Install();

            static RE::TESObjectREFR*                       FindBestPOI(POIAction& a_outAction, float& a_outScore);

            static POIAction                                GetActorAction(RE::Actor* a_actor);

        private:

            static void                                     Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState);

            inline static                                   REL::Relocation<decltype(Update)> _Update;

            static void                                     EndState(RE::AutoVanityState* a_this);

            static inline                                   REL::Relocation<decltype(EndState)> _EndState;
    };

    // ==================================================================================================================================================================================
    // Translation hook (used for custom positionning of the vanity camera
    // ==================================================================================================================================================================================

    struct AutoVanityState_GetTranslationHelper {

            static void                                     thunk(RE::AutoVanityState* a_this, std::int64_t  param_2, RE::NiPoint3* param_3, std::int64_t  param_4, std::uint32_t param_5);

            static inline                                   REL::Relocation<decltype(thunk)*> func;

            static void                                     Install();

    };

    // ==================================================================================================================================================================================
    //  KillMove (kVATS) camera-state hook
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  This hook is used to prevent the vanity camera to trigger during a killmove.
    //  Otherwise it breaks the camera and the UI and forces the player to kill Skyrim's process in the task manager
    // ==================================================================================================================================================================================

    class KillMoveCameraStateHook {

        public:

            static void                                     Install();

        private:

            static void                                     Update(RE::TESCameraState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState);

            inline static                                   REL::Relocation<decltype(Update)> _Update;

            static void                                     EndState(RE::TESCameraState* a_this);

            inline static                                   REL::Relocation<decltype(EndState)> _EndState;

            static inline                                   bool s_installed = false;

    };

    // ==================================================================================================================================================================================
    //  Combat-state hook
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Same purpose as KillMoveCameraStateHook (block allowAutoVanityMode), but combat
    //  isn't a discrete camera state to vtable-hook, so this listens to the vanilla
    //  TESCombatEvent instead.
    // ==================================================================================================================================================================================

    class CombatStateHook : public RE::BSTEventSink<RE::TESCombatEvent> {

        public:

            static CombatStateHook* GetSingleton();

            static void                                     Register();

            RE::BSEventNotifyControl                        ProcessEvent(const RE::TESCombatEvent* a_event, RE::BSTEventSource<RE::TESCombatEvent>* a_source) override;

        private:

            CombatStateHook()                               = default;

    };

}