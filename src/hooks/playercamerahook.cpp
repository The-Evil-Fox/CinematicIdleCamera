#include "hooks/PlayerCameraHook.h"
#include "menu.h"
#include <DrawDebug.hpp>

using namespace DebugAPI_IMPL;
namespace logger = SKSE::log;

namespace Hooks {

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Default POI system state
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static bool                             s_enteringVanity                = true;

    RE::TESObjectREFR*                      g_currentPOI                    = nullptr;

    static float                            s_currentScore                  = 0.0f;

    static float                            s_lockTimer                     = 0.0f;

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Debug-draw gating: Only draw a debug line when this is set to true
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static bool                             s_drawDebugForThisCall          = false;

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Cinematic transition state
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Starting rotation angle when a blend begins (captured at the moment a new POI is acquired or released).

    static float                            s_blendFromRot                  = 0.0f;

    // Destination rotation angle the camera is blending toward (updated every frame to track a moving POI,
    // and, during the exit-to-forward blend, to track the player's live forward-facing angle as well).

    static float                            s_blendTargetRot                = 0.0f;

    // Normalized blend progress [0 = just started, 1 = settled]. Fed through Smoothstep for an eased curve.

    static float                            s_blendT                        = 1.0f;

    // Fade weight [0-1] for how strongly the player's head turns toward the POI. Eased in/out via Smoothstep.

    static float                            s_headTrackWeight               = 0.0f;

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Dynamic dezoom state
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  This system only ever rotates yaw (autoVanityRot) to face a POI. There's no pitch control. A POI
    //  that's close to the player and well above them can end up outside the vanity camera's fixed pitch as
    //  a result. s_dezoomWeight [0-1] is how "zoomed out" we currently are; it's eased toward 1 while the
    //  current POI is inside the trigger radius/height (UI::g_dezoomTriggerRadius / g_dezoomTriggerHeight),
    //  and back toward 0 otherwise. See the translation hook below for how the weight is actually applied.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static float                            s_dezoomWeight                  = 0.0f;

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Helpers for the camera Rotation
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Remaps t to a smooth [0,1] curve (slow start, fast middle, slow end) for natural-feeling transitions.

    static float Smoothstep(float t) {

        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);

    }

    // Wraps an angle to [-π, +π] so rotation deltas always take the shortest arc.

    static float WrapAngle(float a) {

        while (a > std::numbers::pi_v<float>) a -= 2.0f * std::numbers::pi_v<float>;
        while (a < -std::numbers::pi_v<float>) a += 2.0f * std::numbers::pi_v<float>;
        return a;

    }

    // Starts a new camera rotation blend from currentRot to targetRot by resetting s_blendT to 0.

    static void BeginBlend(float currentRot, float targetRot) {

        s_blendFromRot = currentRot;
        s_blendTargetRot = targetRot;
        s_blendT = 0.0f;

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Line-of-sight raycasting
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  
    //  NOTE:
    // 
    //  only relevant when UI::g_debugRaycasts && s_drawDebugForThisCall:
    // 
    //    - While the camera is still blending/rotating toward the focused POI
    //      (s_blendT < 1.0f): draw the ray in BLUE.
    // 
    //    - Once the camera has fully settled on the POI (s_blendT >= 1.0f): draw
    //      the ray in GREEN as a steady "locked on" cue.
    //
    //  Hit detection: only kStatic, kTerrain, and kGround hits count as blockers.
    //  Hits against actors, character controllers, or any other layer are ignored.
    //  The ray start is offset along the direction vector to avoid self-intersection
    //  with the player's own collision capsule.
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------------------------------------------------------------
    //  Debug lines colors
    // ---------------------------------------------------------------------------

    static const RE::NiColorA kBlue{ 0.0f, 0.4f, 1.0f, 1.0f };
    static const RE::NiColorA kGreen{ 0.0f, 1.0f, 0.0f, 1.0f };

    inline bool TESRayHitStatic(RE::bhkWorld* world, RE::NiPoint3 start, RE::NiPoint3 end, const RE::NiColorA& a_drawColor, bool a_allowDraw) {

        const bool canDraw = UI::g_debugRaycasts && s_drawDebugForThisCall && a_allowDraw;

        // Offset the start point along the ray direction to skip past the
        // player's own collision capsule and avoid an immediate self-hit.
        RE::NiPoint3 dir = end - start;
        const float  len = dir.Length();

        if (len < 0.01f) {

            return false;

        }

        dir /= len;
        const float  kSelfHitBias = 30.0f;   // units (Skyrim-scale)
        RE::NiPoint3 adjustedStart = start + dir * kSelfHitBias;

        RE::bhkPickData pickData{};
        const float scale = RE::bhkWorld::GetWorldScale();
        pickData.rayInput.from = adjustedStart * scale;
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

            // Draw from the original (non-offset) start so the debug line reaches all the way from the player position visually.
            DrawDebug::draw_line<100>(start, end, 2.0f, a_drawColor);

        }

        if (!pickData.rayOutput.HasHit()) {

            return false;

        }

        // Layer filter: only treat kStatic, kTerrain, and kGround as real blockers. 
        // Hits against actors, character controllers, or any other collision layer are ignored, matching the engine's own LOS logic.
        auto* collidable = pickData.rayOutput.rootCollidable;
        if (collidable) {

            const auto layer = collidable->GetCollisionLayer();
            if (layer != RE::COL_LAYER::kStatic && layer != RE::COL_LAYER::kTerrain && layer != RE::COL_LAYER::kGround) {

                logger::debug("TESRayHitStatic: ignored hit on layer {}", static_cast<int>(layer));
                return false;

            }

        }

        // Diagnostic only.
        auto* niObj = RE::TES::GetSingleton()->Pick(pickData);

        if (niObj && !niObj->name.empty()) {

            logger::debug("TES::Pick hit node: {}", niObj->name.c_str());

        }

        return true;

    }

    static bool containsCaseInsensitive(std::string_view a_haystack, std::string_view a_needle) {

        auto it = std::search(

            a_haystack.begin(), a_haystack.end(),
            a_needle.begin(), a_needle.end(),
            [](char a, char b) { return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); }

        );

        return it != a_haystack.end();

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  To detect if the raycast is colliding with something between the player & the target
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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

        // While turning the camera: the debug ray is blue.
        // Once settled: the debug ray is replaced by a green one.
        const RE::NiColorA turningColor = kBlue;
        const RE::NiColorA settledColor = kGreen;

        // Determine if the target is a critter (flying critter or pond fish)
        // g_currentPOI is the currently focused POI from the global state
        bool isCritter = false;

        if (g_currentPOI) {

            auto* actor = g_currentPOI->As<RE::Actor>();

            if (!actor) {

                // Not an actor, check if it's a critter Activator
                auto* base = g_currentPOI->GetBaseObject();

                if (base) {

                    auto* acti = base->As<RE::TESObjectACTI>();

                    if (acti) {

                        // Check if it's a critter by EditorID or model path
                        const char* edid = acti->GetFormEditorID();
                        const char* model = acti->model.c_str();

                        if ((edid && containsCaseInsensitive(edid, "Critter")) || (model && model[0] != '\0')) {

                            // Check for critter tokens in the model path
                            static constexpr std::array<std::string_view, 10> kCritterTokens = {

                                "butterfly", "moth", "dragonfly", "firefly", "bee", "luna",
                                "fish", "pondfish", "salmon", "perch"

                            };

                            for (auto token : kCritterTokens) {

                                if (containsCaseInsensitive(model, token)) {

                                    isCritter = true;
                                    break;

                                }

                            }

                        }

                    }

                }

            }

        }

        // Player start point always uses chest height
        const float playerHeight = 110.0f;

        // POI end point uses lower height for critters, chest height for actors/other
        const float poiHeight = isCritter ? 0.0f : 110.0f;

        // From the player
        RE::NiPoint3 startMid = start + RE::NiPoint3(0, 0, playerHeight);

        // To the POI
        RE::NiPoint3 endMid = end + RE::NiPoint3(0, 0, poiHeight);

        bool hitMid = TESRayHitStatic(world, startMid, endMid, settled ? settledColor : turningColor, true);

        logger::debug("Ray results: mid {} (isCritter: {})", hitMid, isCritter);

        return hitMid;

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  SmoothCam compatibility using its API
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void SmoothCamCompat::RegisterListener() noexcept {

        // Registers a listener that fires when SmoothCam sends back its interface pointer.
        // Must be called at kPostLoad, before SmoothCam itself processes kPostPostLoad.
        const bool ok = SmoothCamAPI::RegisterInterfaceLoaderCallback(

            SKSE::GetMessagingInterface(),

            [](void* interfaceInstance, SmoothCamAPI::InterfaceVersion version) {

                if (version < SmoothCamAPI::InterfaceVersion::V3) {

                    logger::warn("SmoothCamCompat: got interface V{} but need V3 – compat inactive", static_cast<int>(version));
                    return;

                }

                s_api = static_cast<SmoothCamAPI::IVSmoothCam3*>(interfaceInstance);
                logger::info("SmoothCamCompat: IVSmoothCam3 acquired successfully");

            }

        );

        if (!ok) {

            logger::info("SmoothCamCompat: SmoothCam not present – compat layer inactive");

        }

    }

    void SmoothCamCompat::RequestInterface() noexcept {

        // Sends the actual interface request to SmoothCam.
        // SmoothCam responds by invoking the callback registered above.
        SmoothCamAPI::RequestInterface(SKSE::GetMessagingInterface(), SmoothCamAPI::InterfaceVersion::V3);

    }

    void SmoothCamCompat::Acquire() noexcept {

        if (!s_api || s_holding) return;

        const auto result = s_api->RequestCameraControl(SKSE::GetPluginHandle());

        if (result == SmoothCamAPI::APIResult::OK || result == SmoothCamAPI::APIResult::AlreadyGiven) {

            s_holding = true;
            logger::debug("SmoothCamCompat: camera control acquired");

        } else {

            logger::warn("SmoothCamCompat: RequestCameraControl returned {}", static_cast<int>(result));

        }

    }

    void SmoothCamCompat::Release() noexcept {

        if (!s_api || !s_holding) return;

        const auto result = s_api->ReleaseCameraControl(SKSE::GetPluginHandle());

        if (result == SmoothCamAPI::APIResult::OK || result == SmoothCamAPI::APIResult::NotOwner) {

            s_holding = false;
            logger::debug("SmoothCamCompat: camera control released");

        } else {

            logger::warn("SmoothCamCompat: ReleaseCameraControl returned {}", static_cast<int>(result));

        }

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Camera position offset hook
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Hooks::AutoVanityState_GetTranslationHelper::thunk(RE::AutoVanityState* a_this, std::int64_t param_2, RE::NiPoint3* param_3, std::int64_t param_4, std::uint32_t param_5) {

        func(a_this, param_2, param_3, param_4, param_5);

        // param_2 is actually float* pointing to local_70 (x), local_6c (y), local_68 (z)
        float* translation = reinterpret_cast<float*>(param_2);

        if (translation) {

            logger::debug("translation: {:.2f} {:.2f} {:.2f} before", translation[0], translation[1], translation[2]);

            // The user-facing sliders are defined in *local* camera space:
            // X = right/left, Y = forward/back, Z = up/down
            // 
            // But "translation" here is world space, so we have to rotate the local offset into world space using the camera's current facing
            // angle before adding it. Otherwise "right" only means "world +X" for the one facing angle it was tuned at.

            const float rot = a_this->autoVanityRot;
            const float s = std::sin(rot);
            const float c = std::cos(rot);

            // Forward = (sin(rot), cos(rot)), Right = (cos(rot), sin(rot))

            // ---------------------------------------------------------------------------------------------------------------
            //  Dynamic dezoom: added on top of the user's local Y (depth) offset. s_dezoomWeight is updated once per
            //  frame in AutoVanityStateHook::Update(), which runs before this thunk fires this frame (the engine's
            //  own Update() and therefore this translation call happens partway through our Update() hook), so
            //  this reads last frame's weight.
            // ---------------------------------------------------------------------------------------------------------------

            const float dezoomOffset = s_dezoomWeight * UI::g_dezoomAmount;
            const float localOffsetY = UI::g_IdleCamOffsetY - dezoomOffset;

            const float worldOffsetX = c * UI::g_IdleCamOffsetX + s * localOffsetY;
            const float worldOffsetY = -s * UI::g_IdleCamOffsetX + c * localOffsetY;

            translation[0] += worldOffsetX;
            translation[1] += worldOffsetY;
            translation[2] += UI::g_IdleCamOffsetZ; // vertical offset is rotation-independent

            logger::debug("translation: {:.2f} {:.2f} {:.2f} after (dezoomWeight {:.2f})", translation[0], translation[1], translation[2], s_dezoomWeight);

        }

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Install function of the custom translation for the vanity camera
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Hooks::AutoVanityState_GetTranslationHelper::Install() {

        SKSE::AllocTrampoline(1 << 7);
        auto& trampoline = SKSE::GetTrampoline();

        // Patch call site inside GetTranslation
        REL::Relocation<std::uintptr_t> site1{ REL::Offset(0x8DDD39) };

        // Patch call site inside Update
        REL::Relocation<std::uintptr_t> site2{ REL::Offset(0x8DDEC5) };

        func = trampoline.write_call<5>(site1.address(), thunk);
        trampoline.write_call<5>(site2.address(), thunk);

        logger::debug("site1: {:x}", site1.address());
        logger::debug("site2: {:x}", site2.address());
        logger::debug("func.get(): {:x}", reinterpret_cast<std::uintptr_t>(func.get()));

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Prevents mannequins from being targetable by the POI System
    //  (This is needed because Bethesda decided that mannequins are fucking actors when they made the game...)
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static bool IsMannequin(RE::Actor* a_actor) {

        if (!a_actor) return false;

        auto* race = a_actor->GetRace();

        if (race) {

            const char* raceEditorID = race->GetFormEditorID();

            if (raceEditorID) {

                // Check for mannequin race EditorIDs
                std::string raceEdid(raceEditorID);
                std::transform(raceEdid.begin(), raceEdid.end(), raceEdid.begin(), ::tolower);

                if (raceEdid.find("mannequin") != std::string::npos || raceEdid.find("manakin") != std::string::npos || raceEdid.find("mannequinrace") != std::string::npos) {

                    return true;

                } else {

                    return false;

                }

            }

        }

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Get the currently selected actor's action
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    POIAction AutoVanityStateHook::GetActorAction(RE::Actor* a_actor) {

        if (!a_actor) {

            return POIAction::None;

        }

        if (IsMannequin(a_actor)) {

            logger::debug("Actor {} detected as mannequin - skipping", a_actor->GetName());
            return POIAction::None;

        }

        // Check if the actor is a dragon
        auto* race = a_actor->GetRace();

        if (race) {

            // Get the race name and check if it's a dragon
            const char* raceName = race->GetName();

            if (raceName && (strstr(raceName, "Dragon") != nullptr || strstr(raceName, "DragonRace") != nullptr)) {

                // Also checks that it's not a dragon priest or other related non-dragon
                return POIAction::Dragon;

            }

        }

        if (a_actor->GetCurrentScene() != nullptr) {

            return POIAction::InScene;

        }

        if (a_actor->IsInCombat()) {

            return POIAction::InCombat;

        }

        if (a_actor->IsMoving()) {

            return POIAction::Moving;

        }

        return POIAction::Idle;

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Install Function Of The AutoVanityHook
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void AutoVanityStateHook::Install() {

        REL::Relocation<std::uintptr_t> vtbl{ RE::VTABLE_AutoVanityState[0] };
        _Update = vtbl.write_vfunc(REL::Module::IsVR() ? 4 : 3, Update);
        _EndState = vtbl.write_vfunc(2, EndState);

        logger::info("AutoVanity hook installed");

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  KillMove (kVATS) camera-state hook
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Used mainly to prevents AutoVanity from firing during the kill move camera
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void KillMoveCameraStateHook::Update(RE::TESCameraState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState) {

        logger::debug("KillMoveCameraStateHook::Update fired");

        auto* camera = RE::PlayerCamera::GetSingleton();

        if (camera) {

            camera->GetRuntimeData2().allowAutoVanityMode = false;
            logger::debug("KillMoveCameraStateHook::Update Vanity mode set to false during the killmove");

        }

        _Update(a_this, a_nextState);

    }

    void KillMoveCameraStateHook::EndState(RE::TESCameraState* a_this) {

        logger::debug("KillMoveCameraStateHook::EndState fired");

        _EndState(a_this);

        auto* camera = RE::PlayerCamera::GetSingleton();

        if (camera) {

            camera->GetRuntimeData2().allowAutoVanityMode = true;
            logger::debug("KillMoveCameraStateHook::EndState Vanity mode set to true after the end of the killmove");

        }

    }

    void KillMoveCameraStateHook::Install() {

        if (s_installed) {

            return;

        }

        auto* camera = RE::PlayerCamera::GetSingleton();

        if (!camera) {

            logger::debug("KillMoveCameraStateHook: PlayerCamera singleton not ready yet");
            return;

        }

        auto& runtimeData = camera->GetRuntimeData();
        RE::TESCameraState* state = runtimeData.cameraStates[RE::CameraState::kVATS].get();

        if (!state) {

            logger::debug("KillMoveCameraStateHook: kVATS camera state not constructed yet");
            return;

        }

        // Read the vtable pointer straight out of the live object (first 8 bytes of any
        // polymorphic C++ object on MSVC/x64). This avoids needing a compile-time
        // RE::VTABLE_XXX constant for whatever concrete class implements kVATS in this fork.

        const auto vtblAddr = *reinterpret_cast<std::uintptr_t*>(state);
        REL::Relocation<std::uintptr_t> vtbl{ vtblAddr };

        // NOTE: index guessed from AutoVanityState's own layout - confirm via the log
        // below that this actually fires when a killmove starts. Adjust 2/3/4 if not.
        _Update = vtbl.write_vfunc(REL::Module::IsVR() ? 4 : 3, Update);
        _EndState = vtbl.write_vfunc(2, EndState);

        s_installed = true;

        logger::info("KillMoveCameraStateHook installed (vtbl @ {:x})", vtblAddr);

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Check if the current POI is still valid based on enabled POI types
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static bool IsCurrentPOITypeStillEnabled(RE::TESObjectREFR* a_poi) {

        if (!a_poi) {

            return false;

        }

        auto* actor = a_poi->As<RE::Actor>();

        if (actor) {

            // It's an actor - check if actor POIs are enabled
            return UI::g_actorPoiEnabled;

        }

        // Not an actor - check if it's a critter
        auto* base = a_poi->GetBaseObject();

        if (base) {

            auto* acti = base->As<RE::TESObjectACTI>();

            if (acti) {

                const char* model = acti->model.c_str();

                if (model && model[0] != '\0') {

                    std::string modelStr = model;
                    std::transform(modelStr.begin(), modelStr.end(), modelStr.begin(), ::tolower);

                    // Check for pond fish keywords
                    if (modelStr.find("fish") != std::string::npos ||
                        modelStr.find("pondfish") != std::string::npos ||
                        modelStr.find("salmon") != std::string::npos ||
                        modelStr.find("perch") != std::string::npos) {

                        return UI::g_fishPoiEnabled;

                    }

                    // Check for flying critters keywords
                    if (modelStr.find("butterfly") != std::string::npos ||
                        modelStr.find("moth") != std::string::npos ||
                        modelStr.find("dragonfly") != std::string::npos ||
                        modelStr.find("firefly") != std::string::npos ||
                        modelStr.find("bee") != std::string::npos ||
                        modelStr.find("luna") != std::string::npos) {

                        return UI::g_flyingCritterPoiEnabled;

                    }

                }

            }

        }

        // If we can't determine the type, assume it's valid
        return true;
    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  POI function:
    //  Compares and select the boi POI to be focused by the vanity camera
    // 
    //  Logic is like this:
    //  CASE 1 = Activable critters
    //  CASE 2 = Actors
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    RE::TESObjectREFR* AutoVanityStateHook::FindBestPOI(POIAction& a_outAction, float& a_outScore) {

        auto* player = RE::PlayerCharacter::GetSingleton();

        if (!player) {

            return nullptr;

        }

        static RE::TESFaction* s_followerFaction = RE::TESForm::LookupByID<RE::TESFaction>(0x84D1B);

        RE::TESObjectREFR*                  bestPOI                         = nullptr;
        POIAction                           bestAction                      = POIAction::None;
        float                               bestScore                       = 0.0f;

        // This entire scan must never draw debug lines. It runs once per candidate actor in range, and we only want lines for the actor that's actually locked as the current POI. 
        // The flag is already on entry (set false at the end of Update() / by default), but we force it here too so FindBestPOI is safe to call from anywhere without
        // leaking debug draws from a stale flag state.

        s_drawDebugForThisCall = false;

        auto callback = [&](RE::TESObjectREFR* ref) {

            if (!ref || ref == player) {

                return RE::BSContainer::ForEachResult::kContinue;

            }

            // Skip FLORA type references (hanging fishs on display rack, ingredients, etc.)
            auto* base = ref->GetBaseObject();

            if (base && base->GetFormType() == RE::FormType::Flora || base && base->GetFormType() == RE::FormType::Furniture) {

                logger::debug("{} rejected: POIs with the FLORA or FURNITURE type are not supported", ref->GetName());
                return RE::BSContainer::ForEachResult::kContinue;

            }

            // Skips everything that is not an actor (humans, creatures, etc), unless it's a critter Activator.
            auto* actor = ref->As<RE::Actor>();

            if (!actor) {

                // -----------------------------------------------------------------------------------------------------------
                //  CASE 1: CRITTER POI PROCESSING (Activators like flying critters, pond fish, etc.)
                // -----------------------------------------------------------------------------------------------------------
                //
                //  Critters (flying bugs, pond fish, etc) are TESObjectACTI refs, not Actors,
                //  so they can't be filtered the way NPCs/creatures are, and don't get a base score from
                //  the switch below. We identify them via:
                //
                //    1. EditorID convention: vanilla + most critter-replacer mods name these "Critter<Name>"
                //       (e.g. "CritterMothBlue", "CritterDragonfly01", "CritterPondFish01").
                //
                //    2. Fallback: model path substring match against common critter tokens, in case a
                //       mod doesn't follow the EditorID convention.
                // -----------------------------------------------------------------------------------------------------------

                if (!UI::g_flyingCritterPoiEnabled && !UI::g_fishPoiEnabled) {

                    return RE::BSContainer::ForEachResult::kContinue;

                }

                auto isCritter = [&](RE::TESObjectREFR* a_ref) {

                    if (!a_ref) {

                        return false;

                    }

                    auto* base = a_ref->GetBaseObject();

                    if (!base) {

                        return false;

                    }

                    auto* acti = base->As<RE::TESObjectACTI>();

                    if (!acti) {

                        return false;

                    }

                    if (const char* edid = acti->GetFormEditorID(); edid && containsCaseInsensitive(edid, "Critter")) {

                        return true;

                    }

                    if (const char* model = acti->model.c_str(); model && model[0] != '\0') {

                        static constexpr std::array<std::string_view, 10> kCritterTokens = {

                            "butterfly", "moth", "dragonfly", "firefly", "bee", "luna",
                            "fish", "pondfish", "salmon", "perch"

                        };

                        for (auto token : kCritterTokens) {

                            if (containsCaseInsensitive(model, token)) {

                                return true;

                            }

                        }

                    }

                    return false;

                };

                if (!isCritter(ref)) {

                    return RE::BSContainer::ForEachResult::kContinue;

                }

                if (!ref->Is3DLoaded() || ref->IsDisabled()) {

                    return RE::BSContainer::ForEachResult::kContinue;

                }

                if (HasAnythingBetween(player->GetPosition(), ref->GetPosition())) {

                    logger::debug("{} rejected: the critter is occluded!", ref->GetName());
                    return RE::BSContainer::ForEachResult::kContinue;

                }

                float dist = player->GetPosition().GetDistance(ref->GetPosition());
                float proximityFactor = std::max(0.0f, (UI::g_poiDetectionRadius - dist) / UI::g_poiDetectionRadius);

                // Determine if this is a flying critter or pond fish
                float score = 0.0f;
                POIAction action = POIAction::None;

                auto* baseObj = ref->GetBaseObject();

                if (baseObj) {

                    auto* acti = baseObj->As<RE::TESObjectACTI>();

                    if (acti) {

                        const char* edid = acti->GetFormEditorID();
                        const char* model = acti->model.c_str();

                        // Skip Creation Club fishing gear
                        if (edid) {

                            std::string edidStr = edid;
                            std::transform(edidStr.begin(), edidStr.end(), edidStr.begin(), ::tolower);

                            // Check for CC content with "fishing" in the EditorID
                            if (edidStr.find("cc") != std::string::npos && edidStr.find("fishing") != std::string::npos) {

                                logger::debug("{} rejected: seems to be a CC fishing gear", ref->GetName());
                                return RE::BSContainer::ForEachResult::kContinue;

                            }

                        }

                        // Skip initially disabled references
                        if (ref->IsInitiallyDisabled()) {

                            logger::debug("{} rejected: initially disabled reference", ref->GetName());
                            return RE::BSContainer::ForEachResult::kContinue;

                        }

                        if (model && model[0] != '\0') {

                            std::string modelStr = model;
                            std::transform(modelStr.begin(), modelStr.end(), modelStr.begin(), ::tolower);

                            // Check for fishing gear models
                            if (modelStr.find("fishinggear") != std::string::npos || modelStr.find("fishingrod") != std::string::npos) {

                                logger::debug("{} rejected: seems to be a fishing gear model", ref->GetName());
                                return RE::BSContainer::ForEachResult::kContinue;

                            }

                            // Check for pond fish keywords
                            if (modelStr.find("fish") != std::string::npos ||
                                modelStr.find("pondfish") != std::string::npos ||
                                modelStr.find("salmon") != std::string::npos ||
                                modelStr.find("perch") != std::string::npos) {

                                if (!UI::g_fishPoiEnabled) {

                                    logger::debug("{} rejected: fish POIs disabled", ref->GetName());
                                    return RE::BSContainer::ForEachResult::kContinue;

                                }

                                // Skip persistent references that are NOT fish (racks, etc.)
                                // But allow persistent fish (CC pond fish are persistent)
                                if (ref->IsPersistent()) {

                                    // Check if it's actually a critter fish by looking at EditorID and allows it to be a targetable POI if that's the case
                                    if (edid && (containsCaseInsensitive(edid, "Fish") || containsCaseInsensitive(edid, "Critter"))) {

                                        logger::debug("{} seems to be a persistent fish critter. Proceeding...", ref->GetName());

                                    } else {

                                        logger::debug("{} rejected: persistent non-fish object", ref->GetName());
                                        return RE::BSContainer::ForEachResult::kContinue;

                                    }

                                }

                                // It's a pond fish
                                logger::debug("{} accepted as fish critter", ref->GetName());
                                score = UI::g_pondFishScore;

                                if (UI::g_pondFishProximityEnabled) {

                                    score += proximityFactor * UI::g_pondFishProximityFactor;

                                }

                                action = POIAction::PondFish;

                            // Otherwise check for flying critters keywords
                            }  else if (modelStr.find("butterfly") != std::string::npos ||
                                modelStr.find("moth") != std::string::npos ||
                                modelStr.find("dragonfly") != std::string::npos ||
                                modelStr.find("firefly") != std::string::npos ||
                                modelStr.find("bee") != std::string::npos ||
                                modelStr.find("luna") != std::string::npos) {

                                if (!UI::g_flyingCritterPoiEnabled) {

                                    logger::debug("{} rejected: flying critter POIs disabled", ref->GetName());
                                    return RE::BSContainer::ForEachResult::kContinue;

                                }

                                // It's a flying critter
                               logger::debug("{} accepted as flying critter", ref->GetName());
                               score = UI::g_flyingCritterScore;

                               if (UI::g_flyingCritterProximityEnabled) {

                                   score += proximityFactor * UI::g_flyingCritterProximityFactor;

                               }

                               action = POIAction::FlyingCritter;

                            }

                        }

                    }

                }

                // If for some reason we couldn't determine the type, skip this ref
                if (action == POIAction::None || score == 0.0f) {

                    return RE::BSContainer::ForEachResult::kContinue;

                }

                if (score > bestScore) {

                    bestScore = score;
                    bestPOI = ref;
                    bestAction = action;

                }

                return RE::BSContainer::ForEachResult::kContinue;

            } else {

                // -----------------------------------------------------------------------------------------------------------
                //  CASE 2: ACTOR POI PROCESSING
                // -----------------------------------------------------------------------------------------------------------

                if (!UI::g_actorPoiEnabled) {

                    return RE::BSContainer::ForEachResult::kContinue;

                }

                if (actor->IsDead() || !actor->Is3DLoaded() || actor->IsDisabled()) {

                    return RE::BSContainer::ForEachResult::kContinue;

                }

                if (UI::IsActorExcluded(actor)) {

                    logger::debug("POI {} rejected: actor is in exclusion list.", ref->GetName());
                    return RE::BSContainer::ForEachResult::kContinue;

                }

                // Skips followers from being targeted
                if (UI::g_preventFollowers && s_followerFaction && actor->IsInFaction(s_followerFaction)) {

                    logger::debug("{} rejected: actor is a follower and those are currently disabled in the POI System settings.", ref->GetName());
                    return RE::BSContainer::ForEachResult::kContinue;

                }

                // Line-of-sight check: skip POIs that are fully occluded.
                // NOTE: s_drawDebugForThisCall stays false for the whole scan, so this never draws regardless of UI::g_debugRaycasts.

                if (HasAnythingBetween(player->GetPosition(), ref->GetPosition())) {

                    logger::debug("{} rejected: the reference is occluded!", ref->GetName());
                    return RE::BSContainer::ForEachResult::kContinue;

                }

                POIAction                       action = GetActorAction(actor);
                float                           score = 0.0f;

                float                           dist = player->GetPosition().GetDistance(ref->GetPosition());
                float                           proximityFactor = std::max(0.0f, (UI::g_poiDetectionRadius - dist) / UI::g_poiDetectionRadius);

                switch (action) {

                case POIAction::Dragon:

                    score = UI::g_dragonScore;

                    if (UI::g_dragonProximityEnabled) {

                        score += proximityFactor * UI::g_dragonProximityFactor;

                    }

                    logger::debug("{} accepted as Dragon with score: {:.1f}", ref->GetName(), score);

                    break;

                case POIAction::InCombat:

                    score = UI::g_actorCombatScore;

                    if (UI::g_actorCombatProximityEnabled) {

                        score += proximityFactor * UI::g_actorCombatProximityFactor;

                    }

                    logger::debug("{} accepted as InCombat with score: {:.1f}", ref->GetName(), score);

                    break;

                case POIAction::Moving:

                    score = UI::g_actorMovingScore;

                    if (UI::g_actorMovingProximityEnabled) {

                        score += proximityFactor * UI::g_actorMovingProximityFactor;

                    }

                    logger::debug("{} accepted as Moving with score: {:.1f}", ref->GetName(), score);

                    break;

                case POIAction::InScene:

                    score = UI::g_actorInSceneScore;

                    if (UI::g_actorInSceneProximityEnabled) {

                        score += proximityFactor * UI::g_actorInSceneProximityFactor;

                    }

                    logger::debug("{} accepted as InScene with score: {:.1f}", ref->GetName(), score);

                    break;

                case POIAction::Idle:

                    score = UI::g_actorIdleScore;

                    if (UI::g_actorIdleProximityEnabled) {

                        score += proximityFactor * UI::g_actorIdleProximityFactor;

                    }

                    logger::debug("{} accepted as Idle with score: {:.1f}", ref->GetName(), score);

                    break;

                default:

                    break;

                }

                if (score > bestScore) {

                    bestScore = score;
                    bestPOI = ref;
                    bestAction = action;

                }

                return RE::BSContainer::ForEachResult::kContinue;

            }

        };

        RE::TES::GetSingleton()->ForEachReferenceInRange(player, UI::g_poiDetectionRadius, callback);

        if (bestScore > 0.0f) {

            a_outAction = bestAction;
            a_outScore = bestScore;
            return bestPOI;

        }

        return nullptr;

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Head-tracking: weight-driven so we can fade in/out instead of toggling
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static RE::NiPoint3 GetActorForwardVector(RE::Actor* a_actor) {

        auto* root = a_actor->Get3D();

        if (!root) {

            return RE::NiPoint3{ 0.0f, 1.0f, 0.0f };

        }

        return root->world.rotate.GetVectorY();

    }

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
            RE::NiPoint3 fwd = GetActorForwardVector(a_player);

            RE::NiPoint3 forwardPos = {

                playerPos.x + fwd.x * 500.0f,
                playerPos.y + fwd.y * 500.0f,
                playerPos.z + 120.0f

            };

            RE::NiPoint3 poiPos = a_target->GetPosition();

            // Determine if the target is a critter (flying critter or pond fish)
            bool isCritter = false;
            auto* actor = a_target->As<RE::Actor>();

            if (!actor) {

                auto* base = a_target->GetBaseObject();

                if (base) {

                    auto* acti = base->As<RE::TESObjectACTI>();

                    if (acti) {

                        const char* edid = acti->GetFormEditorID();
                        const char* model = acti->model.c_str();

                        if ((edid && containsCaseInsensitive(edid, "Critter")) || (model && model[0] != '\0')) {

                            static constexpr std::array<std::string_view, 10> kCritterTokens = {

                                "butterfly", "moth", "dragonfly", "firefly", "bee", "luna",
                                "fish", "pondfish", "salmon", "perch"

                            };

                            for (auto token : kCritterTokens) {

                                if (containsCaseInsensitive(model, token)) {

                                    isCritter = true;
                                    break;

                                }
                            }

                        }

                    }

                }

            }

            // Apply appropriate height offset to the target position
            // Critters (fish, insects) should be tracked at their actual position (0 offset)
            // Actors/NPCs should be tracked at chest height (110 offset)
            const float poiHeight = isCritter ? 0.0f : 110.0f;
            poiPos.z += poiHeight;

            float easedW = Smoothstep(a_weight);

            RE::NiPoint3 blendedPos = {

                forwardPos.x + easedW * (poiPos.x - forwardPos.x),
                forwardPos.y + easedW * (poiPos.y - forwardPos.y),
                forwardPos.z + easedW * (poiPos.z - forwardPos.z)

            };

            currentProcess->SetHeadtrackTarget(a_player, blendedPos);

        } else {

            RE::NiPoint3 playerPos = a_player->GetPosition();
            RE::NiPoint3 fwd = GetActorForwardVector(a_player);

            RE::NiPoint3 forwardPos = {

                playerPos.x + fwd.x * 500.0f,
                playerPos.y + fwd.y * 500.0f,
                playerPos.z + 120.0f

            };

            currentProcess->SetHeadtrackTarget(a_player, forwardPos);

            if (a_weight <= 0.01f) {

                a_player->AsActorState()->actorState2.headTracking = false;
                a_player->SetGraphVariableBool("IsNPC", false);

            }

        }

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Used to wipe all debug lines from the internal list and the screen
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    static void PurgeDebugLines() {

        auto* dbg = DebugAPI::GetSingleton();
        auto hud = DebugAPI::GetHUD();

        if (hud && hud->uiMovie) {

            dbg->ClearLines2D(hud->uiMovie);

        }

        std::unique_lock lock(dbg->mutex_);

        for (auto* line : dbg->LinesToDraw) {

            delete line;

        }

        dbg->LinesToDraw.clear();

    }

    // Reads the camera's actual current facing angle straight from its NiNode world
    // transform, independent of autoVanityRot (which vanilla's own _Update() appears
    // to reset to the player's forward angle on the first vanity frame, before we can
    // read anything useful from it).
    static float GetLiveCameraYaw() {

        auto* camera = RE::PlayerCamera::GetSingleton();

        if (!camera || !camera->cameraRoot) {

            return 0.0f;

        }

        RE::NiPoint3 fwd = camera->cameraRoot->world.rotate.GetVectorY();

        return std::atan2(fwd.x, fwd.y);

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Vanity mode begins here
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void AutoVanityStateHook::Update(RE::AutoVanityState* a_this, RE::BSTSmartPointer<RE::TESCameraState>& a_nextState) {

        auto* player = RE::PlayerCharacter::GetSingleton();

        if (!player) {

            return;

        }

        SmoothCamCompat::Acquire();

        float baseRot = player->GetAngleZ();

        // Capture the live camera facing BEFORE vanilla's _Update() runs, since that's
        // the last point where it still reflects wherever the player was actually looking
        // prior to vanity mode engaging.

        const bool isEnteringThisFrame = s_enteringVanity;

        float capturedCameraYaw = a_this->autoVanityRot;

        if (s_enteringVanity) {

            capturedCameraYaw = GetLiveCameraYaw();

        }

        _Update(a_this, a_nextState);

        if (s_enteringVanity) {

            s_enteringVanity = false;

            logger::debug("Entering vanity mode - capturedCameraYaw {:.2f}, postUpdateAutoVanityRot {:.2f}, baseRot {:.2f}", capturedCameraYaw, a_this->autoVanityRot, baseRot);

            BeginBlend(capturedCameraYaw, baseRot);

        }

        if (!UI::g_poiSystemEnabled) {

            // Reset any lingering POI state
            if (g_currentPOI) {

                g_currentPOI = nullptr;
                s_currentScore = 0.0f;
                s_lockTimer = 0.0f;
                s_headTrackWeight = 0.0f;
                s_dezoomWeight = 0.0f;
                UpdatePlayerHeadtrack(player, nullptr, 0.0f);

            }

            a_this->autoVanityRot = player->GetAngleZ();

            return;
        }

        if (UI::g_debugRaycasts) {

            DebugAPI::GetSingleton()->Update();

        } else {

            PurgeDebugLines();

        }


        const float dt = RE::BSTimer::GetSingleton()->realTimeDelta;

        // ---------------------------------------------------------------------------------------------------------------------
        //  1. Tick the POI lock timer
        // ---------------------------------------------------------------------------------------------------------------------
        //
        //  Only count down once the camera has actually finished blending onto the current
        //  POI (s_blendT >= 1.0f - the same "settled" condition that switches the debug ray
        //  from blue to green). While the camera is still rotating to catch up to a freshly
        //  acquired or moving POI, the lock shouldn't be burning down yet - otherwise a POI
        //  could lose its lock before the camera ever finished turning to look at it.
        //
        //  Note: s_blendT here is last frame's value (section 4 below is what advances it
        //  this frame), so "settled" is detected with a one-frame lag. That's harmless -
        //  it just means the timer starts counting one frame after the camera visually
        //  settles, not before.
        // ---------------------------------------------------------------------------------------------------------------------

        const bool cameraSettledOnPOI = (s_blendT >= 1.0f);

        if (s_lockTimer > 0.0f && cameraSettledOnPOI) {

            s_lockTimer = std::max(0.0f, s_lockTimer - dt);

        }

        // ---------------------------------------------------------------------------------------------------------------------
        //  2. Validate current POI
        // ---------------------------------------------------------------------------------------------------------------------

        if (g_currentPOI) {

            auto* actor = g_currentPOI->As<RE::Actor>();
            bool  gone;

            if (!IsCurrentPOITypeStillEnabled(g_currentPOI)) {

                logger::debug("Current POI {} type was disabled - dropping lock", g_currentPOI->GetName());
                gone = true;

            } else if (actor) {

                gone = actor->IsDead();

            } else {

                // Non-actor POI (flying critter): there's no "dead" state, so validate
                // 3D/enabled status instead.
                gone = (!g_currentPOI->Is3DLoaded() || g_currentPOI->IsDisabled());

            }

            if (!gone) {

                float dist = player->GetPosition().GetDistance(g_currentPOI->GetPosition());
                gone = (dist > UI::g_poiDetectionRadius);

            }

            // ---------------------------------------------------------------------------------------------------------------------
            //  If the POI is still in range and alive, also check whether it has become occluded since we locked onto it. 
            //  If so, drop it so the system can immediately search for a visible alternative. 
            //  This is the ONLY raycast call allowed to draw debug lines: 
            //  it is checking the actor that is actually the current focus.
            // ---------------------------------------------------------------------------------------------------------------------

            s_drawDebugForThisCall = true;
            bool occluded = HasAnythingBetween(player->GetPosition(), g_currentPOI->GetPosition());
            s_drawDebugForThisCall = false;

            if (!gone && occluded) {

                logger::debug("Current POI {} became occluded - dropping lock", g_currentPOI->GetName());
                gone = true;

            }

            if (gone) {

                logger::debug("POI lost (dead, out of range, or occluded) - beginning exit blend");

                BeginBlend(a_this->autoVanityRot, baseRot);

                g_currentPOI = nullptr;
                s_currentScore = 0.0f;
                s_lockTimer = 0.0f;

            }

        }

        // ---------------------------------------------------------------------------------------------------------------------
        //  3. Search for a new / better POI
        // ---------------------------------------------------------------------------------------------------------------------
        //
        //  NOTE: 
        // 
        //  FindBestPOI internally forces s_drawDebugForThisCall = false for its whole scan,
        //  so none of the candidate-checking raycasts draw here
        // ---------------------------------------------------------------------------------------------------------------------

        if (s_lockTimer <= 0.0f) {

            POIAction                               foundAction             = POIAction::None;
            float                                   foundScore              = 0.0f;
            RE::TESObjectREFR*                      candidate               = FindBestPOI(foundAction, foundScore);

            if (candidate && candidate != g_currentPOI) {

                if (!g_currentPOI || foundScore > s_currentScore) {

                    logger::debug("Switching POI to: {} (score {:.1f})", candidate->GetName(), foundScore);

                    auto  dir = candidate->GetPosition() - player->GetPosition();
                    float incomingRot = std::atan2(dir.x, dir.y);

                    float blendFromAngle = isEnteringThisFrame ? capturedCameraYaw : a_this->autoVanityRot;

                    BeginBlend(blendFromAngle, incomingRot);

                    g_currentPOI = candidate;
                    s_currentScore = foundScore;
                    s_lockTimer = UI::g_lockDuration;

                }

            } else if (candidate && candidate == g_currentPOI) {

                // Same actor is still the best candidate - re-arm the lock timer so we
                // don't re-scan for a POI every single frame while nothing has changed.
                s_currentScore = foundScore;
                s_lockTimer = UI::g_lockDuration;

            } else if (!candidate) {

                if (g_currentPOI) {

                    logger::debug("No visible POI in range - beginning exit blend");
                    BeginBlend(a_this->autoVanityRot, baseRot);

                }

                g_currentPOI = nullptr;
                s_currentScore = 0.0f;

            }

        }

        // ---------------------------------------------------------------------------------------------------------------------
        //  4. Drive the camera rotation
        // ---------------------------------------------------------------------------------------------------------------------

        if (g_currentPOI) {

            auto  dir = g_currentPOI->GetPosition() - player->GetPosition();
            float liveAngle = std::atan2(dir.x, dir.y);

            // -----------------------------------------------------------------------------------------------------------------
            //  4b. Dynamic dezoom
            // -----------------------------------------------------------------------------------------------------------------
            //
            //  If the POI is both close to the player (horizontally) and well above them, ease s_dezoomWeight toward 1
            //  so the translation hook pulls the camera back and widens the effective vertical field of view onto it.
            //  Otherwise ease it back toward 0. Using horizDist/heightDiff off the same `dir` vector we already have
            //  for the rotation above, rather than re-querying positions.
            // -----------------------------------------------------------------------------------------------------------------

            const float horizDist = std::sqrt(dir.x * dir.x + dir.y * dir.y);
            const float heightDiff = dir.z;

            const bool  wantsDezoom = (horizDist < UI::g_dezoomTriggerRadius) && (heightDiff > UI::g_dezoomTriggerHeight);
            const float targetDezoom = wantsDezoom ? 1.0f : 0.0f;

            if (s_dezoomWeight < targetDezoom) {

                s_dezoomWeight = std::min(targetDezoom, s_dezoomWeight + dt * UI::g_dezoomBlendSpeed);

            } else if (s_dezoomWeight > targetDezoom) {

                s_dezoomWeight = std::max(targetDezoom, s_dezoomWeight - dt * UI::g_dezoomBlendSpeed);

            }

            logger::debug("Dezoom check: horizDist {:.1f}, heightDiff {:.1f}, wantsDezoom {}, weight {:.2f}", horizDist, heightDiff, wantsDezoom, s_dezoomWeight);

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
            UpdatePlayerHeadtrack(player, g_currentPOI, s_headTrackWeight);

        } else {

            // No current POI - ease the dezoom back out too, same rate as everywhere else.

            if (s_dezoomWeight > 0.0f) {

                s_dezoomWeight = std::max(0.0f, s_dezoomWeight - dt * UI::g_dezoomBlendSpeed);

            }

            if (s_blendT < 1.0f) {

                // -----------------------------------------------------------------------------------
                //  Keep retargeting to the LIVE forward angle every frame (just
                //  like the POI-tracking branch above retargets to liveAngle),
                //  instead of blending toward a stale snapshot taken back when
                //  BeginBlend() first fired. Without this, a player who keeps
                //  turning the camera while the exit blend is in progress would
                //  blend toward where "forward" used to be, then pop to the
                //  correct value the instant s_blendT reaches 1.0f.
                // -----------------------------------------------------------------------------------

                s_blendTargetRot = baseRot;

                s_blendT = std::min(1.0f, s_blendT + dt / std::max(0.01f, UI::g_blendDuration));
                float easedT = Smoothstep(s_blendT);
                float delta = WrapAngle(s_blendTargetRot - s_blendFromRot);
                a_this->autoVanityRot = s_blendFromRot + easedT * delta;

            } else {

                a_this->autoVanityRot = baseRot;

            }

            s_headTrackWeight = std::max(0.0f, s_headTrackWeight - dt * UI::g_headTrackFadeSpeed);
            UpdatePlayerHeadtrack(player, nullptr, s_headTrackWeight);

        }

    }

    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  Vanity mode ends here, the code below executes once player leaves it
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void AutoVanityStateHook::EndState(RE::AutoVanityState* a_this) {

        logger::debug("AutoVanity EndState hook fired");
        _EndState(a_this);

        // -----------------------------------------------------------------------------------
        //  Wipe all debug lines when vanity mode ends
        // -----------------------------------------------------------------------------------

        PurgeDebugLines();

        // -----------------------------------------------------------------------------------
        //  Reset all POI/blend state so the next vanity session starts clean.
        // -----------------------------------------------------------------------------------

        s_enteringVanity = true;
        g_currentPOI = nullptr;
        s_currentScore = 0.0f;
        s_lockTimer = 0.0f;

        s_blendFromRot = 0.0f;
        s_blendTargetRot = 0.0f;
        s_blendT = 1.0f;

        s_headTrackWeight = 0.0f;
        s_dezoomWeight = 0.0f;

        // -----------------------------------------------------------------------------------
        //  Give back the controls of the camera to SmoothCam after exiting Vanity Mode
        // -----------------------------------------------------------------------------------

        SmoothCamCompat::Release();

    }

}