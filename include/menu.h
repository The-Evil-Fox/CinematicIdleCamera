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

    extern float g_idleTimer;
    extern float g_blendDuration;
    extern float g_IdleCamOffsetX;
    extern float g_IdleCamOffsetY;
    extern float g_IdleCamOffsetZ;
    extern float g_dezoomTriggerRadius;
    extern float g_dezoomTriggerHeight;
    extern float g_dezoomAmount;
    extern float g_dezoomBlendSpeed;

    // ---------------------------------------------------------------------------------------------------------------------
    //  Head Tracking
    // ---------------------------------------------------------------------------------------------------------------------

    extern float g_headTrackFadeSpeed;

    // ---------------------------------------------------------------------------------------------------------------------
    //  POI System
    // ---------------------------------------------------------------------------------------------------------------------

    // Main Settings

    extern float g_poiDetectionRadius;
    extern float g_lockDuration;

    // Actors

    extern float g_actorCombatScore;
    extern bool  g_actorCombatProximityEnabled;
    extern float g_actorCombatProximityFactor;

    extern float g_actorMovingScore;
    extern bool  g_actorMovingProximityEnabled;
    extern float g_actorMovingProximityFactor;

    extern float g_actorInSceneScore;
    extern bool  g_actorInSceneProximityEnabled;
    extern float g_actorInSceneProximityFactor;

    extern float g_actorIdleScore;
    extern bool  g_actorIdleProximityEnabled;
    extern float g_actorIdleProximityFactor;

    // Critters

    extern float g_flyingCritterScore;
    extern bool  g_flyingCritterProximityEnabled;
    extern float g_flyingCritterProximityFactor;

    // ---------------------------------------------------------------------------------------------------------------------
    //  Debug
    // ---------------------------------------------------------------------------------------------------------------------

    extern bool  g_debugRaycasts;
    extern int   g_loggingLevel;


    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //  UI Functions
    // ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    void Register();

    void CameraSettings();
    void HeadTrackingSettings();

    static void POISystemMainSettings();
    static void POISystemActorScores();
    static void POISystemCritterScores();

    void DebugSettings();
    void ResetSettings();

    void DrawCinematicBars();

}