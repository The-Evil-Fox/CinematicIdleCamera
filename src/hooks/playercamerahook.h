#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "logger.hpp"

namespace Hooks
{
    enum class POIAction {

        None,
        Idle,
        InCombat,
        InScene,
        Moving

    };

    class AutoVanityStateHook {

        public:
            static void                Install();
            static RE::TESObjectREFR* FindBestPOI(POIAction& a_outAction, float& a_outScore);
            static POIAction           GetActorAction(RE::Actor* a_actor);

        private:
            static void Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState);
            inline static REL::Relocation<decltype(Update)> _Update;

        private:
            static void EndState(RE::AutoVanityState* a_this);
            static inline REL::Relocation<decltype(EndState)> _EndState;

    };
}