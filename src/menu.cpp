#include "menu.h"
#include <format>
#include "hooks/playercamerahook.h"
#include "hooks/menumonitor.h"
#include "utility.h"
#include "template/UITemplate.h"
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace logger = SKSE::log;
using namespace UITemplate;
using namespace UITemplate::Icons;

// ==================================================================================================================================================================================
//  Default settings used when the ini doesn't exist when the game is started (ordered by menus)
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Camera
// =====================================================================================================================

// Main Settings

static constexpr float                  k_defaultIdleTimer                              = 30.0f;
static constexpr bool                   k_defaultPreventVanityInCombat                  = true;
static constexpr bool                   k_defaultBlackBarsEnabled                       = true;
static constexpr float                  k_defaultBlackBarsSpeed                         = 1.0f;
static constexpr bool                   k_defaultBlackBarsSoundEnabled                  = true;

// Position/Transition

static constexpr float                  k_defaultVanityCamOffsetX                       = 75.0f;
static constexpr float                  k_defaultVanityCamOffsetY                       = 130.0f;
static constexpr float                  k_defaultVanityCamOffsetZ                       = 0.0f;
static constexpr bool                   k_defaultVanityCameraCollisionEnabled           = false;
static constexpr float                  k_defaultBlendDuration                          = 5.0f;

// Zoom/Dezoom

static constexpr float                  k_defaultDezoomTriggerRadius                    = 350.0f;
static constexpr float                  k_defaultDezoomTriggerHeight                    = 210.0f;
static constexpr float                  k_defaultDezoomAmount                           = 250.0f;
static constexpr float                  k_defaultDezoomBlendSpeed                       = 0.7f;

// =====================================================================================================================
//  Head Tracking
// =====================================================================================================================

static constexpr float                  k_defaultHeadTrackFadeSpeed                     = 0.3f;

// =====================================================================================================================
//  POI System
// =====================================================================================================================

// Main settings

static constexpr bool                   k_defaultPoiSystemEnabled                       = true;
static constexpr bool                   k_defaultActorPoiEnabled                        = true;
static constexpr bool                   k_defaultPreventFollowers                       = true;
static constexpr bool                   k_defaultInsectsAndFishPoisEnabled              = true;
static constexpr float                  k_defaultPoiDetectionRadius                     = 1050.0f;
static constexpr float                  k_defaultLockDuration                           = 5.0f;

// Actor per-action enable/disable (all considered by default, same as before)

static constexpr bool                   k_defaultDragonEnabled                          = true;
static constexpr bool                   k_defaultActorCombatEnabled                     = true;
static constexpr bool                   k_defaultActorMovingEnabled                     = true;
static constexpr bool                   k_defaultActorInSceneEnabled                    = true;
static constexpr bool                   k_defaultActorIdleEnabled                       = true;

// Insects & Fish per-type enable/disable

static constexpr bool                   k_defaultInsectsEnabled                         = true;
static constexpr bool                   k_defaultFishEnabled                            = true;

// Actors score system

static constexpr float                  k_defaultDragonScore                            = 2000.0f;
static constexpr bool                   k_defaultDragonProximityEnabled                 = true;
static constexpr float                  k_defaultDragonProximityFactor                  = 1000.0f;

static constexpr float                  k_defaultActorCombatScore                       = 600.0f;
static constexpr bool                   k_defaultActorCombatProximityEnabled            = true;
static constexpr float                  k_defaultActorCombatProximityFactor             = 200.0f;

static constexpr float                  k_defaultActorMovingScore                       = 400.0f;
static constexpr bool                   k_defaultActorMovingProximityEnabled            = true;
static constexpr float                  k_defaultActorMovingProximityFactor             = 150.0f;

static constexpr float                  k_defaultActorInSceneScore                      = 300.0f;
static constexpr bool                   k_defaultActorInSceneProximityEnabled           = true;
static constexpr float                  k_defaultActorInSceneProximityFactor            = 50.0f;

static constexpr float                  k_defaultActorIdleScore                         = 10.0f;
static constexpr bool                   k_defaultActorIdleProximityEnabled              = true;
static constexpr float                  k_defaultActorIdleProximityFactor               = 50.0f;

// Insects score system

static constexpr float                  k_defaultInsectsScore                           = 400.0f;
static constexpr bool                   k_defaultInsectsProximityEnabled                = true;
static constexpr float                  k_defaultInsectsProximityFactor                 = 150.0f;

// Fish score system

static constexpr float                  k_defaultFishScore                              = 300.0f;
static constexpr bool                   k_defaultFishProximityEnabled                   = true;
static constexpr float                  k_defaultFishProximityFactor                    = 150.0f;

// =====================================================================================================================
//  Debug
// =====================================================================================================================

static constexpr bool                   k_defaultDebugRaycasts                          = false;
static constexpr int                    k_defaultLoggingLevel                           = 2; // 0 = critical, 1 = warn, 2 = info, 3 = debug

// ==================================================================================================================================================================================
//  Initalize the sliders in the ui with the default values (used only when the INI doesn't exist or a param in it was completely removed)
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Camera
// =====================================================================================================================

float                                   UI::g_idleTimer                                 = k_defaultIdleTimer;
bool                                    UI::g_preventVanityInCombat                     = k_defaultPreventVanityInCombat;
bool                                    UI::g_blackBarsEnabled                          = k_defaultBlackBarsEnabled;
float                                   UI::g_blackBarsSpeed                            = k_defaultBlackBarsSpeed;
bool                                    UI::g_blackBarsSoundEnabled                     = k_defaultBlackBarsSoundEnabled;

float                                   UI::g_IdleCamOffsetX                            = k_defaultVanityCamOffsetX;
float                                   UI::g_IdleCamOffsetY                            = k_defaultVanityCamOffsetY;
float                                   UI::g_IdleCamOffsetZ                            = k_defaultVanityCamOffsetZ;
bool                                    UI::g_vanityCameraCollisionEnabled              = k_defaultVanityCameraCollisionEnabled;
float                                   UI::g_blendDuration                             = k_defaultBlendDuration;

float                                   UI::g_dezoomTriggerRadius                       = k_defaultDezoomTriggerRadius;
float                                   UI::g_dezoomTriggerHeight                       = k_defaultDezoomTriggerHeight;
float                                   UI::g_dezoomAmount                              = k_defaultDezoomAmount;
float                                   UI::g_dezoomBlendSpeed                          = k_defaultDezoomBlendSpeed;

float                                   UI::g_headTrackFadeSpeed                        = k_defaultHeadTrackFadeSpeed;

bool                                    UI::g_poiSystemEnabled                          = k_defaultPoiSystemEnabled;
bool                                    UI::g_actorPoisEnabled                           = k_defaultActorPoiEnabled;
bool                                    UI::g_preventFollowers                          = k_defaultPreventFollowers;
bool                                    UI::g_insectsAndFishPoisEnabled                 = k_defaultInsectsAndFishPoisEnabled;
float                                   UI::g_poiDetectionRadius                        = k_defaultPoiDetectionRadius;
float                                   UI::g_lockDuration                              = k_defaultLockDuration;

bool                                    UI::g_dragonEnabled                             = k_defaultDragonEnabled;
bool                                    UI::g_actorCombatEnabled                        = k_defaultActorCombatEnabled;
bool                                    UI::g_actorMovingEnabled                        = k_defaultActorMovingEnabled;
bool                                    UI::g_actorInSceneEnabled                       = k_defaultActorInSceneEnabled;
bool                                    UI::g_actorIdleEnabled                          = k_defaultActorIdleEnabled;

bool                                    UI::g_insectsEnabled                            = k_defaultInsectsEnabled;
bool                                    UI::g_fishEnabled                               = k_defaultFishEnabled;

float                                   UI::g_dragonScore                               = k_defaultDragonScore;
bool                                    UI::g_dragonProximityEnabled                    = k_defaultDragonProximityEnabled;
float                                   UI::g_dragonProximityFactor                     = k_defaultDragonProximityFactor;

float                                   UI::g_actorCombatScore                          = k_defaultActorCombatScore;
bool                                    UI::g_actorCombatProximityEnabled               = k_defaultActorCombatProximityEnabled;
float                                   UI::g_actorCombatProximityFactor                = k_defaultActorCombatProximityFactor;

float                                   UI::g_actorMovingScore                          = k_defaultActorMovingScore;
bool                                    UI::g_actorMovingProximityEnabled               = k_defaultActorMovingProximityEnabled;
float                                   UI::g_actorMovingProximityFactor                = k_defaultActorMovingProximityFactor;

float                                   UI::g_actorInSceneScore                         = k_defaultActorInSceneScore;
bool                                    UI::g_actorInSceneProximityEnabled              = k_defaultActorInSceneProximityEnabled;
float                                   UI::g_actorInSceneProximityFactor               = k_defaultActorInSceneProximityFactor;

float                                   UI::g_actorIdleScore                            = k_defaultActorIdleScore;
bool                                    UI::g_actorIdleProximityEnabled                 = k_defaultActorIdleProximityEnabled;
float                                   UI::g_actorIdleProximityFactor                  = k_defaultActorIdleProximityFactor;

float                                   UI::g_insectsScore                              = k_defaultInsectsScore;
bool                                    UI::g_insectsProximityEnabled                   = k_defaultInsectsProximityEnabled;
float                                   UI::g_insectsProximityFactor                    = k_defaultInsectsProximityFactor;

float                                   UI::g_fishScore                                 = k_defaultFishScore;
bool                                    UI::g_fishProximityEnabled                      = k_defaultFishProximityEnabled;
float                                   UI::g_fishProximityFactor                       = k_defaultFishProximityFactor;

std::vector<UI::ActorExclusionEntry>    UI::g_actorExclusionList;

namespace Hooks {

    extern RE::TESObjectREFR* g_currentPOI;

}

bool                                    UI::g_debugRaycasts                             = k_defaultDebugRaycasts;
int                                     UI::g_loggingLevel                              = k_defaultLoggingLevel;

// ==================================================================================================================================================================================
//  Mod-specific exclusion-list reset target (list->clear() + IniParser::Save()).
//  Satisfies the HasChanged()/Reset() interface expected by
//  UITemplate::DrawHeaderWithListReset().
// ==================================================================================================================================================================================

struct ExclusionListReset {

    std::vector<UI::ActorExclusionEntry>* list;

    ExclusionListReset(std::vector<UI::ActorExclusionEntry>* l) : list(l) {}

    bool HasChanged() const {

        return !list->empty();

    }

    void Reset() const {

        list->clear();
        IniParser::Save();
        logger::info("POI Exclusion List cleared.");

    }

};

// ==================================================================================================================================================================================
//  POI score cards data (base score + proximity bonus), fed into UITemplate::DrawScoreCard
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Actor Score Cards Data
// =====================================================================================================================

static const std::vector<ScoreCardData> actorCards = {

    ScoreCardData::Make(dragonIcon.c_str(), "Dragons", Colors::Green,
        &UI::g_dragonScore, k_defaultDragonScore, &UI::g_dragonProximityEnabled, k_defaultDragonProximityEnabled,
        &UI::g_dragonProximityFactor, k_defaultDragonProximityFactor,
        "Base score awarded to a dragon.", "Bonus increases as dragon gets closer.", SaveSettings,
        &UI::g_dragonEnabled, k_defaultDragonEnabled, nullptr,
        &UI::g_actorPoisEnabled, "Actors"),

    ScoreCardData::Make(gavelIcon.c_str(), "Actors in combat", Colors::Green,
        &UI::g_actorCombatScore, k_defaultActorCombatScore, &UI::g_actorCombatProximityEnabled, k_defaultActorCombatProximityEnabled,
        &UI::g_actorCombatProximityFactor, k_defaultActorCombatProximityFactor,
        "Base score awarded to an actor who is currently in combat.", "Bonus increases as actor gets closer.", SaveSettings,
        &UI::g_actorCombatEnabled, k_defaultActorCombatEnabled, nullptr,
        &UI::g_actorPoisEnabled, "Actors"),

    ScoreCardData::Make(personWalkingIcon.c_str(), "Actors moving", Colors::Green,
        &UI::g_actorMovingScore, k_defaultActorMovingScore, &UI::g_actorMovingProximityEnabled, k_defaultActorMovingProximityEnabled,
        &UI::g_actorMovingProximityFactor, k_defaultActorMovingProximityFactor,
        "Base score awarded to an actor who is currently moving.", "Bonus increases as actor gets closer.", SaveSettings,
        &UI::g_actorMovingEnabled, k_defaultActorMovingEnabled, nullptr,
        &UI::g_actorPoisEnabled, "Actors"),

    ScoreCardData::Make(masksTheaterIcon.c_str(), "Actors in Scene", Colors::Green,
        &UI::g_actorInSceneScore, k_defaultActorInSceneScore, &UI::g_actorInSceneProximityEnabled, k_defaultActorInSceneProximityEnabled,
        &UI::g_actorInSceneProximityFactor, k_defaultActorInSceneProximityFactor,
        "Base score awarded to an actor who is actively engaged in a scripted sequence (dialogue, cinematic, or quest scene).",
        "Bonus increases as actor gets closer.", SaveSettings,
        &UI::g_actorInSceneEnabled, k_defaultActorInSceneEnabled, nullptr,
        &UI::g_actorPoisEnabled, "Actors"),

    ScoreCardData::Make(personIcon.c_str(), "Actors idling", Colors::Green,
        &UI::g_actorIdleScore, k_defaultActorIdleScore, &UI::g_actorIdleProximityEnabled, k_defaultActorIdleProximityEnabled,
        &UI::g_actorIdleProximityFactor, k_defaultActorIdleProximityFactor,
        "Base score awarded to an actor who is in an idle animation (not moving, not in combat and not in a scene).",
        "Bonus increases as actor gets closer.", SaveSettings,
        &UI::g_actorIdleEnabled, k_defaultActorIdleEnabled, nullptr,
        &UI::g_actorPoisEnabled, "Actors")

};

// =====================================================================================================================
//  Insects & fish Score Cards Data
// =====================================================================================================================

static const std::vector<ScoreCardData> insectsAndFishCards = {

    ScoreCardData::Make(bugIcon.c_str(), "Insects", Colors::Cyan,
        &UI::g_insectsScore, k_defaultInsectsScore, &UI::g_insectsProximityEnabled, k_defaultInsectsProximityEnabled,
        &UI::g_insectsProximityFactor, k_defaultInsectsProximityFactor,
        "Base score awarded to an insect (butterflies, moths, dragonflies, fireflies, bees, etc).",
        "Bonus increases as insect gets closer.", SaveSettings,
        &UI::g_insectsEnabled, k_defaultInsectsEnabled, nullptr,
        &UI::g_insectsAndFishPoisEnabled, "Insects & fish"),

    ScoreCardData::Make(fishFinsIcon.c_str(), "Fish", Colors::Cyan,
        &UI::g_fishScore, k_defaultFishScore, &UI::g_fishProximityEnabled, k_defaultFishProximityEnabled,
        &UI::g_fishProximityFactor, k_defaultFishProximityFactor,
        "Base score awarded to a fish (perches, salmon, pond fish, etc).",
        "Bonus increases as fish gets closer.", SaveSettings,
        &UI::g_fishEnabled, k_defaultFishEnabled, nullptr,
        &UI::g_insectsAndFishPoisEnabled, "Insects & fish")

};

// =====================================================================================================================
//  Draw all actor score cards
// =====================================================================================================================

static void DrawActorScoreCards() {

    for (const auto& card : actorCards) {

        DrawScoreCard(card);

    }

}

// =====================================================================================================================
//  Draw all insects & fish score cards
// =====================================================================================================================

static void DrawInsectsAndFishScoreCards() {

    for (const auto& card : insectsAndFishCards) {

        DrawScoreCard(card);

    }

}

// ==================================================================================================================================================================================
//  Exclusion list specific functions
// ==================================================================================================================================================================================

// =====================================================================================================================
//  Helper: Resolves a possibly-dynamic (temporary, FormID starting with 0xFF) actor base down to a stable FormID by 
//  walking root template -> faceNPC. Returns the original FormID unchanged if it's already stable, or if no stable
//  ID is found.
// =====================================================================================================================

static RE::FormID ResolveCanonicalActorFormID(RE::TESNPC* a_actorBase, bool a_verbose = false) {

    if (!a_actorBase) {

        return 0;

    }

    RE::FormID formID = a_actorBase->GetFormID();

    if ((formID & 0xFF000000) != 0xFF000000) {

        return formID;

    }

    static RE::FormID s_lastLoggedFormID = 0;
    const bool shouldLog = a_verbose && (formID != s_lastLoggedFormID);

    if (shouldLog) {

        s_lastLoggedFormID = formID;
        logger::debug("Actor's base Form ID (0x{:08X}) is temporary/dynamic. Trying to find a stable ID to store", formID);

    }

    auto* rootTemplate = a_actorBase->GetRootFaceNPC();

    if (rootTemplate && rootTemplate != a_actorBase) {

        RE::FormID rootID = rootTemplate->GetFormID();

        if ((rootID & 0xFF000000) != 0xFF000000) {

            if (shouldLog) {

                logger::debug("Found a stable ID via the actor's root template: 0x{:08X}. Using this instead", rootID);

            }

            return rootID;

        }

        if (shouldLog) {

            logger::debug("Root template Form ID (0x{:08X}) is also temporary. Trying faceNPC next", rootID);

        }

    }

    auto* templateNPC = a_actorBase->faceNPC;

    if (templateNPC) {

        RE::FormID faceID = templateNPC->GetFormID();

        if ((faceID & 0xFF000000) != 0xFF000000) {

            if (shouldLog) {

                logger::debug("Found a stable ID via faceNPC: 0x{:08X}. Using this instead", faceID);

            }

            return faceID;

        }

    }

    if (shouldLog) {

        logger::debug("Could not find any stable Form ID for this actor (root template and faceNPC both failed). Caller will need a fallback");

    }

    return formID; // still dynamic: caller decides on a fallback (e.g. reference ID)

}

// =====================================================================================================================
//  Helper: Get actor name from Form ID at runtime (language-independent)
// =====================================================================================================================

static std::string GetActorNameFromFormID(RE::FormID a_formID) {

    auto* form = RE::TESForm::LookupByID(a_formID);

    if (!form) {

        logger::debug("GetActorNameFromFormID: Form 0x{:08X} not found", a_formID);
        return "Unknown";

    }

    // First, search for a loaded reference with this Base ID
    auto* tes = RE::TES::GetSingleton();

    if (tes) {

        std::string foundName;
        int refCount = 0;

        logger::debug("GetActorNameFromFormID: Searching for references with Base ID 0x{:08X}", a_formID);

        tes->ForEachReference([&](RE::TESObjectREFR* a_ref) -> RE::BSContainer::ForEachResult {

            if (!a_ref) return RE::BSContainer::ForEachResult::kContinue;

            refCount++;

            auto* refBase = a_ref->GetBaseObject();

            if (refBase && refBase->GetFormID() == a_formID) {

                const char* refName = a_ref->GetName();
                logger::debug("GetActorNameFromFormID: Found reference with matching Base ID, Name: '{}'", refName ? refName : "null");

                if (refName && refName[0] != '\0') {

                    foundName = refName;
                    return RE::BSContainer::ForEachResult::kStop;

                }

            }

            return RE::BSContainer::ForEachResult::kContinue;

        });

        logger::debug("GetActorNameFromFormID: Checked {} references, found name: '{}'", refCount, foundName.empty() ? "none" : foundName);

        if (!foundName.empty()) {

            return foundName;

        }

    }

    // If no reference found, try as NPC (base form)
    auto* npc = form->As<RE::TESNPC>();

    if (npc) {

        const char* name = npc->GetName();
        logger::debug("GetActorNameFromFormID: Base Form name: '{}'", name ? name : "null");

        if (name && name[0] != '\0') {

            return std::string(name);

        }

    }

    // Fallback to EditorID
    const char* editorID = form->GetFormEditorID();

    if (editorID && editorID[0] != '\0') {

        logger::debug("GetActorNameFromFormID: EditorID: '{}'", editorID);
        return std::string(editorID);

    }

    return "Unknown";

}

// =====================================================================================================================
//  Add the console selected actor to the exclusion list
// =====================================================================================================================

void UI::AddActorToExclusionList(RE::Actor* a_actor) {

    if (!a_actor) {

        logger::warn("AddActorToExclusionList: a_actor is null!");
        return;

    }

    auto* actorBase = a_actor->GetActorBase();

    if (!actorBase) {

        logger::warn("AddActorToExclusionList: actorBase is null!");
        return;

    }

    RE::FormID finalFormID = ResolveCanonicalActorFormID(actorBase, true);

    // If still dynamic, use the Reference ID as last resort
    if ((finalFormID & 0xFF000000) == 0xFF000000) {

        finalFormID = a_actor->GetFormID();
        logger::warn("Using Reference ID as last resort: 0x{:08X}", finalFormID);

    }

    const char* actorName = a_actor->GetName();
    logger::info("=== Adding actor to exclusion list ===");
    logger::info("Actor name: {}", actorName ? actorName : "Unnamed");
    logger::info("Final Form ID: 0x{:08X}", finalFormID);

    // Check if already in list
    for (auto& entry : g_actorExclusionList) {

        if (entry.formID == finalFormID) {

            logger::info("Actor already in exclusion list: {}", actorName ? actorName : "Unnamed");
            return;

        }

    }

    // Only store the Form ID (no name)
    g_actorExclusionList.push_back({ finalFormID });

    logger::info("Added actor (Form ID: 0x{:08X}) to exclusion list", finalFormID);
    IniParser::Save();

    // If this actor is currently the focused POI, clear it
    if (Hooks::g_currentPOI == a_actor) {

        Hooks::g_currentPOI = nullptr;
        logger::debug("Cleared current targeted POI because it was added to exclusion list");

    }

}

// =====================================================================================================================
//  Remove an actor from the exclusion list when pressing the button on its left in the exclusion list
// =====================================================================================================================

void UI::RemoveFromActorExclusionList(size_t index) {

    if (index < g_actorExclusionList.size()) {

        // Get name at runtime for logging
        std::string name = GetActorNameFromFormID(g_actorExclusionList[index].formID);
        logger::info("Removed {} from exclusion list", name);
        g_actorExclusionList.erase(g_actorExclusionList.begin() + index);
        IniParser::Save();

    }

}

// =====================================================================================================================
//  Checks if an actor is excluded. 
//  Also used inside playercamerahook.cpp to check if a potential poi is excluded.
// =====================================================================================================================

bool UI::IsActorExcluded(RE::Actor* a_actor) {

    if (!a_actor) return false;

    auto* actorBase = a_actor->GetActorBase();

    if (!actorBase) return false;

    RE::FormID formID = ResolveCanonicalActorFormID(actorBase, true);

    for (auto& entry : g_actorExclusionList) {

        if (entry.formID == formID) {

            return true;

        }

    }

    return false;

}

// ==================================================================================================================================================================================
//  Register all the different setting sections in the SKSE menu
// ==================================================================================================================================================================================

void UI::Register() {

    if (!SKSEMenuFramework::IsInstalled()) {

        return;

    }

    SKSEMenuFramework::SetSection("Cinematic Idle Camera");

    SKSEMenuFramework::AddSectionItem(std::string("Camera/Main Settings"), CameraMainSettings);
    SKSEMenuFramework::AddSectionItem(std::string("Camera/Position"), CameraPositionSettings);
    SKSEMenuFramework::AddSectionItem(std::string("Camera/Zoom & Dezoom"), CameraZoomSettings);

    SKSEMenuFramework::AddSectionItem(std::string("Head Tracking"), HeadTrackingSettings);

    SKSEMenuFramework::AddSectionItem(std::string("POI System/Main Settings"), POISystemMainSettings);
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Exclusion List"), POISystemExclusionListSettings);
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Actor Scores"), POISystemActorScores);
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Insects & Fish Scores"), POISystemInsectsAndFishScores);

    SKSEMenuFramework::AddSectionItem(std::string("Debug"), DebugSettings);

    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

// ==================================================================================================================================================================================
//  Sound effect player
// ==================================================================================================================================================================================

static void playSoundEffect(const std::string& a_filePath) {

    std::string fullPath = "Data\\" + a_filePath;
    std::replace(fullPath.begin(), fullPath.end(), '/', '\\');
    PlaySoundA(fullPath.c_str(), NULL, SND_ASYNC | SND_FILENAME);

}

// ==================================================================================================================================================================================
//  Black bars used to make the vanity mode more cinematic
// ==================================================================================================================================================================================

void UI::DrawCinematicBars() {

    auto* playerCamera = RE::PlayerCamera::GetSingleton();

    Hooks::RefreshAllowAutoVanityMode();

    if (!playerCamera || !playerCamera->currentState || !g_blackBarsEnabled) {

        return;

    }

    const bool inVanity = playerCamera->currentState->id == RE::CameraState::kAutoVanity;

    // Track previous vanity state
    static bool s_wasInVanity = false;
    static float s_progress = 0.0f;

    auto* io = ImGuiMCP::GetIO();
    const float dt = io->DeltaTime;

    if (inVanity) {

        s_progress = std::min(1.0f, s_progress + g_blackBarsSpeed * dt);

    } else {

        s_progress = std::max(0.0f, s_progress - g_blackBarsSpeed * dt);

    }

    // Only plays the sound effects when sound effects are enabled
    if (g_blackBarsSoundEnabled) {

        if (inVanity != s_wasInVanity) {

            if (inVanity) {

                logger::debug("Cinematic black bars drawing -> Playing entering idle mode sound effect");

                playSoundEffect("SKSE\\Plugins\\cinematicidlecamera\\FX\\entervanitymode.wav");

            } else {

                logger::debug("Cinematic black bars removing -> Playing exiting idle mode sound effect");

                playSoundEffect("SKSE\\Plugins\\cinematicidlecamera\\FX\\exitvanitymode.wav");

            }

        }

    }

    s_wasInVanity = inVanity;

    // Early return if bars are fully hidden
    if (s_progress <= 0.0f) {

        return;

    }

    auto* drawList = ImGuiMCP::GetForegroundDrawList();
    float screenW = io->DisplaySize.x;
    float screenH = io->DisplaySize.y;

    const float barHeight = screenH * 0.08f;

    // Smoothstep easing
    const float t = s_progress * s_progress * (3.0f - 2.0f * s_progress);

    const float topBarBottom = barHeight * t;
    const float botBarTop = screenH - barHeight * t;

    const ImGuiMCP::ImU32 barColor = ImGuiMCP::ColorConvertFloat4ToU32(HexToImVec4RGBA(Colors::Black));

    // Top bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, topBarBottom - barHeight }, ImGuiMCP::ImVec2{ screenW, topBarBottom }, barColor, 0.0f, 0);

    // Bottom bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, botBarTop }, ImGuiMCP::ImVec2{ screenW, botBarTop + barHeight }, barColor, 0.0f, 0);

}

// ==================================================================================================================================================================================
//  Camera Settings - Main Settings
// ==================================================================================================================================================================================

void UI::CameraMainSettings() {

    auto cardStyle = DrawHeaderWithReset("Camera Settings - Main Settings", videoIcon, "Camera Main Settings", "resetCameraMain",
        MakeResetLogger("Camera Main Settings"),
        SettingWithDefault(&g_idleTimer, k_defaultIdleTimer, []() {
            GameSettings::ApplyFloat("fAutoVanityModeDelay:Camera", g_idleTimer);
        }),
        SettingWithDefault(&g_preventVanityInCombat, k_defaultPreventVanityInCombat),
        SettingWithDefault(&g_blackBarsEnabled, k_defaultBlackBarsEnabled),
        SettingWithDefault(&g_blackBarsSpeed, k_defaultBlackBarsSpeed),
        SettingWithDefault(&g_blackBarsSoundEnabled, k_defaultBlackBarsSoundEnabled)
    );

    CardContent idleTimerCard = CardContent::Slider(clockIcon.c_str(), "Idle Timer",
        "How many seconds of player inactivity before the camera switches into idle mode.",
        &g_idleTimer, 1.0f, 300.0f, "%.0f sec", k_defaultIdleTimer, SaveSettings,
        []() {
            GameSettings::ApplyFloat("fAutoVanityModeDelay:Camera", g_idleTimer);
            logger::info("Camera Idle Timer Setting manually set to {} second(s)", g_idleTimer);
        });

    DrawSettingCard("idleTimerCard", idleTimerCard);

    CardContent preventCombatCard = CardContent::Checkbox(shieldHalvedIcon.c_str(), "Block Idle Camera In Combat",
        "When enabled, the idle camera will not engage while the player is in combat.",
        &g_preventVanityInCombat, k_defaultPreventVanityInCombat, SaveSettings);

    DrawSettingCard("preventCombatCard", preventCombatCard);

    CardContent blackBarsCard = CardContent::Checkbox(filmIcon.c_str(), "Cinematic Black Bars",
        "Enable or disable the cinematic black bars that appear when entering idle mode.",
        &g_blackBarsEnabled, k_defaultBlackBarsEnabled, SaveSettings);

    DrawSettingCard("blackBarsCard", blackBarsCard);

    if (g_blackBarsEnabled) {

        CardContent speedCard = CardContent::Slider(gaugeHighIcon.c_str(), "Black Bars Slide Speed",
            "How fast the cinematic bars slide in and out when entering/exiting idle mode.",
            &g_blackBarsSpeed, 0.1f, 5.0f, "%.1f", k_defaultBlackBarsSpeed, SaveSettings);

        DrawSettingCard("speedCard", speedCard);

        CardContent soundCard = CardContent::Checkbox(volumeHighIcon.c_str(), "Black Bars Sound Effects",
            "Enable or disable the sound effects that play when the cinematic black bars appear or disappear.",
            &g_blackBarsSoundEnabled, k_defaultBlackBarsSoundEnabled, SaveSettings);

        DrawSettingCard("soundCard", soundCard);

    }

}

// ==================================================================================================================================================================================
//  Camera Settings - Position
// ==================================================================================================================================================================================

void UI::CameraPositionSettings() {

    auto cardStyle = DrawHeaderWithReset("Camera Settings - Position", videoIcon, "Camera Position", "resetCameraPosition",
        MakeResetLogger("Camera Position"),
        SettingWithDefault(&g_IdleCamOffsetX, k_defaultVanityCamOffsetX),
        SettingWithDefault(&g_IdleCamOffsetY, k_defaultVanityCamOffsetY),
        SettingWithDefault(&g_IdleCamOffsetZ, k_defaultVanityCamOffsetZ),
        SettingWithDefault(&g_blendDuration, k_defaultBlendDuration),
        SettingWithDefault(&g_vanityCameraCollisionEnabled, k_defaultVanityCameraCollisionEnabled)
    );

    CardContent offsetXCard = CardContent::Slider(arrowLeftAndRightIcon.c_str(), "Idle Camera Offset X",
        "Horizontal (X-axis) offset of the idle camera relative to the player.",
        &g_IdleCamOffsetX, -500.0f, 500.0f, "%.0f", k_defaultVanityCamOffsetX, SaveSettings,
        []() { g_IdleCamOffsetX = std::round(g_IdleCamOffsetX); });

    DrawSettingCard("offsetXCard", offsetXCard);

    CardContent zoomCard = CardContent::Slider(magnifyingGlassIcon.c_str(), "Idle Camera Zoom",
        "Zoom (Y-axis) offset of the idle camera relative to the player.",
        &g_IdleCamOffsetY, -500.0f, 500.0f, "%.0f", k_defaultVanityCamOffsetY, SaveSettings,
        []() { g_IdleCamOffsetY = std::round(g_IdleCamOffsetY); });

    DrawSettingCard("zoomCard", zoomCard);

    CardContent offsetZCard = CardContent::Slider(arrowUpAndDownIcon.c_str(), "Idle Camera Offset Z",
        "Vertical (Z-axis) offset of the idle camera relative to the player.",
        &g_IdleCamOffsetZ, -500.0f, 500.0f, "%.0f", k_defaultVanityCamOffsetZ, SaveSettings,
        []() { g_IdleCamOffsetZ = std::round(g_IdleCamOffsetZ); });

    DrawSettingCard("offsetZCard", offsetZCard);

    CardContent collisionCard = CardContent::Checkbox(cubeIcon.c_str(), "Vanity Camera World Collision",
        "When enabled, the vanity camera will collide with world geometry (walls, terrain, objects) instead of clipping through it.\n",
        &g_vanityCameraCollisionEnabled, k_defaultVanityCameraCollisionEnabled, SaveSettings);

    DrawSettingCard("collisionCard", collisionCard);

    CardContent blendCard = CardContent::Slider(clockIcon.c_str(), "Blend Duration",
        "How long the camera takes to rotate to focus on a POI.",
        &g_blendDuration, 0.1f, 5.0f, "%.2f sec", k_defaultBlendDuration, SaveSettings);

    DrawSettingCard("blendCard", blendCard);

}

// ==================================================================================================================================================================================
//  Camera Settings - Zoom/Dezoom
// ==================================================================================================================================================================================

void UI::CameraZoomSettings() {

    auto cardStyle = DrawHeaderWithReset("Camera Settings - Zoom/Dezoom", videoIcon, "Zoom/Dezoom", "resetCameraZoom",
        MakeResetLogger("Zoom/Dezoom"),
        SettingWithDefault(&g_dezoomTriggerRadius, k_defaultDezoomTriggerRadius),
        SettingWithDefault(&g_dezoomTriggerHeight, k_defaultDezoomTriggerHeight),
        SettingWithDefault(&g_dezoomAmount, k_defaultDezoomAmount),
        SettingWithDefault(&g_dezoomBlendSpeed, k_defaultDezoomBlendSpeed)
    );

    CardContent radiusCard = CardContent::Slider(circleDotIcon.c_str(), "Dezoom Trigger Radius",
        "Horizontal distance from the player within which an overhead POI can trigger the dezoom.",
        &g_dezoomTriggerRadius, 0.0f, 1400.0f, "%.1f m", k_defaultDezoomTriggerRadius, SaveSettings,
        nullptr, true, UITemplate::SKYRIM_UNITS_TO_METERS);

    DrawSettingCard("radiusCard", radiusCard);

    CardContent heightCard = CardContent::Slider(arrowUpIcon.c_str(), "Dezoom Trigger Height",
        "How far above the player a POI must be, while inside the trigger radius, before the dezoom kicks in.",
        &g_dezoomTriggerHeight, 0.0f, 1400.0f, "%.1f m", k_defaultDezoomTriggerHeight, SaveSettings,
        nullptr, true, UITemplate::SKYRIM_UNITS_TO_METERS);

    DrawSettingCard("heightCard", heightCard);

    CardContent amountCard = CardContent::Slider(magnifyingGlassIcon.c_str(), "Dezoom Amount",
        "How far the camera pulls back once the dezoom is fully active (added on top of the Idle camera offset Y).",
        &g_dezoomAmount, 0.0f, 500.0f, "%.0f", k_defaultDezoomAmount, SaveSettings,
        []() { g_dezoomAmount = std::round(g_dezoomAmount); });

    DrawSettingCard("amountCard", amountCard);

    CardContent blendSpeedCard = CardContent::Slider(clockIcon.c_str(), "Dezoom Blend Speed",
        "How quickly the dezoom fades in and out as a POI enters or leaves the trigger zone.",
        &g_dezoomBlendSpeed, 0.1f, 10.0f, "%.1f units/s", k_defaultDezoomBlendSpeed, SaveSettings);

    DrawSettingCard("blendSpeedCard", blendSpeedCard);

}

// ==================================================================================================================================================================================
//  Head Tracking Settings
// ==================================================================================================================================================================================

void UI::HeadTrackingSettings() {

    auto cardStyle = DrawHeaderWithReset("Head Tracking Settings", eyeIcon, "Head Tracking", "resetHeadTracking",
        MakeResetLogger("Head Tracking"),
        SettingWithDefault(&g_headTrackFadeSpeed, k_defaultHeadTrackFadeSpeed)
    );

    CardContent fadeSpeedCard = CardContent::Slider(gaugeHighIcon.c_str(), "Head Tracking Fade Speed",
        "How quickly the player's head-tracking rotates toward a focused POI.",
        &g_headTrackFadeSpeed, 0.1f, 1.0f, "%.2f units/s", k_defaultHeadTrackFadeSpeed, SaveSettings);

    DrawSettingCard("fadeSpeedCard", fadeSpeedCard);

}

// ==================================================================================================================================================================================
//  POI System - Main Settings
// ==================================================================================================================================================================================

void UI::POISystemMainSettings() {

    auto cardStyle = DrawHeaderWithReset("POI System - Main Settings", locationDotIcon, "POI System Main Settings", "resetPoiGeneral",
        MakeResetLogger("POI System Main Settings"),
        SettingWithDefault(&g_poiSystemEnabled, k_defaultPoiSystemEnabled),
        SettingWithDefault(&g_actorPoisEnabled, k_defaultActorPoiEnabled),
        SettingWithDefault(&g_preventFollowers, k_defaultPreventFollowers),
        SettingWithDefault(&g_insectsAndFishPoisEnabled, k_defaultInsectsAndFishPoisEnabled),
        SettingWithDefault(&g_poiDetectionRadius, k_defaultPoiDetectionRadius),
        SettingWithDefault(&g_lockDuration, k_defaultLockDuration)
    );

    CardContent masterToggleCard = CardContent::Checkbox(locationDotIcon.c_str(), "POI System Master Toggle",
        "Master toggle for the entire POI (Point of Interest) system. When disabled, the camera will not track any POIs automatically.",
        &g_poiSystemEnabled, k_defaultPoiSystemEnabled, SaveSettings);

    DrawSettingCard("masterToggleCard", masterToggleCard);

    if (!g_poiSystemEnabled) {

        return;

    }

    DrawMultiCheckboxCard("poiTypeToggles", "POI Type Toggles", layerGroupIcon.c_str(),
        {
            { personIcon.c_str(), Colors::Green, &g_actorPoisEnabled, k_defaultActorPoiEnabled, "Actors",
                { "Allow actors to be considered as points of interest." },
                nullptr,
                {
                    { peoplePullingIcon.c_str(), Colors::Orange, &g_preventFollowers, k_defaultPreventFollowers, "Prevent Followers",
                        { "Excludes your followers from being targeted as a POI." } },
                }
            },
            { bugIcon.c_str(), Colors::Cyan, &g_insectsAndFishPoisEnabled, k_defaultInsectsAndFishPoisEnabled, "Insects & Fish",
                { "Allow fish & insects to be considered as points of interest." } },
        },
        SaveSettings
    );

    CardContent radiusCard = CardContent::Slider(circleDotIcon.c_str(), "Maximum Detection Radius",
        "Maximum POI detection range from the player.",
        &g_poiDetectionRadius, 0.0f, 7000.0f, "%.1f m", k_defaultPoiDetectionRadius, SaveSettings,
        nullptr, true, UITemplate::SKYRIM_UNITS_TO_METERS);

    DrawSettingCard("poiRadiusCard", radiusCard);

    CardContent lockCard = CardContent::Slider(locationPinLockIcon.c_str(), "Minimum Lock Duration",
        "Minimum time the camera stays locked onto a point of interest before it can switch to another.",
        &g_lockDuration, 0.0f, 30.0f, "%.1f sec", k_defaultLockDuration, SaveSettings);

    DrawSettingCard("lockCard", lockCard);

}

// ==================================================================================================================================================================================
//  POI System - Exclusion List
// ==================================================================================================================================================================================

void UI::POISystemExclusionListSettings() {

    auto cardStyle = DrawHeaderWithListReset("POI System - Exclusion List", lockIcon, "resetPoiExclusion",
        "Clear All Exclusions", "Removes all actors from the exclusion list.",
        ExclusionListReset(&g_actorExclusionList)
    );

    // =====================================================================================================================
    //  Card 1: Add Actor to Exclusion List
    // =====================================================================================================================

    ImGuiMCP::ImVec2 winSize;
    ImGuiMCP::GetWindowSize(&winSize);
    float cardWidth = winSize.x - (k_marginBetweenBordersInHeader * 2);

    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);
    ImGuiMCP::BeginChild("##addActorCard", ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
    ImGuiMCP::Text("%s", addIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
    ImGuiMCP::Text("Add Actor to Exclusion List");

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // =====================================================================================================================
    //  Add selected actor logic
    // =====================================================================================================================

    auto* player = RE::PlayerCharacter::GetSingleton();
    RE::Actor* selectedActor = nullptr;
    bool hasSelectedActor = false;
    bool isPlayer = false;
    bool isExcluded = false;
    bool isNotActor = false;

    if (player) {

        auto selectedRef = RE::Console::GetSelectedRef();

        if (selectedRef) {

            selectedActor = selectedRef->As<RE::Actor>();

            if (selectedActor) {

                hasSelectedActor = true;

                if (selectedActor->IsPlayerRef()) {

                    isPlayer = true;

                } else {

                    isExcluded = IsActorExcluded(selectedActor);

                }

            } else {

                // Selected object is not an actor
                isNotActor = true;
                hasSelectedActor = false;

            }

        }

    }

    // =====================================================================================================================
    //  Display appropriate message based on selection state
    // =====================================================================================================================

    if (!hasSelectedActor && !selectedActor && !isNotActor) {

        // No actor selected
        ApplyCardContentLineMargin();
        DrawWrappedText(std::format("{} To exclude an actor, open the console, click on the actor, then use the button below to add them.", circleInfoIcon).c_str(), Colors::GrayLight);

        ApplyCardContentLineMargin();

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4RGBA(Colors::GrayDark));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
        ImGuiMCP::Button("Add Selected Actor: [No actor selected]");
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_NotAllowed);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Select an actor with the console first.");
            ImGuiMCP::EndTooltip();

        }

    } else if (isNotActor) {

        // Selected object is not an actor
        ApplyCardContentLineMargin();
        DrawWrappedText(std::format("{} The currently selected object is not an actor.", banIcon).c_str(), Colors::Red);

        ApplyCardContentLineMargin();

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4RGBA(Colors::GrayDark));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
        ImGuiMCP::Button("Add Selected Actor: [No actor selected]");
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_NotAllowed);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("The selected object with the console is not an actor.");
            ImGuiMCP::EndTooltip();

        }

    } else if (isPlayer) {

        ApplyCardContentLineMargin();
        DrawWrappedText(std::format("{} The player cannot be excluded.", banIcon).c_str(), Colors::Red);

        ApplyCardContentLineMargin();

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4RGBA(Colors::GrayDark));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
        const char* playerName = selectedActor->GetName();
        std::string buttonText = std::string("Add Selected Actor: ") + (playerName ? playerName : "Player");
        ImGuiMCP::Button(buttonText.c_str());
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_NotAllowed);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("The player cannot be excluded from the POI System.");
            ImGuiMCP::EndTooltip();

        }

    } else if (isExcluded) {

        ApplyCardContentLineMargin();
        DrawWrappedText(std::format("{} Already Excluded: {}", banIcon, selectedActor->GetName() ? selectedActor->GetName() : "Unnamed").c_str(), Colors::Orange);

        ApplyCardContentLineMargin();

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4RGBA(Colors::GrayDark));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
        const char* actorName = selectedActor->GetName();
        std::string buttonText = std::string("Already Excluded: ") + (actorName ? actorName : "Unnamed");
        ImGuiMCP::Button(buttonText.c_str());
        ImGuiMCP::PopStyleColor(2);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_NotAllowed);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("The selected actor is already excluded.");
            ImGuiMCP::EndTooltip();

        }

    } else {

        const char* actorName = selectedActor->GetName();
        std::string buttonText = std::string("Add Selected Actor: ") + (actorName ? actorName : "Unnamed");

        ApplyCardContentLineMargin();
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4RGBA(Colors::GreenBright));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, HexToImVec4RGBA(Colors::GreenDark));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, HexToImVec4RGBA(Colors::GreenDarker));

        if (ImGuiMCP::Button(buttonText.c_str())) {

            AddActorToExclusionList(selectedActor);

        }

        ImGuiMCP::PopStyleColor(3);

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Adds the currently selected actor to the exclusion list.");
            ImGuiMCP::EndTooltip();

        }

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentBottomMargin));

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardSpaceAfter));

    // =====================================================================================================================
    //  Card 2: Excluded Actors List
    // =====================================================================================================================

    ImGuiMCP::SetCursorPosX(k_marginBetweenBordersInHeader);
    ImGuiMCP::BeginChild("##excludedListCard", ImGuiMCP::ImVec2(cardWidth, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Green));
    ImGuiMCP::Text("%s", personIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine(0.0f, k_spaceAfterIcon);
    ImGuiMCP::Text("Excluded Actors (%zu)", g_actorExclusionList.size());

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    if (g_actorExclusionList.empty()) {

        ApplyCardContentLineMargin();
        DrawWrappedText(std::format("{} No actors in exclusion list.", folderOpenIcon).c_str(), Colors::GrayLight);

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentBottomMargin));

        ImGuiMCP::EndChild();
        return;

    }

    std::vector<UI::ActorExclusionEntry> sortedList = g_actorExclusionList;

    std::sort(sortedList.begin(), sortedList.end(), [](const UI::ActorExclusionEntry& a, const UI::ActorExclusionEntry& b) {

        std::string aName = GetActorNameFromFormID(a.formID);
        std::string bName = GetActorNameFromFormID(b.formID);
        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);
        return aName < bName;

        });

    size_t selectedIndex = SIZE_MAX;

    if (selectedActor && !selectedActor->IsPlayerRef()) {

        RE::FormID selectedFormID = 0;
        auto* actorBase = selectedActor->GetActorBase();

        if (actorBase) {

            selectedFormID = ResolveCanonicalActorFormID(actorBase, true);

        }

        for (size_t i = 0; i < sortedList.size(); ++i) {

            if (sortedList[i].formID == selectedFormID) {

                selectedIndex = i;
                break;

            }

        }

    }

    if (selectedIndex != SIZE_MAX && selectedIndex > 0) {

        UI::ActorExclusionEntry selectedEntry = sortedList[selectedIndex];
        sortedList.erase(sortedList.begin() + selectedIndex);
        sortedList.insert(sortedList.begin(), selectedEntry);

    }

    ImGuiMCP::BeginChild("ExcludedActorsList", ImGuiMCP::ImVec2(0.0f, 600.0f), ImGuiMCP::ImGuiChildFlags_Border);

    for (size_t i = 0; i < sortedList.size(); ++i) {

        auto& entry = sortedList[i];
        std::string actorName = GetActorNameFromFormID(entry.formID);
        bool isSelected = (selectedIndex != SIZE_MAX && i == 0 && selectedActor != nullptr);

        ImGuiMCP::PushID(static_cast<int>(i));

        if (isSelected) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, HexToImVec4RGBA(Colors::BlueDark));
            ImGuiMCP::BeginChild(ImGuiMCP::GetID("Highlight"), ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        }

        ApplyCardContentLineMargin();

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4RGBA(Colors::Red));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, HexToImVec4RGBA(Colors::RedDark));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, HexToImVec4RGBA(Colors::RedDarker));

        size_t originalIndex = SIZE_MAX;

        for (size_t j = 0; j < g_actorExclusionList.size(); ++j) {

            if (g_actorExclusionList[j].formID == entry.formID) {

                originalIndex = j;
                break;

            }

        }

        if (ImGuiMCP::Button("Delete")) {

            if (originalIndex != SIZE_MAX) {

                RemoveFromActorExclusionList(originalIndex);
                ImGuiMCP::PopStyleColor(3);
                ImGuiMCP::PopID();

                if (isSelected) {

                    ImGuiMCP::EndChild();
                    ImGuiMCP::PopStyleColor();

                }

                ImGuiMCP::EndChild();
                return;

            }

        }

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::SetMouseCursor(ImGuiMCP::ImGuiMouseCursor_Hand);

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Remove this actor from the exclusion list.");
            ImGuiMCP::EndTooltip();

        }

        ImGuiMCP::PopStyleColor(3);

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::Green));
        ImGuiMCP::Text("%s", personIcon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();

        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        if (isSelected) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
            ImGuiMCP::Text("%s", actorName.c_str());
            ImGuiMCP::PopStyleColor();

        } else {

            ImGuiMCP::Text("%s", actorName.c_str());

        }

        ImGuiMCP::SameLine();

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GrayLight));
        ImGuiMCP::Text("(%08X)", entry.formID);
        ImGuiMCP::PopStyleColor();

        if (isSelected) {

            ImGuiMCP::SameLine();
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4RGBA(Colors::GoldLight));
            ImGuiMCP::Text("[Currently selected with the console]");
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

        if (isSelected) {

            ImGuiMCP::EndChild();
            ImGuiMCP::PopStyleColor();

        }

        ImGuiMCP::PopID();

    }

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, k_cardContentBottomMargin));
    ImGuiMCP::EndChild();

}

// ==================================================================================================================================================================================
//  POI System - Actor Scores
// ==================================================================================================================================================================================

void UI::POISystemActorScores() {

    auto cardStyle = DrawHeaderWithReset("POI System - Actor Scores", layerGroupIcon, "Actor Score Settings", "resetPoiActors",
        MakeResetLogger("Actor Score Settings"),
        SettingWithDefault(&g_dragonScore, k_defaultDragonScore),
        SettingWithDefault(&g_dragonProximityEnabled, k_defaultDragonProximityEnabled),
        SettingWithDefault(&g_dragonProximityFactor, k_defaultDragonProximityFactor),
        SettingWithDefault(&g_dragonEnabled, k_defaultDragonEnabled),
        SettingWithDefault(&g_actorCombatScore, k_defaultActorCombatScore),
        SettingWithDefault(&g_actorCombatProximityEnabled, k_defaultActorCombatProximityEnabled),
        SettingWithDefault(&g_actorCombatProximityFactor, k_defaultActorCombatProximityFactor),
        SettingWithDefault(&g_actorCombatEnabled, k_defaultActorCombatEnabled),
        SettingWithDefault(&g_actorMovingScore, k_defaultActorMovingScore),
        SettingWithDefault(&g_actorMovingProximityEnabled, k_defaultActorMovingProximityEnabled),
        SettingWithDefault(&g_actorMovingProximityFactor, k_defaultActorMovingProximityFactor),
        SettingWithDefault(&g_actorMovingEnabled, k_defaultActorMovingEnabled),
        SettingWithDefault(&g_actorInSceneScore, k_defaultActorInSceneScore),
        SettingWithDefault(&g_actorInSceneProximityEnabled, k_defaultActorInSceneProximityEnabled),
        SettingWithDefault(&g_actorInSceneProximityFactor, k_defaultActorInSceneProximityFactor),
        SettingWithDefault(&g_actorInSceneEnabled, k_defaultActorInSceneEnabled),
        SettingWithDefault(&g_actorIdleScore, k_defaultActorIdleScore),
        SettingWithDefault(&g_actorIdleProximityEnabled, k_defaultActorIdleProximityEnabled),
        SettingWithDefault(&g_actorIdleProximityFactor, k_defaultActorIdleProximityFactor),
        SettingWithDefault(&g_actorIdleEnabled, k_defaultActorIdleEnabled)
    );

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.5f));

    DrawActorScoreCards();

}

// ==================================================================================================================================================================================
//  POI System - Insects & Fish Scores
// ==================================================================================================================================================================================

void UI::POISystemInsectsAndFishScores() {

    auto cardStyle = DrawHeaderWithReset("POI System - Insects & Fish Scores", layerGroupIcon, "Insects & Fish Score Settings", "resetPoiInsectsFish",
        MakeResetLogger("Insects & Fish Score Settings"),
        SettingWithDefault(&g_insectsScore, k_defaultInsectsScore),
        SettingWithDefault(&g_insectsProximityEnabled, k_defaultInsectsProximityEnabled),
        SettingWithDefault(&g_insectsProximityFactor, k_defaultInsectsProximityFactor),
        SettingWithDefault(&g_insectsEnabled, k_defaultInsectsEnabled),
        SettingWithDefault(&g_fishScore, k_defaultFishScore),
        SettingWithDefault(&g_fishProximityEnabled, k_defaultFishProximityEnabled),
        SettingWithDefault(&g_fishProximityFactor, k_defaultFishProximityFactor),
        SettingWithDefault(&g_fishEnabled, k_defaultFishEnabled)
    );

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.5f));

    DrawInsectsAndFishScoreCards();

}

// ==================================================================================================================================================================================
//  Debug Settings
// ==================================================================================================================================================================================

void UI::DebugSettings() {

    auto cardStyle = DrawHeaderWithReset("Debug Settings", screwdriverWrenchIcon, "Debug", "resetDebug",
        MakeResetLogger("Debug Settings"),
        SettingWithDefault(&g_debugRaycasts, k_defaultDebugRaycasts),
        SettingWithDefault(&g_loggingLevel, k_defaultLoggingLevel, []() {
            auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
            spdlog::set_level(lvl);
            spdlog::flush_on(lvl);
            logger::info("Logging level reset to default ('{}')", k_loggingLevelNames[g_loggingLevel]);
            })
    );

    CardContent raycastCard = CardContent::Checkbox(crosshairsIcon.c_str(), "Debug Raycast Visualization",
        "Draws debug raycast lines used by the POI detection system, for troubleshooting purposes.",
        &g_debugRaycasts, k_defaultDebugRaycasts, SaveSettings);

    DrawSettingCard("raycastCard", raycastCard);

    CardContent loggingCard = CardContent::IntSlider(terminalIcon.c_str(), "Logging Level",
        "Controls how much detail is written to the log file:\n"
        "\n"
        "Quiet - only critical errors that prevent the mod from working.\n"
        "\n"
        "Warnings - critical errors plus non-fatal issues worth noticing.\n"
        "\n"
        "Info - warnings plus general status messages.\n"
        "\n"
        "Debug - everything, including detailed internal values for troubleshooting.",
        &g_loggingLevel, 0, UITemplate::k_loggingLevelCount - 1, k_defaultLoggingLevel, UITemplate::k_loggingLevelNames,
        SaveSettings,
        []() {
            auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
            spdlog::set_level(lvl);
            spdlog::flush_on(lvl);
            logger::info("Logging level manually set to '{}'", UITemplate::k_loggingLevelNames[g_loggingLevel]);
        });

    DrawSettingCard("loggingCard", loggingCard);

}