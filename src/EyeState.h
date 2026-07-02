// =============================================================================
//  EyeState.h
//  Shared data types describing HOW an eye should look. In the AURA style each
//  eye is a SINGLE glowing cyan shape (no white sclera / separate pupil): the
//  whole shape reshapes per emotion and slides for gaze, and squishes to a line
//  when blinking. Kept in its own header so renderer + managers share the types.
// =============================================================================
#pragma once
#include <Arduino.h>
#include "Config.h"

// Which physical eye - used for direction-aware shaping (angry/sad slants
// point toward the nose, so "inner" differs between left and right).
enum class Side : uint8_t { LEFT, RIGHT };

// Overall eye silhouette for a given emotion.
enum class EyeShapeMode : uint8_t { RECT, CIRCLE, HEART };

// -----------------------------------------------------------------------------
//  EyeParams - the STATIC shape of an eye for a given emotion. EmotionManager
//  tweens between presets of this struct so emotions cross-fade smoothly.
// -----------------------------------------------------------------------------
struct EyeParams {
    float width     = Cfg::EYE_W;       // eye width  (px)
    float height    = Cfg::EYE_H;       // eye height (px)
    float radius    = Cfg::EYE_RADIUS;  // corner radius (px)
    float openness  = 1.0f;             // 0..1 vertical opening (before blink)
    float slant     = 0.0f;             // -1..1  +angry(inner-top) / -sad(outer-top)
    float bottomArc = 0.0f;             // 0..1 carve bottom -> happy upward arc
    float topArc    = 0.0f;             // 0..1 carve top    -> sad downward arc
    EyeShapeMode shape = EyeShapeMode::RECT;
    uint16_t color  = Cfg::EYE_COLOR;   // the glow colour of the eye
};

// -----------------------------------------------------------------------------
//  EyeState - the FULLY RESOLVED eye for the current frame: emotion shape plus
//  live gaze offset (moves the whole shape), blink amount, breathing bob and
//  per-eye tilt. This is what the renderer consumes.
// -----------------------------------------------------------------------------
struct EyeState {
    EyeParams p;             // resolved shape for this frame
    Side  side = Side::LEFT; // which eye
    float gazeX = 0;         // whole-eye offset from centre (px)
    float gazeY = 0;
    float blink = 1.0f;      // 1 = open, 0 = shut (multiplies openness)
    float bob   = 0;         // vertical breathing / tilt offset (px)
};
