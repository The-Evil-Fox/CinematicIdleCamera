#include "PlayerCameraHook.h"
#include "menu.h"

namespace logger = SKSE::log;

namespace Hooks {

    void AutoVanityStateHook::Install() {

        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_AutoVanityState[0] };
        _Update = vtbl.write_vfunc(REL::Module::IsVR() ? 4 : 3, Update);
        logger::info("Hook installed");

    }

    RE::TESObjectREFR* AutoVanityStateHook::FindBestPOI() {

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return nullptr;

        RE::TESObjectREFR* bestPOI = nullptr;
        float bestScore = 0.0f;

        auto callback = [&](RE::TESObjectREFR* ref) {

            if (!ref || ref == player) return RE::BSContainer::ForEachResult::kContinue;

            auto* actor = ref->As<RE::Actor>();
            if (!actor) return RE::BSContainer::ForEachResult::kContinue;

            float score = 0.0f;

            if (actor->GetCurrentScene() != nullptr) {

                score = 300.0f;

            }
            else if (actor->IsMoving()) {

                score = 200.0f;

            } else {

                score = 10.0f;

            }

            float dist = player->GetPosition().GetDistance(ref->GetPosition());
            score += (UI::g_poiDetectionRadius - dist) / UI::g_poiDetectionRadius * 50.0f;

            if (score > bestScore) {

                bestScore = score;
                bestPOI = ref;

            }

            return RE::BSContainer::ForEachResult::kContinue;

        };

        RE::TES::GetSingleton()->ForEachReferenceInRange(player, UI::g_poiDetectionRadius, callback);
        return bestScore > 0.0f ? bestPOI : nullptr;

    }

    void AutoVanityStateHook::Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState) {

        float savedRot = a_this->autoVanityRot;
        _Update(a_this, a_nextState);

        auto* poi = FindBestPOI();

        if (poi) {

            auto* player = RE::PlayerCharacter::GetSingleton();
            auto direction = poi->GetPosition() - player->GetPosition();

            float worldAngle = std::atan2(direction.x, direction.y);
            float targetAngle = worldAngle - player->GetAngleZ();

            while (targetAngle > std::numbers::pi_v<float>)  targetAngle -= 2.0f * std::numbers::pi_v<float>;
            while (targetAngle < -std::numbers::pi_v<float>) targetAngle += 2.0f * std::numbers::pi_v<float>;

            const float delta = RE::BSTimer::GetSingleton()->realTimeDelta;; // fallback if no frame delta API
            const float speed = 1.5f;
            a_this->autoVanityRot = std::lerp(savedRot, targetAngle, 1.0f - std::exp(-speed * delta));

        }
        else {

            a_this->autoVanityRot = savedRot;

        }

    }

}