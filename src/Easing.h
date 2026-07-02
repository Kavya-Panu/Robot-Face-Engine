// =============================================================================
//  Easing.h  (header-only)
//  Reusable easing curves, linear interpolation, and a small Tween class used
//  everywhere in the engine so that NOTHING ever teleports - every value that
//  changes moves through an eased curve over time.
// =============================================================================
#pragma once
#include <Arduino.h>

namespace Ease {

// ---- Basic interpolation ---------------------------------------------------
// Linear interpolation between a and b by t in [0,1].
static inline float lerp(float a, float b, float t) { return a + (b - a) * t; }

// Clamp helper.
static inline float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

// ---- Easing curves (all take t in [0,1], return eased value in [0,1]) -------

// Smooth acceleration and deceleration (the workhorse for pupil movement).
static inline float inOutQuad(float t) {
    return t < 0.5f ? 2.0f * t * t : 1.0f - powf(-2.0f * t + 2.0f, 2.0f) / 2.0f;
}

// Stronger ease-in/out for expressive transitions.
static inline float inOutCubic(float t) {
    return t < 0.5f ? 4.0f * t * t * t
                    : 1.0f - powf(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

// Decelerate only (good for eyes "arriving" at a target).
static inline float outCubic(float t) {
    float f = 1.0f - t;
    return 1.0f - f * f * f;
}

// Slight overshoot then settle - gives a lively, springy feel (wake up, pop).
static inline float outBack(float t) {
    const float c1 = 1.70158f, c3 = c1 + 1.0f;
    float f = t - 1.0f;
    return 1.0f + c3 * f * f * f + c1 * f * f;
}

// Cubic Bezier Y for x==t approximation via control points p1y,p2y (p0=0,p3=1).
// Cheap enough for per-frame use; good for custom transition shaping.
static inline float bezier(float t, float p1y, float p2y) {
    float u = 1.0f - t;
    return 3.0f * u * u * t * p1y + 3.0f * u * t * t * p2y + t * t * t;
}

// =============================================================================
//  Tween - animates one float from `from` to `to` over `durationMs`.
//  Non-blocking: call update(dtMs) each frame and read value().
// =============================================================================
class Tween {
public:
    // Curve selector so callers can pick a feel per transition.
    enum Curve { QUAD, CUBIC, OUT_CUBIC, OUT_BACK };

    Tween() {}

    // Begin a new tween from the current value to `target`.
    void start(float from, float to, uint32_t durationMs, Curve c = QUAD) {
        _from = from; _to = to; _cur = from;
        _dur = (durationMs < 1) ? 1 : durationMs;
        _elapsed = 0; _curve = c; _active = true;
    }

    // Retarget while keeping the present value as the new start (smooth redirect).
    void retarget(float to, uint32_t durationMs, Curve c = QUAD) {
        start(_cur, to, durationMs, c);
    }

    // Advance by dt milliseconds.
    void update(float dtMs) {
        if (!_active) return;
        _elapsed += (uint32_t)dtMs;
        float t = (float)_elapsed / (float)_dur;
        if (t >= 1.0f) { t = 1.0f; _active = false; }
        _cur = lerp(_from, _to, shape(t));
    }

    float value()  const { return _cur; }
    bool  active() const { return _active; }
    void  set(float v)   { _cur = _from = _to = v; _active = false; }

private:
    // Map raw progress through the selected easing curve.
    float shape(float t) const {
        switch (_curve) {
            case CUBIC:     return inOutCubic(t);
            case OUT_CUBIC: return outCubic(t);
            case OUT_BACK:  return outBack(t);
            default:        return inOutQuad(t);
        }
    }
    float _from = 0, _to = 0, _cur = 0;
    uint32_t _dur = 1, _elapsed = 0;
    Curve _curve = QUAD;
    bool _active = false;
};

} // namespace Ease
