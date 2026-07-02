// =============================================================================
//  AnimationManager.cpp
// =============================================================================
#include "AnimationManager.h"

AnimationManager::AnimationManager() {
    _gx.set(0); _gy.set(0);
    _winkL.set(1.0f); _winkR.set(1.0f);
    _tilt.set(0);
    _intro.set(1.0f);            // start fully open unless boot() is called
    _saccadeGap = Cfg::SACCADE_MIN_GAP;
}

// Choose a new random gaze target. Bias slightly toward centre so the eyes
// keep returning "to you". Range and frequency scale with gazeEnergy.
void AnimationManager::scheduleSaccade(float gazeEnergy) {
    float range = Cfg::PUPIL_RANGE * (0.3f + 0.7f * gazeEnergy);
    // 1-in-3 chance to just recentre.
    float tx, ty;
    if (random(3) == 0) { tx = 0; ty = 0; }
    else {
        tx = (random(201) - 100) / 100.0f * range;
        ty = (random(201) - 100) / 100.0f * range * 0.7f;
    }
    _gx.retarget(tx, Cfg::SACCADE_MS, Ease::Tween::OUT_CUBIC);
    _gy.retarget(ty, Cfg::SACCADE_MS, Ease::Tween::OUT_CUBIC);

    uint32_t span = Cfg::SACCADE_MAX_GAP - Cfg::SACCADE_MIN_GAP;
    _saccadeGap = (uint32_t)((Cfg::SACCADE_MIN_GAP + random(span)) /
                             max(0.2f, gazeEnergy));
    _saccadeTimer = 0;
}

void AnimationManager::lookAt(float nx, float ny) {
    nx = Ease::clampf(nx, -1, 1);
    ny = Ease::clampf(ny, -1, 1);
    _gx.retarget(nx * Cfg::PUPIL_RANGE, 160, Ease::Tween::OUT_CUBIC);
    _gy.retarget(ny * Cfg::PUPIL_RANGE, 160, Ease::Tween::OUT_CUBIC);
    _holdExternal = 900;         // ignore random saccades while tracking
}

void AnimationManager::boot() {
    _intro.start(0.0f, 1.0f, 900, Ease::Tween::OUT_BACK);
}

void AnimationManager::wake() {
    _intro.start(0.2f, 1.0f, 700, Ease::Tween::OUT_BACK);
}

void AnimationManager::wink(Side s) {
    Ease::Tween& w = (s == Side::LEFT) ? _winkL : _winkR;
    w.set(1.0f);
    w.start(1.0f, 0.0f, 110, Ease::Tween::QUAD);   // shut fast; reopen handled below
    // Reopen is triggered in update() once the close finishes.
}

void AnimationManager::setSearching(bool on) {
    _searching = on;
    _searchT = 0;
}

void AnimationManager::setHeadTilt(float px) {
    _tilt.retarget(px, 300, Ease::Tween::CUBIC);
}

void AnimationManager::update(float dtMs, float gazeEnergy, BlinkSystem& blink) {
    _gx.update(dtMs); _gy.update(dtMs);
    _winkL.update(dtMs); _winkR.update(dtMs);
    _tilt.update(dtMs); _intro.update(dtMs);

    // Reopen an eye once its wink close-tween has finished.
    if (!_winkL.active() && _winkL.value() < 0.05f) _winkL.start(0, 1, 150, Ease::Tween::OUT_CUBIC);
    if (!_winkR.active() && _winkR.value() < 0.05f) _winkR.start(0, 1, 150, Ease::Tween::OUT_CUBIC);

    // --- Breathing bob: smooth sine over BREATH_PERIOD_MS ---
    _breathT = (_breathT + (uint32_t)dtMs) % Cfg::BREATH_PERIOD_MS;
    float ph = (float)_breathT / Cfg::BREATH_PERIOD_MS * TWO_PI;
    _bob = sinf(ph) * Cfg::BREATH_AMPLITUDE;

    // --- Micro-tremor: refresh a tiny random offset a few times a second ---
    _jitterTimer += (uint32_t)dtMs;
    if (_jitterTimer > 180) {
        _jitterTimer = 0;
        _jx = (random(201) - 100) / 100.0f * Cfg::MICRO_JITTER * 0.05f * gazeEnergy;
        _jy = (random(201) - 100) / 100.0f * Cfg::MICRO_JITTER * 0.05f * gazeEnergy;
    }

    // --- Searching sweep overrides idle saccades ---
    if (_searching) {
        _searchT += (uint32_t)dtMs;
        float s = sinf((float)_searchT / 700.0f);
        _gx.retarget(s * Cfg::PUPIL_RANGE, 250, Ease::Tween::OUT_CUBIC);
        _gy.set(sinf((float)_searchT / 1600.0f) * Cfg::PUPIL_RANGE * 0.4f);
        return;
    }

    // --- External (face-tracking) hold, else random idle saccades ---
    if (_holdExternal > (uint32_t)dtMs) { _holdExternal -= (uint32_t)dtMs; }
    else {
        _holdExternal = 0;
        _saccadeTimer += (uint32_t)dtMs;
        if (_saccadeTimer >= _saccadeGap) {
            scheduleSaccade(gazeEnergy);
            // Occasionally add character with a double or slow blink.
            int roll = random(10);
            if (roll == 0) blink.trigger(BlinkSystem::DOUBLE);
            else if (roll == 1) blink.trigger(BlinkSystem::SLOW);
        }
    }
}
