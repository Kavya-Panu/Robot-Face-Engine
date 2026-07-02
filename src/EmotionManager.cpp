// =============================================================================
//  EmotionManager.cpp  (AURA cyan-blob presets)
// =============================================================================
#include "EmotionManager.h"

// Blend two EyeParams by t in [0,1].
static EyeParams blend(const EyeParams& a, const EyeParams& b, float t) {
    EyeParams o;
    o.width     = Ease::lerp(a.width,     b.width,     t);
    o.height    = Ease::lerp(a.height,    b.height,    t);
    o.radius    = Ease::lerp(a.radius,    b.radius,    t);
    o.openness  = Ease::lerp(a.openness,  b.openness,  t);
    o.slant     = Ease::lerp(a.slant,     b.slant,     t);
    o.bottomArc = Ease::lerp(a.bottomArc, b.bottomArc, t);
    o.topArc    = Ease::lerp(a.topArc,    b.topArc,    t);
    // Discrete fields switch at the halfway point of the cross-fade.
    o.shape = (t < 0.5f) ? a.shape : b.shape;
    o.color = (t < 0.5f) ? a.color : b.color;
    return o;
}

EmotionManager::EmotionManager() {
    _from = _to = _live = preset(NORMAL);
    _mix.set(1.0f);
    behaviour(NORMAL, _blinkScale, _gazeEnergy);
}

void EmotionManager::setEmotion(Emotion e, uint32_t durationMs) {
    if (e == _target && !_mix.active()) return;
    _from = _live;
    _to   = preset(e);
    _target = e;
    _mix.start(0.0f, 1.0f, durationMs, Ease::Tween::CUBIC);
    behaviour(e, _blinkScale, _gazeEnergy);
}

void EmotionManager::update(float dtMs) {
    _mix.update(dtMs);
    _live = blend(_from, _to, _mix.value());
}

// -----------------------------------------------------------------------------
//  Presets - the "personality". Every eye is a single cyan shape; emotions are
//  expressed by silhouette + arcs + slant, matching the AURA reference sheet.
// -----------------------------------------------------------------------------
EyeParams EmotionManager::preset(Emotion e) {
    EyeParams p;  // defaults = NEUTRAL rounded square, cyan
    switch (e) {
        case HAPPY:
            p.bottomArc = 0.60f; p.height = Cfg::EYE_H * 1.05f;
            break;
        case EXCITED:
            p.bottomArc = 0.45f; p.width = Cfg::EYE_W * 1.10f;
            p.height = Cfg::EYE_H * 1.15f;
            break;
        case THINKING:
            p.height = Cfg::EYE_H * 0.95f;   // gaze-up handled by command
            break;
        case SAD:
            p.topArc = 0.45f; p.slant = -0.35f; p.openness = 0.9f;
            break;
        case ANGRY:
            p.slant = 0.75f; p.height = Cfg::EYE_H * 0.9f;
            break;
        case SURPRISED:
            p.shape = EyeShapeMode::CIRCLE;
            p.width = Cfg::EYE_W * 1.05f; p.height = Cfg::EYE_H * 1.15f;
            break;
        case CONFUSED:
            p.slant = 0.2f; p.openness = 0.92f;   // asymmetry via head tilt
            break;
        case WORRIED:
            p.topArc = 0.35f; p.slant = -0.2f; p.openness = 0.9f;
            break;
        case SLEEPY:
            p.openness = 0.4f; p.bottomArc = 0.15f;
            break;
        case SLEEP:
            p.openness = 0.05f;
            break;
        case CURIOUS:
            p.width = Cfg::EYE_W * 1.05f; p.height = Cfg::EYE_H * 1.05f;
            break;
        case LOVE:
            p.shape = EyeShapeMode::HEART; p.color = Cfg::LOVE_COLOR;
            p.width = Cfg::EYE_W * 1.05f; p.height = Cfg::EYE_H * 1.05f;
            break;
        case LISTENING:
            p.bottomArc = 0.3f;
            break;
        case SEARCHING:
            p.width = Cfg::EYE_W * 1.03f;
            break;
        case ERROR:
            p.color = Cfg::ERROR_COLOR; p.openness = 0.55f;
            p.height = Cfg::EYE_H * 0.65f;
            break;
        case NORMAL:
        default:
            break;
    }
    return p;
}

// Behaviour hints: how lively the idle system is during each emotion.
void EmotionManager::behaviour(Emotion e, float& blinkScale, float& gazeEnergy) {
    switch (e) {
        case SLEEPY:   blinkScale = 2.5f; gazeEnergy = 0.2f; break;
        case SLEEP:    blinkScale = 6.0f; gazeEnergy = 0.0f; break;
        case EXCITED:  blinkScale = 0.6f; gazeEnergy = 1.0f; break;
        case ANGRY:    blinkScale = 0.7f; gazeEnergy = 0.5f; break;
        case SAD:      blinkScale = 1.6f; gazeEnergy = 0.3f; break;
        case WORRIED:  blinkScale = 1.3f; gazeEnergy = 0.4f; break;
        case THINKING: blinkScale = 1.2f; gazeEnergy = 0.4f; break;
        case SEARCHING:blinkScale = 1.0f; gazeEnergy = 1.0f; break;
        case LISTENING:blinkScale = 1.1f; gazeEnergy = 0.6f; break;
        default:       blinkScale = 1.0f; gazeEnergy = 0.8f; break;
    }
}
