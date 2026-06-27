#pragma once
#include "SKSEMenuFramework.h"
#include "logger.hpp"

namespace UI {

    extern float g_idleTimer;
    extern float g_poiDetectionRadius;
    extern float g_lockDuration;
    extern float g_blendDuration;
    extern float g_headTrackFadeSpeed;
    extern bool  g_debugRaycasts;
    extern int   g_loggingLevel;

    void Register();
    void Settings();
    void DrawCinematicBars();

}