#pragma once
#include "SKSEMenuFramework.h"
#include "logger.hpp"

namespace UI {


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  UI Settings
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // ---------------------------------------------------------------------------------------------------------------------
    //  Camera
    // ---------------------------------------------------------------------------------------------------------------------

    // Main settings

    extern float        g_idleTimer;
    extern bool         g_blackBarsEnabled;
    extern float        g_blackBarsSpeed;
    extern bool         g_blackBarsSoundEnabled;

    // Camera position

    extern float        g_IdleCamOffsetX;
    extern float        g_IdleCamOffsetY;
    extern float        g_IdleCamOffsetZ;
    extern float        g_blendDuration;

    // Zoom/Dezoom
    
    extern float        g_dezoomTriggerRadius;
    extern float        g_dezoomTriggerHeight;
    extern float        g_dezoomAmount;
    extern float        g_dezoomBlendSpeed;

    // ---------------------------------------------------------------------------------------------------------------------
    //  Head Tracking
    // ---------------------------------------------------------------------------------------------------------------------

    extern float        g_headTrackFadeSpeed;

    // ---------------------------------------------------------------------------------------------------------------------
    //  POI System
    // ---------------------------------------------------------------------------------------------------------------------

    // Main Settings

    extern bool         g_poiSystemEnabled;
    extern bool         g_actorPoiEnabled;
    extern bool         g_preventFollowers;
    extern bool         g_flyingCritterPoiEnabled;
    extern bool         g_fishCritterPoiEnabled;
    extern float        g_poiDetectionRadius;
    extern float        g_lockDuration;

    // Actors scores

    extern float        g_dragonScore;
    extern bool         g_dragonProximityEnabled;
    extern float        g_dragonProximityFactor;

    extern float        g_actorCombatScore;
    extern bool         g_actorCombatProximityEnabled;
    extern float        g_actorCombatProximityFactor;

    extern float        g_actorMovingScore;
    extern bool         g_actorMovingProximityEnabled;
    extern float        g_actorMovingProximityFactor;

    extern float        g_actorInSceneScore;
    extern bool         g_actorInSceneProximityEnabled;
    extern float        g_actorInSceneProximityFactor;

    extern float        g_actorIdleScore;
    extern bool         g_actorIdleProximityEnabled;
    extern float        g_actorIdleProximityFactor;

    // Flying critters scores

    extern float        g_flyingCritterScore;
    extern bool         g_flyingCritterProximityEnabled;
    extern float        g_flyingCritterProximityFactor;

    // Fish critters scores

    extern float        g_fishCritterScore;
    extern bool         g_fishCritterProximityEnabled;
    extern float        g_fishCritterProximityFactor;

    // Exclusion list
    struct ActorExclusionEntry {

        RE::FormID formID;

    };

    extern std::vector<ActorExclusionEntry> g_actorExclusionList;

    // ---------------------------------------------------------------------------------------------------------------------
    //  Debug
    // ---------------------------------------------------------------------------------------------------------------------

    extern bool         g_debugRaycasts;
    extern int          g_loggingLevel;


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  UI Functions
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Register();

    void                DrawCinematicBars();

    void                AddActorToExclusionList(RE::Actor* a_actor);
    void                RemoveFromActorExclusionList(size_t index);
    bool                IsActorExcluded(RE::Actor* a_actor);

    static void         CameraMainSettings();
    static void         CameraPositionSettings();
    static void         CameraZoomSettings();

    void                HeadTrackingSettings();

    static void         POISystemMainSettings();
    static void         POISystemActorScores();
    void                POISystemExclusionListSettings();
    static void         POISystemCritterScores();

    void                DebugSettings();

    void                ResetSettings();

}