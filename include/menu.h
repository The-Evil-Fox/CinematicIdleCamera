#pragma once
#include "SKSEMenuFramework.h"
#include "logger.hpp"

namespace UI {

    void Register();
    extern float g_idleTimer;
    extern float g_poiDetectionRadius;
    extern float g_lockDuration;
    extern float g_blendDuration;
    extern float g_headTrackFadeSpeed;
    extern bool  g_debugRaycasts;
    extern int   g_loggingLevel;
    void Settings();
    void DrawCinematicBars();

}