#include "PlayerCameraHook.h"
#include "menu.h"

namespace logger = SKSE::log;

namespace Hooks {

    static RE::TESObjectREFR* s_currentPOI = nullptr;
    static float               s_currentScore = 0.0f;

    // How long the camera must stay on a POI before it's allowed to switch to another
    static constexpr float k_lockDuration = 5.0f;

    // Counts down from k_lockDuration to 0 — switching is only allowed when this hits 0
    static float s_lockTimer = 0.0f;

    // ---------------------------------------------------------------------------

    // Classify what the actor is currently doing so we can score them
    POIAction AutoVanityStateHook::GetActorAction(RE::Actor* a_actor) {

        if (!a_actor) return POIAction::None;

        if (a_actor->GetCurrentScene() != nullptr) return POIAction::InScene;
        if (a_actor->IsMoving())                   return POIAction::Moving;

        return POIAction::Idle;

    }

    // ---------------------------------------------------------------------------

    void AutoVanityStateHook::Install() {

        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_AutoVanityState[0] };
        _Update = vtbl.write_vfunc(REL::Module::IsVR() ? 4 : 3, Update);
        logger::info("Hook installed");

    }

    // ---------------------------------------------------------------------------

    RE::TESObjectREFR* AutoVanityStateHook::FindBestPOI(POIAction& a_outAction, float& a_outScore) {

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return nullptr;

        RE::TESObjectREFR* bestPOI = nullptr;
        POIAction          bestAction = POIAction::None;
        float              bestScore = 0.0f;

        auto callback = [&](RE::TESObjectREFR* ref) {

            if (!ref || ref == player) return RE::BSContainer::ForEachResult::kContinue;

            // Only care about actors (NPCs, creatures, etc.)
            auto* actor = ref->As<RE::Actor>();
            if (!actor) return RE::BSContainer::ForEachResult::kContinue;

            POIAction action = GetActorAction(actor);
            float     score = 0.0f;

            float dist = player->GetPosition().GetDistance(ref->GetPosition());

            // Proximity factor: 1.0 when right next to the player, 0.0 at the edge of detection radius
            float proximityFactor = std::max(0.0f, (UI::g_poiDetectionRadius - dist) / UI::g_poiDetectionRadius);

            switch (action) {

                // An NPC walking near the player is the most cinematic — score scales
                // with how close they are so nearby walkers beat distant ones
            case POIAction::Moving:
                score = 400.0f + proximityFactor * 150.0f;
                break;

                // A scripted scene is very interesting but fixed score since
                // proximity matters less when something scripted is happening
            case POIAction::InScene:
                score = 300.0f;
                break;

                // An idle NPC is the least interesting baseline
            case POIAction::Idle:
                score = 10.0f;
                break;

            default:
                break;

            }

            // Everyone also gets a small flat proximity bonus on top of their base score
            // so that a nearby idle NPC can still beat a distant idle NPC
            score += proximityFactor * 50.0f;

            // Keep track of whoever has the highest score so far
            if (score > bestScore) {
                bestScore = score;
                bestPOI = ref;
                bestAction = action;
            }

            return RE::BSContainer::ForEachResult::kContinue;

            };

        // Search all refs within the detection radius
        RE::TES::GetSingleton()->ForEachReferenceInRange(player, UI::g_poiDetectionRadius, callback);

        // Only return a result if we actually found something worth looking at
        if (bestScore > 0.0f) {
            a_outAction = bestAction;
            a_outScore = bestScore;
            return bestPOI;
        }

        return nullptr;

    }

    // ---------------------------------------------------------------------------

    // Drives the player's head toward the locked POI using the same technique
    // as TrueDirectionalMovement: calling AIProcess::SetHeadtrackTarget with a
    // world-space position rather than a TESObjectREFR. This is the only method
    // confirmed to actually move the player's head in-game.
    // Pass a_target = nullptr to release headtracking back to the game.
    static void UpdatePlayerHeadtrack(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_target) {

        if (!a_player) return;

        auto* currentProcess = a_player->GetActorRuntimeData().currentProcess;
        if (!currentProcess) return;

        if (a_target) {

            // Enable BSLookAtModifier on the player — TDM sets this before
            // calling SetHeadtrackTarget to ensure the engine honours the request
            a_player->SetGraphVariableBool("IsNPC", true);
            a_player->AsActorState()->actorState2.headTracking = true;

            // Target the POI at eye level — origins are at ground level so we
            // offset upward by ~120 units to aim at face height
            // Must be a named variable — SetHeadtrackTarget takes a non-const ref
            RE::NiPoint3 targetPos = a_target->GetPosition();
            targetPos.z += 120.0f;

            // AIProcess::SetHeadtrackTarget(Actor* owner, NiPoint3& position) is
            // the overload TDM uses for camera headtracking — it works for the
            // player where the HighProcessData REFRhandle approach does not
            currentProcess->SetHeadtrackTarget(a_player, targetPos);

        }
        else {

            // Release: clear the headtrack target and restore vanilla behaviour
            RE::NiPoint3 zeroPos{ 0.f, 0.f, 0.f };
            currentProcess->SetHeadtrackTarget(a_player, zeroPos);
            a_player->AsActorState()->actorState2.headTracking = false;
            a_player->SetGraphVariableBool("IsNPC", false);

        }

    }

    // ---------------------------------------------------------------------------

    void AutoVanityStateHook::Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState) {

        // Save the current rotation before the vanilla update runs,
        // so we can restore it if there's no POI to look at
        float savedRot = a_this->autoVanityRot;
        _Update(a_this, a_nextState);

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return;

        const float frameDelta = RE::BSTimer::GetSingleton()->realTimeDelta;

        // Tick the lock timer down every frame
        if (s_lockTimer > 0.0f) {
            s_lockTimer = std::max(0.0f, s_lockTimer - frameDelta);
        }

        // --- Check if the current POI has become invalid (died, out of range) ---
        // This bypasses the lock timer — a dead or gone POI is always released immediately
        bool currentGone = false;

        if (s_currentPOI) {

            auto* actor = s_currentPOI->As<RE::Actor>();

            // Invalid if the actor no longer exists or has died
            if (!actor || actor->IsDead()) {
                currentGone = true;
            }
            else {
                // Invalid if the actor walked out of detection range
                float dist = player->GetPosition().GetDistance(s_currentPOI->GetPosition());
                if (dist > UI::g_poiDetectionRadius) currentGone = true;
            }

            if (currentGone) {
                logger::info("POI lost (dead or out of range), releasing immediately");
                s_currentPOI = nullptr;
                s_currentScore = 0.0f;
                s_lockTimer = 0.0f; // Reset timer so we pick up a new POI right away
            }

        }

        // --- Only search for a new POI if the lock timer has expired ---
        // While the timer is running we stay on the current POI no matter what
        if (s_lockTimer <= 0.0f) {

            POIAction          foundAction = POIAction::None;
            float              foundScore = 0.0f;
            RE::TESObjectREFR* candidate = FindBestPOI(foundAction, foundScore);

            if (candidate && candidate != s_currentPOI) {

                // Switch if we have no POI, or the candidate scores strictly better
                if (!s_currentPOI || foundScore > s_currentScore) {

                    logger::info("Locking POI: {} (score {:.1f})", candidate->GetName(), foundScore);

                    s_currentPOI = candidate;
                    s_currentScore = foundScore;

                    // Start the lock timer — camera won't switch again until this expires
                    s_lockTimer = k_lockDuration;

                }

            }
            else if (!candidate) {

                // Nothing in range at all — clear the lock
                if (s_currentPOI) {
                    logger::info("No POI in range, releasing lock");
                }

                s_currentPOI = nullptr;
                s_currentScore = 0.0f;

            }

        }

        // --- Rotate the camera + turn the player's head toward the locked POI ---
        if (s_currentPOI) {

            // Get the world-space direction from the player to the POI
            auto  direction = s_currentPOI->GetPosition() - player->GetPosition();
            float targetAngle = std::atan2(direction.x, direction.y);

            // Find the shortest angular path to the target (avoids spinning the long way around)
            float delta_angle = targetAngle - a_this->autoVanityRot;
            while (delta_angle > std::numbers::pi_v<float>) delta_angle -= 2.0f * std::numbers::pi_v<float>;
            while (delta_angle < -std::numbers::pi_v<float>) delta_angle += 2.0f * std::numbers::pi_v<float>;

            // Exponential smoothing — camera eases toward the target rather than snapping
            const float speed = 1.5f;
            const float alpha = 1.0f - std::exp(-speed * frameDelta);
            a_this->autoVanityRot = a_this->autoVanityRot + alpha * delta_angle;

            // Point the player's head at the POI using TDM's AIProcess approach
            UpdatePlayerHeadtrack(player, s_currentPOI);

        }
        else {

            // No POI — restore the pre-hook rotation so vanilla drift is unaffected
            a_this->autoVanityRot = savedRot;

            // Release head-track so idle animations can take over again
            UpdatePlayerHeadtrack(player, nullptr);

        }

    }

}