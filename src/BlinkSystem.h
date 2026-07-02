// =============================================================================
//  BlinkSystem.h
//  A self-contained blink controller. Runs on its own random schedule, is
//  interruptible (can be told to blink NOW), and supports fast, slow, double
//  and sleep-style blinks. Output is a single openness value in [0,1] that the
//  main loop multiplies into each eye - so blinking is fully decoupled from
//  emotion and gaze.
// =============================================================================
#pragma once
#include <Arduino.h>
#include "Config.h"

class BlinkSystem {
public:
    // Blink flavours requested over serial or by the animation manager.
    enum Mode { FAST, SLOW, DOUBLE, SLEEP };

    BlinkSystem();

    // Advance the blink state machine by dt milliseconds.
    void update(float dtMs);

    // Force a blink immediately, interrupting any idle wait.
    void trigger(Mode mode = FAST);

    // Bias the auto-blink rate (e.g. sleepy emotions blink slower/longer).
    void setRateScale(float scale) { _rateScale = max(0.1f, scale); }

    // Current eyelid openness: 1 = fully open, 0 = shut.
    float openness() const { return _open; }

private:
    // Pick the next random idle wait before the next automatic blink.
    void scheduleNext();

    enum Phase { WAIT, CLOSING, OPENING } _phase = WAIT;
    float    _open = 1.0f;      // live openness output
    uint32_t _timer = 0;        // ms accumulated in current phase
    uint32_t _dur = Cfg::BLINK_FAST_MS;
    uint32_t _nextGap = 3000;   // ms to wait before next auto blink
    int      _repeats = 0;      // remaining blinks (for DOUBLE)
    Mode     _mode = FAST;
    float    _rateScale = 1.0f;
};
