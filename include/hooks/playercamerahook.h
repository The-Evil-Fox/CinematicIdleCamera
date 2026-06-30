#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "logger.hpp"
#define SMOOTHCAM_API_COMMONLIB
#include "api/SmoothCamAPI.h"

namespace Hooks {

    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  POI actions that can be processed by the FindBestPOI system
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    enum class POIAction {

        None,
        Idle,
        InCombat,
        InScene,
        Moving

    };

    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // SmoothCam compatibility hook
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    class SmoothCamCompat {

        public:
            
            static void                                 RegisterListener() noexcept;
            
            static void                                 RequestInterface() noexcept;

            static void                                 Acquire() noexcept;

            static void                                 Release() noexcept;

        private:

            static inline SmoothCamAPI::IVSmoothCam3*   s_api = nullptr;

            static inline bool                          s_holding = false;
    };

    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Auto Vanity hook
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    class AutoVanityStateHook {

        public:

            static void                                 Install();

            static RE::TESObjectREFR*                   FindBestPOI(POIAction& a_outAction, float& a_outScore);

            static POIAction                            GetActorAction(RE::Actor* a_actor);

        private:

            static void                                 Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState);

            inline static                               REL::Relocation<decltype(Update)> _Update;

            static void                                 EndState(RE::AutoVanityState* a_this);

            static inline                               REL::Relocation<decltype(EndState)> _EndState;
    };

    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Translation hook (used for custom positionning of the vanity camera
    // -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    struct AutoVanityState_GetTranslationHelper {

        static void                                     thunk(RE::AutoVanityState* a_this,std::int64_t  param_2, RE::NiPoint3* param_3, std::int64_t  param_4, std::uint32_t param_5);

        static inline                                   REL::Relocation<decltype(thunk)*> func;

        static void                                     Install();

    };

}