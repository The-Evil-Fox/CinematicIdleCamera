#include "PlayerCameraHook.h"
#include "menu.h"
#include <DrawDebug.hpp>

using namespace DebugAPI_IMPL;
namespace logger = SKSE::log;

namespace Hooks {

    // ---------------------------------------------------------------------------
    // POI state
    // ---------------------------------------------------------------------------

    static RE::TESObjectREFR* s_currentPOI = nullptr;
    static float                s_currentScore = 0.0f;
    static float                s_lockTimer = 0.0f;

    // ---------------------------------------------------------------------------
    // Cinematic transition state
    // ---------------------------------------------------------------------------

    static float s_blendFromRot = 0.0f;
    static float s_blendTargetRot = 0.0f;
    static float s_blendT = 1.0f;
    static float s_headTrackWeight = 0.0f;

    // ---------------------------------------------------------------------------
    // Debug-draw gating: Only draw a debug line when this is set to true
    // ---------------------------------------------------------------------------

    static bool s_drawDebugForThisCall = false;

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
    // Debug lines colors
    // ---------------------------------------------------------------------------

    static const RE::NiColorA kBlue{ 0.0f, 0.4f, 1.0f, 1.0f };
    static const RE::NiColorA kGreen{ 0.0f, 1.0f, 0.0f, 1.0f };

    // ---------------------------------------------------------------------------
    // Line-of-sight raycasting
    // ---------------------------------------------------------------------------
    //
    // Visual contract (only relevant when UI::g_debugRaycasts && s_drawDebugForThisCall):
    //   - While the camera is still blending/rotating toward the focused POI
    //     (s_blendT < 1.0f): draw all three staggered rays (low/mid/high) in BLUE.
    //   - Once the camera has fully settled on the POI (s_blendT >= 1.0f): draw
    //     only the single mid-height ray, in GREEN, as a steady "locked on" cue.
    //
    // TESRayHitStatic no longer colors by hit/miss - color is now purely a
    // function of blend state, decided by the caller (HasAnythingBetween).
    //
    // Hit detection: ANY raycast collision counts as a hit now - we no longer
    // filter by collision layer (static/terrain/ground) or by node name
    // (wall/floor/intcor/etc, excluding shelf). The caller (HasAnythingBetween)
    // is responsible for deciding how many hit rays are enough to reject a POI.
    // ---------------------------------------------------------------------------

    inline bool TESRayHitStatic(RE::bhkWorld* world, RE::NiPoint3 start, RE::NiPoint3 end, const RE::NiColorA& a_drawColor, bool a_allowDraw) {

        const bool canDraw = UI::g_debugRaycasts && s_drawDebugForThisCall && a_allowDraw;

        RE::bhkPickData pickData{};
        const float scale = RE::bhkWorld::GetWorldScale();
        pickData.rayInput.from = start * scale;
        pickData.rayInput.to = end * scale;
        pickData.rayInput.enableShapeCollectionFilter = true;

        RE::CFilter filter{};
        filter.SetCollisionLayer(RE::COL_LAYER::kLOS);
        static const std::uint32_t sSystemGroup = RE::bhkCollisionFilter::GetSingleton()->GetNewSystemGroup();
        filter.SetSystemGroup(sSystemGroup);
        pickData.rayInput.filterInfo = filter;

        {
            RE::BSReadLockGuard lock(world->worldLock);
            world->PickObject(pickData);
        }

        if (canDraw) {

            DrawDebug::draw_line<100>(start, end, 2.0f, a_drawColor);

        }

        if (!pickData.rayOutput.HasHit()) {

            return false;

        }

        // Diagnostic only - no longer used to decide hit/miss. Any collision
        // at all is treated as a hit now, regardless of what it is.
        if (auto* niObj = RE::TES::GetSingleton()->Pick(pickData); niObj && !niObj->name.empty()) {

            logger::debug("TES::Pick hit node: {}", niObj->name.c_str());

        }

        return true;

    }

    inline bool HasAnythingBetween(RE::NiPoint3 start, RE::NiPoint3 end) {

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {

            return false;

        }

        auto* cell = player->GetParentCell();
        if (!cell) {

            return false;

        }

        auto* world = cell->GetbhkWorld();
        if (!world) {

            return false;

        }

        // Settled = camera has finished blending onto the current focus target.
        const bool settled = (s_blendT >= 1.0f);

        // While turning: all three rays draw, in blue.
        // Once settled: only the mid ray draws, in green.
        const RE::NiColorA turningColor = kBlue;
        const RE::NiColorA settledColor = kGreen;

        // -----------------------------------------------------------------------
        // Ray heights: anchored independently to each point's own body, not
        // leveled to a shared plane. GetPosition() returns feet height, so
        // "low/mid/high" are feet + an offset that approximates ankle/chest/
        // head height for that specific point.
        //
        // This matters for floor-to-floor occlusion in interiors: if we level
        // both ends of a ray at the same world Z (e.g. by interpolating
        // between the player's feet Z and the target's feet Z), the ray
        // travels mostly horizontally between two different floors and can
        // skate straight through a stairwell gap, vent, or railing opening
        // without ever actually crossing the floor slab that's between them.
        // By anchoring start to the player's own body heights and end to the
        // target's own body heights, a real difference in floor level now
        // produces a genuinely diagonal ray (player's chest down/up to the
        // target's chest, for example), which is what actually clips the
        // floor/ceiling slab separating the two.
        // -----------------------------------------------------------------------

        const float ankleHeight = 35.0f;
        const float chestHeight = 70.0f;
        const float headHeight = 105.0f;

        RE::NiPoint3 startLow = start + RE::NiPoint3(0, 0, ankleHeight);
        RE::NiPoint3 startMid = start + RE::NiPoint3(0, 0, chestHeight);
        RE::NiPoint3 startHigh = start + RE::NiPoint3(0, 0, headHeight);

        RE::NiPoint3 endLow = end + RE::NiPoint3(0, 0, ankleHeight);
        RE::NiPoint3 endMid = end + RE::NiPoint3(0, 0, chestHeight);
        RE::NiPoint3 endHigh = end + RE::NiPoint3(0, 0, headHeight);

        bool hitLow = TESRayHitStatic(world, startLow, endLow, turningColor, /*allowDraw*/ !settled);

        bool hitMid = TESRayHitStatic(world, startMid, endMid, settled ? settledColor : turningColor, /*allowDraw*/ true);

        bool hitHigh = TESRayHitStatic(world, startHigh, endHigh, turningColor, /*allowDraw*/ !settled);

        logger::debug("Ray results: low {} mid {} high {}", hitLow, hitMid, hitHigh);

        // Require at least two of the three rays to hit something before we
        // treat the POI as blocked. A single ray hit (e.g. clipping a thin
        // prop) is not enough to reject an otherwise visible POI.
        int hitCount = (hitLow ? 1 : 0) + (hitMid ? 1 : 0) + (hitHigh ? 1 : 0);

        // -----------------------------------------------------------------------
        // Floor/ceiling guard: even if fewer than two of the staggered rays
        // hit something, a large Z difference between the player and the
        // candidate strongly suggests they're on different floors (e.g. one
        // below the other through a gap), which our raycasts can still miss
        // depending on the exact geometry. If the vertical gap is larger than
        // a normal standing height difference, treat the POI as occluded
        // regardless of ray results.
        // -----------------------------------------------------------------------

        const float maxReasonableZDiff = 150.0f;
        if (std::abs(end.z - start.z) > maxReasonableZDiff) {

            logger::debug("POI rejected: vertical gap {:.1f} exceeds same-floor threshold", end.z - start.z);
            return true;

        }

        return hitCount >= 2;

    }

    // ---------------------------------------------------------------------------

    POIAction AutoVanityStateHook::GetActorAction(RE::Actor* a_actor) {

        if (!a_actor) {

            return POIAction::None;

        }

        if (a_actor->GetCurrentScene() != nullptr) {

            return POIAction::InScene;

        }

        if (a_actor->IsMoving()) {

            return POIAction::Moving;

        }

        return POIAction::Idle;

    }

    // ---------------------------------------------------------------------------

    void AutoVanityStateHook::Install() {

        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_AutoVanityState[0] };
        _Update = vtbl.write_vfunc(REL::Module::IsVR() ? 4 : 3, Update);
        _EndState = vtbl.write_vfunc(2, EndState);

        logger::info("Hook installed");

    }

    // ---------------------------------------------------------------------------

    RE::TESObjectREFR* AutoVanityStateHook::FindBestPOI(POIAction& a_outAction, float& a_outScore) {

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {

            return nullptr;

        }

        RE::TESObjectREFR* bestPOI = nullptr;
        POIAction          bestAction = POIAction::None;
        float              bestScore = 0.0f;

        // This entire scan must never draw debug lines - it runs once per
        // candidate actor in range, and we only want lines for the actor
        // that's actually locked as the current POI. The flag is already
        // false on entry (set false at the end of Update() / by default),
        // but we force it here too so FindBestPOI is safe to call from
        // anywhere without leaking debug draws from a stale flag state.
        s_drawDebugForThisCall = false;

        auto callback = [&](RE::TESObjectREFR* ref) {

            if (!ref || ref == player) {

                return RE::BSContainer::ForEachResult::kContinue;

            }

            auto* actor = ref->As<RE::Actor>();
            if (!actor) {

                return RE::BSContainer::ForEachResult::kContinue;

            }

            // --- Line-of-sight check: skip POIs that are fully occluded --------
            // We cast three staggered rays (low / mid / high). Only if at least
            // two of the three are blocked by something do we treat the POI as
            // invisible. A partially-occluded actor (zero or one ray blocked)
            // still counts.
            // NOTE: s_drawDebugForThisCall stays false for the whole scan, so
            // this never draws regardless of UI::g_debugRaycasts.
            if (HasAnythingBetween(player->GetPosition(), ref->GetPosition())) {

                logger::debug("POI {} rejected: occluded by at least two of three rays", ref->GetName());
                return RE::BSContainer::ForEachResult::kContinue;

            }

            POIAction action = GetActorAction(actor);
            float     score = 0.0f;

            float dist = player->GetPosition().GetDistance(ref->GetPosition());
            float proximityFactor = std::max(0.0f, (UI::g_poiDetectionRadius - dist) / UI::g_poiDetectionRadius);

            switch (action) {

            case POIAction::Moving:  score = 400.0f + proximityFactor * 150.0f; break;

            case POIAction::InScene: score = 300.0f; break;

            case POIAction::Idle:    score = 10.0f;  break;

            default: break;

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

    static void UpdatePlayerHeadtrack(RE::PlayerCharacter* a_player, RE::TESObjectREFR* a_target, float a_weight) {

        if (!a_player) {

            return;

        }

        auto* currentProcess = a_player->GetActorRuntimeData().currentProcess;
        if (!currentProcess) {

            return;

        }

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
    // Wipe all debug lines from the internal list and the screen
    // ---------------------------------------------------------------------------

    static void PurgeDebugLines() {

        auto* dbg = DebugAPI::GetSingleton();

        auto hud = DebugAPI::GetHUD();
        if (hud && hud->uiMovie)
            dbg->ClearLines2D(hud->uiMovie);

        std::unique_lock lock(dbg->mutex_);
        for (auto* line : dbg->LinesToDraw)
            delete line;
        dbg->LinesToDraw.clear();

    }

    // ---------------------------------------------------------------------------
    // Vanity mode begins here
    // ---------------------------------------------------------------------------

    void AutoVanityStateHook::Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState) {

        float savedRot = a_this->autoVanityRot;
        _Update(a_this, a_nextState);

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player) {

            return;

        }

        if (UI::g_debugRaycasts) {

            DebugAPI::GetSingleton()->Update();

        }
        else {

            PurgeDebugLines();

        }


        const float dt = RE::BSTimer::GetSingleton()->realTimeDelta;

        // -----------------------------------------------------------------------
        // 1. Tick the POI lock timer
        // -----------------------------------------------------------------------

        if (s_lockTimer > 0.0f) {

            s_lockTimer = std::max(0.0f, s_lockTimer - dt);
        }

        // -----------------------------------------------------------------------
        // 2. Validate current POI
        // -----------------------------------------------------------------------

        if (s_currentPOI) {

            auto* actor = s_currentPOI->As<RE::Actor>();
            bool  gone = (!actor || actor->IsDead());

            if (!gone) {

                float dist = player->GetPosition().GetDistance(s_currentPOI->GetPosition());
                gone = (dist > UI::g_poiDetectionRadius);

            }

            // If the POI is still in range and alive, also check whether it has
            // become occluded (at least two of three rays blocked) since we
            // locked onto it. If so, drop it so the system can immediately
            // search for a visible alternative.
            // This is the ONLY raycast call allowed to draw debug lines: it is
            // checking the actor that is actually the current focus.

            s_drawDebugForThisCall = true;
            bool occluded = HasAnythingBetween(player->GetPosition(), s_currentPOI->GetPosition());
            s_drawDebugForThisCall = false;

            if (!gone && occluded) {

                logger::info("Current POI {} became occluded - dropping lock", s_currentPOI->GetName());
                gone = true;

            }

            if (gone) {

                logger::info("POI lost (dead, out of range, or occluded) - beginning exit blend");

                BeginBlend(a_this->autoVanityRot, savedRot);

                s_currentPOI = nullptr;
                s_currentScore = 0.0f;
                s_lockTimer = 0.0f;

            }

        }

        // -----------------------------------------------------------------------
        // 3. Search for a new / better POI
        // -----------------------------------------------------------------------
        // NOTE: FindBestPOI internally forces s_drawDebugForThisCall = false for
        // its whole scan, so none of the candidate-checking raycasts draw here,
        // regardless of UI::g_debugRaycasts.

        if (s_lockTimer <= 0.0f) {

            POIAction           foundAction = POIAction::None;
            float               foundScore = 0.0f;
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

                    logger::info("No visible POI in range - beginning exit blend");
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

            if (s_blendT < 1.0f) {

                s_blendT = std::min(1.0f, s_blendT + dt / std::max(0.01f, UI::g_blendDuration));

            }

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

                if (s_blendT >= 1.0f) {

                    a_this->autoVanityRot = savedRot;

                }

            }
            else {

                a_this->autoVanityRot = savedRot;

            }

            s_headTrackWeight = std::max(0.0f, s_headTrackWeight - dt * UI::g_headTrackFadeSpeed);
            UpdatePlayerHeadtrack(player, nullptr, s_headTrackWeight);

        }

    }

    // ---------------------------------------------------------------------------
    // Vanity mode ends here, the code below executes once player leaves it
    // ---------------------------------------------------------------------------

    void AutoVanityStateHook::EndState(RE::AutoVanityState* a_this) {

        logger::info("EndState hook fired");
        _EndState(a_this);

        // Wipe all debug lines when vanity mode ends
        PurgeDebugLines();

        // -----------------------------------------------------------------------
        // Reset all POI/blend state so the next vanity session starts clean.
        // Without this, s_currentPOI and the blend variables (s_blendFromRot,
        // s_blendTargetRot, s_blendT) keep whatever value they had when this
        // session ended. The next time the player goes AFK - possibly in a
        // different spot, facing a different direction, with no relevant POI
        // nearby at all - Update() can still see a stale s_currentPOI that
        // happens to still be alive and in range, or can compute a blend from
        // a stale s_blendFromRot/s_blendT pair that has nothing to do with the
        // camera's actual current rotation. Either case can produce a sudden,
        // unexplained rotation on entering vanity mode with no POI actually
        // locked (and therefore no debug lines), which is exactly this bug.
        // -----------------------------------------------------------------------

        s_currentPOI = nullptr;
        s_currentScore = 0.0f;
        s_lockTimer = 0.0f;

        s_blendFromRot = 0.0f;
        s_blendTargetRot = 0.0f;
        s_blendT = 1.0f;

        s_headTrackWeight = 0.0f;

    }

}