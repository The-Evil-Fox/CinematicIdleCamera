#include "Menu.h"
#include <format>
#include "hooks/PlayerCameraHook.h"
#include "utility.h"
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

namespace logger = SKSE::log;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Default settings used when the ini doesn't exist when the game is started (ordered by menus)
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
//  Camera
// ---------------------------------------------------------------------------------------------------------------------

// Main Settings

static constexpr float              k_defaultIdleTimer                                          = 5.0f;
static constexpr bool               k_defaultBlackBarsEnabled                                   = true;
static constexpr float              k_defaultBlackBarsSpeed                                     = 1.0f;
static constexpr bool               k_defaultBlackBarsSoundEnabled                              = true;

// Position/Transition

static constexpr float              k_defaultVanityCamOffsetX                                   = 75.0f;
static constexpr float              k_defaultVanityCamOffsetY                                   = 130.0f;
static constexpr float              k_defaultVanityCamOffsetZ                                   = 0.0f;
static constexpr float              k_defaultBlendDuration                                      = 5.0f;

// Zoom/Dezoom

static constexpr float              k_defaultDezoomTriggerRadius                                = 350.0f;
static constexpr float              k_defaultDezoomTriggerHeight                                = 210.0f;
static constexpr float              k_defaultDezoomAmount                                       = 250.0f;
static constexpr float              k_defaultDezoomBlendSpeed                                   = 0.7f;

// ---------------------------------------------------------------------------------------------------------------------
//  Head Tracking
// ---------------------------------------------------------------------------------------------------------------------

static constexpr float              k_defaultHeadTrackFadeSpeed = 0.3f;

// ---------------------------------------------------------------------------------------------------------------------
//  POI System
// ---------------------------------------------------------------------------------------------------------------------

// Main settings

static constexpr bool               k_defaultPoiSystemEnabled                                   = true;
static constexpr bool               k_defaultActorPoiEnabled                                    = true;
static constexpr bool               k_defaultPreventFollowers                                   = true;
static constexpr bool               k_defaultFlyingCritterPoiEnabled                            = true;
static constexpr bool               k_defaultFishPoiEnabled                                     = true;
static constexpr float              k_defaultPoiDetectionRadius                                 = 1050.0f;
static constexpr float              k_defaultLockDuration                                       = 5.0f;

// Actors score system

static constexpr float              k_defaultDragonScore                                        = 2000.0f;
static constexpr bool               k_defaultDragonProximityEnabled                             = true;
static constexpr float              k_defaultDragonProximityFactor                              = 1000.0f;

static constexpr float              k_defaultActorCombatScore                                   = 600.0f;
static constexpr bool               k_defaultActorCombatProximityEnabled                        = true;
static constexpr float              k_defaultActorCombatProximityFactor                         = 200.0f;

static constexpr float              k_defaultActorMovingScore                                   = 400.0f;
static constexpr bool               k_defaultActorMovingProximityEnabled                        = true;
static constexpr float              k_defaultActorMovingProximityFactor                         = 150.0f;

static constexpr float              k_defaultActorInSceneScore                                  = 300.0f;
static constexpr bool               k_defaultActorInSceneProximityEnabled                       = true;
static constexpr float              k_defaultActorInSceneProximityFactor                        = 50.0f;

static constexpr float              k_defaultActorIdleScore                                     = 10.0f;
static constexpr bool               k_defaultActorIdleProximityEnabled                          = true;
static constexpr float              k_defaultActorIdleProximityFactor                           = 50.0f;

// Flying critters score system

static constexpr float              k_defaultFlyingCritterScore                                 = 400.0f;
static constexpr bool               k_defaultFlyingCritterProximityEnabled                      = true;
static constexpr float              k_defaultFlyingCritterProximityFactor                       = 150.0f;

// Fish critters score system

static constexpr float              k_defaultFishScore                                          = 300.0f;
static constexpr bool               k_defaultFishProximityEnabled                               = true;
static constexpr float              k_defaultFishProximityFactor                                = 150.0f;

// ---------------------------------------------------------------------------------------------------------------------
//  Debug
// ---------------------------------------------------------------------------------------------------------------------

static constexpr bool               k_defaultDebugRaycasts                                      = false;
static constexpr int                k_defaultLoggingLevel                                       = 2; // 0 = critical, 1 = warn, 2 = info, 3 = debug

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Initalize the sliders in the ui with the default values (used only when the INI doesn't exist or a param in it was completely removed)
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
//  Camera
// ---------------------------------------------------------------------------------------------------------------------

// Main settings

float                               UI::g_idleTimer                                             = k_defaultIdleTimer;
bool                                UI::g_blackBarsEnabled                                      = k_defaultBlackBarsEnabled;
float                               UI::g_blackBarsSpeed                                        = k_defaultBlackBarsSpeed;
bool                                UI::g_blackBarsSoundEnabled                                 = k_defaultBlackBarsSoundEnabled;

// Camera position

float                               UI::g_IdleCamOffsetX                                        = k_defaultVanityCamOffsetX;
float                               UI::g_IdleCamOffsetY                                        = k_defaultVanityCamOffsetY;
float                               UI::g_IdleCamOffsetZ                                        = k_defaultVanityCamOffsetZ;
float                               UI::g_blendDuration                                         = k_defaultBlendDuration;

// Zoom/Dezoom

float                               UI::g_dezoomTriggerRadius                                   = k_defaultDezoomTriggerRadius;
float                               UI::g_dezoomTriggerHeight                                   = k_defaultDezoomTriggerHeight;
float                               UI::g_dezoomAmount                                          = k_defaultDezoomAmount;
float                               UI::g_dezoomBlendSpeed                                      = k_defaultDezoomBlendSpeed;

// ---------------------------------------------------------------------------------------------------------------------
//  Head Tracking
// ---------------------------------------------------------------------------------------------------------------------

float                               UI::g_headTrackFadeSpeed                                    = k_defaultHeadTrackFadeSpeed;

// ---------------------------------------------------------------------------------------------------------------------
//  POI System
// ---------------------------------------------------------------------------------------------------------------------

// Main settings

bool                                UI::g_poiSystemEnabled                                      = k_defaultPoiSystemEnabled;
bool                                UI::g_actorPoiEnabled                                       = k_defaultActorPoiEnabled;
bool                                UI::g_preventFollowers                                      = k_defaultPreventFollowers;
bool                                UI::g_flyingCritterPoiEnabled                               = k_defaultFlyingCritterPoiEnabled;
bool                                UI::g_fishPoiEnabled                                        = k_defaultFishPoiEnabled;
float                               UI::g_poiDetectionRadius                                    = k_defaultPoiDetectionRadius;
float                               UI::g_lockDuration                                          = k_defaultLockDuration;

// Actors

float                               UI::g_dragonScore                                           = k_defaultDragonScore;
bool                                UI::g_dragonProximityEnabled                                = k_defaultDragonProximityEnabled;
float                               UI::g_dragonProximityFactor                                 = k_defaultDragonProximityFactor;

float                               UI::g_actorCombatScore                                      = k_defaultActorCombatScore;
bool                                UI::g_actorCombatProximityEnabled                           = k_defaultActorCombatProximityEnabled;
float                               UI::g_actorCombatProximityFactor                            = k_defaultActorCombatProximityFactor;

float                               UI::g_actorMovingScore                                      = k_defaultActorMovingScore;
bool                                UI::g_actorMovingProximityEnabled                           = k_defaultActorMovingProximityEnabled;
float                               UI::g_actorMovingProximityFactor                            = k_defaultActorMovingProximityFactor;

float                               UI::g_actorInSceneScore                                     = k_defaultActorInSceneScore;
bool                                UI::g_actorInSceneProximityEnabled                          = k_defaultActorInSceneProximityEnabled;
float                               UI::g_actorInSceneProximityFactor                           = k_defaultActorInSceneProximityFactor;

float                               UI::g_actorIdleScore                                        = k_defaultActorIdleScore;
bool                                UI::g_actorIdleProximityEnabled                             = k_defaultActorIdleProximityEnabled;
float                               UI::g_actorIdleProximityFactor                              = k_defaultActorIdleProximityFactor;

// Flying critters

float                               UI::g_flyingCritterScore                                    = k_defaultFlyingCritterScore;
bool                                UI::g_flyingCritterProximityEnabled                         = k_defaultFlyingCritterProximityEnabled;
float                               UI::g_flyingCritterProximityFactor                          = k_defaultFlyingCritterProximityFactor;

// Fish critters

float                               UI::g_pondFishScore                                         = k_defaultFishScore;
bool                                UI::g_pondFishProximityEnabled                              = k_defaultFishProximityEnabled;
float                               UI::g_pondFishProximityFactor                               = k_defaultFishProximityFactor;

// Exclusion list

std::vector<UI::ActorExclusionEntry> UI::g_actorExclusionList;

namespace Hooks {

    extern RE::TESObjectREFR* g_currentPOI;

}

// ---------------------------------------------------------------------------------------------------------------------
//  Debug
// ---------------------------------------------------------------------------------------------------------------------

bool                                UI::g_debugRaycasts                                         = k_defaultDebugRaycasts;
int                                 UI::g_loggingLevel                                          = k_defaultLoggingLevel;

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Logging level names, indexed 0-3 to match the simplified scheme:
//  0 = Quiet (critical only), 1 = Warnings, 2 = Info (default), 3 = Debug
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static constexpr const char* k_loggingLevelNames[] = {

    "Quiet",
    "Warnings",
    "Info",
    "Debug"

};

static constexpr int k_loggingLevelCount = static_cast<int>(std::size(k_loggingLevelNames));

// Mirrors the mapping used in IniParser::Load() / utility.cpp, so the slider's
// live preview matches what gets applied and saved.
static spdlog::level::level_enum LoggingLevelToSpdlog(int loggingLevel) {

    switch (loggingLevel) {

        case 0:  return spdlog::level::critical;

        case 1:  return spdlog::level::warn;

        case 3:  return spdlog::level::debug;

        default: return spdlog::level::info;

    }

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Used to force apply some new settings to the virtual copy of the ini user side. Only used for the idle timer for now.
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void ApplyIdleTimerToIniSettings(std::string settingName, float value) {

    auto* iniSettings = RE::INISettingCollection::GetSingleton();

    if (iniSettings) {

        auto* setting = iniSettings->GetSetting(settingName);

        if (setting) {

            setting->data.f = value;

        } else {

            logger::error("Setting not found in INISettingCollection!");

        }

    }

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Font Awesome Icons
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// =====================================================================================================================
//  Section Icons (Headers)
// =====================================================================================================================

auto cameraIcon = FontAwesome::UnicodeToUtf8(0xf03d);  // Camera
auto headTrackIcon = FontAwesome::UnicodeToUtf8(0xf06e);  // Eye (Head Tracking)
auto poiSystemIcon = FontAwesome::UnicodeToUtf8(0xf3c5);  // Map Marker
auto debugIcon = FontAwesome::UnicodeToUtf8(0xf7d9);  // Bug

// =====================================================================================================================
//  Arrows & Direction Icons
// =====================================================================================================================

auto arrowUpIcon = FontAwesome::UnicodeToUtf8(0xf062);  // Arrow Up
auto anglesRightIcon = FontAwesome::UnicodeToUtf8(0xf101);  // Angles Right
auto arrowLeftAndRightIcon = FontAwesome::UnicodeToUtf8(0xf07e);  // Arrows Left-Right
auto arrowUpAndDownIcon = FontAwesome::UnicodeToUtf8(0xf07d);  // Arrows Up-Down
auto zoomIcon = FontAwesome::UnicodeToUtf8(0xf002);  // Magnifying Glass

// =====================================================================================================================
//  Camera Settings Icons
// =====================================================================================================================

auto clockIcon = FontAwesome::UnicodeToUtf8(0xf017);  // Clock (Timer, Duration)
auto filmIcon = FontAwesome::UnicodeToUtf8(0xf008);  // Film (Black Bars)
auto speedIcon = FontAwesome::UnicodeToUtf8(0xf625);  // Gauge (Speed)
auto soundIcon = FontAwesome::UnicodeToUtf8(0xf028);  // Volume (Sound Effects)
auto radiusIcon = FontAwesome::UnicodeToUtf8(0xf192);  // Circle Dot (Radius)

// =====================================================================================================================
//  POI System Icons
// =====================================================================================================================

auto poiTypesIcon = FontAwesome::UnicodeToUtf8(0xf5fd);  // Layer Group (POI Types)
auto poiLockIcon = FontAwesome::UnicodeToUtf8(0xe51f);  // Lock (Lock Duration)
auto followerIcon = FontAwesome::UnicodeToUtf8(0xe535);  // User Group (Followers)

// =====================================================================================================================
//  Actor POI Icons
// =====================================================================================================================

auto dragonScoreIcon = FontAwesome::UnicodeToUtf8(0xf6d5);  // Dragon
auto inCombatScoreIcon = FontAwesome::UnicodeToUtf8(0xf0e3);  // Crossed Swords
auto movingScoreIcon = FontAwesome::UnicodeToUtf8(0xf554);  // Person Walking
auto inSceneScoreIcon = FontAwesome::UnicodeToUtf8(0xf630);  // Theater Masks
auto personIcon = FontAwesome::UnicodeToUtf8(0xf183);  // Person (General purpose)

// =====================================================================================================================
//  Critter POI Icons
// =====================================================================================================================

auto flyingCritterIcon = FontAwesome::UnicodeToUtf8(0xf4ba);  // Butterfly
auto fishCritterIcon = FontAwesome::UnicodeToUtf8(0xf578);  // Fish

// =====================================================================================================================
//  Exclusion List Icons
// =====================================================================================================================

auto trashIcon = FontAwesome::UnicodeToUtf8(0xf2ed);  // Trash (Clear All)
auto addIcon = FontAwesome::UnicodeToUtf8(0xf067);  // Plus (Add Actor)
auto infoIcon = FontAwesome::UnicodeToUtf8(0xf05a);  // Info Circle

// =====================================================================================================================
//  Debug Icons
// =====================================================================================================================

auto raycastIcon = FontAwesome::UnicodeToUtf8(0xf05b);  // Crosshairs
auto loggingIcon = FontAwesome::UnicodeToUtf8(0xf120);  // Terminal/Code

// =====================================================================================================================
//  General UI Icons
// =====================================================================================================================

auto resetIcon = FontAwesome::UnicodeToUtf8(0xf2ea);  // Undo (Reset)


// ---------------------------------------------------------------------------------------------------------------------
//  UI Helper function to convert hex color values to ImVec4
// ---------------------------------------------------------------------------------------------------------------------

inline ImGuiMCP::ImVec4 HexToImVec4(uint32_t hex) {

    // Pass "0x"+hex code of the color as a param

    float r = ((hex >> 16) & 0xFF) / 255.0f;
    float g = ((hex >> 8) & 0xFF) / 255.0f;
    float b = (hex & 0xFF) / 255.0f;

    return ImGuiMCP::ImVec4{ r, g, b, 1.0f };

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Shows a hovering tooltip describing the parameter. Call this right after the widget (slider/checkbox) it
//  belongs to, while the widget is still the "last item" (ImGui tracks hover state per last-submitted item).
//
//  The wrap width is computed dynamically from the longest line in `text` (splitting on '\n'), so the
//  tooltip box is always exactly as wide as its widest line and nothing wraps unexpectedly.
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void HelpTooltip(const char* text) {

    if (ImGuiMCP::IsItemHovered()) {

        // ---------------------------------------------------------------------------------------------------------------
        //  Walk the string, splitting on '\n', and measure each line's pixel width via ImGui's own text
        //  measurement (CalcTextSize) so the result accounts for the actual font metrics rather than raw
        //  character counts (which would be wrong for proportional fonts). SKSEMenuFramework's CalcTextSize
        //  writes its result through an out-param (ImVec2*) rather than returning by value, and has no
        //  default arguments, so all 5 parameters must be supplied explicitly.
        // ---------------------------------------------------------------------------------------------------------------

        float       longestLineWidth = 0.0f;
        const char* lineStart = text;

        for (const char* p = text; ; ++p) {

            if (*p == '\n' || *p == '\0') {

                ImGuiMCP::ImVec2 lineSize{};
                ImGuiMCP::CalcTextSize(&lineSize, lineStart, p, false, -1.0f);

                if (lineSize.x > longestLineWidth) {

                    longestLineWidth = lineSize.x;

                }

                if (*p == '\0') {

                    break;

                }

                lineStart = p + 1;

            }

        }

        // Pad slightly so the wrap boundary sits just past the widest line's actual pixel width,
        // preventing any edge-case wrapping caused by rounding.
        const float wrapWidth = longestLineWidth + 1.0f;

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::PushTextWrapPos(ImGuiMCP::GetCursorPosX() + wrapWidth);
        ImGuiMCP::TextUnformatted(text);
        ImGuiMCP::PopTextWrapPos();
        ImGuiMCP::EndTooltip();

    }

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Exclusion list specific functions
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ---------------------------------------------------------------------------------------------------------------------
//  Helper: Get actor name from Form ID at runtime (language-independent)
// ---------------------------------------------------------------------------------------------------------------------

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

// ---------------------------------------------------------------------------------------------------------------------
//  Add the console selected actor to the exclusion list
// ---------------------------------------------------------------------------------------------------------------------

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

    // Get the actor base ID
    RE::FormID actorBaseID = actorBase->GetFormID();
    logger::info("ActorBase Form ID: 0x{:08X}", actorBaseID);

    RE::FormID finalFormID = actorBaseID;

    // Check if this is a dynamic/temporary ID (starts with FF)
    if ((finalFormID & 0xFF000000) == 0xFF000000) {

        // Try to get the root template
        auto* rootTemplate = actorBase->GetRootFaceNPC();

        if (rootTemplate && rootTemplate != actorBase) {

            RE::FormID rootID = rootTemplate->GetFormID();
            logger::info("Root Template Form ID: 0x{:08X}", rootID);

            // If root ID is not dynamic, use it
            if ((rootID & 0xFF000000) != 0xFF000000) {

                finalFormID = rootID;
                logger::info("Using Root Template Form ID: 0x{:08X}", finalFormID);

            }

        }

        // If still dynamic, try faceNPC
        if ((finalFormID & 0xFF000000) == 0xFF000000) {

            auto* templateNPC = actorBase->faceNPC;

            if (templateNPC) {

                RE::FormID faceID = templateNPC->GetFormID();
                logger::info("faceNPC Form ID: 0x{:08X}", faceID);

                if ((faceID & 0xFF000000) != 0xFF000000) {

                    finalFormID = faceID;
                    logger::info("Using faceNPC Form ID: 0x{:08X}", finalFormID);

                }

            }

        }

    }

    // If we STILL have a dynamic ID (FF), use the Reference ID as last resort
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

// ---------------------------------------------------------------------------------------------------------------------
//  Remove an actor from the exclusion list when pressing the button on its left in the exclusion list
// ---------------------------------------------------------------------------------------------------------------------

void UI::RemoveFromActorExclusionList(size_t index) {

    if (index < g_actorExclusionList.size()) {

        // Get name at runtime for logging
        std::string name = GetActorNameFromFormID(g_actorExclusionList[index].formID);
        logger::info("Removed {} from exclusion list", name);
        g_actorExclusionList.erase(g_actorExclusionList.begin() + index);
        IniParser::Save();

    }

}

// ---------------------------------------------------------------------------------------------------------------------
//  Checks if an actor is excluded. 
//  Also used inside playercamerahook.cpp to check if a potential poi is excluded.
// ---------------------------------------------------------------------------------------------------------------------

bool UI::IsActorExcluded(RE::Actor* a_actor) {

    if (!a_actor) return false;

    auto* actorBase = a_actor->GetActorBase();

    if (!actorBase) return false;

    RE::FormID formID = actorBase->GetFormID();

    // If dynamic, try to get the root template
    if ((formID & 0xFF000000) == 0xFF000000) {

        auto* rootTemplate = actorBase->GetRootFaceNPC();

        if (rootTemplate && rootTemplate != actorBase) {

            RE::FormID rootID = rootTemplate->GetFormID();

            if ((rootID & 0xFF000000) != 0xFF000000) {

                formID = rootID;
            
            }
        
        }

        // If still dynamic, try faceNPC
        if ((formID & 0xFF000000) == 0xFF000000) {

            auto* templateNPC = actorBase->faceNPC;

            if (templateNPC) {

                RE::FormID faceID = templateNPC->GetFormID();
                
                if ((faceID & 0xFF000000) != 0xFF000000) {
                    
                    formID = faceID;
                
                }
            
            }
        
        }
    
    }

    for (auto& entry : g_actorExclusionList) {

        if (entry.formID == formID) {

            return true;

        }
    
    }
    
    return false;

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Register all the different setting sections in the SKSE menu
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

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
    SKSEMenuFramework::AddSectionItem(std::string("POI System/Critter Scores"), POISystemCritterScores);

    SKSEMenuFramework::AddSectionItem(std::string("Debug"), DebugSettings);

    SKSEMenuFramework::AddHudElement(DrawCinematicBars);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Sound effect player
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

static void playSoundEffect(const std::string& a_filePath) {

    std::string fullPath = "Data\\" + a_filePath;
    std::replace(fullPath.begin(), fullPath.end(), '/', '\\');
    PlaySoundA(fullPath.c_str(), NULL, SND_ASYNC | SND_FILENAME);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Black bars used to make the vanity mode more cinematic
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::DrawCinematicBars() {

    auto* playerCamera = RE::PlayerCamera::GetSingleton();

    if (!playerCamera || !playerCamera->currentState || !g_blackBarsEnabled) {

        return;

    }

    const bool inVanity = playerCamera->currentState->id == RE::CameraState::kAutoVanity;

    static bool s_wasInVanity = false;              // Track previous vanity state
    static bool s_soundPlayedForAppear = false;     // Prevent duplicate enter sounds
    static bool s_soundPlayedForDisappear = false;  // Prevent duplicate exit sounds
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

                logger::debug("Cinematic black bars drawing -> Playing entering vanity mode sound effect");

                playSoundEffect("SKSE\\Plugins\\cinematicidlecamera\\FX\\entervanitymode.wav");

            } else {

                logger::debug("Cinematic black bars removing -> Playing exiting vanity mode sound effect");

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

    const ImGuiMCP::ImU32 barColor = ImGuiMCP::ColorConvertFloat4ToU32(ImGuiMCP::ImVec4{ 0.0f, 0.0f, 0.0f, 1.0f });

    // Top bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, topBarBottom - barHeight }, ImGuiMCP::ImVec2{ screenW, topBarBottom }, barColor, 0.0f, 0);

    // Bottom bar
    ImGuiMCP::ImDrawListManager::AddRectFilled(drawList, ImGuiMCP::ImVec2{ 0.0f, botBarTop }, ImGuiMCP::ImVec2{ screenW, botBarTop + barHeight }, barColor, 0.0f, 0);
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Camera Settings - Main Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::CameraMainSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Camera Settings - Main Settings", cameraIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetCameraMain", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_idleTimer = k_defaultIdleTimer;
        g_blackBarsEnabled = k_defaultBlackBarsEnabled;
        g_blackBarsSpeed = k_defaultBlackBarsSpeed;
        g_blackBarsSoundEnabled = k_defaultBlackBarsSoundEnabled;
        ApplyIdleTimerToIniSettings("fAutoVanityModeDelay:Camera", g_idleTimer);
        IniParser::Save();
        logger::info("Camera Main Settings reset to default values.");

    }

    HelpTooltip("Restores all camera main settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Idle Timer
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##timerCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", clockIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Idle Timer");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string valueText = std::format("{:.0f} sec", g_idleTimer);
    ImGuiMCP::ImVec2 valueTextSize;
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f sec", g_idleTimer);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Idle Timer: %.0f seconds", g_idleTimer);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##idleTimer", &g_idleTimer, 1.0f, 300.0f, "%.0f sec")) {

        IniParser::Save();
        ApplyIdleTimerToIniSettings("fAutoVanityModeDelay:Camera", g_idleTimer);
        logger::info("Camera Idle Timer Setting manually set to {} second(s)", g_idleTimer);

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("How many seconds of player inactivity before the camera switches into idle mode.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Cinematic Black Bars Enabled
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##blackBarsCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", filmIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Cinematic Black Bars");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string statusText = g_blackBarsEnabled ? "Enabled" : "Disabled";
    ImGuiMCP::CalcTextSize(&valueTextSize, statusText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, g_blackBarsEnabled ? HexToImVec4(0x2ecc71) : HexToImVec4(0xe74c3c));
    ImGuiMCP::Text("%s", statusText.c_str());
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Cinematic Black Bars: %s", statusText.c_str());
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Checkbox
    if (ImGuiMCP::Checkbox("##blackBarsEnabled", &g_blackBarsEnabled)) {

        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Enable or disable the cinematic black bars that appear when entering vanity mode.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Black Bars Slide Speed (Only shown if enabled)
    // =====================================================================================================================

    if (g_blackBarsEnabled) {

        ImGuiMCP::BeginChild("##speedCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        // Header with icon and current value
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
        ImGuiMCP::Text("%s", speedIcon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(" Black Bars Slide Speed");
        ImGuiMCP::SameLine();
        ImGuiMCP::GetContentRegionAvail(&avail);
        valueText = std::format("{:.1f}", g_blackBarsSpeed);
        ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
        ImGuiMCP::Text("%.1f", g_blackBarsSpeed);
        ImGuiMCP::PopStyleColor();

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Current Slide Speed: %.1f", g_blackBarsSpeed);
            ImGuiMCP::EndTooltip();

        }

        ImGuiMCP::Separator();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Slider
        ImGuiMCP::SetNextItemWidth(-1.0f);

        if (ImGuiMCP::SliderFloat("##blackBarsSlideSpeed", &g_blackBarsSpeed, 0.1f, 5.0f, "%.1f")) {

            IniParser::Save();

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

        // Helper text
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
        ImGuiMCP::Text("How fast the cinematic bars slide in and out when entering/exiting vanity mode.");
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::EndChild();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

        // =====================================================================================================================
        //  Black Bars Sound Effects
        // =====================================================================================================================

        ImGuiMCP::BeginChild("##soundCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        // Header with icon and current value
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
        ImGuiMCP::Text("%s", soundIcon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();
        ImGuiMCP::Text(" Black Bars Sound Effects");
        ImGuiMCP::SameLine();
        ImGuiMCP::GetContentRegionAvail(&avail);
        statusText = g_blackBarsSoundEnabled ? "Enabled" : "Disabled";
        ImGuiMCP::CalcTextSize(&valueTextSize, statusText.c_str(), nullptr, false, -1.0f);
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, g_blackBarsSoundEnabled ? HexToImVec4(0x2ecc71) : HexToImVec4(0xe74c3c));
        ImGuiMCP::Text("%s", statusText.c_str());
        ImGuiMCP::PopStyleColor();

        if (ImGuiMCP::IsItemHovered()) {

            ImGuiMCP::BeginTooltip();
            ImGuiMCP::Text("Sound Effects: %s", statusText.c_str());
            ImGuiMCP::EndTooltip();

        }

        ImGuiMCP::Separator();
        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Checkbox
        if (ImGuiMCP::Checkbox("##blackBarsSoundEnabled", &g_blackBarsSoundEnabled)) {

            IniParser::Save();

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

        // Helper text
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
        ImGuiMCP::Text("Enable or disable the sound effects that play when the cinematic black bars appear or disappear.");
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::EndChild();

    }

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

/// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Camera Settings - Position
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::CameraPositionSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Camera Settings - Position", cameraIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetCameraPosition", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_IdleCamOffsetX = k_defaultVanityCamOffsetX;
        g_IdleCamOffsetY = k_defaultVanityCamOffsetY;
        g_IdleCamOffsetZ = k_defaultVanityCamOffsetZ;
        g_blendDuration = k_defaultBlendDuration;
        IniParser::Save();

    }

    HelpTooltip("Restores all camera position settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Idle Camera Offset X
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##offsetXCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", arrowLeftAndRightIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Idle Camera Offset X");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string valueText = std::format("{:.0f}", g_IdleCamOffsetX);
    ImGuiMCP::ImVec2 valueTextSize;
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", g_IdleCamOffsetX);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Offset X: %.0f", g_IdleCamOffsetX);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##idleCameraOffsetX", &g_IdleCamOffsetX, -500.0f, 500.0f, "%.0f")) {

        g_IdleCamOffsetX = std::round(g_IdleCamOffsetX);
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Horizontal (X-axis) offset of the idle camera relative to the player.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Idle Camera Zoom (Offset Y)
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##offsetYCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", zoomIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Idle Camera Zoom");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    valueText = std::format("{:.0f}", g_IdleCamOffsetY);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", g_IdleCamOffsetY);
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Zoom: %.0f", g_IdleCamOffsetY);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##idleCameraOffsetY", &g_IdleCamOffsetY, -500.0f, 500.0f, "%.0f")) {

        g_IdleCamOffsetY = std::round(g_IdleCamOffsetY);
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Zoom (Y-axis offset) of the idle camera relative to the player.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Idle Camera Offset Z
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##offsetZCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", arrowUpAndDownIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Idle Camera Offset Z");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    valueText = std::format("{:.0f}", g_IdleCamOffsetZ);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", g_IdleCamOffsetZ);
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Offset Z: %.0f", g_IdleCamOffsetZ);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##idleCameraOffsetZ", &g_IdleCamOffsetZ, -500.0f, 500.0f, "%.0f")) {

        g_IdleCamOffsetZ = std::round(g_IdleCamOffsetZ);
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Vertical (Z-axis) offset of the idle camera relative to the player.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Blend Duration
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##blendCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", clockIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Blend Duration");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    valueText = std::format("{:.2f} sec", g_blendDuration);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.2f sec", g_blendDuration);
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Blend Duration: %.2f sec", g_blendDuration);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##blendDuration", &g_blendDuration, 0.1f, 5.0f, "%.2f sec")) {

        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("How long the camera takes to rotate to focus on a POI.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Camera Settings - Zoom/Dezoom
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::CameraZoomSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Camera Settings - Zoom/Dezoom", cameraIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetCameraZoom", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_dezoomTriggerRadius = k_defaultDezoomTriggerRadius;
        g_dezoomTriggerHeight = k_defaultDezoomTriggerHeight;
        g_dezoomAmount = k_defaultDezoomAmount;
        g_dezoomBlendSpeed = k_defaultDezoomBlendSpeed;
        IniParser::Save();

    }

    HelpTooltip("Restores all zoom/dezoom settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Dezoom Trigger Radius
    // =====================================================================================================================
    //
    //  A POI above the player can fall outside the vanity camera's fixed pitch, since this system only ever
    //  rotates yaw to face a POI. Pulling the camera back (see Dezoom Amount below) widens the effective
    //  vertical field of view onto it. This slider controls how close the POI has to be, horizontally, for
    //  that to kick in.
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##radiusCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", radiusIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Dezoom Trigger Radius");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    float dezoomRadiusMeters = g_dezoomTriggerRadius / 70.0f;
    std::string valueText = std::format("{:.1f} m", dezoomRadiusMeters);
    ImGuiMCP::ImVec2 valueTextSize;
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.1f m", dezoomRadiusMeters);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Trigger Radius: %.1f m (%.0f units)", dezoomRadiusMeters, g_dezoomTriggerRadius);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##dezoomTriggerRadius", &dezoomRadiusMeters, 0.0f, 20.0f, "%.1f m")) {

        g_dezoomTriggerRadius = dezoomRadiusMeters * 70.0f;
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Horizontal distance from the player within which an overhead POI can trigger the dezoom.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Dezoom Trigger Height
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##heightCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", arrowUpIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Dezoom Trigger Height");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    float dezoomHeightMeters = g_dezoomTriggerHeight / 70.0f;
    valueText = std::format("{:.1f} m", dezoomHeightMeters);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.1f m", dezoomHeightMeters);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Trigger Height: %.1f m (%.0f units)", dezoomHeightMeters, g_dezoomTriggerHeight);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##dezoomTriggerHeight", &dezoomHeightMeters, 0.0f, 20.0f, "%.1f m")) {

        g_dezoomTriggerHeight = dezoomHeightMeters * 70.0f;
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("How far above the player a POI must be, while inside the trigger radius, before the dezoom kicks in.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Dezoom Amount
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##dezoomAmountCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", zoomIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Dezoom Amount");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    valueText = std::format("{:.0f}", g_dezoomAmount);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", g_dezoomAmount);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Dezoom Amount: %.0f units", g_dezoomAmount);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##dezoomAmount", &g_dezoomAmount, 0.0f, 500.0f, "%.0f")) {

        g_dezoomAmount = std::round(g_dezoomAmount);
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("How far the camera pulls back once the dezoom is fully active (added on top of the Idle camera offset Y).");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Dezoom Blend Speed
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##blendSpeedCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", clockIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Dezoom Blend Speed");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    valueText = std::format("{:.1f} units/s", g_dezoomBlendSpeed);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.1f units/s", g_dezoomBlendSpeed);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Blend Speed: %.1f units/s", g_dezoomBlendSpeed);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##dezoomBlendSpeed", &g_dezoomBlendSpeed, 0.1f, 10.0f, "%.1f units/s")) {

        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("How quickly the dezoom fades in and out as a POI enters or leaves the trigger zone.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Head Tracking Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::HeadTrackingSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Head Tracking Settings", headTrackIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetHeadTracking", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_headTrackFadeSpeed = k_defaultHeadTrackFadeSpeed;
        IniParser::Save();
        logger::info("Head Tracking settings reset to default values.");

    }

    HelpTooltip("Restores all head tracking settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Head Tracking Fade Speed
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##fadeSpeedCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", speedIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Head Tracking Fade Speed");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string valueText = std::format("{:.2f} units/s", g_headTrackFadeSpeed);
    ImGuiMCP::ImVec2 valueTextSize;
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.2f units/s", g_headTrackFadeSpeed);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Fade Speed: %.2f units/s", g_headTrackFadeSpeed);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##headTrackFadeSpeed", &g_headTrackFadeSpeed, 0.1f, 1.0f, "%.2f units/s")) {

        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("How quickly the player's head-tracking rotates toward a focused POI.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  POI System - Main Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::POISystemMainSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s POI System - Main Settings", poiSystemIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetPoiGeneral", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_poiSystemEnabled = k_defaultPoiSystemEnabled;
        g_actorPoiEnabled = k_defaultActorPoiEnabled;
        g_preventFollowers = k_defaultPreventFollowers;
        g_flyingCritterPoiEnabled = k_defaultFlyingCritterPoiEnabled;
        g_fishPoiEnabled = k_defaultFishPoiEnabled;
        g_poiDetectionRadius = k_defaultPoiDetectionRadius;
        g_lockDuration = k_defaultLockDuration;
        IniParser::Save();
        logger::info("POI System Main Settings reset to default values.");

    }

    HelpTooltip("Restores all general POI system settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  POI System Master Toggle
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##masterToggleCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", poiSystemIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" POI System Master Toggle");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string statusText = g_poiSystemEnabled ? "Enabled" : "Disabled";
    ImGuiMCP::ImVec2 statusTextSize;
    ImGuiMCP::CalcTextSize(&statusTextSize, statusText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - statusTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, g_poiSystemEnabled ? HexToImVec4(0x2ecc71) : HexToImVec4(0xe74c3c));
    ImGuiMCP::Text("%s", statusText.c_str());
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("POI System: %s", statusText.c_str());
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Checkbox
    if (ImGuiMCP::Checkbox("##poiSystemMasterToggle", &g_poiSystemEnabled)) {

        IniParser::Save();
        logger::info("POI System master toggle set to: {}", g_poiSystemEnabled);

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Master toggle for the entire POI (Point of Interest) system. When disabled, the camera will not track any POIs automatically.");
    ImGuiMCP::PopStyleColor();

    // If master toggle is disabled, show message inside the same card with red text
    if (!g_poiSystemEnabled) {

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 8.0f));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xe74c3c));
        ImGuiMCP::Text("POI System is currently disabled. Toggle it on to configure settings.");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // If master toggle is disabled, hide everything else below
    if (!g_poiSystemEnabled) {

        // Pop styling
        ImGuiMCP::PopStyleColor(2);
        ImGuiMCP::PopStyleVar(2);
        return;

    }

    // =====================================================================================================================
    //  Enabled POI Types Section
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##poiTypesCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", poiTypesIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Enabled POI Types");

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));  // Small padding after separator

    // Actor POI toggle
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", personIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

    if (ImGuiMCP::Checkbox("##actorPoiToggle", &g_actorPoiEnabled)) {

        IniParser::Save();
        logger::debug("Actor POI toggle set to: {}", g_actorPoiEnabled);

    }

    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);
    ImGuiMCP::Text("Actors (NPCs, creatures)");
    HelpTooltip(
        "When enabled, the system will detect and track actors (NPCs, creatures)\n"
        "as points of interest based on their current action state (combat, moving, etc)."
    );

    // Prevent Followers (nested under Actor POI)
    if (g_actorPoiEnabled) {

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));  // Small spacing before nested
        ImGuiMCP::Indent(40.0f);

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
        ImGuiMCP::Text("%s", followerIcon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        if (ImGuiMCP::Checkbox("##preventFollowers", &g_preventFollowers)) {

            IniParser::Save();
            logger::debug("Prevent Followers toggle set to: {}", g_preventFollowers);

        }

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);
        ImGuiMCP::Text("Prevent Followers");
        HelpTooltip(
            "When enabled, followers (NPCs that are in the follower faction)\n"
            "will NOT be targeted by the POI system."
        );

        ImGuiMCP::Unindent(40.0f);

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));  // ← SPACING BETWEEN ITEMS

    // Flying Critter POI toggle
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xf566dd));
    ImGuiMCP::Text("%s", flyingCritterIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

    if (ImGuiMCP::Checkbox("##flyingCritterPoiToggle", &g_flyingCritterPoiEnabled)) {

        IniParser::Save();
        logger::debug("Flying Critter POI toggle set to: {}", g_flyingCritterPoiEnabled);

    }

    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);
    ImGuiMCP::Text("Flying Critters (butterflies, moths, dragonflies, etc)");
    HelpTooltip(
        "When enabled, the system will detect and track flying critters\n"
        "(butterflies, moths, dragonflies, fireflies, bees, etc)."
    );

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));  // ← SPACING BETWEEN ITEMS

    // Fish Critter POI toggle
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x1ff0ff));
    ImGuiMCP::Text("%s", fishCritterIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

    if (ImGuiMCP::Checkbox("##fishPoiToggle", &g_fishPoiEnabled)) {

        IniParser::Save();
        logger::debug("Fish Critter POI toggle set to: {}", g_fishPoiEnabled);

    }

    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);
    ImGuiMCP::Text("Fish Critters (perches, salmon, pond fish, etc)");
    HelpTooltip(
        "When enabled, the system will detect and track fish critters\n"
        "(perches, salmon, pond fish, and other aquatic critters)."
    );

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));  // Bottom padding

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Maximum Detection Radius
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##radiusCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", radiusIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Maximum Detection Radius");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    float poiRadiusMeters = g_poiDetectionRadius / 70.0f;
    std::string valueText = std::format("{:.1f} m", poiRadiusMeters);
    ImGuiMCP::ImVec2 valueTextSize;
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.1f m", poiRadiusMeters);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Detection Radius: %.1f m (%.0f units)", poiRadiusMeters, g_poiDetectionRadius);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderFloat("##poiDetectionRadius", &poiRadiusMeters, 0.0f, 100.0f, "%.1f m")) {

        g_poiDetectionRadius = poiRadiusMeters * 70.0f;
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Maximum POI detection range from the player.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Minimum Lock Duration
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##lockCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", poiLockIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Minimum Lock Duration");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    valueText = std::format("{:.1f} sec", g_lockDuration);
    ImGuiMCP::CalcTextSize(&valueTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - valueTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.1f sec", g_lockDuration);
    ImGuiMCP::PopStyleColor();

    // Tooltip for current value
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Lock Duration: %.1f seconds", g_lockDuration);
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);
    if (ImGuiMCP::SliderFloat("##lockDuration", &g_lockDuration, 0.0f, 30.0f, "%.1f sec")) {
        IniParser::Save();
    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Minimum time the camera stays locked onto a point of interest before it can switch to another.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  POI System - Exclusion List
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::POISystemExclusionListSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s POI System - Exclusion List", FontAwesome::UnicodeToUtf8(0xf023).c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Clear All Exclusions##resetPoiExclusion", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_actorExclusionList.clear();
        IniParser::Save();

    }

    HelpTooltip("Removes all actors from the exclusion list.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Card 1: Add Actor to Exclusion List
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##addActorCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", addIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Add Actor to Exclusion List");

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // ---------------------------------------------------------------------------------------------------------------------
    //  Add selected actor logic
    // ---------------------------------------------------------------------------------------------------------------------

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

    // ---------------------------------------------------------------------------------------------------------------------
    //  Display appropriate message based on selection state
    // ---------------------------------------------------------------------------------------------------------------------

    if (!hasSelectedActor && !selectedActor && !isNotActor) {

        // No actor selected
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
        ImGuiMCP::Text("%s To exclude an actor, open the console, click on the actor, then use the button below to add them.", infoIcon.c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Button("Add Selected Actor: [No actor selected]");
        ImGuiMCP::PopStyleColor(2);
        HelpTooltip("Select an actor with the console first.");

    }
    else if (isNotActor) {

        // Selected object is not an actor
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xe74c3c));
        ImGuiMCP::Text("%s The currently selected object is not an actor.", FontAwesome::UnicodeToUtf8(0xf05e).c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Button("Add Selected Actor: [No actor selected]");
        ImGuiMCP::PopStyleColor(2);
        HelpTooltip("The selected object with the console is not an actor.");

    } else if (isPlayer) {

        // Player cannot be excluded
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xe74c3c));
        ImGuiMCP::Text("%s The player cannot be excluded.", FontAwesome::UnicodeToUtf8(0xf05e).c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Disabled button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        const char* playerName = selectedActor->GetName();
        std::string buttonText = std::string("Add Selected Actor: ") + (playerName ? playerName : "Player");
        ImGuiMCP::Button(buttonText.c_str());
        ImGuiMCP::PopStyleColor(2);
        HelpTooltip("The player cannot be excluded from the POI System.");

    } else if (isExcluded) {

        // Already excluded - show warning message and gray button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xf39c12));
        ImGuiMCP::Text("%s Already Excluded: %s", FontAwesome::UnicodeToUtf8(0xf05e).c_str(), selectedActor->GetName() ? selectedActor->GetName() : "Unnamed");
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

        // Grayed out button
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, ImGuiMCP::ImVec4{ 0.3f, 0.3f, 0.35f, 1.0f });
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        const char* actorName = selectedActor->GetName();
        std::string buttonText = std::string("Already Excluded: ") + (actorName ? actorName : "Unnamed");
        ImGuiMCP::Button(buttonText.c_str());
        ImGuiMCP::PopStyleColor(2);
        HelpTooltip("The selected actor is already excluded.");

    } else {

        // Valid actor to add
        const char* actorName = selectedActor->GetName();
        std::string buttonText = std::string("Add Selected Actor: ") + (actorName ? actorName : "Unnamed");

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4(0x2ecc71));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, HexToImVec4(0x27ae60));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, HexToImVec4(0x1e8449));

        if (ImGuiMCP::Button(buttonText.c_str())) {

            AddActorToExclusionList(selectedActor);

        }

        ImGuiMCP::PopStyleColor(3);
        HelpTooltip("Adds the currently selected actor to the exclusion list.");

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Card 2: Excluded Actors List
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##excludedListCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and count
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", personIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Excluded Actors (%zu)", g_actorExclusionList.size());

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // ---------------------------------------------------------------------------------------------------------------------
    //  Display exclusion list
    // ---------------------------------------------------------------------------------------------------------------------

    if (g_actorExclusionList.empty()) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Text("%s No actors in exclusion list.", FontAwesome::UnicodeToUtf8(0xf07c).c_str());
        ImGuiMCP::PopStyleColor();

        ImGuiMCP::EndChild();
        ImGuiMCP::PopStyleColor(2);
        ImGuiMCP::PopStyleVar(2);
        return;

    }

    // ---------------------------------------------------------------------------------------------------------------------
    //  Build a sorted copy of the exclusion list
    // ---------------------------------------------------------------------------------------------------------------------

    std::vector<ActorExclusionEntry> sortedList = g_actorExclusionList;

    std::sort(sortedList.begin(), sortedList.end(), [](const ActorExclusionEntry& a, const ActorExclusionEntry& b) {
        std::string aName = GetActorNameFromFormID(a.formID);
        std::string bName = GetActorNameFromFormID(b.formID);
        std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
        std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);
        return aName < bName;
        }
    );

    // Find the selected actor in the sorted list
    size_t selectedIndex = SIZE_MAX;

    if (selectedActor && !selectedActor->IsPlayerRef()) {

        RE::FormID selectedFormID = 0;
        auto* actorBase = selectedActor->GetActorBase();

        if (actorBase) {

            selectedFormID = actorBase->GetFormID();

            if ((selectedFormID & 0xFF000000) == 0xFF000000) {

                auto* rootTemplate = actorBase->GetRootFaceNPC();

                if (rootTemplate && rootTemplate != actorBase) {

                    RE::FormID rootID = rootTemplate->GetFormID();

                    if ((rootID & 0xFF000000) != 0xFF000000) {

                        selectedFormID = rootID;

                    }

                }

                if ((selectedFormID & 0xFF000000) == 0xFF000000) {

                    auto* templateNPC = actorBase->faceNPC;

                    if (templateNPC) {

                        RE::FormID faceID = templateNPC->GetFormID();

                        if ((faceID & 0xFF000000) != 0xFF000000) {

                            selectedFormID = faceID;

                        }

                    }

                }

            }

        }

        for (size_t i = 0; i < sortedList.size(); ++i) {

            if (sortedList[i].formID == selectedFormID) {

                selectedIndex = i;
                break;

            }

        }

    }

    // Move selected actor to the top
    if (selectedIndex != SIZE_MAX && selectedIndex > 0) {

        ActorExclusionEntry selectedEntry = sortedList[selectedIndex];
        sortedList.erase(sortedList.begin() + selectedIndex);
        sortedList.insert(sortedList.begin(), selectedEntry);

    }

    // ---------------------------------------------------------------------------------------------------------------------
    //  Scrollable list
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::BeginChild("ExcludedActorsList", ImGuiMCP::ImVec2(0.0f, 600.0f), ImGuiMCP::ImGuiChildFlags_Border);

    for (size_t i = 0; i < sortedList.size(); ++i) {

        auto& entry = sortedList[i];
        std::string actorName = GetActorNameFromFormID(entry.formID);
        bool isSelected = (selectedIndex != SIZE_MAX && i == 0 && selectedActor != nullptr);

        ImGuiMCP::PushID(static_cast<int>(i));

        // Entry background with rounded corners
        if (isSelected) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.2f, 0.4f, 0.8f, 0.3f });
            ImGuiMCP::BeginChild(ImGuiMCP::GetID("Highlight"), ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

        }

        ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

        // Delete button (first thing on the left)
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Button, HexToImVec4(0xe74c3c));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonHovered, HexToImVec4(0xc0392b));
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ButtonActive, HexToImVec4(0x922b21));

        // Find original index
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
                ImGuiMCP::PopStyleColor(2);
                ImGuiMCP::PopStyleVar(2);
                return;

            }

        }

        ImGuiMCP::PopStyleColor(3);

        ImGuiMCP::SameLine();
        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        // Icon
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
        ImGuiMCP::Text("%s", personIcon.c_str());
        ImGuiMCP::PopStyleColor();
        ImGuiMCP::SameLine();

        ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 4.0f);

        // Actor name (bold if selected)
        if (isSelected) {

            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
            ImGuiMCP::Text("%s", actorName.c_str());
            ImGuiMCP::PopStyleColor();

        } else {

            ImGuiMCP::Text("%s", actorName.c_str());

        }

        ImGuiMCP::SameLine();

        // Form ID (muted)
        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.5f, 0.5f, 0.5f, 1.0f });
        ImGuiMCP::Text("(%08X)", entry.formID);
        ImGuiMCP::PopStyleColor();

        // "Currently selected in console" in brackets
        if (isSelected) {

            ImGuiMCP::SameLine();
            ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
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

    ImGuiMCP::EndChild();  // End scrollable list

    ImGuiMCP::EndChild();  // End Excluded Actors card

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  POI System - Actor Scores
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::POISystemActorScores() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("POI System - Actor Scores");
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset the actor score settings back to default
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::ImVec2 availWidthVec;
    ImGuiMCP::GetContentRegionAvail(&availWidthVec);
    float availWidth = availWidthVec.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetPoiActor", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_dragonScore = k_defaultDragonScore;
        g_dragonProximityEnabled = k_defaultDragonProximityEnabled;
        g_dragonProximityFactor = k_defaultDragonProximityFactor;

        g_actorCombatScore = k_defaultActorCombatScore;
        g_actorCombatProximityEnabled = k_defaultActorCombatProximityEnabled;
        g_actorCombatProximityFactor = k_defaultActorCombatProximityFactor;

        g_actorMovingScore = k_defaultActorMovingScore;
        g_actorMovingProximityEnabled = k_defaultActorMovingProximityEnabled;
        g_actorMovingProximityFactor = k_defaultActorMovingProximityFactor;

        g_actorInSceneScore = k_defaultActorInSceneScore;
        g_actorInSceneProximityEnabled = k_defaultActorInSceneProximityEnabled;
        g_actorInSceneProximityFactor = k_defaultActorInSceneProximityFactor;

        g_actorIdleScore = k_defaultActorIdleScore;
        g_actorIdleProximityEnabled = k_defaultActorIdleProximityEnabled;
        g_actorIdleProximityFactor = k_defaultActorIdleProximityFactor;

        IniParser::Save();

    }

    HelpTooltip("Restores all actor score settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Actor Score System - Card Based Layout
    // =====================================================================================================================

    // Card styling
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Dragon Card
    // =====================================================================================================================

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.5f));
    ImGuiMCP::BeginChild("##dragonCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", dragonScoreIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Dragon");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    // Calculate total score (base + proximity bonus if enabled)
    float dragonTotalScore = g_dragonScore + (g_dragonProximityEnabled ? g_dragonProximityFactor : 0.0f);
    std::string scoreText = std::format("{:.0f}", dragonTotalScore);
    ImGuiMCP::ImVec2 scoreTextSize;
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", dragonTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", dragonTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_dragonScore);

        if (g_dragonProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_dragonProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to a dragon.");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    if (ImGuiMCP::SliderFloat("##dragonBaseScore", &g_dragonScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##dragonProxToggle", &g_dragonProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the dragon is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_dragonProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##dragonProxFactor", &g_dragonProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();  // Save when slider changes

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to a dragon.");

    if (g_dragonProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as dragon gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  In Combat Card
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##combatCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", inCombatScoreIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" In Combat");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    float combatTotalScore = g_actorCombatScore + (g_actorCombatProximityEnabled ? g_actorCombatProximityFactor : 0.0f);
    scoreText = std::format("{:.0f}", combatTotalScore);
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", combatTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", combatTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_actorCombatScore);

        if (g_actorCombatProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_actorCombatProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to an actor who is currently in combat.");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    if (ImGuiMCP::SliderFloat("##combatBaseScore", &g_actorCombatScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##combatProxToggle", &g_actorCombatProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the actor is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_actorCombatProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##combatProxFactor", &g_actorCombatProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to an actor who is currently in combat.");

    if (g_actorCombatProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as actor gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Moving Card
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##movingCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", movingScoreIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Moving");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    float movingTotalScore = g_actorMovingScore + (g_actorMovingProximityEnabled ? g_actorMovingProximityFactor : 0.0f);
    scoreText = std::format("{:.0f}", movingTotalScore);
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", movingTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", movingTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_actorMovingScore);

        if (g_actorMovingProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_actorMovingProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to an actor who is currently moving.");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });
    if (ImGuiMCP::SliderFloat("##movingBaseScore", &g_actorMovingScore, 0.0f, 2000.0f, "%.0f")) {
        IniParser::Save();
    }
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##movingProxToggle", &g_actorMovingProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the actor is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_actorMovingProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##movingProxFactor", &g_actorMovingProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to an actor who is currently moving.");

    if (g_actorMovingProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as actor gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  In Scene Card
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##sceneCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", inSceneScoreIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" In Scene");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    float sceneTotalScore = g_actorInSceneScore + (g_actorInSceneProximityEnabled ? g_actorInSceneProximityFactor : 0.0f);
    scoreText = std::format("{:.0f}", sceneTotalScore);
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", sceneTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", sceneTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_actorInSceneScore);

        if (g_actorInSceneProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_actorInSceneProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to an actor who is actively engaged in a scripted sequence (dialogue, cinematic, or quest scene)");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    if (ImGuiMCP::SliderFloat("##sceneBaseScore", &g_actorInSceneScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##sceneProxToggle", &g_actorInSceneProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the actor is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_actorInSceneProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##sceneProxFactor", &g_actorInSceneProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to an actor who is actively engaged in a scripted sequence (dialogue, cinematic, or quest scene)");

    if (g_actorInSceneProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as actor gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Idle Card
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##idleCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x7ef566));
    ImGuiMCP::Text("%s", personIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Idle");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    float idleTotalScore = g_actorIdleScore + (g_actorIdleProximityEnabled ? g_actorIdleProximityFactor : 0.0f);
    scoreText = std::format("{:.0f}", idleTotalScore);
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", idleTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", idleTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_actorIdleScore);

        if (g_actorIdleProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_actorIdleProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to an actor who is in a idle animation (not moving, not in combat and not in a scene).");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    if (ImGuiMCP::SliderFloat("##idleBaseScore", &g_actorIdleScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##idleProxToggle", &g_actorIdleProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the actor is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_actorIdleProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##idleProxFactor", &g_actorIdleProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to an actor who is in a idle animation (not moving, not in combat and not in a scene).");

    if (g_actorIdleProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as actor gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    // Pop card styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  POI System - Critter Scores
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::POISystemCritterScores() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("POI System - Critter Scores");
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset the critter score settings back to default
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::ImVec2 availWidthVec;
    ImGuiMCP::GetContentRegionAvail(&availWidthVec);
    float availWidth = availWidthVec.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetPoiCritter", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_flyingCritterScore = k_defaultFlyingCritterScore;
        g_flyingCritterProximityEnabled = k_defaultFlyingCritterProximityEnabled;
        g_flyingCritterProximityFactor = k_defaultFlyingCritterProximityFactor;

        g_pondFishScore = k_defaultFishScore;
        g_pondFishProximityEnabled = k_defaultFishProximityEnabled;
        g_pondFishProximityFactor = k_defaultFishProximityFactor;

        IniParser::Save();

    }

    HelpTooltip("Restores all critter score settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Critter Score System - Card Based Layout
    // =====================================================================================================================

    // Card styling
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Flying Critter Card
    // =====================================================================================================================

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.5f));
    ImGuiMCP::BeginChild("##flyingCritterCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xf566dd));
    ImGuiMCP::Text("%s", flyingCritterIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Flying Critter");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    // Calculate total score (base + proximity bonus if enabled)
    float flyingTotalScore = g_flyingCritterScore + (g_flyingCritterProximityEnabled ? g_flyingCritterProximityFactor : 0.0f);
    std::string scoreText = std::format("{:.0f}", flyingTotalScore);
    ImGuiMCP::ImVec2 scoreTextSize;
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", flyingTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", flyingTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_flyingCritterScore);

        if (g_flyingCritterProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_flyingCritterProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");
        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to a flying critter (butterflies, moths, dragonflies, etc).");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    if (ImGuiMCP::SliderFloat("##flyingCritterBaseScore", &g_flyingCritterScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##flyingCritterProxToggle", &g_flyingCritterProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the critter is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_flyingCritterProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##flyingCritterProxFactor", &g_flyingCritterProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to a flying critter (butterflies, moths, dragonflies, etc).");

    if (g_flyingCritterProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as critter gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Pond Fish Card
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##pondFishCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0x1ff0ff));
    ImGuiMCP::Text("%s", fishCritterIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Pond Fish");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);

    float fishTotalScore = g_pondFishScore + (g_pondFishProximityEnabled ? g_pondFishProximityFactor : 0.0f);
    scoreText = std::format("{:.0f}", fishTotalScore);
    ImGuiMCP::CalcTextSize(&scoreTextSize, scoreText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - scoreTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%.0f", fishTotalScore);
    ImGuiMCP::PopStyleColor();

    // Tooltip for total score
    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Total Score: %.0f", fishTotalScore);
        ImGuiMCP::Text("Base Score: %.0f", g_pondFishScore);

        if (g_pondFishProximityEnabled) {

            ImGuiMCP::Text("Proximity Bonus: +%.0f", g_pondFishProximityFactor);

        } else {

            ImGuiMCP::Text("Proximity Bonus: Disabled");

        }

        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Base Score
    ImGuiMCP::Text("Base Score");
    HelpTooltip("Base score awarded to a fish critter (perches, salmon, etc).");
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.7f, 1.0f, 1.0f });

    if (ImGuiMCP::SliderFloat("##pondFishBaseScore", &g_pondFishScore, 0.0f, 2000.0f, "%.0f")) {

        IniParser::Save();

    }

    ImGuiMCP::PopStyleColor();

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Proximity Bonus Section
    ImGuiMCP::Text("Proximity Bonus");
    ImGuiMCP::SameLine();
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + 10.0f);

    if (ImGuiMCP::Checkbox("##pondFishProxToggle", &g_pondFishProximityEnabled)) {

        IniParser::Save();

    }

    HelpTooltip("How much score is added the closer the critter is to the player (max bonus at point-blank range, 0 at the detection radius).");

    if (g_pondFishProximityEnabled) {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_PlotHistogram, ImGuiMCP::ImVec4{ 0.4f, 0.9f, 0.4f, 1.0f });

        if (ImGuiMCP::SliderFloat("##pondFishProxFactor", &g_pondFishProximityFactor, 0.0f, 1000.0f, "+%.0f")) {

            IniParser::Save();

        }

        ImGuiMCP::PopStyleColor();

    } else {

        ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.4f, 0.4f, 0.4f, 1.0f });
        ImGuiMCP::Text("+0 (disabled)");
        ImGuiMCP::PopStyleColor();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text - Always visible
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Base score awarded to a fish critter (perches, salmon, etc).");

    if (g_pondFishProximityEnabled) {

        ImGuiMCP::Text("Bonus increases as critter gets closer.");

    }

    ImGuiMCP::PopStyleColor();
    ImGuiMCP::EndChild();

    // Pop card styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//  Debug Settings
// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void UI::DebugSettings() {

    FontAwesome::PushSolid();

    // Header with Reset button at top right
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::ImVec2 avail;
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s Debug Settings", debugIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();

    // ---------------------------------------------------------------------------------------------------------------------
    //  Reset button at top right
    // ---------------------------------------------------------------------------------------------------------------------

    ImGuiMCP::GetContentRegionAvail(&avail);
    float availWidth = avail.x;

    std::string resetButtonText = std::format("{} Reset To Default##resetDebug", resetIcon);
    ImGuiMCP::ImVec2 resetTextSize;
    ImGuiMCP::CalcTextSize(&resetTextSize, resetButtonText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + availWidth - resetTextSize.x);

    if (ImGuiMCP::Button(resetButtonText.c_str())) {

        g_debugRaycasts = k_defaultDebugRaycasts;
        g_loggingLevel = k_defaultLoggingLevel;

        auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
        spdlog::set_level(lvl);
        spdlog::flush_on(lvl);
        logger::info("Logging level reset to default ('{}')", k_loggingLevelNames[g_loggingLevel]);

        IniParser::Save();

    }

    HelpTooltip("Restores all debug settings on this page to their default values.");

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 5.0f));
    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 15.0f));

    // =====================================================================================================================
    //  Mini-Card Styling
    // =====================================================================================================================

    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildRounding, 8.0f);
    ImGuiMCP::PushStyleVar(ImGuiMCP::ImGuiStyleVar_ChildBorderSize, 1.0f);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_ChildBg, ImGuiMCP::ImVec4{ 0.15f, 0.15f, 0.18f, 1.0f });
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Border, ImGuiMCP::ImVec4{ 0.25f, 0.25f, 0.30f, 1.0f });

    // =====================================================================================================================
    //  Debug Raycast Visualization
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##raycastCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", raycastIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Debug Raycast Visualization");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string statusText = g_debugRaycasts ? "Enabled" : "Disabled";
    ImGuiMCP::ImVec2 statusTextSize;
    ImGuiMCP::CalcTextSize(&statusTextSize, statusText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - statusTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, g_debugRaycasts ? HexToImVec4(0x2ecc71) : HexToImVec4(0xe74c3c));
    ImGuiMCP::Text("%s", statusText.c_str());
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Debug Raycast Visualization: %s", statusText.c_str());
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Checkbox
    if (ImGuiMCP::Checkbox("##debugRaycasts", &g_debugRaycasts)) {

        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text("Draws debug raycast lines used by the POI detection system, for troubleshooting purposes.");
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 10.0f));

    // =====================================================================================================================
    //  Logging Level
    // =====================================================================================================================

    ImGuiMCP::BeginChild("##loggingCard", ImGuiMCP::ImVec2(0.0f, 0.0f), ImGuiMCP::ImGuiChildFlags_Border | ImGuiMCP::ImGuiChildFlags_AutoResizeY);

    // Header with icon and current value
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, HexToImVec4(0xF5D966));
    ImGuiMCP::Text("%s", loggingIcon.c_str());
    ImGuiMCP::PopStyleColor();
    ImGuiMCP::SameLine();
    ImGuiMCP::Text(" Logging Level");
    ImGuiMCP::SameLine();
    ImGuiMCP::GetContentRegionAvail(&avail);
    std::string valueText = k_loggingLevelNames[g_loggingLevel];
    ImGuiMCP::CalcTextSize(&statusTextSize, valueText.c_str(), nullptr, false, -1.0f);
    ImGuiMCP::SetCursorPosX(ImGuiMCP::GetCursorPosX() + avail.x - statusTextSize.x);
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 1.0f, 0.85f, 0.4f, 1.0f });
    ImGuiMCP::Text("%s", valueText.c_str());
    ImGuiMCP::PopStyleColor();

    if (ImGuiMCP::IsItemHovered()) {

        ImGuiMCP::BeginTooltip();
        ImGuiMCP::Text("Current Logging Level: %s", valueText.c_str());
        ImGuiMCP::EndTooltip();

    }

    ImGuiMCP::Separator();
    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 4.0f));

    // Slider
    ImGuiMCP::SetNextItemWidth(-1.0f);

    if (ImGuiMCP::SliderInt("##loggingLevel", &g_loggingLevel, 0, k_loggingLevelCount - 1, k_loggingLevelNames[g_loggingLevel])) {

        g_loggingLevel = std::clamp(g_loggingLevel, 0, k_loggingLevelCount - 1);

        auto lvl = LoggingLevelToSpdlog(g_loggingLevel);
        spdlog::set_level(lvl);
        spdlog::flush_on(lvl);
        logger::info("Logging level manually set to '{}'", k_loggingLevelNames[g_loggingLevel]);
        IniParser::Save();

    }

    ImGuiMCP::Dummy(ImGuiMCP::ImVec2(0.0f, 2.0f));

    // Helper text
    ImGuiMCP::PushStyleColor(ImGuiMCP::ImGuiCol_Text, ImGuiMCP::ImVec4{ 0.6f, 0.6f, 0.7f, 1.0f });
    ImGuiMCP::Text(
        "Controls how much detail is written to the log file:\n"
        "\n"
        "Quiet - only critical errors that prevent the mod from working.\n"
        "\n"
        "Warnings - critical errors plus non-fatal issues worth noticing.\n"
        "\n"
        "Info - warnings plus general status messages.\n"
        "\n"
        "Debug - everything, including detailed internal values for troubleshooting."
    );
    ImGuiMCP::PopStyleColor();

    ImGuiMCP::EndChild();

    // Pop styling
    ImGuiMCP::PopStyleColor(2);
    ImGuiMCP::PopStyleVar(2);

}