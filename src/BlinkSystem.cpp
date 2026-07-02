// =============================================================================
//  BlinkSystem.cpp
// =============================================================================
#include "BlinkSystem.h"
#include "Easing.h"

BlinkSystem::BlinkSystem() { scheduleNext(); }

// Random idle wait, scaled by emotion (sleepy -> longer gaps).
void BlinkSystem::scheduleNext() {
    uint32_t span = Cfg::BLINK_MAX_GAP - Cfg::BLINK_MIN_GAP;
    _nextGap = (uint32_t)((Cfg::BLINK_MIN_GAP + random(span)) * _rateScale);
    _timer = 0;
    _phase = WAIT;
}

// Start a blink now. DOUBLE queues a second blink after the first completes.
void BlinkSystem::trigger(Mode mode) {
    _mode = mode;
    _dur = (mode == FAST) ? Cfg::BLINK_FAST_MS : Cfg::BLINK_SLOW_MS;
    _repeats = (mode == DOUBLE) ? 1 : 0;
    _phase = CLOSING;
    _timer = 0;
}

void BlinkSystem::update(float dtMs) {
    _timer += (uint32_t)dtMs;

    switch (_phase) {
        case WAIT:
            // Idle until the scheduled gap elapses, then auto-blink.
            if (_timer >= _nextGap) {
                trigger(_mode == SLEEP ? SLEEP : FAST);
            }
            break;

        case CLOSING: {
            // Eyelid falls: openness 1 -> 0 over half the blink duration.
            float t = Ease::clampf((float)_timer / (_dur * 0.5f), 0, 1);
            _open = 1.0f - Ease::inOutQuad(t);
            if (t >= 1.0f) { _phase = OPENING; _timer = 0; }
            break;
        }

        case OPENING: {
            // Eyelid rises: openness 0 -> 1 over the second half.
            float t = Ease::clampf((float)_timer / (_dur * 0.5f), 0, 1);
            _open = Ease::inOutQuad(t);
            if (t >= 1.0f) {
                _open = 1.0f;
                if (_repeats > 0) {           // DOUBLE blink: go again
                    _repeats--;
                    _phase = CLOSING; _timer = 0;
                } else {
                    scheduleNext();           // back to idle waiting
                }
            }
            break;
        }
    }
}
