#include "PlayerCameraHook.h"
#include "menu.h"

namespace logger = SKSE::log;

namespace Hooks {

    // ---------------------------------------------------------------------------
    // POI state
    // ---------------------------------------------------------------------------

    static RE::TESObjectREFR* s_currentPOI = nullptr;
    static float               s_currentScore = 0.0f;

    // How long the camera must stay on a POI before it's allowed to switch
    static constexpr float k_lockDuration = 5.0f;
    static float           s_lockTimer = 0.0f;

    // ---------------------------------------------------------------------------
    // Cinematic transition state
    // ---------------------------------------------------------------------------

    // The rotation we are blending FROM at the start of a transition
    static float s_blendFromRot = 0.0f;

    // The rotation we are blending TO (updated each frame while a POI is locked)
    static float s_blendTargetRot = 0.0f;

    // 0.0 = fully at blendFrom, 1.0 = fully at blendTarget
    static float s_blendT = 1.0f;

    // How many seconds a POI-switch / entry / exit blend takes
    static constexpr float k_blendDuration = 1.2f;

    // Head-track fade: 1.0 = full headtrack, 0.0 = released
    // We drive this smoothly instead of toggling it abruptly
    static float s_headTrackWeight = 0.0f;
    static constexpr float k_headTrackFadeSpeed = 2.0f;   // units/sec

    // ---------------------------------------------------------------------------
    // Helpers
    // ---------------------------------------------------------------------------

    // Standard smoothstep — gives an ease-in / ease-out feel to blends
    static float Smoothstep(float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    // Wrap an angle difference into (-π, π]
    static float WrapAngle(float a) {
        while (a > std::numbers::pi_v<float>) a -= 2.0f * std::numbers::pi_v<float>;
        while (a < -std::numbers::pi_v<float>) a += 2.0f * std::numbers::pi_v<float>;
        return a;
    }

    // Start a new blend from the camera's current rotation toward a new target.
    // Call this whenever we switch POI, acquire the first POI, or lose all POIs.
    static void BeginBlend(float currentRot, float targetRot) {
        s_blendFromRot = currentRot;
        s_blendTargetRot = targetRot;
        s_blendT = 0.0f;
    }

    // ---------------------------------------------------------------------------

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

            auto* actor = ref->As<RE::Actor>();
            if (!actor) return RE::BSContainer::ForEachResult::kContinue;

            POIAction action = GetActorAction(actor);
            float     score = 0.0f;

            float dist = player->GetPosition().GetDistance(ref->GetPosition());
            float proximityFactor = std::max(0.0f, (UI::g_poiDetectionRadius - dist) / UI::g_poiDetectionRadius);

            switch (action) {
            case POIAction::Moving:
                score = 400.0f + proximityFactor * 150.0f;
                break;
            case POIAction::InScene:
                score = 300.0f;
                break;
            case POIAction::Idle:
                score = 10.0f;
                break;
            default:
                break;
            }

            score += proximityFactor * 50.0f;

            if (score > bestScore) {
                bestScore = score;
                bestPOI = ref;
                bestAction = action;
            }

            return RE::BSContainer::ForEachResult::kContinue;
            };

        RE::TES::GetSingleton()->ForEachReferenceInRange(player, UI::g_poiDetectionRadius, callback);

        if (bestScore > 0.0f) {
            a_outAction = bestAction;
            a_outScore = bestScore;
            return bestPOI;
        }

        return nullptr;

    }

    // ---------------------------------------------------------------------------
    // Head-tracking: weight-driven so we can fade in/out instead of toggling
    // ---------------------------------------------------------------------------

    static void UpdatePlayerHeadtrack(RE::PlayerCharacter* a_player,
        RE::TESObjectREFR* a_target,
        float                a_weight) {

        if (!a_player) return;

        auto* currentProcess = a_player->GetActorRuntimeData().currentProcess;
        if (!currentProcess) return;

        const bool trackingActive = (a_weight > 0.01f);

        if (trackingActive && a_target) {

            // Enable BSLookAtModifier (TDM technique)
            a_player->SetGraphVariableBool("IsNPC", true);
            a_player->AsActorState()->actorState2.headTracking = true;

            // Blend between a forward-facing position and the actual POI position
            // so the head eases in rather than snapping to the target immediately
            RE::NiPoint3 playerPos = a_player->GetPosition();
            float        playerYaw = a_player->GetAngleZ();

            RE::NiPoint3 forwardPos = {
                playerPos.x + std::sin(playerYaw) * 500.0f,
                playerPos.y + std::cos(playerYaw) * 500.0f,
                playerPos.z + 120.0f
            };

            RE::NiPoint3 poiPos = a_target->GetPosition();
            poiPos.z += 120.0f;   // eye-level offset

            // Smoothstep the weight so the lerp itself has an ease curve
            float easedW = Smoothstep(a_weight);

            RE::NiPoint3 blendedPos = {
                forwardPos.x + easedW * (poiPos.x - forwardPos.x),
                forwardPos.y + easedW * (poiPos.y - forwardPos.y),
                forwardPos.z + easedW * (poiPos.z - forwardPos.z)
            };

            currentProcess->SetHeadtrackTarget(a_player, blendedPos);

        }
        else {

            // Fade out: point straight ahead so the head doesn't snap to origin
            RE::NiPoint3 playerPos = a_player->GetPosition();
            float        playerYaw = a_player->GetAngleZ();
            RE::NiPoint3 forwardPos = {
                playerPos.x + std::sin(playerYaw) * 500.0f,
                playerPos.y + std::cos(playerYaw) * 500.0f,
                playerPos.z + 120.0f
            };

            currentProcess->SetHeadtrackTarget(a_player, forwardPos);

            if (a_weight <= 0.01f) {
                // Fully released — disable modifier so idle anims can take over
                a_player->AsActorState()->actorState2.headTracking = false;
                a_player->SetGraphVariableBool("IsNPC", false);
            }

        }

    }

    // ---------------------------------------------------------------------------

    void AutoVanityStateHook::Update(RE::AutoVanityState* a_this,
        RE::BSTSmartPointer<RE::TESCameraState>& a_nextState) {

        float savedRot = a_this->autoVanityRot;
        _Update(a_this, a_nextState);

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return;

        const float dt = RE::BSTimer::GetSingleton()->realTimeDelta;

        // -----------------------------------------------------------------------
        // 1. Tick the POI lock timer
        // -----------------------------------------------------------------------

        if (s_lockTimer > 0.0f)
            s_lockTimer = std::max(0.0f, s_lockTimer - dt);

        // -----------------------------------------------------------------------
        // 2. Validate current POI (dead / out of range bypass the lock timer)
        // -----------------------------------------------------------------------

        if (s_currentPOI) {

            auto* actor = s_currentPOI->As<RE::Actor>();
            bool  gone = (!actor || actor->IsDead());

            if (!gone) {
                float dist = player->GetPosition().GetDistance(s_currentPOI->GetPosition());
                gone = (dist > UI::g_poiDetectionRadius);
            }

            if (gone) {

                logger::info("POI lost (dead or out of range) — beginning exit blend");

                // Begin a smooth exit toward vanilla drift (savedRot is as good a
                // target as any since we don't know where vanilla wants to go yet;
                // the blend will naturally hand back to vanilla as blendT → 1)
                BeginBlend(a_this->autoVanityRot, savedRot);

                s_currentPOI = nullptr;
                s_currentScore = 0.0f;
                s_lockTimer = 0.0f;

            }

        }

        // -----------------------------------------------------------------------
        // 3. Search for a new / better POI (only when lock timer has expired)
        // -----------------------------------------------------------------------

        if (s_lockTimer <= 0.0f) {

            POIAction          foundAction = POIAction::None;
            float              foundScore = 0.0f;
            RE::TESObjectREFR* candidate = FindBestPOI(foundAction, foundScore);

            if (candidate && candidate != s_currentPOI) {

                if (!s_currentPOI || foundScore > s_currentScore) {

                    logger::info("Switching POI to: {} (score {:.1f})", candidate->GetName(), foundScore);

                    // Compute the angle to the incoming POI so the blend target is
                    // already correct from frame 0 — no one-frame jump
                    auto  dir = candidate->GetPosition() - player->GetPosition();
                    float incomingRot = std::atan2(dir.x, dir.y);

                    // Start blending from wherever the camera sits right now
                    BeginBlend(a_this->autoVanityRot, incomingRot);

                    s_currentPOI = candidate;
                    s_currentScore = foundScore;
                    s_lockTimer = k_lockDuration;

                }

            }
            else if (!candidate) {

                if (s_currentPOI) {
                    logger::info("No POI in range — beginning exit blend");
                    BeginBlend(a_this->autoVanityRot, savedRot);
                }

                s_currentPOI = nullptr;
                s_currentScore = 0.0f;

            }

        }

        // -----------------------------------------------------------------------
        // 4. Drive the camera rotation
        // -----------------------------------------------------------------------

        if (s_currentPOI) {

            // Recompute the world-space angle to the POI every frame (POI may be
            // moving) and keep blendTargetRot chasing it with short-arc wrapping
            auto  dir = s_currentPOI->GetPosition() - player->GetPosition();
            float liveAngle = std::atan2(dir.x, dir.y);

            // Keep blendTargetRot continuous (no sudden ±2π jumps as the POI moves)
            s_blendTargetRot += WrapAngle(liveAngle - s_blendTargetRot);

            // Advance the blend
            if (s_blendT < 1.0f)
                s_blendT = std::min(1.0f, s_blendT + dt / k_blendDuration);

            float easedT = Smoothstep(s_blendT);

            // Interpolate from the starting rotation toward the live target angle,
            // using short-arc delta so we never spin the long way around
            float delta = WrapAngle(s_blendTargetRot - s_blendFromRot);
            float blendedAngle = s_blendFromRot + easedT * delta;

            // On top of the blend, apply gentle exponential smoothing so the
            // camera keeps tracking a moving POI smoothly after the blend settles
            float trackDelta = WrapAngle(liveAngle - blendedAngle);
            const float speed = 1.5f;
            const float alpha = 1.0f - std::exp(-speed * dt);
            a_this->autoVanityRot = blendedAngle + alpha * trackDelta;

            // Fade head-tracking in
            s_headTrackWeight = std::min(1.0f, s_headTrackWeight + dt * k_headTrackFadeSpeed);
            UpdatePlayerHeadtrack(player, s_currentPOI, s_headTrackWeight);

        }
        else {

            // No POI — play out any remaining exit blend, then hand back to vanilla

            if (s_blendT < 1.0f) {

                s_blendT = std::min(1.0f, s_blendT + dt / k_blendDuration);
                float easedT = Smoothstep(s_blendT);
                float delta = WrapAngle(s_blendTargetRot - s_blendFromRot);
                a_this->autoVanityRot = s_blendFromRot + easedT * delta;

                // Once the exit blend finishes, let vanilla take over completely
                if (s_blendT >= 1.0f)
                    a_this->autoVanityRot = savedRot;

            }
            else {

                // Fully idle — restore vanilla drift
                a_this->autoVanityRot = savedRot;

            }

            // Fade head-tracking out
            s_headTrackWeight = std::max(0.0f, s_headTrackWeight - dt * k_headTrackFadeSpeed);
            UpdatePlayerHeadtrack(player, nullptr, s_headTrackWeight);

        }

    }

} // namespace Hooks