// =============================================================================
//  EmotionManager.h
//  The emotional state machine. Each emotion is a preset EyeParams plus a few
//  behaviour hints (blink rate, gaze energy). setEmotion() does not snap - it
//  tweens every shape parameter from the current look to the target look, so
//  emotions cross-fade smoothly. update() produces the current interpolated
//  EyeParams that the renderer uses this frame.
// =============================================================================
#pragma once
#include "EyeState.h"
#include "Easing.h"

class EmotionManager {
public:
    // Full emotion set (superset of the enum in your spec; extra states reuse
    // the same preset machinery).
    enum Emotion {
        NORMAL, HAPPY, EXCITED, SAD, ANGRY, SURPRISED, CONFUSED,
        SLEEPY, SLEEP, CURIOUS, LOVE, LISTENING, THINKING, SEARCHING, WORRIED, ERROR
    };

    EmotionManager();

    // Request a new emotion. duration = cross-fade time in ms.
    void setEmotion(Emotion e, uint32_t durationMs = 350);

    // Advance the cross-fade.
    void update(float dtMs);

    Emotion current() const { return _target; }

    // Resolved shape for this frame, applied equally to both eyes (per-eye
    // asymmetry like CONFUSED is layered on later by the AnimationManager).
    const EyeParams& params() const { return _live; }

    // Behaviour hints derived from the current emotion.
    float blinkRateScale() const { return _blinkScale; } // >1 slower blinks
    float gazeEnergy()     const { return _gazeEnergy; } // 0..1 idle liveliness

private:
    // Look up the preset EyeParams + behaviour for an emotion.
    static EyeParams preset(Emotion e);
    static void      behaviour(Emotion e, float& blinkScale, float& gazeEnergy);

    EyeParams _from, _to, _live;
    Emotion   _target = NORMAL;
    Ease::Tween _mix;          // 0..1 blend between _from and _to
    float _blinkScale = 1.0f;
    float _gazeEnergy = 1.0f;
};
