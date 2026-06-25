#include "PlayerCameraHook.h"
#include "menu.h"

namespace logger = SKSE::log;

namespace Hooks {

    // ---------------------------------------------------------------------------
    // POI state
    // ---------------------------------------------------------------------------

    static RE::TESObjectREFR* s_currentPOI = nullptr;
    static float                s_currentScore = 0.0f;

    static float           s_lockTimer = 0.0f;

    // ---------------------------------------------------------------------------
    // Cinematic transition state
    // ---------------------------------------------------------------------------

    static float s_blendFromRot = 0.0f;
    static float s_blendTargetRot = 0.0f;
    static float s_blendT = 1.0f;

    static float s_headTrackWeight = 0.0f;

    // ---------------------------------------------------------------------------
    // Helpers
    // ---------------------------------------------------------------------------

    static float Smoothstep(float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }

    static float WrapAngle(float a) {
        while (a > std::numbers::pi_v<float>) a -= 2.0f * std::numbers::pi_v<float>;
        while (a < -std::numbers::pi_v<float>) a += 2.0f * std::numbers::pi_v<float>;
        return a;
    }

    static void BeginBlend(float currentRot, float targetRot) {
        s_blendFromRot = currentRot;
        s_blendTargetRot = targetRot;
        s_blendT = 0.0f;
    }

    // ---------------------------------------------------------------------------
    // Line-of-sight raycasting
    // ---------------------------------------------------------------------------

    inline bool TESRayHitStatic(RE::bhkWorld* world, RE::NiPoint3 start, RE::NiPoint3 end) {

        RE::bhkPickData pickData{};
        const float scale = RE::bhkWorld::GetWorldScale();
        pickData.rayInput.from = start * scale;
        pickData.rayInput.to = end * scale;
        pickData.rayInput.enableShapeCollectionFilter = true;

        RE::CFilter filter{};
        filter.SetCollisionLayer(RE::COL_LAYER::kLOS);
        static const std::uint32_t sSystemGroup =
            RE::bhkCollisionFilter::GetSingleton()->GetNewSystemGroup();
        filter.SetSystemGroup(sSystemGroup);
        pickData.rayInput.filterInfo = filter;

        {
            RE::BSReadLockGuard lock(world->worldLock);
            world->PickObject(pickData);
        }

        if (!pickData.rayOutput.HasHit())
            return false;

        auto* collidable = pickData.rayOutput.rootCollidable;
        if (!collidable)
            return false;

        auto layer = collidable->GetCollisionLayer();
        if (layer != RE::COL_LAYER::kStatic &&
            layer != RE::COL_LAYER::kTerrain &&
            layer != RE::COL_LAYER::kGround)
            return false;

        auto* niObj = RE::TES::GetSingleton()->Pick(pickData);
        if (!niObj || niObj->name.empty())
            return false;

        logger::debug("TES::Pick hit node: {}", niObj->name.c_str());

        return (niObj->name.contains("wall") ||
            niObj->name.contains("floor") ||
            niObj->name.contains("intcor") ||
            niObj->name.contains("farmintinnend")) &&
            !niObj->name.contains("shelf");

    }

    inline bool HasAnythingBetween(RE::NiPoint3 start, RE::NiPoint3 end) {

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) return false;

        auto* cell = player->GetParentCell();
        if (!cell) return false;

        auto* world = cell->GetbhkWorld();
        if (!world) return false;

        bool hitLow = TESRayHitStatic(world, start + RE::NiPoint3(0, 0, 35.0f), end + RE::NiPoint3(0, 0, 35.0f));
        bool hitMid = TESRayHitStatic(world, start + RE::NiPoint3(0, 0, 70.0f), end + RE::NiPoint3(0, 0, 70.0f));
        bool hitHigh = TESRayHitStatic(world, start + RE::NiPoint3(0, 0, 105.0f), end + RE::NiPoint3(0, 0, 105.0f));

        logger::debug("Ray results: low {} mid {} high {}", hitLow, hitMid, hitHigh);

        // All three rays must be blocked before we consider the POI occluded.
        // A single blocked ray (e.g. a low beam clipping a step) is not enough.
        return hitLow || hitMid || hitHigh;

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

            // --- Line-of-sight check: skip POIs that are fully occluded --------
            // We cast three staggered rays (low / mid / high). Only if all three
            // are blocked by static geometry do we treat the POI as invisible.
            // A partially-occluded actor (one or two rays blocked) still counts.
            if (HasAnythingBetween(player->GetPosition(), ref->GetPosition())) {
                logger::debug("POI {} rejected: fully occluded by static geometry", ref->GetName());
                return RE::BSContainer::ForEachResult::kContinue;
            }

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

            a_player->SetGraphVariableBool("IsNPC", true);
            a_player->AsActorState()->actorState2.headTracking = true;

            RE::NiPoint3 playerPos = a_player->GetPosition();
            float        playerYaw = a_player->GetAngleZ();

            RE::NiPoint3 forwardPos = {
                playerPos.x + std::sin(playerYaw) * 500.0f,
                playerPos.y + std::cos(playerYaw) * 500.0f,
                playerPos.z + 120.0f
            };

            RE::NiPoint3 poiPos = a_target->GetPosition();
            poiPos.z += 120.0f;

            float easedW = Smoothstep(a_weight);

            RE::NiPoint3 blendedPos = {
                forwardPos.x + easedW * (poiPos.x - forwardPos.x),
                forwardPos.y + easedW * (poiPos.y - forwardPos.y),
                forwardPos.z + easedW * (poiPos.z - forwardPos.z)
            };

            currentProcess->SetHeadtrackTarget(a_player, blendedPos);

        }
        else {

            RE::NiPoint3 playerPos = a_player->GetPosition();
            float        playerYaw = a_player->GetAngleZ();
            RE::NiPoint3 forwardPos = {
                playerPos.x + std::sin(playerYaw) * 500.0f,
                playerPos.y + std::cos(playerYaw) * 500.0f,
                playerPos.z + 120.0f
            };

            currentProcess->SetHeadtrackTarget(a_player, forwardPos);

            if (a_weight <= 0.01f) {
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
        // 2. Validate current POI (dead / out of range / now occluded bypass lock)
        // -----------------------------------------------------------------------

        if (s_currentPOI) {

            auto* actor = s_currentPOI->As<RE::Actor>();
            bool  gone = (!actor || actor->IsDead());

            if (!gone) {
                float dist = player->GetPosition().GetDistance(s_currentPOI->GetPosition());
                gone = (dist > UI::g_poiDetectionRadius);
            }

            // If the POI is still in range and alive, also check whether it has
            // become fully occluded since we locked onto it. If so, drop it so
            // the system can immediately search for a visible alternative.
            if (!gone && HasAnythingBetween(player->GetPosition(), s_currentPOI->GetPosition())) {
                logger::info("Current POI {} became fully occluded — dropping lock", s_currentPOI->GetName());
                gone = true;
            }

            if (gone) {

                logger::info("POI lost (dead, out of range, or occluded) — beginning exit blend");

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

                    auto  dir = candidate->GetPosition() - player->GetPosition();
                    float incomingRot = std::atan2(dir.x, dir.y);

                    BeginBlend(a_this->autoVanityRot, incomingRot);

                    s_currentPOI = candidate;
                    s_currentScore = foundScore;
                    s_lockTimer = UI::g_lockDuration;

                }

            }
            else if (!candidate) {

                if (s_currentPOI) {
                    logger::info("No visible POI in range — beginning exit blend");
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

            auto  dir = s_currentPOI->GetPosition() - player->GetPosition();
            float liveAngle = std::atan2(dir.x, dir.y);

            s_blendTargetRot += WrapAngle(liveAngle - s_blendTargetRot);

            if (s_blendT < 1.0f)
                s_blendT = std::min(1.0f, s_blendT + dt / std::max(0.01f, UI::g_blendDuration));

            float easedT = Smoothstep(s_blendT);

            float delta = WrapAngle(s_blendTargetRot - s_blendFromRot);
            float blendedAngle = s_blendFromRot + easedT * delta;

            float trackDelta = WrapAngle(liveAngle - blendedAngle);
            const float speed = 1.5f;
            const float alpha = 1.0f - std::exp(-speed * dt);
            a_this->autoVanityRot = blendedAngle + alpha * trackDelta;

            s_headTrackWeight = std::min(1.0f, s_headTrackWeight + dt * UI::g_headTrackFadeSpeed);
            UpdatePlayerHeadtrack(player, s_currentPOI, s_headTrackWeight);

        }
        else {

            if (s_blendT < 1.0f) {

                s_blendT = std::min(1.0f, s_blendT + dt / std::max(0.01f, UI::g_blendDuration));
                float easedT = Smoothstep(s_blendT);
                float delta = WrapAngle(s_blendTargetRot - s_blendFromRot);
                a_this->autoVanityRot = s_blendFromRot + easedT * delta;

                if (s_blendT >= 1.0f)
                    a_this->autoVanityRot = savedRot;

            }
            else {

                a_this->autoVanityRot = savedRot;

            }

            s_headTrackWeight = std::max(0.0f, s_headTrackWeight - dt * UI::g_headTrackFadeSpeed);
            UpdatePlayerHeadtrack(player, nullptr, s_headTrackWeight);

        }

    }

}