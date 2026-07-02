// =============================================================================
//  AnimationManager.h
//  Everything that MOVES the eyes without changing the emotion: idle saccades
//  (random look left/right/up/down/centre), constant micro-tremor so the eyes
//  are never perfectly still, a gentle breathing bob, per-eye winks, a head-
//  tilt offset (curious/confused) and scripted one-shots (boot grow-in, wake,
//  searching sweep). It also nudges the BlinkSystem for occasional double and
//  slow blinks. Face-tracking feeds in through lookAt().
// =============================================================================
#pragma once
#include "Config.h"
#include "Easing.h"
#include "BlinkSystem.h"
#include "EyeState.h"   // <-- ADD THIS

class AnimationManager {
public:
    AnimationManager();

    // Per-frame update. gazeEnergy (0..1) comes from the current emotion and
    // scales how lively the idle wandering is.
    void update(float dtMs, float gazeEnergy, BlinkSystem& blink);

    // External gaze target in normalised [-1,1] (e.g. from MediaPipe face
    // tracking on the laptop). Suppresses random saccades briefly.
    void lookAt(float nx, float ny);

    // One-shots.
    void boot();                 // grow eyes in from nothing
    void wake();                 // stretch open from sleep
    void wink(Side s);           // close one eye briefly
    void setSearching(bool on);  // continuous left-right sweep
    void setHeadTilt(float px);  // raise one eye / lower the other (curious)

    // Resolved outputs consumed by the main loop.
    float gazeX()  const { return _gx.value() + _jx; }
    float gazeY()  const { return _gy.value() + _jy; }
    float bob()    const { return _bob; }
    float winkL()  const { return _winkL.value(); }
    float winkR()  const { return _winkR.value(); }
    float tiltL()  const { return -_tilt.value(); } // left eye up when tilt>0
    float tiltR()  const { return  _tilt.value(); }
    float introOpen() const { return _intro.value(); }

private:
    void scheduleSaccade(float gazeEnergy);

    Ease::Tween _gx, _gy;        // gaze target (px)
    Ease::Tween _winkL, _winkR;  // per-eye openness for winks (1=open)
    Ease::Tween _tilt;           // head-tilt offset (px)
    Ease::Tween _intro;          // boot/wake openness ramp (0..1)

    float _jx = 0, _jy = 0;      // micro-tremor offsets
    float _bob = 0;              // breathing bob (px)
    uint32_t _breathT = 0;       // breathing phase accumulator
    uint32_t _saccadeTimer = 0;  // ms until next random look
    uint32_t _saccadeGap = 2000;
    uint32_t _jitterTimer = 0;
    uint32_t _holdExternal = 0;  // ms left holding an external lookAt target
    bool     _searching = false;
    uint32_t _searchT = 0;
};
