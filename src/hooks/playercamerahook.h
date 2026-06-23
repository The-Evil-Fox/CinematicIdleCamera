#pragma once
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"
#include "logger.hpp"

namespace Hooks
{
    class AutoVanityStateHook
    {
    public:
        static void Install();
        static RE::TESObjectREFR* FindBestPOI();

    private:
        static void Update(
            RE::AutoVanityState* a_this,
            RE::BSTSmartPointer<RE::TESCameraState>& a_nextState);

        inline static REL::Relocation<decltype(Update)> _Update;
    };
}